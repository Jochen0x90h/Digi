#include <llvm/Analysis/PostDominators.h>
#include <llvm/Assembly/PrintModulePass.h>

#include "ScalarizerPass.h"
#include "ReplacePass.h"
#include "PrintPass.h"
#include "SplittedGlobalPrinter.h"
#include "CompileHelper.h"


namespace digi {

llvm::Module* scalarizeModule(llvm::Module& module, Pointer<CompileResult> compileResult)
{
	// passmanager takes ownership of passes
	llvm::PassManager passManager;

	if (compileResult != null)
	{
		// optimization (intermediate mode)
		compileResult->addPasses(module, passManager, false);
	}

	// add scalarizer pass
	ScalarizerPass* scalarizerPass = new ScalarizerPass();
	passManager.add(scalarizerPass);

	// run all passes
	passManager.run(module);

	// get and return the scalarized module
	return scalarizerPass->module.take();
}

void printModule(llvm::Module& module, Pointer<CompileResult> compileResult,
	std::map<std::string, GlobalPrinterAccess>& globalPrinters, std::map<std::string, Pointer<IODevice> >& deviceMap,
	Language language)
{
	// passmanager takes ownership of passes
	llvm::PassManager passManager;

	if (compileResult != null)
	{
		// optimization (intermediate mode)
		compileResult->addPasses(module, passManager, false);
	}
	
	// replace instruction sequences by functions, e.g. min, max, step
	passManager.add(new ReplacePass(language));

	#ifndef NDEBUG
		addPrintPass(passManager, "replaced.ll");
	#endif

	// needed by print pass
	passManager.add(new llvm::PostDominatorTree());
		
	// print pass
	passManager.add(new PrintPass(&deviceMap, language, &globalPrinters));

	passManager.run(module);
}

} // namespace digi
