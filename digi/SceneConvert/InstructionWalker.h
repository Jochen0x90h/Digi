#ifndef digi_SceneConvert_InstructionWalker_h
#define digi_SceneConvert_InstructionWalker_h

#include <string>
#include <map>

#include <llvm/Analysis/PostDominators.h>


namespace digi {

/// @addtogroup SceneConvert
/// @{


class InstructionWalker
{
	public:

		InstructionWalker(llvm::PostDominatorTree* postDominatorTree)
			: postDominatorTree(postDominatorTree) {}
		
		// return true if A post dominates B
		bool postDominates(llvm::BasicBlock* A, llvm::BasicBlock* B)
		{
			return this->postDominatorTree->dominates(A, B);
		}
		
		// find nearest common post dominator of basic blocks A and B
		llvm::BasicBlock* findNearestCommonPostDominator(llvm::BasicBlock* A, llvm::BasicBlock* B)
		{
			return this->postDominatorTree->findNearestCommonDominator(A, B);
		}

		// get static predecessor of basic block or NULL if predecessor is unknown at compile time
		llvm::BasicBlock* getPredecessor(llvm::BasicBlock* basicBlock)
		{
			return this->predecessors[basicBlock];
		}

		// do basic blocks and follow branches until stopBlock is encountered
		void doBasicBlock(llvm::BasicBlock* predecessor, llvm::BasicBlock* basicBlock, llvm::BasicBlock* stopBlock);
		
		// do a branch		
		virtual void doBranch(llvm::BasicBlock* basicBlock, llvm::Value* condition, llvm::BasicBlock* trueBranch,
			llvm::BasicBlock* falseBranch, llvm::BasicBlock* join, llvm::BasicBlock* stopBlock) = 0;
		
		// notify that a join point is reached
		virtual void doJoin(llvm::BasicBlock* predecessor, llvm::BasicBlock* basicBlock);

		// do an instruction
		virtual void doInstruction(llvm::BasicBlock* predecessor, llvm::Instruction* instruction) = 0;
		
	protected:

		llvm::PostDominatorTree* postDominatorTree;
		std::map<llvm::BasicBlock*, llvm::BasicBlock*> predecessors;
};


/// @}

} // namespace digi

#endif
