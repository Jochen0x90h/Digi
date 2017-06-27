#ifndef digi_SceneConvert_TextureFileInfo_h
#define digi_SceneConvert_TextureFileInfo_h

#include <map>

#include <digi/Base/Platform.h>
#include <digi/Image/Image.h>
#include <digi/ImageConvert/ImageConverter.h>
#include <digi/CodeGenerator/All.h>
#include <digi/Scene/ObjectWriter.h>
#include <digi/Scene/Texture.h>

#include "SceneOptions.h"
#include "TextureFormat.h"


namespace digi {

/// @addtogroup SceneConvert
/// @{

// info for texture file (e.g. for webgl)
struct TextureFileInfo
{
	// data and target format of texture
	TextureFormat f;
	
	// file formats in which image is stored (TextureOptions::FormatFlags, can be more than one)
	//int formatFlags;

	// flag that indicates if jpeg has to be replaced by png because texture has alpha channel
	//bool jpeg2png;
	
	// file format extension (e.g. ".png")
	std::vector<std::string> extensions;

				
	TextureFileInfo()
		{}
};

// write an image to image file using given options
void writeImage(Pointer<Image> image, Pointer<ImageConverter> imageConverter, 
	TextureFormatRange textureFormats,
	const fs::path& outputDir, const std::string& textureName, TextureFileInfo& textureInfo,
	Texture::Type type, const SceneOptions& options);

/// @}

} // namespace digi

#endif
