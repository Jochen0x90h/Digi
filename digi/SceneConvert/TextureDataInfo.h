#ifndef digi_SceneConvert_TextureInfo_h
#define digi_SceneConvert_TextureInfo_h

#include <map>

#include <digi/Base/Platform.h>
#include <digi/Data/DataWriter.h>
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

// helpers

// copy data from temp file into the given data writer
void copyData(Pointer<File> dataFile, uint64_t dataOffset, uint64_t dataSize, DataWriter& w);

// get sequence name and index (e.g. "sequence.1" -> "sequence.#", 1)
std::pair<std::string, int> getSequenceNameAndIndex(const std::string& str);


// info for texture data in data file or memory block
struct TextureDataInfo
{
	// offset of texture
	int64_t dataOffset;
	
	// size of texture
	int64_t dataSize;

	// type of texture (IMAGE, VOLUME or CUBEMAP)
	Image::Type type;
	
	// data and target format of texture
	TextureFormat f;
	
	// texture dimensions
	int3 size;
	int numMipmaps;

						
	TextureDataInfo()
		: dataOffset(), dataSize(),
		type(), size(), numMipmaps() {}
};

// write an image to data file using given options
void writeImage(DataWriter& d, Pointer<Image> image, Pointer<ImageConverter> imageConverter,
	TextureFormatRange textureFormats, TextureDataInfo& textureInfo,
	Texture::Type type, const SceneOptions& options);

/// @}

} // namespace digi

#endif
