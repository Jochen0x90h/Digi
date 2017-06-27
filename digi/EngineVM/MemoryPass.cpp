#include <llvm/Instructions.h>
#include <llvm/IntrinsicInst.h>
#include <llvm/Module.h>
#include <llvm/Analysis/PostDominators.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/Target/TargetData.h>

#include <digi/Utility/MapUtility.h>
#include <digi/Utility/MemoryPool.h>
#include <digi/Utility/VectorUtility.h>
#include <digi/Utility/foreach.h>
#include <digi/System/Log.h>
#include <digi/CodeGenerator/NameGenerator.h>

#include "MemoryPass.h"


namespace digi {

MemoryPass::~MemoryPass()
{
}

void MemoryPass::getAnalysisUsage(llvm::AnalysisUsage& analysisUsage) const
{
	// tell llvm that we don't modify anything
	//analysisUsage.setPreservesAll();

	// tell llvm that we don't add or remove basic blocks or modify terminator instructions
	// module is modified to store globals depth and sort
	analysisUsage.setPreservesCFG();
	
	// tell llvm wich other passes we need
	analysisUsage.addRequired<llvm::TargetData>();
}

namespace
{
	typedef MemoryPool<void*, 1024> Pool;

	int getConstantIndex(llvm::Value* value)
	{
		if (llvm::ConstantInt* constantInt = llvm::dyn_cast<llvm::ConstantInt>(value))
		{
			return int(constantInt->getZExtValue());
		}		
		return -1;
	}
	
	llvm::Value* createUInt32(llvm::LLVMContext& c, int value)
	{
		return llvm::ConstantInt::get(llvm::IntegerType::get(c, 32), value);
	}
	

	// traverse type and allocate pointers for members
	void* allocType(Pool& pool, llvm::Type* type, NameGenerator& nameGenerator, llvm::IRBuilder<>& builder)
	{
			if (const llvm::StructType* structType = llvm::dyn_cast<llvm::StructType>(type))
			{
				// struct type
				int numElements = structType->getNumElements();
				void** members = pool.alloc(numElements);
				for (int i = 0; i < numElements; ++i)
				{
					members[i] = allocType(pool, structType->getElementType(i), nameGenerator, builder);
				}
				return (void*)members;
			}
			else if (const llvm::ArrayType* arrayType = llvm::dyn_cast<llvm::ArrayType>(type))
			{
				// array type
				int numElements = int(arrayType->getNumElements());
				void** members = pool.alloc(numElements);
				llvm::Type* elementType = arrayType->getElementType();
				for (int i = 0; i < numElements; ++i)
				{
					members[i] = allocType(pool, elementType, nameGenerator, builder);
				}
				return (void*)members;
			}
			
			// basic or vector type: create alloca
			return (void*)builder.CreateAlloca(type, NULL, nameGenerator.get());			
	}
	
	void replacePointer(llvm::BasicBlock* basicBlock, llvm::Instruction* instruction,
		int index, std::map<llvm::Value*, void*>& pointers, NameGenerator& nameGenerator)
	{
		llvm::Value* ptr = instruction->getOperand(index);
		
		// the loads we want to replace load using constant getelementptr
		if (llvm::ConstantExpr* constantExpr = llvm::dyn_cast<llvm::ConstantExpr>(ptr))
		{
			// ptr is a constant getelementptr instruction
			if (constantExpr->isGEPWithNoNotionalOverIndexing())
			{
				// get root pointer
				llvm::Value* root = constantExpr->getOperand(0);
			
				// lookup root in pointers map (is e.g. alloca)
				void* p = getValue(pointers, root);
				if (p != NULL)
				{
					// this is a known pointer that needs to be replaced
					
					// apply indices
					int numOperands = constantExpr->getNumOperands();
					for (int i = 2; i < numOperands; ++i)
					{
						int index = getConstantIndex(constantExpr->getOperand(i));
						p = ((void**)p)[index];
					}
					
					// now p points to the pointer that we want to load from instead
					llvm::Value* pointer = (llvm::Value*)p;

					// add load from pointer
					llvm::IRBuilder<> builder(basicBlock, instruction);	
					instruction->setOperand(index, builder.CreateLoad(pointer, nameGenerator.get()));
				}
			}
			return;
		}

		// lookup pointer in pointers map (is e.g. getElementPtr instruction)
		void* p = getValue(pointers, ptr);
		if (p != NULL)
		{
			// this is a known pointer that needs to be replaced
			llvm::Value* pointer = (llvm::Value*)p;
			instruction->setOperand(index, pointer);
		}
	}

