#ifndef digi_SceneConvert_TextureFormat_h
#define digi_SceneConvert_TextureFormat_h

#include <boost/range.hpp>

#include <digi/Image/ImageFormat.h>

#include "SceneOptions.h"


namespace digi {

/// @addtogroup SceneConvert
/// @{

struct TextureFormat
{
	int minVersion;
	int maxVersion;

	ImageFormat::Mapping mapping;
	ImageFormat::Layout layout;
	ImageFormat::Type type;
	
	const char* targetInternalFormat;
	const char* targetFormat;
	const char* targetType;	
	
	ImageFormat getFormat() const
	{
		return ImageFormat(this->layout, this->type, this->mapping);
	}
};
typedef boost::iterator_range<const TextureFormat*> TextureFormatRange;

TextureFormat getBestTextureFormat(GraphicsApi api, TextureFormatRange textureFormats,
	const ImageFormat& imageFormat);
	
/// @}

} // namespace digi

#endif
