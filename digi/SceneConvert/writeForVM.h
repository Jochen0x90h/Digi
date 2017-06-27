#ifndef digi_SceneConvert_writeForVM_h
#define digi_SceneConvert_writeForVM_h

#include <digi/Scene/SceneFile.h>

#include "SceneOptions.h"


namespace digi {

/// @addtogroup SceneConvert
/// @{

void writeForVM(Pointer<SceneFile> sceneFile, ObjectWriter& ow, const SceneOptions& sceneOptions);

/// @}

} // namespace digi

#endif
