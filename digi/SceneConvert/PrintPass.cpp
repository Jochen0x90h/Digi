#include <llvm/Instructions.h>
#include <llvm/Module.h>
#include <llvm/Analysis/PostDominators.h>

#include <digi/Utility/foreach.h>
#include <digi/System/Log.h>
#include <digi/CodeGenerator/TypeInfo.h>
#include <digi/CodeGenerator/NameMangler.h>

#include "PrintWalker.h"
#include "PrintPass.h"


namespace digi {


PrintPass::~PrintPass()
{
}

void PrintPass::getAnalysisUsage(llvm::AnalysisUsage& analysisUsage) const
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

bool PrintPass::runOnFunction(llvm::Function& function)
{
	std::string name = function.getName();

	std::map<std::string, Pointer<IODevice> >::iterator it = this->devices->find(name);
	if (it != this->devices->end())
	{
		Pointer<IODevice> device = it->second;
		
		PrintWalker walker(&this->getAnalysis<llvm::PostDominatorTree>(), device, this->language,
			*this->globalPrinters);	

		// start processing at entry block
		walker.doBasicBlock(NULL, &function.getEntryBlock(), NULL);

		// close device
		walker.close();
	}
	else
	{
		// error: unexprected function
		dWarning("unexpected function '" << name << "' encountered");
	}
		
	return false;
}


char PrintPass::ID = 0;

static llvm::RegisterPass<PrintPass> registerPrintPass("digi-print", "print code");


} // namespace digi
