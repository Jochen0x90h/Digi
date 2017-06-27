#include <iostream>
#include <fstream>

#include <llvm/Instructions.h>

#include <digi/System/Log.h>

#include "InstructionWalker.h"


namespace digi {


void InstructionWalker::doBasicBlock(llvm::BasicBlock* predecessor, llvm::BasicBlock* basicBlock, llvm::BasicBlock* stopBlock)
{
	// set static predecessor (NULL if not known at compile time)
	this->predecessors[basicBlock] = predecessor;

	// check if we are at a basic block where the control flow joins
	if (basicBlock == stopBlock)
	{
		// derived class can e.g. assign values to phi variables 
		this->doJoin(predecessor, basicBlock);
	}
	else
	{
		// iterate over instructions
		llvm::BasicBlock::iterator it  = basicBlock->begin();
		llvm::BasicBlock::iterator end = basicBlock->end();
		for (; it != end; ++it)
		{
			llvm::Instruction* instruction = it;
		
			// check if this is a terminator instruction (e.g. branch or ret)
			if (llvm::TerminatorInst* terminatorInst = llvm::dyn_cast<llvm::TerminatorInst>(instruction))
			{
				if (llvm::BranchInst* branchInst = llvm::dyn_cast<llvm::BranchInst>(instruction))
				{
					if (branchInst->isUnconditional())
					{
						// unconditional branch
						llvm::BasicBlock* successor = branchInst->getSuccessor(0);
						this->doBasicBlock(basicBlock, successor, stopBlock);
					}
					else if (branchInst->getNumSuccessors() == 2)
					{
						// conditional branch

						// get the condition
						llvm::Value* condition = branchInst->getOperand(0);
						
						// get the two branches
						llvm::BasicBlock* trueBranch = branchInst->getSuccessor(0);
						llvm::BasicBlock* falseBranch = branchInst->getSuccessor(1);

						// find the node where the control flow graph joins again
						llvm::BasicBlock* join = this->findNearestCommonPostDominator(trueBranch, falseBranch);	

						// do branch and stop at join node
						this->doBranch(basicBlock, condition, trueBranch, falseBranch, join, stopBlock);
						
						// continue with join node (predecessor is unknown at compile time)
						this->doBasicBlock(NULL, join, stopBlock);
					}
				}
				else
				{
					// return instruction
					this->doInstruction(predecessor, instruction);
				}
			}
			else
			{
				this->doInstruction(predecessor, instruction);
			}
		}
	}
	
	// clear static predecessor
	this->predecessors[basicBlock] = NULL;
}
		
void InstructionWalker::doJoin(llvm::BasicBlock* predecessor, llvm::BasicBlock* basicBlock)
{
}


} // namespace digi
