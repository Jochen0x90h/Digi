#include <llvm/Instructions.h>
#include <llvm/Module.h>

#include <digi/CodeGenerator/NameMangler.h>

#include "ValueHelper.h"
#include "ReplacePass.h"


namespace digi {


namespace
{
	llvm::Function* getCalledFunction(llvm::CallInst* callInst, llvm::StringRef name)
	{
		if (llvm::Function* function = callInst->getCalledFunction())
		{
			if (function->getName() == name)
				return function;
		}
		return NULL;
	}

	llvm::Instruction* createCall1(llvm::Module* module, llvm::Instruction* instruction,
		StringRef name, llvm::Value* op0)
	{
		llvm::Type* type = op0->getType();
		
		llvm::Function* f = llvm::cast<llvm::Function>(module->getOrInsertFunction(
			NameMangler::mangle(name, getVectorInfo(type)),
			type, type, (llvm::Type*)NULL));
		f->addFnAttr(llvm::Attribute::ReadNone);
		
		llvm::Value* args[1] = {op0};
		return llvm::CallInst::Create(f, args, "", instruction);
	}

	void replaceInstructionByCall1(llvm::Module* module, llvm::Instruction* instruction,
		StringRef name, llvm::Value* op0)
	{
		llvm::Instruction* callInst = createCall1(module, instruction, name, op0);
		
		// replace instruction with newly created call instruction
		instruction->replaceAllUsesWith(callInst);
		
		// remove instruction from basic block and delete it
		instruction->eraseFromParent();
	}

	llvm::Instruction* createCall2(llvm::Module* module, llvm::Instruction* instruction,
		StringRef name, llvm::Value* op0, llvm::Value* op1)
	{
		llvm::Type* type = op0->getType();
		llvm::Type* type1 = op1->getType();
		
		llvm::Function* f = llvm::cast<llvm::Function>(module->getOrInsertFunction(
			NameMangler::mangle(name, getVectorInfo(type), getVectorInfo(type1)),
			type, type, type, (llvm::Type*)NULL));
		f->addFnAttr(llvm::Attribute::ReadNone);
		
		llvm::Value* args[2] = {op0, op1};
		return llvm::CallInst::Create(f, args, "", instruction);
	}

	void replaceInstructionByCall2(llvm::Module* module, llvm::Instruction* instruction,
		StringRef name, llvm::Value* op0, llvm::Value* op1)
	{
		llvm::Instruction* callInst = createCall2(module, instruction, name, op0, op1);
		
		// replace instruction with newly created call instruction
		instruction->replaceAllUsesWith(callInst);
		
		// remove instruction from basic block and delete it
		instruction->eraseFromParent();
	}

