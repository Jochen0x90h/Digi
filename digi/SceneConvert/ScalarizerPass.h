#ifndef digi_SceneConvert_ScalarizerPass_h
#define digi_SceneConvert_ScalarizerPass_h

#include <llvm/Pass.h>

#include "Scalarizer.h"


namespace digi {

/// @addtogroup SceneConvert
/// @{

class ScalarizerPass : public llvm::FunctionPass
{
public:

	// llvm pass identification, replacement for typeid 
	static char ID;

	ScalarizerPass()
		: llvm::FunctionPass(ID)
	{
	}

	virtual ~ScalarizerPass();

	virtual void getAnalysisUsage(llvm::AnalysisUsage& analysisUsage) const;

	virtual bool doInitialization(llvm::Module& module);

	virtual bool runOnFunction(llvm::Function& function);

	
	llvm::OwningPtr<llvm::Module> module;
};

/// @}

} // namespace digi

#endif