	llvm::Value* getBitCastOperand(llvm::Value* value)
	{
		if (llvm::BitCastInst* bitCastInst = llvm::dyn_cast<llvm::BitCastInst>(value))
		{
			return bitCastInst->getOperand(0);
		}
		if (llvm::ConstantExpr* constantExpr = llvm::dyn_cast<llvm::ConstantExpr>(value))
		{
			return constantExpr->getOperand(0);
		}
		return NULL;
	}

	void eraseBitCast(llvm::Value* value)
	{
		if (llvm::Instruction* instruction = llvm::dyn_cast<llvm::BitCastInst>(value))
		{
			if (!instruction->hasNUsesOrMore(1))
				instruction->eraseFromParent();
		}
	}

	llvm::Value* getRoot(llvm::Value* ptr, std::vector<llvm::Value*>& indices)
	{
		if (llvm::ConstantExpr* constantExpr = llvm::dyn_cast<llvm::ConstantExpr>(ptr))
		{
			// ptr is a constant getelementptr instruction
			if (constantExpr->isGEPWithNoNotionalOverIndexing())
			{
				// get root
				llvm::Value* root = constantExpr->getOperand(0);

				// get indices (all constant)
				int numOperands = constantExpr->getNumOperands();
				for (int i = 1; i < numOperands; ++i)
					indices += constantExpr->getOperand(i);
				return root;
			}
		}

		// add first zero for generated getElementPtrInst
		indices += createUInt32(ptr->getContext(), 0);
		return ptr;
	}

	struct StackElement
	{
		// index in parent type
		int index;
		
		llvm::Type* type;

		// offset in root type
		int offset;

		StackElement() {}
		StackElement(int index, llvm::Type* type, int offset)
			: index(index), type(type), offset(offset) {}
	};

	int getAddIndex(std::vector<llvm::Value*>& indices, size_t ii)
	{
		if (ii < indices.size())
			return getConstantIndex(indices[ii]);
		indices += createUInt32(indices.front()->getContext(), 0);
		return 0;
	}

	int applyIndices(llvm::TargetData& targetData, size_t ii, std::vector<llvm::Value*>& indices, std::vector<StackElement>& stack)
	{
		// get root type
		llvm::Type* type = stack[ii - 1].type;
		int offset = stack[ii - 1].offset;
		
		// step into indices
		while (true)
		{		
			if (llvm::StructType* structType = llvm::dyn_cast<llvm::StructType>(type))
			{
				// struct type

				// get index. append 0 if no index present
				int index = getAddIndex(indices, ii);

				type = structType->getElementType(index);
				
				const llvm::StructLayout* layout = targetData.getStructLayout(structType);
				offset += int(layout->getElementOffsetInBits(index) / 8);
				
				stack += StackElement(index, type, offset);
			}
			else if (llvm::ArrayType* arrayType = llvm::dyn_cast<llvm::ArrayType>(type))
			{
				// array type
				// get index. append 0 if no index present
				int index = getAddIndex(indices, ii);

				type = arrayType->getElementType();
				
				offset += index * int(targetData.getTypeSizeInBits(type) / 8);

				stack += StackElement(index, type, offset);
			}
			else
			{
				// reached basic type
				break;
			}
			++ii;
		}
		
		return stack.back().offset;
	}

	int getNextElement(llvm::TargetData& targetData, std::vector<llvm::Value*>& indices, std::vector<StackElement>& stack)
	{
		size_t ii;
		for (ii = stack.size() - 1; ii >= 1; --ii)
		{
			llvm::Type* type = stack[ii - 1].type;
			int offset = stack[ii - 1].offset;
			int index = stack[ii].index;
			
			++index;
			
			if (llvm::StructType* structType = llvm::dyn_cast<llvm::StructType>(type))
			{
				// struct type
				if (index == structType->getNumElements())
					continue;
							
				// set index
				indices[ii] = createUInt32(indices.front()->getContext(), index);

				type = structType->getElementType(index);

				const llvm::StructLayout* layout = targetData.getStructLayout(structType);
				offset += int(layout->getElementOffsetInBits(index) / 8);
				
				stack[ii] = StackElement(index, type, offset);
			}
			else if (llvm::ArrayType* arrayType = llvm::dyn_cast<llvm::ArrayType>(type))
			{
				// array type
				if (index == arrayType->getNumElements())
					continue;
				
				// set index
				indices[ii] = createUInt32(indices.front()->getContext(), index);

				type = arrayType->getElementType();

				offset += index * int(targetData.getTypeSizeInBits(type) / 8);

				stack[ii] = StackElement(index, type, offset);
			}

			break;
		}
		
		// check if end of struct
		if (ii == 0)
			return std::numeric_limits<int>::max();
		
		++ii;
		indices.resize(ii);
		stack.resize(ii);
		
		return applyIndices(targetData, ii, indices, stack);
	}
	
