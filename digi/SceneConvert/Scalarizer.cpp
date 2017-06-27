#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/Find.h>
#include <digi/Utility/StringUtility.h>
#include <digi/Utility/VectorUtility.h>
#include <digi/Utility/foreach.h>
#include <digi/System/Log.h>
#include <digi/CodeGenerator/NameMangler.h>

#include "ValueHelper.h"
#include "Scalarizer.h"


#define UNARY_OP(op) \
	case llvm::Instruction::op: \
		{ \
			ValueInfo value = this->getValue(instruction->getOperand(0)); \
			ValueInfo& result = this->valueInfos[instruction]; \
			llvm::Type* sType = getScalarType(instruction->getType()); \
			for (int i = 0; i < numElements; ++i) \
			{ \
				result.values[i] = builder.Create##op(value.values[i], sType, name); \
			} \
		} \
		break;


namespace digi {

static llvm::Type* getScalarType(llvm::Type* type)
{
	if (const llvm::VectorType* vectorType = llvm::dyn_cast<llvm::VectorType>(type))
		type = vectorType->getElementType();
	return type;
}


Scalarizer::Scalarizer(llvm::Function& function, llvm::Module& sModule)
	: sModule(sModule)	
{
	llvm::FunctionType* functionType = function.getFunctionType();
	llvm::StringRef functionName = function.getName();

	// create function with same name and signature
	this->sFunction = llvm::cast<llvm::Function>(sModule.getOrInsertFunction(functionName, functionType));

	// iterate over basic blocks
	for (llvm::Function::iterator bbIt = function.begin(); bbIt != function.end(); ++bbIt)
	{
		llvm::BasicBlock* basicBlock = bbIt;

		// get scalarized basic block
		llvm::BasicBlock* sBasicBlock = this->getBasicBlock(basicBlock);
		
		llvm::IRBuilder<> builder(sBasicBlock);
		
		// iterate over instructions
		llvm::BasicBlock::iterator it  = basicBlock->begin();
		llvm::BasicBlock::iterator end = basicBlock->end();
		for (; it != end; ++it)
		{
			llvm::Instruction* instruction = it;

			llvm::StringRef name = instruction->getName();
			int numElements = getNumElements(instruction);
			if (llvm::BinaryOperator* binaryOp = llvm::dyn_cast<llvm::BinaryOperator>(instruction))
			{
				// binary operator
				ValueInfo left = this->getValue(instruction->getOperand(0));
				ValueInfo right = this->getValue(instruction->getOperand(1));
				ValueInfo& result = this->valueInfos[instruction];
				
				for (int i = 0; i < numElements; ++i)
				{
					//if (llvm::isa<llvm::UndefValue>(left.values[i]) || llvm::isa<llvm::UndefValue>(right.values[i]))
					//	dWarning("encountered an undefined value");
						
					result.values[i] = builder.CreateBinOp(binaryOp->getOpcode(), left.values[i], right.values[i], name);
				}
			}
			else
			{
				switch (instruction->getOpcode())
				{
				case llvm::Instruction::Load:
					{
						// get pointer(s) where we load from
						ValueInfo ptr = this->getPointer(builder, instruction->getOperand(0), numElements);
						ValueInfo& result = this->valueInfos[instruction];

						for (int i = 0; i < numElements; ++i)
						{
							result.values[i] = builder.CreateLoad(ptr.values[i], name);
						}
					}
					break;
				case llvm::Instruction::Store:
					{
						// get pointer(s) where we store to
						llvm::Value* storeValue = instruction->getOperand(0);
						numElements = getNumElements(storeValue);
						ValueInfo value = this->getValue(storeValue);
						ValueInfo ptr = this->getPointer(builder, instruction->getOperand(1), numElements);

						for (int i = 0; i < numElements; ++i)
						{
							builder.CreateStore(value.values[i], ptr.values[i]);
						}
					}
					break;

				case llvm::Instruction::PHI:
					{
						llvm::PHINode* phiNode = llvm::cast<llvm::PHINode>(instruction);
						ValueInfo& result = this->valueInfos[instruction];

						// scalarize type
						llvm::Type* sType = getScalarType(phiNode->getType());
						int numIncomingValues = phiNode->getNumIncomingValues();

						// create phi nodes for all vector elements
						for (int i = 0; i < numElements; ++i)
							result.values[i] = builder.CreatePHI(sType, numIncomingValues, name);
						
						for (int j = 0; j < numIncomingValues; ++j)
						{
							llvm::BasicBlock* sBasicBlock = this->getBasicBlock(phiNode->getIncomingBlock(j));
							ValueInfo value = this->getValue(phiNode->getIncomingValue(j));

							for (int i = 0; i < numElements; ++i)
							{
								llvm::cast<llvm::PHINode>(result.values[i])->addIncoming(value.values[i], sBasicBlock);
							}
						}
					}
					break;

				case llvm::Instruction::ICmp:
					{
						llvm::ICmpInst* cmpInst = llvm::cast<llvm::ICmpInst>(instruction);
					
						ValueInfo left = this->getValue(instruction->getOperand(0));
						ValueInfo right = this->getValue(instruction->getOperand(1));
						ValueInfo& result = this->valueInfos[instruction];
					
						for (int i = 0; i < numElements; ++i)
						{
							result.values[i] = builder.CreateICmp(cmpInst->getPredicate(), left.values[i], right.values[i], name);
						}
					}
					break;
				case llvm::Instruction::FCmp:
					{
						llvm::FCmpInst* cmpInst = llvm::cast<llvm::FCmpInst>(instruction);

						ValueInfo left = this->getValue(instruction->getOperand(0));
						ValueInfo right = this->getValue(instruction->getOperand(1));
						ValueInfo& result = this->valueInfos[instruction];

						for (int i = 0; i < numElements; ++i)
						{
							result.values[i] = builder.CreateFCmp(cmpInst->getPredicate(), left.values[i], right.values[i], name);
						}
					}
					break;

				UNARY_OP(ZExt)
				UNARY_OP(SExt)
				UNARY_OP(Trunc)
				UNARY_OP(FPExt)
				UNARY_OP(FPTrunc)
				UNARY_OP(SIToFP)
				UNARY_OP(UIToFP)
				UNARY_OP(FPToSI)
				UNARY_OP(FPToUI)
				
				case llvm::Instruction::Select:
					{
						ValueInfo condition = this->getValue(instruction->getOperand(0));
						ValueInfo trueCase = this->getValue(instruction->getOperand(1));
						ValueInfo falseCase = this->getValue(instruction->getOperand(2));
						ValueInfo& result = this->valueInfos[instruction];

						bool conditionIsVector = getNumElements(instruction->getOperand(0)) > 1;
						for (int i = 0; i < numElements; ++i)
						{
							result.values[i] = builder.CreateSelect(condition.values[conditionIsVector ? i : 0], trueCase.values[i], falseCase.values[i], name);
						}
					}
					break;

				case llvm::Instruction::InsertElement:
					{
						ValueInfo vector = this->getValue(instruction->getOperand(0));
						ValueInfo scalar = this->getValue(instruction->getOperand(1));
						int index = getConstantIndex(instruction->getOperand(2));
						ValueInfo& result = this->valueInfos[instruction];

						if (index == -1)
						{
							// error: variable index not supported
							index = 0;
						}
						for (int i = 0; i < numElements; ++i)
						{
							result.values[i] = (i == index) ? scalar.values[0] : vector.values[i];
						}
					}
					break;
				case llvm::Instruction::ExtractElement:
					{
						ValueInfo vector = this->getValue(instruction->getOperand(0));
						int index = getConstantIndex(instruction->getOperand(1));
						ValueInfo& result = this->valueInfos[instruction];

						if (index == -1)
						{
							// error: variable index not supported
							index = 0;
						}
						result.values[0] = vector.values[index];
					}
					break;
				case llvm::Instruction::ShuffleVector:
					{
						ValueInfo vector1 = this->getValue(instruction->getOperand(0));
						ValueInfo vector2 = this->getValue(instruction->getOperand(1));
						int4 indices = getConstantIndexVector(instruction->getOperand(2));
						ValueInfo& result = this->valueInfos[instruction];

						int dim1 = getNumElements(instruction->getOperand(0));
						int dim2 = getNumElements(instruction->getOperand(1));
						
						for (int i = 0; i < numElements; ++i)
						{
							int index = indices[i];
							if (index == -1)
							{
								// error: variable index not supported
								index = 0;
							}

							if (index < dim1)
							{
								result.values[i] = vector1.values[index];
							}
							else if (index - dim1 < dim2)
							{
								result.values[i] = vector2.values[index - dim1];
							}
							else
							{
								// error: index out of range
							}							
						}
					}
					break;

				case llvm::Instruction::Call:
					{
						llvm::CallInst* callInst = llvm::cast<llvm::CallInst>(instruction);
				
						llvm::Function* function = callInst->getCalledFunction();
						if (function != NULL)
						{
							// get name of funtion
							llvm::StringRef functionName = function->getName();

							// demangle function name
							std::string baseName = NameDemangler::demangle(StringRef(functionName.data(), functionName.size()));
							
							
							
							int numOperands = callInst->getNumArgOperands();							
							
							// get result type
							llvm::Type* resultType = callInst->getType();
							if (resultType->isVectorTy())
								resultType = llvm::cast<llvm::VectorType>(resultType)->getElementType();
							
							// get parameter types and mangled function name
							std::vector<llvm::Type*> types;
							NameMangler mangler;
							mangler.addFunction(baseName);
							for (int i = 0; i < numOperands; ++i)
							{
								llvm::Value* arg = callInst->getArgOperand(i);
								
								// scalarize
								llvm::Type* type = arg->getType();
								if (const llvm::VectorType* vectorType = llvm::dyn_cast<llvm::VectorType>(type))
								{
									type = vectorType->getElementType();
								}
							
								types += type;
								mangler.addArgument(getVectorInfo(type));
							}
							
							// name of scalarized function
							std::string sFunctionName;
							if (baseName == functionName)
							{
								// name was not mangled, e.g. sin()
								sFunctionName = functionName;
							}
							else
							{
								sFunctionName = mangler.get();
							}
							
							// create function type
							llvm::FunctionType* sFunctionType = llvm::FunctionType::get(resultType, types, false);
							
							// add scalarized function 
							llvm::Function* sFunction = llvm::cast<llvm::Function>(this->sModule.getOrInsertFunction(sFunctionName, sFunctionType));
							sFunction->setAttributes(function->getAttributes());

							ValueInfo& result = this->valueInfos[instruction];
							
							// create scalarized calls
							for (int elementIndex = 0; elementIndex < numElements; ++elementIndex)
							{
								// iterate over arguments
								llvm::SmallVector<llvm::Value*, 4> args;
								for (int i = 0; i < numOperands; ++i)
								{
									llvm::Value* arg = callInst->getArgOperand(i);

									ValueInfo sArg;

									// check for pointer argument
									if (const llvm::PointerType* pointerType = llvm::dyn_cast<llvm::PointerType>(arg->getType()))
										sArg = this->getPointer(builder, arg, getNumElements(pointerType->getElementType()));
									else
										sArg = this->getValue(arg);
									
									// add arg (and auto-splat scalars)
									args.push_back(sArg.values[elementIndex] != NULL ? sArg.values[elementIndex] : sArg.values[0]);
								}
								
								result.values[elementIndex] = builder.CreateCall(sFunction, args, name);
							}
						}
					}					
					break;

				case llvm::Instruction::Br:
					{
						llvm::BranchInst* branchInst = llvm::dyn_cast<llvm::BranchInst>(instruction);
						if (branchInst->isUnconditional())
						{
							// unconditional branch
							builder.CreateBr(this->getBasicBlock(branchInst->getSuccessor(0)));
						}
						else if (branchInst->getNumSuccessors() == 2)
						{
							// conditional branch

							// get the condition
							ValueInfo condition = this->getValue(branchInst->getOperand(0));
							
							// get the two branches
							llvm::BasicBlock* trueBranch = this->getBasicBlock(branchInst->getSuccessor(0));
							llvm::BasicBlock* falseBranch = this->getBasicBlock(branchInst->getSuccessor(1));
							
							builder.CreateCondBr(condition.values[0], trueBranch, falseBranch);						
						}
					}					
					break;
				case llvm::Instruction::Switch:
					break;
				case llvm::Instruction::Ret:
					{
						if (instruction->getNumOperands() > 0)
						{
							// has return value. note: we can only return the first component
							ValueInfo vector1 = this->getValue(instruction->getOperand(0));
							
							ValueInfo value = this->getValue(instruction->getOperand(0));
							builder.CreateRet(value.values[0]);
						}
						else
						{
							builder.CreateRetVoid();
						}
					}
					break;
				}
			}
		}
	}
}

llvm::BasicBlock* Scalarizer::getBasicBlock(llvm::BasicBlock* basicBlock)
{
	llvm::BasicBlock*& sBasicBlock = this->basicBlocks[basicBlock];
	if (sBasicBlock == NULL)
		sBasicBlock = llvm::BasicBlock::Create(basicBlock->getContext(), basicBlock->getName(), this->sFunction);
	return sBasicBlock;
}

Scalarizer::ValueInfo Scalarizer::getValue(llvm::Value* value)
{
	// check for constant
	if (llvm::isa<llvm::Constant>(value))
	{
		ValueInfo v;
		fill(v.values, (llvm::Value*)NULL);
		llvm::Type* type = value->getType();
		
		int numElements = 1;
		if (type->isVectorTy())
		{
			const llvm::VectorType* vectorType = llvm::cast<llvm::VectorType>(type);
			numElements = vectorType->getNumElements();
			type = vectorType->getElementType();
		}
		
		// constant vector
		if (llvm::ConstantVector* constantVector = llvm::dyn_cast<llvm::ConstantVector>(value))
		{
			// check for splat value
			if (llvm::Constant* splatValue = constantVector->getSplatValue())
			{
				for (int i = 0; i < numElements; ++i)
					v.values[i] = splatValue;
				return v;
			}

			// get vector elements
			llvm::SmallVector<llvm::Constant*, 4> elements;
			constantVector->getVectorElements(elements);
			numElements = int(elements.size());
			for (int i = 0; i < numElements; ++i)
				v.values[i] = elements[i];
			return v;
		}
		
		llvm::Value* constValue = value;

		// constant zero (scalar or vector) or undef
		if (llvm::isa<llvm::ConstantAggregateZero>(value) || llvm::isa<llvm::UndefValue>(value)
			|| llvm::isa<llvm::ConstantExpr>(value)) //! treat ConstantExpr as zero for now
		{
			if (llvm::isa<llvm::UndefValue>(value))
			{
				constValue = llvm::UndefValue::get(type);
			}
			else
			{
				if (type->isFloatingPointTy())
					constValue = llvm::ConstantFP::get(type, 0.0);
				else
					constValue = llvm::ConstantInt::get(type, 0);
			}
		}

		for (int i = 0; i < numElements; ++i)
			v.values[i] = constValue;
		return v;
	}

	// lookup value
	return this->valueInfos[value];
}

Scalarizer::ValueInfo Scalarizer::getPointer(llvm::IRBuilder<>& builder, llvm::Value* ptr, int numElements)
{
	// numElements is number of elements of value that ptr points to

	ValueInfo v;
	fill(v.values, (llvm::Value*)NULL);

	if (llvm::ConstantExpr* constantExpr = llvm::dyn_cast<llvm::ConstantExpr>(ptr))
	{
		// ptr is a constant getelementptr instruction
		if (constantExpr->isGEPWithNoNotionalOverIndexing())
		{
			// get scalarized global
			llvm::Value* sGlobal = this->getGlobal(constantExpr->getOperand(0));
		
			// get indices (all constant)
			int numOperands = constantExpr->getNumOperands();
			std::vector<llvm::Value*> indices;
			for (int i = 1; i < numOperands; ++i)
				indices += constantExpr->getOperand(i);
			
			if (numElements == 1)
			{
				// scalar
				v.values[0] = llvm::ConstantExpr::getGetElementPtr(llvm::cast<llvm::Constant>(sGlobal), indices);
			}
			else
			{
				// vector
				add(indices);
				llvm::LLVMContext& c = ptr->getContext();
				for (int i = 0; i < numElements; ++i)
				{			
					indices.back() = llvm::ConstantInt::get(llvm::IntegerType::get(c, 32), i);
					v.values[i] = llvm::ConstantExpr::getGetElementPtr(llvm::cast<llvm::Constant>(sGlobal), indices);
				}
			}
			return v;
		}
	}
	else if (llvm::GetElementPtrInst* getElementPtrInst = llvm::dyn_cast<llvm::GetElementPtrInst>(ptr))
	{
		// ptr is a getelementptr instruction with runtime array lookup (e.g. "transform.matrices[index].x")
					
		// get scalarized global
		llvm::Value* sGlobal = this->getGlobal(getElementPtrInst->getPointerOperand());

		// get indices
		std::vector<llvm::Value*> indices;
		llvm::GetElementPtrInst::op_iterator it = getElementPtrInst->idx_begin();
		llvm::GetElementPtrInst::op_iterator end = getElementPtrInst->idx_end();
		for (; it != end; ++it)
			indices += this->getValue(*it).values[0];
			
		if (numElements == 1)
		{
			// scalar
			v.values[0] = builder.CreateGEP(sGlobal, indices, getElementPtrInst->getName());
		}
		else
		{
			// vector
			add(indices);
			llvm::LLVMContext& c = ptr->getContext();
			for (int i = 0; i < numElements; ++i)
			{			
				indices.back() = llvm::ConstantInt::get(llvm::IntegerType::get(c, 32), i);
				v.values[i] = builder.CreateGEP(sGlobal, indices, getElementPtrInst->getName());
			}
		}
		return v;
	}

	llvm::Value* sGlobal = this->getGlobal(ptr);
	if (numElements == 1)
	{
		// scalar
		v.values[0] = sGlobal;
	}
	else
	{
		// vector
		llvm::LLVMContext& c = ptr->getContext();
		std::vector<llvm::Constant*> indices(2);
		indices[0] = llvm::ConstantInt::get(llvm::IntegerType::get(c, 32), 0);
		for (int i = 0; i < numElements; ++i)
		{			
			indices[1] = llvm::ConstantInt::get(llvm::IntegerType::get(c, 32), i);
			v.values[i] = llvm::ConstantExpr::getGetElementPtr(llvm::cast<llvm::Constant>(sGlobal), indices);
		}
	}
	
	return v;
}

llvm::Value* Scalarizer::getGlobal(llvm::Value* value)
{
	llvm::GlobalVariable* global = llvm::cast<llvm::GlobalVariable>(value);
	llvm::StringRef globalName = global->getName();
	llvm::Type* globalType = llvm::dyn_cast<llvm::PointerType>(global->getType())->getElementType();

	// check if global already has beed copied/scalarized
	llvm::GlobalVariable* sGlobal = this->sModule.getGlobalVariable(globalName, true);
	if (sGlobal != NULL)
		return sGlobal;

	llvm::Type* sGlobalType = this->scalarizeType(globalType);
		
	// get or insert into module of this scope
	sGlobal = llvm::cast<llvm::GlobalVariable>(this->sModule.getOrInsertGlobal(globalName, sGlobalType));

	// copy attributes
	sGlobal->copyAttributesFrom(global);
	sGlobal->setConstant(global->isConstant());
	sGlobal->setLinkage(global->getLinkage());
	
	return sGlobal;
}

llvm::Type* Scalarizer::scalarizeType(llvm::Type* type)
{
	if (llvm::StructType* structType = llvm::dyn_cast<llvm::StructType>(type))
	{
		// struct type
		std::vector<llvm::Type*> elements;

		llvm::StructType::element_iterator it = structType->element_begin();
		llvm::StructType::element_iterator end = structType->element_end();
		for (; it != end; ++it)
		{
			llvm::Type* elementType = *it;
			elements += this->scalarizeType(elementType);
		}
				
		return llvm::StructType::get(type->getContext(), elements);
	}
	
	if (const llvm::ArrayType* arrayType = llvm::dyn_cast<llvm::ArrayType>(type))
	{
		// array type
		uint64_t numElements = arrayType->getNumElements();
	
		return llvm::ArrayType::get(this->scalarizeType(arrayType->getElementType()), numElements);
	}
	
	if (const llvm::VectorType* vectorType = llvm::dyn_cast<llvm::VectorType>(type))
	{
		// vector type: convert to array type
		uint64_t numElements = vectorType->getNumElements();
		
		return llvm::ArrayType::get(vectorType->getElementType(), numElements);
	}
	
	// scalar type
	return type;	
}

} // namespace digi
