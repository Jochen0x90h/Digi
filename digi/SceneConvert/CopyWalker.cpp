#include <llvm/Module.h>
#include <llvm/Instructions.h>
#include <llvm/Constants.h>
#include <llvm/Support/IRBuilder.h>

#include <digi/Utility/VectorUtility.h>
#include <digi/Utility/MapUtility.h>
#include <digi/Utility/foreach.h>
#include <digi/System/Log.h>
#include <digi/CodeGenerator/NameMangler.h>

#include "ValueHelper.h"
#include "CopyWalker.h"

#define UNARY_OP(op) \
		case llvm::Instruction::op: \
			{ \
				llvm::Value* arg = this->getValue(scope, instruction->getOperand(0)); \
				llvm::Value* value = builder.Create##op(arg, instruction->getType(), name); \
				scope->insert(instruction, value); \
			} \
			break;		


namespace digi {

CopyWalker::CopyWalker(llvm::PostDominatorTree* postDominatorTree, llvm::Function& function,
	ArrayRef<llvm::OwningPtr<llvm::Module> > modules)
	: InstructionWalker(postDominatorTree), scopes(modules.size())
{
	// setup scopes	
	llvm::LLVMContext& c = function.getContext();
	llvm::FunctionType* functionType = function.getFunctionType();
	llvm::StringRef functionName = function.getName();
	llvm::BasicBlock* entryBlock = this->entryBlock = &function.getEntryBlock();
	size_t numScopes = modules.size();
	for (size_t i = 0; i < numScopes; ++i)
	{
		Scope& scope = this->scopes[i];
		scope.index = i;

		scope.module = modules[i].get();
		
		// create function with same signature
		scope.currentFunction = llvm::cast<llvm::Function>(scope.module->getOrInsertFunction(functionName, functionType));

		// create entry block for scope
		llvm::BasicBlock* sEntryBlock = llvm::BasicBlock::Create(c, entryBlock->getName(), scope.currentFunction);
		scope.currentBasicBlock = sEntryBlock;
		
		// add to mapping
		scope.basicBlocks[entryBlock] = sEntryBlock;
	}
}

CopyWalker::~CopyWalker()
{
	// finish scopes
	foreach (Scope& scope, this->scopes)
	{
		if (scope.currentBasicBlock->getTerminator() == NULL)
		{
			llvm::IRBuilder<> builder(scope.currentBasicBlock);
			builder.CreateRetVoid();
		}
	}
}

void CopyWalker::doBranch(llvm::BasicBlock* basicBlock, llvm::Value* condition, llvm::BasicBlock* trueBranch,
	llvm::BasicBlock* falseBranch, llvm::BasicBlock* join, llvm::BasicBlock* stopBlock)
{
	// collect sopes from phi nodes of join node
	int scopes = 0;
	llvm::BasicBlock::iterator it  = join->begin();
	llvm::BasicBlock::iterator end = join->end();
	for (; it != end; ++it)
	{
		llvm::Instruction* instruction = it;

		if (llvm::PHINode* phiNode = llvm::dyn_cast<llvm::PHINode>(instruction))
		{
			// set scope to phi node
			ValueInfo& valueInfo = this->valueInfos[phiNode];
			if (valueInfo.scope >= 0)
				scopes |= 1 << valueInfo.scope;
		}
		else
		{
			// no more phi nodes
			break;
		}
	}					
	
	// collect scope from basic block (if explicitly set via classifyBasicBlock e.g. for discard() function)
	{
		ValueInfo& valueInfo = this->valueInfos[basicBlock];
		if (valueInfo.scope >= 0)
			scopes |= 1 << valueInfo.scope;
	}
	
	// generate if statement in scopes. it is possible that there is no scope, then the calculations
	// inside the branches are not used and can be omitted
	int scopeIndex = 0;
	while (scopes > 0)
	{
		if (scopes & 1)
		{
			Scope* scope = &this->scopes[scopeIndex];
				
			// get condition
			llvm::Value* cCondition = this->getValue(scope, condition);

			// get/create new basic blocks
			llvm::BasicBlock* cTrueBranch = this->copyBasicBlock(scope, trueBranch);
			llvm::BasicBlock* cFalseBranch = this->copyBasicBlock(scope, falseBranch);

			// copy branch instruction
			{	
				llvm::IRBuilder<> builder(scope->currentBasicBlock);
				builder.CreateCondBr(cCondition, cTrueBranch, cFalseBranch);
			}	

			// continue with true branch
			scope->currentBasicBlock = cTrueBranch;
									
			// follow true branch until join block is hit
			this->doBasicBlock(basicBlock, trueBranch, join);
			bool trueAddJump = trueBranch != join && scope->currentBasicBlock == cTrueBranch;
			
			// now switch to false branch
			scope->currentBasicBlock = cFalseBranch;

			// follow false branch until join block is hit
			this->doBasicBlock(basicBlock, falseBranch, join);
			bool falseAddJump = falseBranch != join && scope->currentBasicBlock == cFalseBranch;


			// copy join basic block
			llvm::BasicBlock* cJoin = this->copyBasicBlock(scope, join);

			// add jumps to join
			if (trueAddJump)
			{
				llvm::IRBuilder<> builder(cTrueBranch);
				builder.CreateBr(cJoin);
			}
			if (falseAddJump)
			{
				llvm::IRBuilder<> builder(cFalseBranch);
				builder.CreateBr(cJoin);
			}

			// now continue with join
			scope->currentBasicBlock = cJoin;
		}
		++scopeIndex;
		scopes >>= 1;
	}
}

void CopyWalker::doInstruction(llvm::BasicBlock* predecessor, llvm::Instruction* instruction)
{
	// copy instruction
	Scope* scope = this->getScope(instruction);
	if (scope == NULL)
		return;

	llvm::IRBuilder<> builder(scope->currentBasicBlock);
	llvm::StringRef name = instruction->getName();

	if (llvm::BinaryOperator* binaryOp = llvm::dyn_cast<llvm::BinaryOperator>(instruction))
	{
		llvm::Value* left = this->getValue(scope, instruction->getOperand(0));
		llvm::Value* right = this->getValue(scope, instruction->getOperand(1));
		
		llvm::Value* value = builder.CreateBinOp(binaryOp->getOpcode(), left, right, name);
		scope->insert(instruction, value);
	}
	else
	{
		switch (instruction->getOpcode())
		{
		case llvm::Instruction::Load:
			{
				// the pointer where the instruction loads from
				llvm::Value* ptr = instruction->getOperand(0);

				llvm::LoadInst* cLoadInst;
				if (llvm::GetElementPtrInst* getElementPtrInst = llvm::dyn_cast<llvm::GetElementPtrInst>(ptr))
				{
					// load with runtime array lookup (e.g. "transform.matrices[index].x")
								
					// the global variable is the first argument of getElementPointer
					llvm::Value* global = getElementPtrInst->getPointerOperand();
					
					// get scope of global variable
					Scope* srcScope = this->getScope(global);
					
					// copy global in source scope
					llvm::Value* cGlobal = this->getGlobal(srcScope, global);
					
					if (srcScope != scope)
					{
						// global and accessed value are in different scopes
						ScopePair scopePair(srcScope, scope);
						
						Transfer& transfer = this->transfers[scopePair];
									
						// get type of global
						llvm::Type* type = llvm::cast<llvm::PointerType>(global->getType())->getElementType();

						llvm::GetElementPtrInst::op_iterator it = getElementPtrInst->idx_begin();
						llvm::GetElementPtrInst::op_iterator end = getElementPtrInst->idx_end();
						Transfer::GlobalAccess globalAccess(global);
						++it;
						llvm::Value* indexValue = NULL;
						int numElements = 0;
						for (; it != end; ++it)
						{
							int index = getConstantIndex(*it);
							
							if (index == -1)
							{
								// this index is a variable
								llvm::Value* cValue = this->getValue(scope, *it);
								indexValue = cValue;
								
								const llvm::ArrayType* arrayType = llvm::cast<llvm::ArrayType>(type);
								type = arrayType->getElementType();
								numElements = int(arrayType->getNumElements());
							}
							else
							{
								type = llvm::cast<llvm::CompositeType>(type)->getTypeAtIndex(index);			
							}
							globalAccess.access += index;
						}

						// type is now the type of the accessed element

						// create dummy undef pointer where we load from. gets replaced in fixAndGetTransfer
						llvm::Value* undefPointer = llvm::UndefValue::get(llvm::PointerType::get(type, 0));
						cLoadInst = builder.CreateLoad(undefPointer, name);
						
						// get/add array info for global access
						std::pair<Transfer::ArrayIterator, bool> p = transfer.arrayInfos.insert(
							std::make_pair(globalAccess, Transfer::ArrayInfo()));
						Transfer::ArrayInfo& arrayInfo = p.first->second;

						if (p.second)
						{
							// store iterator in the order as they are inserted
							transfer.arrayIterators += p.first;
						}

						arrayInfo.type = llvm::ArrayType::get(type, numElements);
						arrayInfo.loads += Transfer::ArrayInfo::Load(cLoadInst, indexValue);
					}
					else
					{
						// global and accessed value are in the same scope
					
						// copy GetElementPtrInst
						llvm::GetElementPtrInst::op_iterator it = getElementPtrInst->idx_begin();
						llvm::GetElementPtrInst::op_iterator end = getElementPtrInst->idx_end();
						std::vector<llvm::Value*> indices;
						for (; it != end; ++it)
						{
							llvm::Value* value = this->getValue(scope, *it);
							indices += value;
						}
						
						cLoadInst = builder.CreateLoad(builder.CreateInBoundsGEP(cGlobal, indices), name);
					}
				}
				else
				{
					cLoadInst = builder.CreateLoad(this->getGlobal(scope, ptr), name);
				}
				scope->insert(instruction, cLoadInst);
			}
			break;
		case llvm::Instruction::Store:
			{
				llvm::Value* value = this->getValue(scope, instruction->getOperand(0));				
				llvm::Value* global = this->getGlobal(scope, instruction->getOperand(1));
				scope->insert(instruction, builder.CreateStore(value, global));			
			}
			break;
			
		case llvm::Instruction::PHI:
			{
				llvm::PHINode* phiNode = llvm::cast<llvm::PHINode>(instruction);
				int numIncomingValues = phiNode->getNumIncomingValues();

				llvm::PHINode* cPhiNode = builder.CreatePHI(phiNode->getType(), numIncomingValues, name);
				
				if (predecessor == NULL)
				{
					for (int i = 0; i < numIncomingValues; ++i)
					{
						// get basic block
						llvm::BasicBlock* cBasicBlock = scope->basicBlocks[phiNode->getIncomingBlock(i)];
						
						// getValue may need to add instructions to the incoming basic block. therefore set current
						scope->currentBasicBlock = cBasicBlock;
						llvm::Value* value = this->getValue(scope, phiNode->getIncomingValue(i));
						
						cPhiNode->addIncoming(value, cBasicBlock);
					}
					scope->insert(instruction, cPhiNode);
					
					// restore current basic block
					scope->currentBasicBlock = cPhiNode->getParent();
				}
				else
				{
					// static predecessor is known (e.g. after unconditional branch): can evaluate phi node
					for (int i = 0; i < numIncomingValues; ++i)
					{
						if (phiNode->getIncomingBlock(i) == predecessor)
						{
							llvm::Value* value = this->getValue(scope, phiNode->getIncomingValue(i));
							scope->insert(instruction, value);
							break;
						}
					}
				}
			}
			break;

		case llvm::Instruction::ICmp:
			{
				llvm::ICmpInst* iCmpInst = llvm::cast<llvm::ICmpInst>(instruction);
				llvm::Value* left = this->getValue(scope, instruction->getOperand(0));
				llvm::Value* right = this->getValue(scope, instruction->getOperand(1));

				llvm::Value* value = builder.CreateICmp(iCmpInst->getPredicate(), left, right, name);
				scope->insert(instruction, value);
			}
			break;
		case llvm::Instruction::FCmp:
			{
				llvm::FCmpInst* fCmpInst = llvm::cast<llvm::FCmpInst>(instruction);
				llvm::Value* left = this->getValue(scope, instruction->getOperand(0));
				llvm::Value* right = this->getValue(scope, instruction->getOperand(1));

				llvm::Value* value = builder.CreateFCmp(fCmpInst->getPredicate(), left, right, name);
				scope->insert(instruction, value);
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
				llvm::Value* condition = this->getValue(scope, instruction->getOperand(0));
				llvm::Value* trueCase = this->getValue(scope, instruction->getOperand(1));
				llvm::Value* falseCase = this->getValue(scope, instruction->getOperand(2));

				llvm::Value* value = builder.CreateSelect(condition, trueCase, falseCase, name);
				scope->insert(instruction, value);
			}
			break;		

		case llvm::Instruction::InsertElement:
			// insert operand1 into operand0 at index operand2
			{
				llvm::Value* vector = this->getValue(scope, instruction->getOperand(0));
				llvm::Value* scalar = this->getValue(scope, instruction->getOperand(1));
				llvm::Value* index = this->getValue(scope, instruction->getOperand(2));

				llvm::Value* value = builder.CreateInsertElement(vector, scalar, index, name);
				scope->insert(instruction, value);
			}
			break;				
		case llvm::Instruction::ExtractElement:
			// extract the element of operand0 at index operand1
			{
				llvm::Value* vector = this->getValue(scope, instruction->getOperand(0));
				llvm::Value* index = this->getValue(scope, instruction->getOperand(1));

				llvm::Value* value = builder.CreateExtractElement(vector, index, name);
				scope->insert(instruction, value);
			}
			break;
		case llvm::Instruction::ShuffleVector:
			// shuffle vector
			{
				llvm::Value* vector1 = this->getValue(scope, instruction->getOperand(0));
				llvm::Value* vector2 = this->getValue(scope, instruction->getOperand(1));
				llvm::Value* indices = this->getValue(scope, instruction->getOperand(2));

				llvm::Value* value = builder.CreateShuffleVector(vector1, vector2, indices, name);
				scope->insert(instruction, value);
			}
			break;

		case llvm::Instruction::Call:
			{
				llvm::CallInst* callInst = llvm::cast<llvm::CallInst>(instruction);
		
				llvm::Function* function = callInst->getCalledFunction();
				if (function != NULL)
				{
					llvm::FunctionType* functionType = function->getFunctionType();
					llvm::StringRef functionName = function->getName();
					llvm::Function* cFunction = llvm::cast<llvm::Function>(scope->module->getOrInsertFunction(functionName, functionType));
					cFunction->setAttributes(function->getAttributes());
					
					int numOperands = callInst->getNumArgOperands();
					llvm::SmallVector<llvm::Value*, 4> args;
					for (int i = 0; i < numOperands; ++i)
					{
						args.push_back(this->getValue(scope, callInst->getArgOperand(i)));
					}
					
					llvm::Value* value = builder.CreateCall(cFunction, args, name);
					scope->insert(instruction, value);
				}
				else
				{
					// error: indirect call not supported
				}
			}
			break;	

		default:
			// error: instruction not supported
			break;
		}
	}	

	// error: instruction not supported
}

llvm::BasicBlock* CopyWalker::copyBasicBlock(Scope* scope, llvm::BasicBlock* basicBlock)
{
	llvm::BasicBlock*& bb = scope->basicBlocks[basicBlock];
	if (bb == NULL)
	{
		// create basic block
		bb = llvm::BasicBlock::Create(basicBlock->getContext(), basicBlock->getName(), scope->currentFunction);
	}
	else
	{
		// move to end of function
		llvm::Function* function = bb->getParent();
		bb->removeFromParent();
		function->getBasicBlockList().push_back(bb);
	}
	return bb;
}

llvm::Value* CopyWalker::getValue(Scope* scope, llvm::Value* value)
{
	// constants can be reused directly
	if (llvm::isa<llvm::Constant>(value))
		return value;
	
	// get scope of value
	Scope* srcScope = this->getScope(value);
	
	if (srcScope == NULL)
	{
		return llvm::UndefValue::get(value->getType());
	}
	
	// check if this is an instruction
	if (llvm::Instruction* instruction = llvm::dyn_cast<llvm::Instruction>(value))
	{
		// check if value comes from different scope
		if (srcScope != scope)
		{
			ScopePair scopePair(srcScope, scope);
			
			Transfer& transfer = this->transfers[scopePair];
			
			// get/add value info for value
			std::pair<Transfer::ValueIterator, bool> p = transfer.valueInfos.insert(
				std::make_pair(value, Transfer::ValueInfo()));
			Transfer::ValueInfo& valueInfo = p.first->second;
			
			// check if value is encountered the first time
			if (p.second)
			{
				// store iterator in the order as they are inserted
				transfer.valueIterators += p.first;
				
				// get type of instruction
				llvm::Type* type = instruction->getType();
				
				// true if vector value is transferred as scalar
				bool scalarize = getPrintType(instruction) == PRINT_VECTOR_SCALAR;
				
				// we load from an undef pointer that serves as a dummy. gets replaced in fixAndGetTransfer
				llvm::Type* loadType = scalarize ? llvm::cast<llvm::VectorType>(type)->getElementType() : type;
				llvm::Value* undefPointer = llvm::UndefValue::get(llvm::PointerType::get(loadType, 0));
				
				// create instruction builder
				llvm::BasicBlock* bb = scope->currentBasicBlock;
				llvm::IRBuilder<> builder(bb);
					
				// insert load before terminator (branch, ret)
				if (llvm::Instruction* terminator = bb->getTerminator())
					builder.SetInsertPoint(bb, terminator);

				// create load inst
				llvm::LoadInst* loadInst = builder.CreateLoad(undefPointer);
				
				// vectorize again if vector value gets stored as scalar
				llvm::Value* vectorized = loadInst;
				if (scalarize)
				{
					llvm::Value* undefVector = llvm::UndefValue::get(type);
					llvm::Type* intVectorType = llvm::VectorType::get(builder.getInt32Ty(), llvm::cast<llvm::VectorType>(type)->getNumElements());
					vectorized = builder.CreateInsertElement(undefVector, vectorized, builder.getInt32(0));
					vectorized = builder.CreateShuffleVector(vectorized, undefVector, llvm::ConstantAggregateZero::get(intVectorType));
				}
				valueInfo.loadInst = loadInst;
				valueInfo.vectorized = vectorized;
			}
			return valueInfo.vectorized;
		}
		else
		{
			// instruction is in same scope: lookup
			ValueIterator it = scope->values.find(instruction);
			if (it != scope->values.end())
				return it->second;
		}
	}

	// error: should never reach this point
	return NULL;
}

llvm::Value* CopyWalker::getGlobal(Scope* scope, llvm::Value* value)
{
	if (llvm::ConstantExpr* constantExpr = llvm::dyn_cast<llvm::ConstantExpr>(value))
	{
		// value is a constant getelementptr instruction
		if (constantExpr->isGEPWithNoNotionalOverIndexing())
		{
			// the global variable is the first argument of getElementPointer
			llvm::Value* cGlobal = this->getGlobal(scope, constantExpr->getOperand(0));

			int numOperands = constantExpr->getNumOperands();
			std::vector<llvm::Value*> indices;
			for (int i = 1; i < numOperands; ++i)
				indices += constantExpr->getOperand(i);

			return llvm::ConstantExpr::getGetElementPtr(llvm::cast<llvm::Constant>(cGlobal), indices);
		}
	}

	// value is a global
	llvm::GlobalVariable* global = llvm::cast<llvm::GlobalVariable>(value);
	llvm::StringRef globalName = global->getName();
	llvm::Type* globalType = llvm::dyn_cast<llvm::PointerType>(global->getType())->getElementType();
	
	// get or insert into module of this scope
	llvm::GlobalVariable* cGlobal = llvm::cast<llvm::GlobalVariable>(scope->module->getOrInsertGlobal(globalName, globalType));

	// copy attributes
	cGlobal->copyAttributesFrom(global);
	cGlobal->setConstant(global->isConstant());
	cGlobal->setLinkage(global->getLinkage());
	if (global->hasInitializer())
		cGlobal->setInitializer(global->getInitializer());

	// add mapping from global to copied global
	scope->insert(global, cGlobal);

	return cGlobal;
}

CopyWalker::Scope* CopyWalker::getScope(llvm::Value* value)
{
	const ValueInfo& valueInfo = this->valueInfos[value];

	// if no flag is set or invalid scope then the value is dead
	// e.g. reading from an output variable yields invalid type
	if (valueInfo.flags == 0 || valueInfo.scope == -1)
		return NULL;

	Scope* scope = &this->scopes[valueInfo.scope];
	return scope;
}


void CopyWalker::setScope(llvm::Function& function, const std::string& globalName, int scope, int flags)
{
	llvm::GlobalValue* global = function.getParent()->getNamedValue(globalName);
	if (global != NULL)
		this->valueInfos[global] = ValueInfo(scope, flags);
}

std::pair<llvm::Value*, int> CopyWalker::classifyInstructions(llvm::Function& function,
	const std::string& globalName, int outputScope, int flags)
{
	std::pair<llvm::Value*, int> result = std::make_pair((llvm::Value*)NULL, 0);
	
	// get global output variable (e.g. "output") or function (e.g. "discard") where instruction classification starts
	llvm::GlobalValue* global = function.getParent()->getNamedValue(globalName);
	if (global != NULL)
	{
		// iterate over uses of the global value
		llvm::Value::use_iterator it = global->use_begin(), end = global->use_end();
		for (; it != end; ++it)
		{
			llvm::Value* value = *it;
			if (llvm::StoreInst* storeInst = llvm::dyn_cast<llvm::StoreInst>(value))
			{
				// is a store instruction

				// check if instruction belongs to this function
				if (storeInst->getParent()->getParent() == &function)
				{
					llvm::Value* storeValue = storeInst->getOperand(0);
					int scope = this->classifyValue(storeValue, flags);
				
					// set output scope
					this->valueInfos[storeInst] = ValueInfo(outputScope, flags);
					
					// return value that is stored into the global (for alpha mode determination)
					result = std::make_pair(storeValue, scope);
				}
			}
			else if (llvm::ConstantExpr* constantExpr = llvm::dyn_cast<llvm::ConstantExpr>(value))
			{
				// is a constant expression
				if (constantExpr->isGEPWithNoNotionalOverIndexing())
				{
					// is constant array access (e.g. output[0])
					llvm::Value::use_iterator it = constantExpr->use_begin(), end = constantExpr->use_end();
					for (; it != end; ++it)
					{
						llvm::Value* value = *it;

						// check for store instruction
						if (llvm::StoreInst* storeInst = llvm::dyn_cast<llvm::StoreInst>(value))
						{
							// check if instruction belongs to this function
							if (storeInst->getParent()->getParent() == &function)
							{
								llvm::Value* storeValue = storeInst->getOperand(0);
								this->classifyValue(storeValue, flags);
				
								// set output scope
								this->valueInfos[storeInst] = ValueInfo(outputScope, flags);							
							}
						}
					}
				}
			}
			else if (llvm::CallInst* callInst = llvm::dyn_cast<llvm::CallInst>(value))
			{
				// is a call instruction (e.g. discard())
				
				// check if instruction belongs to this function
				if (callInst->getParent()->getParent() == &function)
				{
					// get containing basic block
					llvm::BasicBlock* bb = callInst->getParent();

					this->classifyBasicBlock(bb, flags, outputScope);

					// set output scope
					this->valueInfos[callInst] = ValueInfo(outputScope, flags);
				}				
			}
		}
	}
	return result;
}

int CopyWalker::classifyInstruction(llvm::Instruction* instruction, int flags)
{
	ValueInfo& valueInfo = this->valueInfos[instruction];
	
	// check if instruction already has all given flags
	if ((~valueInfo.flags & flags) != 0)
	{
		// no: accumulate flags
		valueInfo.flags |= flags;
		
		switch (instruction->getOpcode())
		{
		case llvm::Instruction::Load:
			{
				// get pointer that we load from
				llvm::Value* global = instruction->getOperand(0);
				
				int scope = -1;
				
				// analyze the pointer that we load from
				if (llvm::ConstantExpr* constantExpr = llvm::dyn_cast<llvm::ConstantExpr>(global))
				{
					if (constantExpr->isGEPWithNoNotionalOverIndexing())
					{
						// this is a member selection e.g. "material.color"
						global = constantExpr->getOperand(0);
					}
				}
				else if (llvm::GetElementPtrInst* getElementPtrInst = llvm::dyn_cast<llvm::GetElementPtrInst>(global))
				{
					// this is a array indexing e.g. "x[i]"
					global = getElementPtrInst->getPointerOperand();
					scope = this->classifyValue(global, flags);
									
					// classify indices
					llvm::GetElementPtrInst::op_iterator it = getElementPtrInst->idx_begin();
					llvm::GetElementPtrInst::op_iterator end = getElementPtrInst->idx_end();
					for (; it != end; ++it)
					{
						scope = max(scope, this->classifyValue(*it, flags));
					}
				}

				// debug: get name of global variable
				#ifndef NDEBUG
					std::string name = global->getNameStr();
				#endif
				
				ValueInfo& globalInfo = this->valueInfos[global];
				
				// get scope from global
				scope = max(scope, globalInfo.scope);
				
				// combine flags and write to both infos (e.g. input flag goes from global to load)
				valueInfo.flags = globalInfo.flags = valueInfo.flags | globalInfo.flags;
				
				this->setValueScope(valueInfo, scope);
			}
			break;

		case llvm::Instruction::PHI:
			{
//dNotify(instruction->getNameStr());
				llvm::PHINode* phiNode = llvm::cast<llvm::PHINode>(instruction);
				
				// determine highest scope of incoming values and basic blocks
				int numIncomingValues = phiNode->getNumIncomingValues();
				int scope = -1;
				for (int i = 0; i < numIncomingValues; ++i)
				{
					scope = max(scope, this->classifyValue(phiNode->getIncomingValue(i), flags));
					scope = max(scope, this->classifyBasicBlock(phiNode->getIncomingBlock(i), flags));
				}				
				this->setValueScope(valueInfo, scope);
			}
			break;
			
		case llvm::Instruction::Call:
			{
				llvm::CallInst* callInst = llvm::cast<llvm::CallInst>(instruction);
		
				llvm::Function* function = callInst->getCalledFunction();
				if (function != NULL)
				{
					// get demangled function name
					std::string name = function->getName();
					name = NameDemangler::demangle(name);

					int scope = this->classifyFunction(callInst, name, flags);
						
					this->setValueScope(valueInfo, scope);
				}
			}
			break;
						
		default:
			{
				int numOperands = instruction->getNumOperands();
				int scope = -1;
				for (int i = 0; i < numOperands; ++i)
				{
					llvm::Value* value = instruction->getOperand(i);
					scope = max(scope, this->classifyValue(value, flags));
				}
				this->setValueScope(valueInfo, scope);
			}
		}
		
		// check if the containing basic block is entered conditionally
		llvm::BasicBlock* basicBlock = instruction->getParent();
		llvm::BasicBlock* predecessor = basicBlock->getUniquePredecessor();
		if (predecessor != NULL)
		{
			// classify the branch instruction
			classifyInstruction(predecessor->getTerminator(), flags);
		}
	}
		
	// return scope
	return valueInfo.scope;
}

int CopyWalker::classifyBasicBlock(llvm::BasicBlock* bb, int flags, int forceScope)
{
	// scope of basicblock is set explicitly e.g. to force all branches that lead to
	// a call to discard() to execute in pixel scope
	
	ValueInfo& valueInfo = this->valueInfos[bb];
	
	int scope = std::max(valueInfo.scope, forceScope);
	
	// check if basic block already has all given flags
	if (valueInfo.scope < forceScope || (~valueInfo.flags & flags) != 0)
	{
		// no: accumulate flags
		valueInfo.flags |= flags;

		// check if basic block is terminated by a branch instruction
		llvm::TerminatorInst* terminatorInst = bb->getTerminator();		
		if (llvm::BranchInst* branchInst = llvm::dyn_cast<llvm::BranchInst>(terminatorInst))
		{
			if (branchInst->getNumSuccessors() == 2)
			{
				// conditional branch: classify condition
				llvm::Value* condition = branchInst->getOperand(0);
				scope = max(scope, this->classifyValue(condition, flags));
			}
		}

		// stop if basic block is a post dominator of entry block
		//if (this->postDominates(bb, this->entryBlock))
		//	return;

		// classify predecessors
		for (llvm::pred_iterator it = llvm::pred_begin(bb), end = llvm::pred_end(bb); it != end; ++it)
		{
			llvm::BasicBlock* pred = *it;
			scope = max(scope, this->classifyBasicBlock(pred, flags, forceScope));
		}

	}
	return valueInfo.scope = scope;
}

int CopyWalker::classifyFunction(llvm::CallInst* callInst, const std::string& name, int flags)
{
	int scope = 0;

	// classify arguments
	int numOperands = callInst->getNumArgOperands();
	for (int i = 0; i < numOperands; ++i)
	{
		llvm::Value* arg = callInst->getArgOperand(i);
		scope = max(scope, this->classifyValue(arg, flags));
	}

	return scope;
}

void CopyWalker::setValueScope(ValueInfo& valueInfo, int scope)
{
	valueInfo.scope = scope;
}


void CopyWalker::fixAndGetTransfer(int srcScopeIndex, int dstScopeIndex, std::vector<ShaderType>& shaderTypes,
	StringRef name, bool getObjectTypes)
{
	ScopePair scopePair = ScopePair(&this->scopes[srcScopeIndex], &this->scopes[dstScopeIndex]);

	// get transfer values and arrays for given scope pair
	Transfer& transfer = this->transfers[scopePair];
	
	Scope* srcScope = scopePair.first;
	Scope* dstScope = scopePair.second;

	// collect the types
	std::vector<llvm::Type*> types;
	foreach (Transfer::ValueIterator it, transfer.valueIterators)
	{
		// get load instruction and type
		llvm::Instruction* loadInst = it->second.loadInst;
		llvm::Type* type = loadInst->getType();

		// get the type
		ShaderType shaderType = getShaderType(type);
		if ((shaderType.type == ShaderType::TEXTURE) == getObjectTypes)
		{
			types += type;
			shaderTypes += shaderType;
		}
	}
	foreach (Transfer::ArrayIterator it, transfer.arrayIterators)
	{
		// get the array type
		llvm::Type* type = it->second.type;
	
		ShaderType shaderType = getShaderType(type);
		if ((shaderType.type == ShaderType::TEXTURE) == getObjectTypes)
		{
			types += type;
			shaderTypes += shaderType;
		}
	}
	
	if (types.empty())
		return;
	
	// build struct type from the list of types of the transfer variables
	llvm::Type* globalType = llvm::StructType::get(types[0]->getContext(), types, true);
	
	// add global to source and destination scope
	llvm::Constant* srcGlobal = srcScope->module->getOrInsertGlobal(llvm::StringRef(name.data(), name.length()), globalType);
	llvm::Constant* dstGlobal = dstScope->module->getOrInsertGlobal(llvm::StringRef(name.data(), name.length()), globalType);

	// add store to source scope and patch load of destination scope
	int index = 0;
	foreach (Transfer::ValueIterator it, transfer.valueIterators)
	{
		llvm::Value* value = it->first;
		const Transfer::ValueInfo& valueInfo = it->second;

		// get load instruction and type
		llvm::Instruction* loadInst = valueInfo.loadInst;
		llvm::Type* type = loadInst->getType();

		// get the type
		ShaderType shaderType = getShaderType(type);
		if ((shaderType.type == ShaderType::TEXTURE) == getObjectTypes)
		{
			// store value to global in source scope
			{
				// get value in source scope
				ValueIterator it = srcScope->values.find(value);
				if (it == srcScope->values.end())
				{
					// error: value not found
				}
				else
				{
					llvm::Value* srcValue = it->second;
					
					llvm::BasicBlock* bb = llvm::cast<llvm::Instruction>(srcValue)->getParent();
					llvm::IRBuilder<> builder(bb);
					
					// insert before terminator (branch, ret)
					if (llvm::Instruction* terminator = bb->getTerminator())
						builder.SetInsertPoint(bb, terminator);
					
					// scalarize
					if (loadInst != valueInfo.vectorized)
						srcValue = builder.CreateExtractElement(srcValue, builder.getInt32(0));

					// get element pointer from global transfer struct and store value into it
					llvm::Value* elementPointer = builder.CreateConstGEP2_32(srcGlobal, 0, index); 
					builder.CreateStore(srcValue, elementPointer);
				}
			}
			
			// load value from global in destination scope
			{
				// instruction builder, insert before load instruction
				llvm::IRBuilder<> builder(loadInst->getParent(), loadInst);

				// get element pointer
				llvm::Value* elementPointer = builder.CreateConstGEP2_32(dstGlobal, 0, index);
				
				// replace undef pointer with element pointer
				loadInst->setOperand(0, elementPointer);
			}
			
			++index;
		}
	}
	foreach (Transfer::ArrayIterator it, transfer.arrayIterators)
	{
		const Transfer::GlobalAccess& globalAccess = it->first;
		const Transfer::ArrayInfo& arrayInfo = it->second;
		
		// get the array type
		llvm::Type* type = arrayInfo.type;
		
		ShaderType shaderType = getShaderType(type);
		if ((shaderType.type == ShaderType::TEXTURE) == getObjectTypes)
		{
			llvm::BasicBlock* bb = &srcScope->currentFunction->getEntryBlock();
			llvm::IRBuilder<> builder(bb, bb->begin());
			int numIndices = int(globalAccess.access.size());
			std::vector<llvm::Value*> indices(3);
			indices[0] = builder.getInt32(0);
			indices[1] = builder.getInt32(index);
			
			
			// store array to global in source scope
			{
				// get global in source scope
				ValueIterator it = srcScope->values.find(globalAccess.global);
				if (it == srcScope->values.end())
				{
					// error: value not found
				}
				else
				{
					llvm::Value* cGlobal = it->second;
					
					// copy all array elements, insert instructions at beginning of function
					std::vector<llvm::Value*> loadIndices(numIndices + 1);
					loadIndices[0] = indices[0];
					for (int elementIndex = 0; elementIndex < shaderType.numElements; ++elementIndex)
					{
						for (int i = 0; i < numIndices; ++i)
						{
							int index = globalAccess.access[i];
							llvm::Value* indexValue = builder.getInt32(index == -1 ? elementIndex : index);
							loadIndices[i + 1] = indexValue;
							if (index == -1)
								indices[2] = indexValue;
						}
						llvm::Value* loadPtr = builder.CreateInBoundsGEP(cGlobal, loadIndices);
						llvm::Value* value = builder.CreateLoad(loadPtr);
						llvm::Value* storePtr = builder.CreateInBoundsGEP(srcGlobal, indices);
						builder.CreateStore(value, storePtr);
					}
				}
			}
			
			// load value from global in destination scope
			{
				foreach (const Transfer::ArrayInfo::Load load, arrayInfo.loads)
				{
					// get the load instruction
					llvm::Instruction* loadInst = load.inst;
					
					// set array index
					indices[2] = load.index;

					llvm::IRBuilder<> builder(loadInst->getParent(), loadInst);

					// get element pointer
					llvm::Value* elementPointer = builder.CreateInBoundsGEP(dstGlobal, indices);

					// replace undef pointer with element pointer
					loadInst->setOperand(0, elementPointer);				
				}
			}
			
			++index;
		}	
	}	
}

} // namespace digi
