#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/Ascii.h>
#include <digi/Utility/IntUtility.h>
#include <digi/System/Log.h>

#include "generateTextureOpenGL.h"
#include "TextureDataInfo.h"


namespace digi {

void copyData(Pointer<File> dataFile, uint64_t dataOffset, uint64_t dataSize, DataWriter& w)
{
	dataFile->setPosition(dataOffset);

	uint8_t buffer[8192];
	while (dataSize > 0)
	{
		size_t toRead = size_t(min(dataSize, uint64_t(sizeof(buffer))));
		size_t numRead = dataFile->read(buffer, toRead);
		if (numRead == 0)
			break;
		w.writeData(buffer, numRead);
		dataSize -= numRead;
	}
}

std::pair<std::string, int> getSequenceNameAndIndex(const std::string& str)
{
	size_t e = str.length();
	if (e == 0)
		return std::pair<std::string, int>(std::string(), -1);
	if (!Ascii::isDecimal(str[e - 1]))
		return std::pair<std::string, int>(str + ".#", -1);
	size_t s = e;
	while (s > 0 && Ascii::isDecimal(str[s - 1]))
		--s;
	
	int index = 0;
	for (size_t i = s; i < e; ++i)
	{
		index *= 10;
		index += str[i] - '0';	
	}

	return std::pair<std::string, int>(str.substr(0, s) + '#' + str.substr(e), index);	
}


namespace
{		
	int3 downToPowerOfTwo(int3 value)
	{
		return vector3(
			digi::downToPowerOfTwo(value.x),
			digi::downToPowerOfTwo(value.y),
			digi::downToPowerOfTwo(value.z));
	}

	const TextureFormat& getCompressedTextureFormat(TextureOptions::TextureFormat textureFormat,
		const ImageFormat& imageFormat)
	{
		if (textureFormat == TextureOptions::S3TC)
		{
			// dxt1 or dxt5
			int4 numBits = imageFormat.getNumBits();
			bool alpha = ((imageFormat.mapping == ImageFormat::RGBA || imageFormat.mapping == ImageFormat::BGRA) && numBits.w > 1)
				|| (imageFormat.mapping == ImageFormat::YA && numBits.y > 1);

			static const TextureFormat formats[] =
			{
				{0, 0, ImageFormat::DXT1,  ImageFormat::BLOCK8,   ImageFormat::UNORM,  "GL_COMPRESSED_RGBA_S3TC_DXT1_EXT"},
				{0, 0, ImageFormat::DXT5,  ImageFormat::BLOCK16,  ImageFormat::UNORM,  "GL_COMPRESSED_RGBA_S3TC_DXT5_EXT"},
			};

			return formats[alpha ? 1 : 0];
		}
		else if (textureFormat == TextureOptions::PVRTC2)
		{
			// pvrtc 2 bit per pixel
			static const TextureFormat format =
			{
				0, 0, ImageFormat::PVRTC2,  ImageFormat::BLOCK8,   ImageFormat::UNORM,  "GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG"
			};
			
			return format;
		}
		else// if (compression == TextureOptions::PVRTC4)
		{
			// pvrtc 4 bit per pixel
			static const TextureFormat format =
			{
				0, 0, ImageFormat::PVRTC4,  ImageFormat::BLOCK8,   ImageFormat::UNORM,  "GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG"
			};
			
			return format;
		}
	}
} // anonymous namespace

	
// process an image: fill in textureInfo and write to dataFile
void writeImage(DataWriter& d, Pointer<Image> image, Pointer<ImageConverter> imageConverter,
	TextureFormatRange textureFormats, TextureDataInfo& textureInfo,
	Texture::Type type, const SceneOptions& options)
{
	const TextureOptions& textureOptions = options.textureOptions[0];
	{
		// texture size
		int3 imageSize = image->getSize();
		int3 size = min(clamp(imageSize / textureOptions.divisor, textureOptions.preserveSize, textureOptions.maxSize),
			imageSize);

		// determine if we use mipmaps
		bool mipmaps = false;
		if (type == Texture::TEXTURE)
		{
			// opengl es 2.0: adjust size to power of two
			if (!options.api.supportsTextureNonPowerOfTwo())
				size = downToPowerOfTwo(size + size / 3);

			// create mipmaps if target format is a compressed format, otherwise let opengl create them
			mipmaps = textureOptions.format != TextureOptions::RAW;
		}
		
		// pvrtc: textures must be power of two and square
		if (textureOptions.format == TextureOptions::PVRTC2 || textureOptions.format == TextureOptions::PVRTC4)
		{
			size = downToPowerOfTwo(size + size / 3);
			size.x = size.y = max(size.x, size.y);
		}

		// calc number of mipmaps
		int numMipmaps = mipmaps ? Image::calcNumMipmaps(size) : 1;

		// set texture info
		textureInfo.type = image->getType();
		if (textureOptions.format == TextureOptions::RAW)
			textureInfo.f = getBestTextureFormat(options.api, textureFormats, image->getFormat());
		else
			textureInfo.f = getCompressedTextureFormat(textureOptions.format, image->getFormat());
		textureInfo.size = size;
		textureInfo.numMipmaps = numMipmaps;
		textureInfo.dataOffset = d.getPosition();
		
		// allocate memory for texture in destination format
		ImageFormat dstFormat = textureInfo.f.getFormat();
		size_t dataSize = Image::calcMemorySize(dstFormat, size, numMipmaps);
		std::vector<uint8_t> data(dataSize);
		uint8_t* dstData = data.data();
			
		// convert and write images
		int numImages = image->getNumImages();	
		for (int imageIndex = 0; imageIndex < numImages; ++imageIndex)
		{
			// convert image into byte buffer
			DataConverter::Mode dstMode = d.isBigEndian() ? DataConverter::BE : DataConverter::LE;
			imageConverter->convert(image, dstFormat, size, mipmaps, dstMode, dstData, imageIndex);
				
			// write data
			d.writeData(dstData, dataSize);
		}
			
		// set data size to texture info
		textureInfo.dataSize = d.getPosition() - textureInfo.dataOffset;
	}
}

} // namespace digi
