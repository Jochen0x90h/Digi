#ifndef digi_SceneConvert_compileShader_h
#define digi_SceneConvert_compileShader_h

#include <digi/CodeGenerator/CodeWriter.h>
//#include <digi/Scene/FieldBinding.h>
#include <digi/EngineVM/Compiler.h>

#include "ShaderOptions.h"
#include "ShaderType.h"
#include "SplittedGlobalPrinter.h"


namespace digi {

/// @addtogroup SceneConvert
/// @{

enum ShaderUseFlags
{
	// shader reads viewport variable
	SHADER_USES_VIEWPORT = 1,
	
	// shader reads objectId variable
	SHADER_USES_ID = 2,
	
	// shader reads scale/offset variable (s_o)
	SHADER_USES_SCALE_OFFSET = 4,

	// shader reads frontFacing uniform variable (not built-in variable)
	SHADER_USES_FRONT_FACING = 8,

	// shader writes to flip flag
	SHADER_USES_FLIP = 16
};

/// compile a shader. gets split up into vertex and pixel shader,
/// materialBinding and transformBinding are populated with uniform variables,
/// vertexBinding is populated with vertex attributes
bool compileShader(
	Compiler& compiler,
	const std::string& inputCode,
	int3 deformerOffset, // offsets of ints, floats and objects of deformer struct for languages with no struct support
	int3 materialOffset, // offsets of ints, floats and objects of material struct for languages with no struct support
	std::string& materialCode,
	std::string& transformCode,
	std::string& vertexShader,
	std::string& pixelShader,
	std::vector<ShaderVariable>& transferVariables,
	std::vector<ShaderVariable>& materialVariables, // uniform variables including textures
	std::vector<ShaderVariable>& materialLargeArrays, // large uniform arrays that are stored in textures
	std::vector<ShaderVariable>& transformVariables, // uniform variables
	std::map<std::string, ShaderVariable>& vertexBindings,
	SortMode& sortMode,
	int& useFlags,
	int2& numTextures,
	Language mainLanguage,
	Language shaderLanguage,
	ShaderOptions options,
	bool flipY);

/// @}

} // namespace digi

#endif
