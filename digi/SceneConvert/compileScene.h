#ifndef digi_SceneConvert_compileScene_h
#define digi_SceneConvert_compileScene_h

#include <digi/CodeGenerator/CodeWriter.h>
#include <digi/EngineVM/Compiler.h>


namespace digi {

/// @addtogroup SceneConvert
/// @{

enum SceneUseFlags
{
	// the scene uses seed variable
	SCENE_USES_SEED = 1,
};

/// compile a scene
bool compileScene(
	Compiler& compiler, const std::string& inputCode,
	std::string& outputCode,
	Language language);


bool compileDeformer(
	Compiler& compiler,
	const std::string& inputCode,
	int3 deformerOffset, // offsets of ints, floats and objects of deformer struct for languages with no struct support	
	std::string& uniformCode,
	std::string& vertexCode,
	std::vector<ShaderVariable>& uniformVariables,
	std::map<std::string, ShaderVariable>& vertexBindings,
	std::map<std::string, ShaderVariable>& resultBindings,
	Language mainLanguage,
	Language deformerLanguage);


/// @}

} // namespace digi

#endif
