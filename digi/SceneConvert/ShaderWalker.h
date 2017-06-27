#ifndef digi_SceneConvert_ShaderWalker_h
#define digi_SceneConvert_ShaderWalker_h

#include "CopyWalker.h"


namespace digi {

/// @addtogroup SceneConvert
/// @{

// used to classify instructions into material, transform, vertex shader, pixel shader 
// and then split them into 4 modules.
// also used to split deformers into uniform and vertex processor.
class ShaderWalker : public CopyWalker
{
public:

	enum Scope
	{
		MATERIAL = 0,
		TRANSFORM = 1,
		VERTEX = 2,
		PIXEL = 3
	};

	// usage flags, propagated against control flow direction
	enum Flags
	{
		USED_BY_MATERIAL = 1,
		USED_BY_TRANSFORM = 2,
		USED_BY_VERTEX = 4,
		USED_BY_PIXEL = 8,
		
		// input to shader (flag prevents lifting load instructions into pixel scope)
		INPUT = 16
	};	

	// create a shader walker with source postdominator tree and function and destination modules
	ShaderWalker(llvm::PostDominatorTree* postDominatorTree, llvm::Function& function,
		ArrayRef<llvm::OwningPtr<llvm::Module> > modules, bool avoidPixelShader)
		: CopyWalker(postDominatorTree, function, modules), avoidPixelShader(avoidPixelShader)
	{
	}
	~ShaderWalker();

	// flip y coordinate of texture sample() functions
	void sampleFlipY(llvm::Function& function, llvm::StringRef name);

	// classify function, returns scope
	virtual int classifyFunction(llvm::CallInst* callInst, const std::string& name, int flags);

	// set scope to value info
	virtual void setValueScope(ValueInfo& valueInfo, int scope);


	// calc as much as possible in vertex shader instead of pixel shader
	bool avoidPixelShader;
};

/// @}

} // namespace digi

#endif
