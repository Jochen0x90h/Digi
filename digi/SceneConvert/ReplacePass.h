#ifndef digi_SceneConvert_ReplacePass_h
#define digi_SceneConvert_ReplacePass_h

#include <llvm/Function.h>
#include <llvm/Pass.h>

#include <digi/CodeGenerator/CodeWriter.h>


namespace digi {

/// @addtogroup SceneConvert
/// @{

// detects functions min(), max(), step() etc. from sequences of llvm instructions and replace them by function calls
class ReplacePass : public llvm::FunctionPass
{
public:

	// llvm pass identification, replacement for typeid 
	static char ID;

	ReplacePass()
		: llvm::FunctionPass(ID), language()
	{
	}

	ReplacePass(Language language)
		: llvm::FunctionPass(ID), language(language)
	{
	}

	virtual ~ReplacePass();

	virtual void getAnalysisUsage(llvm::AnalysisUsage& analysisUsage) const;

	virtual bool runOnFunction(llvm::Function& function);
	
	Language language;
};


/// @}

} // namespace digi

#endif
