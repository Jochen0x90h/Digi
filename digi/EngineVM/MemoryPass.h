#ifndef digi_EngineVM_MemoryPass_h
#define digi_EngineVM_MemoryPass_h

#include <llvm/Function.h>
#include <llvm/Pass.h>


namespace digi {

/// @addtogroup SceneConvert
/// @{

// memory pass, replaces allocas of structs by alocas of basic types which are suitable
// for the llvm mem2reg pass. Also replaces memcpy by copy of basic types.
class MemoryPass : public llvm::FunctionPass
{
public:

	// llvm pass identification, replacement for typeid 
	static char ID;

	MemoryPass()
		: llvm::FunctionPass(ID)
	{
	}

	virtual ~MemoryPass();

	virtual void getAnalysisUsage(llvm::AnalysisUsage& analysisUsage) const;

	virtual bool runOnFunction(llvm::Function& function);	
};

/// @}

} // namespace digi

#endif
