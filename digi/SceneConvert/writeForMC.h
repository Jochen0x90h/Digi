#ifndef digi_SceneConvert_writeForMC_h
#define digi_SceneConvert_writeForMC_h

#include <digi/Scene/SceneFile.h>

#include "SceneOptions.h"


namespace digi {

/// @addtogroup SceneConvert
/// @{

enum MCTarget
{
	X86_GCC,
	X64_GCC,
	X86_WINDOWS,
	X64_WINDOWS,
};

void writeForMC(Pointer<SceneFile> sceneFile, ObjectWriter& ow, const SceneOptions& sceneOptions, MCTarget mcTarget);

/// @}

} // namespace digi

#endif
