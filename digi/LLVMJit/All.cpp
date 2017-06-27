#include <llvm/Support/TargetSelect.h>


void digiLLVMJitInit()
{
	// init targets (see llvm/Config/Targets.def)
	llvm::InitializeAllTargetInfos();
	llvm::InitializeAllTargets();
	llvm::InitializeAllTargetMCs();
	llvm::InitializeAllAsmPrinters(); // for ELF generation
}

void digiLLVMJitDone()
{
}