	llvm::Value* getElementPointer(llvm::Value* root, std::vector<llvm::Value*>& indices, std::vector<StackElement>& stack,
		std::map<llvm::Value*, void*>& pointers, NameGenerator& nameGenerator, llvm::IRBuilder<>& builder)
	{
		// check if this is a pointer to replace
		void* p = getValue(pointers, root);
		if (p != NULL)
		{
			// apply indices
			int numIndices = int(stack.size());
			for (int i = 1; i < numIndices; ++i)
			{
				int index = stack[i].index;
				p = ((void**)p)[index];
			}
			return (llvm::Value*)p;		
		}
		
		llvm::Value* elementPtr = root;
		if (indices.size() > 1)
			elementPtr = builder.CreateInBoundsGEP(root, indices, nameGenerator.get());
		return elementPtr;
	}
	
} // anonymous namespace

bool MemoryPass::runOnFunction(llvm::Function& function)
{
	// data for alloca simplification
	std::map<llvm::Value*, void*> pointers;
	std::vector<llvm::Instruction*> deleteInstructions;
	Pool pool;
	
	// data for memcpy removal
	llvm::TargetData& targetData = this->getAnalysis<llvm::TargetData>();

	NameGenerator nameGenerator;
	
	// iterate over basic blocks
	for (llvm::Function::iterator bbIt = function.begin(); bbIt != function.end(); ++bbIt)
	{
		llvm::BasicBlock* basicBlock = bbIt;
		
		// iterate over instructions
		llvm::BasicBlock::iterator it = basicBlock->begin();
		while (it != basicBlock->end())
		{
			llvm::Instruction* instruction = it;
			
			bool alreadyIncremented = false;
		
			switch (instruction->getOpcode())
			{
			case llvm::Instruction::Alloca:
				{
					if (llvm::AllocaInst* allocaInst = llvm::dyn_cast<llvm::AllocaInst>(instruction))
					{					
						llvm::Type* type = allocaInst->getAllocatedType();
						
						// simplify allocas
						if (type->isStructTy() || type->isArrayTy())
						{
							// alloca of struct or array type
							
							// create allocas for each basic or vector type contained in the struct or array
							llvm::IRBuilder<> builder(basicBlock, allocaInst);	
							void* p = allocType(pool, type, nameGenerator, builder);
							
							// associate with pointer to allocated memory
							pointers[allocaInst] = p;
	
							// schedule instruction for deletion
							deleteInstructions += allocaInst;
						}
						else if (type->isPointerTy())
						{
							// alloca of pointer type: can only be simplifed if pointer is written only once
							// which (should be) the case if no pointers are used in the input code
							
							{
								// iterate over uses to get store value
								llvm::Value* storeValue = NULL;
								llvm::Value::use_iterator it = allocaInst->use_begin(), end = allocaInst->use_end();
								while (it != end)
								{
									llvm::Value* value = *it;
									++it;
									if (llvm::StoreInst* storeInst = llvm::dyn_cast<llvm::StoreInst>(value))
									{
										if (storeValue == NULL)
										{
											storeValue = storeInst->getOperand(0);
										
											// delete store instruction
											storeInst->eraseFromParent();
										}
										else
										{
											dError("second store into local pointer variable '" << allocaInst->getNameStr() << "'");
										}
									}
								}
								
								if (storeValue != NULL)
								{
									it = allocaInst->use_begin();
									while (it != end)
									{
										llvm::Value* value = *it;
										++it;
										if (llvm::LoadInst* loadInst = llvm::dyn_cast<llvm::LoadInst>(value))
										{
											loadInst->replaceAllUsesWith(storeValue);
											
											// delete load instruction
											loadInst->eraseFromParent();
										}
									}
								}
								else
								{
									// error: nothing was stored
									dError("no store into local pointer variable '" << allocaInst->getNameStr() << "'");							
								}
							}
														
							// increment instruction iterator because we delete the instruction now
							//++it;
							//alreadyIncremented = true;
							
							// delete alloca instruction
							//allocaInst->eraseFromParent();
							
							// schedule alloca for deletion because it still may be used by llvm.lifetime.start and llvm.lifetime.end
							deleteInstructions += allocaInst;
						}
					}
				}
				break;
			
			case llvm::Instruction::GetElementPtr:
				{
					// get root pointer (e.g. alloca)
					llvm::Value* root = instruction->getOperand(0);
					
					// lookup in pointers map
					void* p = getValue(pointers, root);
					if (p != NULL)
					{
						// this is a known root pointer that needs to be replaced

						int numOperands = instruction->getNumOperands();
						for (int i = 2; i < numOperands; ++i)
						{
							int index = getConstantIndex(instruction->getOperand(i));
							p = ((void**)p)[index];
						}
						
						pointers[instruction] = p;
						
						// schedule instruction for deletion because it is still used later
						deleteInstructions += instruction;
					}
				}
				break;
			
			case llvm::Instruction::Load:
				replacePointer(basicBlock, instruction, 0, pointers, nameGenerator);
				break;
			case llvm::Instruction::Store:
				replacePointer(basicBlock, instruction, 1, pointers, nameGenerator);
				break;

			case llvm::Instruction::Call:
				{
					if (llvm::MemCpyInst* memCpyInst = llvm::dyn_cast<llvm::MemCpyInst>(instruction))
					{
						// this is a call to memcpy instruction
						
						llvm::Value* dstArg = memCpyInst->getArgOperand(0);
						llvm::Value* srcArg = memCpyInst->getArgOperand(1);
						
						// get destination and source pointers (must be bitcast instructions)
						llvm::Value* dst = getBitCastOperand(dstArg);
						llvm::Value* src = getBitCastOperand(srcArg);					
						
						// get size of memory to copy (must be constant)
						int size = getConstantIndex(memCpyInst->getArgOperand(2));
						if (dst != NULL && src != NULL && size != -1)
						{
							// get roots and indices
							std::vector<llvm::Value*> dstIndices;
							std::vector<llvm::Value*> srcIndices;
							llvm::Value* dstRoot = getRoot(dst, dstIndices);
							llvm::Value* srcRoot = getRoot(src, srcIndices);

							// get root types
							const llvm::PointerType* dstPointerType = llvm::dyn_cast<llvm::PointerType>(dstRoot->getType());
							const llvm::PointerType* srcPointerType = llvm::dyn_cast<llvm::PointerType>(srcRoot->getType());							
							llvm::Type* dstType = dstPointerType->getElementType();
							llvm::Type* srcType = srcPointerType->getElementType();

							// step into types
							std::vector<StackElement> dstStack(1, StackElement(0, dstType, 0));
							std::vector<StackElement> srcStack(1, StackElement(0, srcType, 0));
							int dstOffset = applyIndices(targetData, 1, dstIndices, dstStack);						
							int srcOffset = applyIndices(targetData, 1, srcIndices, srcStack);						
							int dstEndOffset = dstOffset + size;
							int srcEndOffset = srcOffset + size;
							
							// copy elements
							llvm::IRBuilder<> builder(basicBlock, memCpyInst);	
							while (dstOffset < dstEndOffset && srcOffset < srcEndOffset)
							{						
								llvm::Value* srcElementPtr = getElementPointer(srcRoot, srcIndices, srcStack, pointers, nameGenerator, builder);								
								llvm::Value* element = builder.CreateLoad(srcElementPtr, nameGenerator.get());
								llvm::Value* dstElementPtr = getElementPointer(dstRoot, dstIndices, dstStack, pointers, nameGenerator, builder);
								builder.CreateStore(element, dstElementPtr);
													
								dstOffset = getNextElement(targetData, dstIndices, dstStack);
								srcOffset = getNextElement(targetData, srcIndices, srcStack);
							}
							
							// increment instruction iterator because we delete the instruction now
							++it;
							alreadyIncremented = true;

							// delete memcopy and bitcasts
							memCpyInst->eraseFromParent();
							eraseBitCast(dstArg);
							eraseBitCast(srcArg);
						}
					}
					else if (llvm::IntrinsicInst* intrinsicInst = llvm::dyn_cast<llvm::IntrinsicInst>(instruction))
					{
						llvm::Function* function = intrinsicInst->getCalledFunction();
						llvm::StringRef name = function->getName();
						if (name == "llvm.lifetime.start" || name == "llvm.lifetime.end")
						{
							// increment instruction iterator because we delete the instruction now
							++it;
							alreadyIncremented = true;

							// delete llvm.lifetime.start or llvm.lifetime.end and bitcast
							llvm::Value* arg = intrinsicInst->getArgOperand(1);
							intrinsicInst->eraseFromParent();
							eraseBitCast(arg);
						}
					}
				}
				break;
			}
			
			// increment instruction iterator if not yet done
			if (!alreadyIncremented)
				++it;
		}
	}
	
	// erase all allocas and getelementptr instructions
	reverse_foreach (llvm::Instruction* instruction, deleteInstructions)
	{
		instruction->eraseFromParent();
	}
	
	return false;
}

	
char MemoryPass::ID = 0;

static llvm::RegisterPass<MemoryPass> registerShaderPass("memory", "memory optimizations");

} // namespace digi
