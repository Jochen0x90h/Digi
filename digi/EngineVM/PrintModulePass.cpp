#include <llvm/Support/raw_ostream.h>

#include <digi/System/Log.h>

#include "PrintModulePass.h"


namespace digi {

PrintModulePass::~PrintModulePass()
{
}

void PrintModulePass::getAnalysisUsage(llvm::AnalysisUsage& analysisUsage) const
{
	// tell llvm that we don't modify anything
	analysisUsage.setPreservesAll();
}

bool PrintModulePass::runOnModule(llvm::Module& module)
{
	std::string name = module.getModuleIdentifier() + '.' + this->name;
	std::string error;
	llvm::raw_fd_ostream s(name.c_str(), error, llvm::raw_fd_ostream::F_Binary);	

	s << module;

	s.close();

	return false;
}

char PrintModulePass::ID = 0;

static llvm::RegisterPass<PrintModulePass> registerPrintPass("printModule", "print module");

} // namespace digi
