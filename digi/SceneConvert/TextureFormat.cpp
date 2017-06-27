#include <digi/Utility/IntUtility.h>
#include <digi/Utility/foreach.h>

#include "TextureFormat.h"


namespace digi {

namespace
{
	int getNumMappings(const TextureFormat& textureFormat)
	{
		return textureFormat.getFormat().getNumMappings();
	}

	int getMappingFlags(const TextureFormat& textureFormat)
	{
		return textureFormat.getFormat().getMappingFlags();
	}
} // anonymous namespace

TextureFormat getBestTextureFormat(GraphicsApi api, TextureFormatRange textureFormats,
	const ImageFormat& imageFormat)
{
	int lowestError = 0xfffffff;
	int4 imageNumBits = imageFormat.getNumBits();
	
	// iterate over formats and determine best format
	TextureFormat bestFormat = {};
	foreach (const TextureFormat& textureFormat, textureFormats)
	{
		if (api.version >= textureFormat.minVersion && api.version <= textureFormat.maxVersion)
		{
			int4 numBits = imageNumBits;
			
			// determine mapping error
			int mappingError;// = getMappingError(api, imageFormat.mapping, textureFormat.mapping);
			if (api.supportsTextureSwizzle())
			{
				// texture swizzle: only number of channels is relevant
				int d = getNumMappings(textureFormat) - imageFormat.getNumMappings();
				
				// number of excess channels and number of missing channels with large weight
				mappingError = d >= 0 ? d : -d * 1000;
			}
			else
			{
				// no texture swizzle: match via channel flags
				int imageFlags = imageFormat.getMappingFlags();
				int textureFlags = getMappingFlags(textureFormat);
				
				// if image has luminance but texture has no luminance then use rgb insead
				if (imageFlags & ~textureFlags & ImageFormat::Y_FLAG)
				{
					imageFlags = (imageFlags & ~ImageFormat::Y_FLAG) | ImageFormat::R_FLAG | ImageFormat::G_FLAG | ImageFormat::B_FLAG;
					numBits.w = numBits.y;
					numBits.y = numBits.z = numBits.x;
				}

				// number of excess channels and number of missing channels with large weight
				mappingError = countSetBits(textureFlags & ~imageFlags) + countSetBits(imageFlags & ~textureFlags) * 1000;
			}
			
			// determine layout error (squared distance of number of bits per channel)
			int layoutError = lengthSquared(numBits - ImageFormat::getNumBits(textureFormat.layout));
			
			// determine type error
			int typeError = (imageFormat.type - textureFormat.type) != 0 ? 100 : 0;
				
			int error = mappingError + layoutError + typeError;
			if (error < lowestError)
			{
				lowestError = error;
				bestFormat = textureFormat;
			}
		}
	}
	if (api.supportsTextureSwizzle())
	{
		// texture swizzle: use mapping of image (e.g. Y for R texture, YA for RG texture)
		bestFormat.mapping = imageFormat.mapping;
	}
	return bestFormat;
}

} // namespace digi
