#ifndef digi_EngineVM_PrintModulePass_h
#define digi_EngineVM_PrintModulePass_h

#include <llvm/Module.h>
#include <llvm/Pass.h>
#include <digi/Utility/StringRef.h>


namespace digi {

/// @addtogroup SceneConvert
/// @{

// print a module (see VMCore/PrintModulePass)
class PrintModulePass : public llvm::ModulePass
{
public:
	// llvm pass identification, replacement for typeid 
	static char ID;

	PrintModulePass()
		: llvm::ModulePass(ID)
	{
	}

	PrintModulePass(StringRef name)
		: llvm::ModulePass(ID), name(name)
	{
	}

	virtual ~PrintModulePass();

	virtual void getAnalysisUsage(llvm::AnalysisUsage& analysisUsage) const;

	virtual bool runOnModule(llvm::Module& module);

	
	std::string name;
};

/// @}

} // namespace digi

#endif
