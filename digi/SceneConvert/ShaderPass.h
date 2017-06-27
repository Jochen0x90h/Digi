#ifndef digi_SceneConvert_ShaderPass_h
#define digi_SceneConvert_ShaderPass_h

#include <llvm/Function.h>
#include <llvm/Pass.h>

#include "ShaderOptions.h"
#include "ShaderType.h"


namespace digi {

/// @addtogroup SceneConvert
/// @{

// ShaderPass, splits a shader into material, transform, vertex shader and pixel shader
class ShaderPass : public llvm::FunctionPass
{
public:

	// llvm pass identification, replacement for typeid 
	static char ID;

	ShaderPass()
		: llvm::FunctionPass(ID), avoidPixelShader(), frontFacingIsUniform(), flipY()
	{
	}

	ShaderPass(bool avoidPixelShader, bool frontFacingIsUniform, bool flipY)
		: llvm::FunctionPass(ID), avoidPixelShader(avoidPixelShader), frontFacingIsUniform(frontFacingIsUniform), flipY(flipY)
	{
	}

	virtual ~ShaderPass();

	virtual void getAnalysisUsage(llvm::AnalysisUsage& analysisUsage) const;

	virtual bool doInitialization(llvm::Module& module);

	virtual bool runOnFunction(llvm::Function& function);
	
	
	// calc as much as possible in vertex shader instead of pixel shader	
	bool avoidPixelShader;
	
	// front facing is a uniform variable
	bool frontFacingIsUniform;
	
	// flip y coordinate of texture sample() functions
	bool flipY;

	llvm::OwningPtr<llvm::Module> modules[4];

	// indicates if alpha channel is always transparent (< 1.0), always opaque (== 1.0) or variable
	SortMode sortMode;
	
	// transfer variables
	std::vector<ShaderType> m2t;
	std::vector<ShaderType> m2v;
	std::vector<ShaderType> m2p;
	std::vector<ShaderType> m2po;
	std::vector<ShaderType> t2v;
	std::vector<ShaderType> t2p;
	std::vector<ShaderType> v2p;
};

/// @}

} // namespace digi

#endif
