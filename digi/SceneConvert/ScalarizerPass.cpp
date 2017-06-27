#include "ScalarizerPass.h"


namespace digi {

ScalarizerPass::~ScalarizerPass()
{
}

void ScalarizerPass::getAnalysisUsage(llvm::AnalysisUsage& analysisUsage) const
{
	// tell llvm that we don't modify anything
	analysisUsage.setPreservesAll();
}

bool ScalarizerPass::doInitialization(llvm::Module& module)
{
	llvm::LLVMContext& c = module.getContext();
	
	// create module for scalarized function
	this->module.reset(new llvm::Module(module.getModuleIdentifier() + "_s", c));

	return false;	
} 

bool ScalarizerPass::runOnFunction(llvm::Function& function)
{
	// create scalarizer and run on function
	Scalarizer scalarizer(function, *this->module);

	return false;
}


char ScalarizerPass::ID = 0;

static llvm::RegisterPass<ScalarizerPass> registerScalarizerPass("digi-scalarize", "scalarize");

} // namespace digi