	inline void eraseIfUnused(llvm::Instruction* instruction)
	{
		if (!instruction->hasNUsesOrMore(1))
			instruction->eraseFromParent();
	}

} // anonymous namespace



ReplacePass::~ReplacePass()
{
}

void ReplacePass::getAnalysisUsage(llvm::AnalysisUsage& analysisUsage) const
{
	// tell llvm that we don't modify anything
	//analysisUsage.setPreservesAll();
	
	// tell llvm that we don't add or remove basic blocks or modify terminator instructions
	analysisUsage.setPreservesCFG();
}

bool ReplacePass::runOnFunction(llvm::Function& function)
{
	llvm::Module* module = function.getParent();
	
	// iterate over basic blocks
	llvm::Function::iterator it = function.begin();
	llvm::Function::iterator end = function.end();
	for (; it != end; ++it)
	{
		llvm::BasicBlock* basicBlock = it;
	
		// iterate over instructions
		llvm::BasicBlock::iterator it = basicBlock->begin();
		while (it != basicBlock->end())
		{
			llvm::Instruction* instruction = it;
			
			// increment here because instruction may be removed
			++it;
			
			switch (instruction->getOpcode())
			{
			case llvm::Instruction::Sub:
			case llvm::Instruction::FSub:
				// detect a - floor(a) = frac(a)
				if (!this->language.isJS())
				{
					if (llvm::CallInst* callInst = llvm::dyn_cast<llvm::CallInst>(instruction->getOperand(1)))
					{
						// if floor(a) is used elsewhere we don't need to replace
						if (callInst->hasOneUse())
						{
							if (llvm::Function* function = getCalledFunction(callInst, "floor"))
							{
								// found call to floor
								llvm::Value* a = instruction->getOperand(0);
								if (a == callInst->getArgOperand(0))
								{
									// can replace by frac									
									replaceInstructionByCall1(module, instruction, "frac", a);
																		
									// erase call to floor (now has zero uses)
									callInst->eraseFromParent();
								}
							}
						}
					}
				}
				break;

			case llvm::Instruction::UDiv:
			case llvm::Instruction::SDiv:
			case llvm::Instruction::FDiv:		
				// detect a / sqrt(b) = a * inversesqrt(b)
				// a / sqrt(b).xxx = a * inversesqrt(b).xxx
				if (this->language.isGLSL() || this->language.isHLSL())
				{
					llvm::BinaryOperator::BinaryOps mulOp = instruction->getOpcode() == llvm::Instruction::FDiv ? llvm::Instruction::FMul : llvm::Instruction::Mul;
				
					llvm::Value* divisor = instruction->getOperand(1);
					
					if (llvm::ShuffleVectorInst* shuffleInst = llvm::dyn_cast<llvm::ShuffleVectorInst>(divisor))
					{
						if (llvm::isa<llvm::ConstantAggregateZero>(shuffleInst->getOperand(2)))
						{
							// is .xx, .xxx or .xxxx

							if (llvm::InsertElementInst* insertInst = llvm::dyn_cast<llvm::InsertElementInst>(shuffleInst->getOperand(0)))
							{
								if (getConstantIndex(insertInst->getOperand(2)) == 0)
								{
									// is conversion from scalar to vector
									
									if (llvm::CallInst* callInst = llvm::dyn_cast<llvm::CallInst>(insertInst->getOperand(1)))
									{
										if (llvm::Function* function = getCalledFunction(callInst, "sqrt"))
										{
											// found call to sqrt, ran replace by rsqrt
											llvm::Value* a = instruction->getOperand(0);
											llvm::Value* b = callInst->getArgOperand(0);

											llvm::Instruction* newCallInst = createCall1(module, instruction, "rsqrt", b);
											llvm::Value* newUndef = llvm::UndefValue::get(divisor->getType());
											llvm::Instruction* newInsertInst = llvm::InsertElementInst::Create(newUndef, newCallInst, insertInst->getOperand(2), "", instruction);
											llvm::Instruction* newShuffleInst = new llvm::ShuffleVectorInst(newInsertInst, newUndef, shuffleInst->getOperand(2), "", instruction);
											llvm::Instruction* newMulInst = llvm::BinaryOperator::Create(mulOp, a, newShuffleInst, "", instruction);

											// replace instruction with newly created mul instruction
											instruction->replaceAllUsesWith(newMulInst);
											instruction->eraseFromParent();
												
											// erase now unused instructions
											eraseIfUnused(shuffleInst);
											eraseIfUnused(insertInst);
											eraseIfUnused(callInst);
										}
									}
								}
							}							
						}
					}
					else
					{
						if (llvm::CallInst* callInst = llvm::dyn_cast<llvm::CallInst>(divisor))
						{
							if (llvm::Function* function = getCalledFunction(callInst, "sqrt"))
							{
								// found call to sqrt, ran replace by rsqrt
								llvm::Value* a = instruction->getOperand(0);
								llvm::Value* b = callInst->getArgOperand(0);

								llvm::Instruction* newCallInst = createCall1(module, instruction, "rsqrt", b);
								llvm::Instruction* newMulInst = llvm::BinaryOperator::Create(mulOp, a, newCallInst, "", instruction);

								// replace instruction with newly created mul instruction
								instruction->replaceAllUsesWith(newMulInst);
								instruction->eraseFromParent();
									
								// erase now unused instructions
								eraseIfUnused(callInst);
							}
						}
					}
				}
				break;
								
			case llvm::Instruction::Select:
				{
					llvm::Value* condition = instruction->getOperand(0);
					llvm::Value* op0 = instruction->getOperand(1);
					llvm::Value* op1 = instruction->getOperand(2);
					
					// detect min(a, b)
					// a < b ? a : b

					// detect max(a, b)
					// a > b ? a : b

					// detect step(edge, x)
					// x < edge ? 0.0 : 1.0
					
					if (llvm::CmpInst* cmpInst = llvm::dyn_cast<llvm::CmpInst>(condition))
					{
						llvm::Value* cmp0 = cmpInst->getOperand(0);
						llvm::Value* cmp1 = cmpInst->getOperand(1);

						bool b1 = op0 == cmp0 && op1 == cmp1;
						bool b2 = op1 == cmp0 && op0 == cmp1;
						if (b1 || b2)
						{
							// min/max
							boost::optional<bool> isMin;
							switch (cmpInst->getPredicate())
							{
							case llvm::CmpInst::ICMP_ULT:
							case llvm::CmpInst::ICMP_SLT:
							case llvm::CmpInst::FCMP_OLT:
							case llvm::CmpInst::FCMP_ULT:
							case llvm::CmpInst::ICMP_ULE:
							case llvm::CmpInst::ICMP_SLE:
							case llvm::CmpInst::FCMP_OLE:
							case llvm::CmpInst::FCMP_ULE:
								isMin = b1;
								break;
							case llvm::CmpInst::ICMP_UGT:
							case llvm::CmpInst::ICMP_SGT:
							case llvm::CmpInst::FCMP_OGT:
							case llvm::CmpInst::FCMP_UGT:
							case llvm::CmpInst::ICMP_UGE:
							case llvm::CmpInst::ICMP_SGE:
							case llvm::CmpInst::FCMP_OGE:
							case llvm::CmpInst::FCMP_UGE:
								isMin = b2;
								break;
							default:
								break;
							}
							
							if (isMin != null)
							{
								const char* name = *isMin ? "min" : "max";
									
								replaceInstructionByCall2(module, instruction, name, op0, op1);
								
								// remove compare instruction if no uses are left
								eraseIfUnused(cmpInst);
							}
						}
						else if (!this->language.isJS())
						{
							// step(edge, x)
							boost::optional<double> op0Value = getConstantDouble(op0);
							boost::optional<double> op1Value = getConstantDouble(op1);
							llvm::CmpInst::Predicate p = cmpInst->getPredicate();

							bool isStep = false;
							if (p == llvm::CmpInst::ICMP_UGT || p == llvm::CmpInst::ICMP_SGT || p == llvm::CmpInst::FCMP_OGT || p == llvm::CmpInst::FCMP_UGT)
							{
								// edge > x ? 0.0 : 1.0
								isStep = op0Value == 0.0 && op1Value != null;
							}
							else if (p == llvm::CmpInst::ICMP_ULT || p == llvm::CmpInst::ICMP_SLT || p == llvm::CmpInst::FCMP_OLT || p == llvm::CmpInst::FCMP_ULT)
							{
								// x < edge ? 0.0 : 1.0 => edge > x ? 0.0 : 1.0
								isStep = op0Value == 0.0 && op1Value != null;
								std::swap(cmp0, cmp1);
							}
							else if (p == llvm::CmpInst::ICMP_UGE || p == llvm::CmpInst::ICMP_SGE || p == llvm::CmpInst::FCMP_OGE || p == llvm::CmpInst::FCMP_UGE)
							{
								// edge >= x ? 1.0 : 0.0 => edge > x ? 0.0 : 1.0
								isStep = op0Value != null && op1Value == 0.0;
								std::swap(op0, op1);
								std::swap(op0Value, op1Value);
							}
							else if (p == llvm::CmpInst::ICMP_ULE || p == llvm::CmpInst::ICMP_SLE || p == llvm::CmpInst::FCMP_OLE || p == llvm::CmpInst::FCMP_ULE)
							{
								// x <= edge ? 1.0 : 0.0 => edge > x ? 0.0 : 1.0
								isStep = op0Value != null && op1Value == 0.0;
								std::swap(cmp0, cmp1);
								std::swap(op0, op1);
								std::swap(op0Value, op1Value);
							}

							if (isStep)
							{
								llvm::Instruction* step = createCall2(module, instruction, "step", cmp0, cmp1);
								
								// check if step is multiplied by constant factor (e.g. edge > x ? 0.0 : 3.0 = step(edge, x) * 3.0)
								if (op1Value != 1.0)
								{
									llvm::Instruction::BinaryOps op = step->getType()->isFloatingPointTy() ? llvm::Instruction::FMul : llvm::Instruction::Mul;
									step = llvm::BinaryOperator::Create(op, step, op1, llvm::Twine(), instruction);
								}
							
								// replace instruction with newly created step function
								instruction->replaceAllUsesWith(step);
								
								// remove select instruction from basic block and delete it
								instruction->eraseFromParent();
								
								// remove compare instruction if no uses are left
								eraseIfUnused(cmpInst);
							}
						}
					}
				}
				break;
							
			default:
				break;
			}
		}
	}
	
	return false;
}

char ReplacePass::ID = 0;

static llvm::RegisterPass<ReplacePass> registerPrintPass("digi-replace", "replace code");


} // namespace digi
