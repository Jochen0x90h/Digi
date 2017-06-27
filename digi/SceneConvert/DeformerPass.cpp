#include <llvm/Instructions.h>
#include <llvm/Module.h>
#include <llvm/Analysis/PostDominators.h>
#include <llvm/Support/IRBuilder.h>

#include <digi/System/Log.h>
#include <digi/CodeGenerator/TypeInfo.h>
#include <digi/CodeGenerator/NameMangler.h>

#include "CopyWalker.h"
#include "DeformerPass.h"
#include "PrintHelper.h"


namespace digi {


DeformerPass::~DeformerPass()
{
}

void DeformerPass::getAnalysisUsage(llvm::AnalysisUsage& analysisUsage) const
{
	// tell llvm that we don't modify anything
	analysisUsage.setPreservesAll();

	// tell llvm that we don't add or remove basic blocks or modify terminator instructions
	//analysisUsage.setPreservesCFG();

	// tell llvm wich other passes we need
//	analysisUsage.addRequired<llvm::TargetData>();
//	analysisUsage.addRequired<llvm::LoopInfo>();
	analysisUsage.addRequired<llvm::PostDominatorTree>();	
}

bool DeformerPass::doInitialization(llvm::Module& module)
{
	llvm::LLVMContext& c = module.getContext();
	
	// create modules for copied functions
	this->modules[UNIFORM].reset(new llvm::Module("deformerU", c));
	this->modules[VERTEX].reset(new llvm::Module("deformerV", c));

	return false;	
} 

bool DeformerPass::runOnFunction(llvm::Function& function)
{
	llvm::StringRef name = function.getName();
	if (name == "main")
	{
		// create shader walker, creates the function in the modules. this object has ownership of modules
		CopyWalker walker(&this->getAnalysis<llvm::PostDominatorTree>(), function, this->modules);
	
		// set scopes of global inputs (get accessed by load instructions)
		walker.setScope(function, "deformer", UNIFORM);
		walker.setScope(function, "vertex", VERTEX);

		// classify instructions

		// output
		walker.classifyInstructions(function, "result", VERTEX, USED_BY_VERTEX);
		
		// start processing at entry block
		walker.doBasicBlock(NULL, &function.getEntryBlock(), NULL);

		walker.fixAndGetTransfer(UNIFORM, VERTEX, this->u2v, "u2v", false);
	}
	else
	{
		// error: unexprected function
		dWarning("unexpected deformer function '" << name << "' encountered");
	}

	return false;
}

	
char DeformerPass::ID = 0;

static llvm::RegisterPass<DeformerPass> registerDeformerPass("digi-deformer", "process deformers");

} // namespace digi
