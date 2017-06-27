#ifndef digi_SceneConvert_DeformerPass_h
#define digi_SceneConvert_DeformerPass_h

#include <llvm/Function.h>
#include <llvm/Pass.h>

#include "ShaderOptions.h"
#include "ShaderType.h"


namespace digi {

/// @addtogroup SceneConvert
/// @{

// DeformerPass, splits a deformer into uniform and vertex code
class DeformerPass : public llvm::FunctionPass
{
public:

	enum Scope
	{
		UNIFORM = 0,
		VERTEX = 1,
	};

	enum Flags
	{
		USED_BY_UNIFORM = 1,
		USED_BY_VERTEX = 2,
	};	


	// llvm pass identification, replacement for typeid 
	static char ID;

	DeformerPass()
		: llvm::FunctionPass(ID)
	{
	}

	virtual ~DeformerPass();

	virtual void getAnalysisUsage(llvm::AnalysisUsage& analysisUsage) const;

	virtual bool doInitialization(llvm::Module& module);

	virtual bool runOnFunction(llvm::Function& function);
	
	
	llvm::OwningPtr<llvm::Module> modules[2];

	
	// transfer variables (uniform -> vertex)
	std::vector<ShaderType> u2v;
};

/// @}

} // namespace digi

#endif
