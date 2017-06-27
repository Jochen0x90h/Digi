#ifndef digi_SceneConvert_generateTextureOpenGL_h
#define digi_SceneConvert_generateTextureOpenGL_h

#include <digi/Scene/SceneFile.h>

#include "TextureDataInfo.h"


namespace digi {

/// @addtogroup SceneConvert
/// @{

// write texture
void generateTextureOpenGL(Pointer<ImageConverter> imageConverter, Pointer<Image> image,
	CodeWriter& w, DataWriter& d, Texture::Type type, const SceneOptions& options);

// write texture sequence
void generateTextureSequenceOpenGL(Pointer<ImageConverter> imageConverter, const ImageSequence& textureSequence,
	CodeWriter& w, DataWriter& d, Texture::Type type, const SceneOptions& options);

void generateSymbolMapOpenGL(Pointer<SymbolMap> symbolMap,
	CodeWriter& w, DataWriter& d, Texture::Type type, const SceneOptions& options);

/// @}

} // namespace digi

#endif
