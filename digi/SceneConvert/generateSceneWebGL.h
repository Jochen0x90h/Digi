#ifndef digi_SceneConvert_generateSceneWebGL_h
#define digi_SceneConvert_generateSceneWebGL_h

#include <map>

#include <digi/Data/DataWriter.h>
#include <digi/ImageConvert/BufferConverter.h>
#include <digi/Scene/Scene.h>

#include "compileSceneWebGL.h"
#include "SceneOptions.h"

namespace digi {

/// @addtogroup SceneConvert
/// @{

/*
	generate code and data for scene (using OpenGL api calls).
	deferredShaders returns the set of shaders that need deferred rendering for transparency sorting
*/
Pointer<SplittedGlobalPrinter> generateSceneWebGL(Pointer<BufferConverter> bufferConverter, Pointer<Scene> scene,
	CodeWriter& w, DataWriter& d, const SceneOptions& options, SceneStatistics& stats);


/// @}

} // namespace digi

#endif
