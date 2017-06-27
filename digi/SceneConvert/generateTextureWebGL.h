#ifndef digi_SceneConvert_generateTextureWebGL_h
#define digi_SceneConvert_generateTextureWebGL_h

#include <digi/Scene/SceneFile.h>

#include "TextureFileInfo.h"
#include "TextureDataInfo.h"


namespace digi {

/// @addtogroup SceneConvert
/// @{

ImageFormat getTextureFormatWebGL(ImageFormat format);


// write texture into separate image file
int generateTextureWebGL(Pointer<ImageConverter> imageConverter, Pointer<Image> image,
	CodeWriter& w, const fs::path& dir, const std::string& name, Texture::Type type, const SceneOptions& options);

// write texture sequence into separate image files
int generateTextureSequenceWebGL(Pointer<ImageConverter> imageConverter, const ImageSequence& textureSequence,
	CodeWriter& w, const fs::path& dir, const std::string& name, Texture::Type type, const SceneOptions& options);


// write texture into global data file
void generateTextureWebGL(Pointer<ImageConverter> imageConverter, Pointer<Image> image, CodeWriter& w, DataWriter& d,
	 Texture::Type type, const SceneOptions& options);

// write texture sequence into global data file
void generateTextureSequenceWebGL(Pointer<ImageConverter> imageConverter, const ImageSequence& textureSequence,
	CodeWriter& w, DataWriter& d, Texture::Type type, const SceneOptions& options);

/// @}

} // namespace digi

#endif
