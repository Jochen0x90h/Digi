#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/Ascii.h>
#include <digi/Utility/IntUtility.h>
#include <digi/System/Log.h>
#include <digi/System/MemoryDevices.h>
#include <digi/Image/JPEGWrapper.h>
#include <digi/Image/PNGWrapper.h>
#include <digi/Image/WebPWrapper.h>

#include "generateTextureWebGL.h"
#include "TextureFileInfo.h"


namespace digi {

namespace
{
	int3 downToPowerOfTwo(int3 value)
	{
		return vector3(
			digi::downToPowerOfTwo(value.x),
			digi::downToPowerOfTwo(value.y),
			digi::downToPowerOfTwo(value.z));
	}
} // anonymous namespace


// process an image: fill in textureInfo and write to image file (.png, .jpg or .webp)
void writeImage(Pointer<Image> image, Pointer<ImageConverter> imageConverter, 
	TextureFormatRange textureFormats,
	const fs::path& outputDir, const std::string& textureName, TextureFileInfo& textureInfo,
	Texture::Type type, const SceneOptions& options)
{
	// get number of texture sets
	size_t setCount = options.textureOptions.size();
	textureInfo.extensions.resize(setCount);
	for (size_t index = 0; index < setCount; ++index)
	{
		const TextureOptions& textureOptions = options.textureOptions[index];

		// texture size
		int3 imageSize = image->getSize();
		int3 size = min(clamp(imageSize / textureOptions.divisor, textureOptions.preserveSize, textureOptions.maxSize),
			imageSize);

		// adjust size to power of two if necessary
		if (type == Texture::TEXTURE)
			size = downToPowerOfTwo(size);

		// texture format
		textureInfo.f = getBestTextureFormat(options.api, textureFormats, image->getFormat());
		ImageFormat dstFormat = textureInfo.f.getFormat();
/*
		// file format
		int formatFlags = textureOptions.formatFlags;
		if ((formatFlags & TextureOptions::JPEG) != 0
			&& (dstFormat.mapping == ImageFormat::YA || dstFormat.mapping == ImageFormat::RGBA))
		{
			// jpeg does not support alpha channel: use png
			formatFlags &= ~TextureOptions::JPEG;
			formatFlags |= TextureOptions::PNG;
		
			textureInfo.jpeg2png = true;
		}
		textureInfo.formatFlags |= formatFlags;
*/
		
		// mipmaps (can be auto-generated for png, jpeg and webp
		bool mipmaps = false;
		
		// convert and write images
		int numImages = image->getNumImages();	
		for (int imageIndex = 0; imageIndex < numImages; ++imageIndex)
		{
			// convert image into destination format
			Pointer<Image> converted = imageConverter->convert(image, dstFormat, size, mipmaps);
			
			// append index for array textures
			std::string tn = textureName;
			if (numImages > 1)
				tn += '.' + toString(imageIndex);

			const int qualities[] = {100, 90, 75, 60};
			int quality = qualities[textureOptions.quality];

			// write image
			fs::path dir = outputDir / textureOptions.output;
			fs::create_directories(dir);
/*
			if (formatFlags & TextureOptions::PNG)
				savePNG(dir / (tn + ".png"), converted, quality);
			if (formatFlags & TextureOptions::JPEG)
				saveJPEG(dir / (tn + ".jpg"), converted, quality);
			if (formatFlags & TextureOptions::WEBP)
				saveWebP(dir / (tn + ".webp"), converted, quality);
*/
			if (textureOptions.format == TextureOptions::PNG
				|| (textureOptions.format == TextureOptions::JPEG_PNG && (quality == 100 || dstFormat.mapping == ImageFormat::YA || dstFormat.mapping == ImageFormat::RGBA)))
			{
				savePNG(dir / (tn + ".png"), converted, quality);
				textureInfo.extensions[index] = ".png";
			}
			else if (textureOptions.format == TextureOptions::JPEG_PNG)
			{
				// write the image format that is smaller
				Pointer<MemoryDevice> png = new MemoryDevice();
				Pointer<MemoryDevice> jpg = new MemoryDevice();
				savePNG(png, converted, quality);
				saveJPEG(jpg, converted, quality);
				
				if (png->getSize() < jpg->getSize())
				{
					// png
					Pointer<File> f = File::create(dir / (tn + ".png"));
					f->write(png->container.data(), png->container.size());
					f->close();
					textureInfo.extensions[index] = ".png";
				}
				else
				{
					// jpg
					Pointer<File> f = File::create(dir / (tn + ".jpg"));
					f->write(jpg->container.data(), jpg->container.size());
					f->close();
					textureInfo.extensions[index] = ".jpg";
				}
			}
			else if (textureOptions.format == TextureOptions::WEBP)
			{
				saveWebP(dir / (tn + ".webp"), converted, quality);
				textureInfo.extensions[index] = ".webp";
			}
		}
	}
}

} // namespace digi
