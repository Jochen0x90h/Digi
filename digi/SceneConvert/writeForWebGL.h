#ifndef writeForWebGL_h
#define writeForWebGL_h

#include <digi/Scene/SceneFile.h>

#include "SceneOptions.h"


namespace digi {

/// @addtogroup SceneConvert
/// @{

/// writes javascript or json into given outputPath and an additional data file with extension ".dat"
void writeForWebGL(Pointer<SceneFile> sceneFile, const fs::path& outputPath, bool json,
	const SceneOptions& options, SceneStatistics& stats);

/// @}

} // namespace digi

#endif
