#ifndef digi_SceneConvert_compileSceneWebGL_h
#define digi_SceneConvert_compileSceneWebGL_h

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>

#include <digi/EngineVM/Compiler.h>
#include <digi/Scene/Scene.h>

#include "GlobalCollector.h"
#include "SplittedGlobalPrinter.h"
#include "compileScene.h"


namespace digi {

/// @addtogroup SceneConvert
/// @{

bool compileSceneWebGL(
	Compiler& compiler,
	const std::string& inputCode,
	const std::map<std::string, int>& buffers,
	Pointer<SplittedGlobalPrinter>& statePrinter,
	Pointer<SplittedGlobalPrinter>& uniformsPrinter,
	Pointer<SplittedGlobalPrinter>& transformsPrinter,
	Pointer<SplittedGlobalPrinter>& boundingBoxesPrinter,	
	std::string& initStateCode,
	std::string& updateCode,
	int& useFlags);

bool compileRenderJobWebGL(
	Compiler& compiler,
	const std::string& inputCode,
	int3 transformOffset,
	int3 boundingBoxOffset,
	NameGenerator& nameGenerator,
	std::string& outputCode);

bool compileParticleSystem(
	Compiler& compiler,
	const std::string& inputCode,
	int3 emitterOffset,
	int3 uniformOffset,
	Pointer<SplittedGlobalPrinter>& particlePrinter,
	std::string& createUniformCode,
	std::string& createParticleCode,
	std::string& updateUniformCode,
	std::string& updateParticleCode,
	int& useFlags,
	Language mainLanguage);

enum ShapeParticleUseFlags
{
	// shape particle instancer uses index
	SHAPE_PARTICLE_USES_INDEX = 1,
};

bool compileShapeParticleInstancer(
	Compiler& compiler,
	const std::string& inputCode,
	int3 transformOffset,
	int3 uniformOffset,
	NameGenerator& nameGenerator,
	std::string& uniformCode,
	std::string& particleCode,
	int& useFlags,
	Language mainLanguage);

/// @}

} // namespace digi

#endif
