#include <boost/range.hpp>

#include <digi/Utility/Convert.h>
#include <digi/System/Log.h>
#include <digi/System/MemoryDevices.h>
#include <digi/Checksum/CRC32.h>
#include <digi/ImageIO/ImageIO.h>

#include "ImageUtil.h"
#include "ImageConverter.h"

// include last because of global defines (e.g. BOOL)
#ifdef HAVE_S3TC
	#include <squish.h>
#endif
#ifdef HAVE_PVRTC
	#include <PVRTexture.h>
	#include <PVRTextureUtilities.h>
#endif


namespace digi {

#ifdef HAVE_S3TC
	void convertMipmapToS3TC(ImageConverter* imageConverter,
		Pointer<Image> currentImage, int3 size, int currentMipmapIndex, int currentImageIndex,
		ImageFormat dstFormat, void* dstData)
	{
		ImageFormat srcFormat = ImageFormat(ImageFormat::XYZW8, ImageFormat::UNORM, ImageFormat::RGBA);
	
		// convert current image to RGBA8 if necessary
		if (currentImage->getFormat() != srcFormat)
		{
			Pointer<Image> image = new Image(currentImage->getType(), srcFormat, size);
			imageConverter->convertPixels(currentImage->getFormat(), currentImage->getData<void>(currentMipmapIndex, currentImageIndex),
				srcFormat, image->getData<void>(), Image::calcNumPixels(size));
		
			currentImage = image;
			currentMipmapIndex = 0;
			currentImageIndex = 0;
		}
	
		// convert to s3tc
		int flags;
		switch (dstFormat.mapping)
		{
		case ImageFormat::DXT1:
			flags = squish::kDxt1;
			break;
		case ImageFormat::DXT3:
			flags = squish::kDxt3;
			break;
		default:
			flags = squish::kDxt5;
		}

		int numBlocksX = (size.x + 3) >> 2;
		int numBlocksY = (size.y + 3) >> 2;

		int partialBlockX = size.x >> 2;
		int partialBlockY = size.y >> 2;

		ubyte4* sourceRow = currentImage->getData<ubyte4>(currentMipmapIndex, currentImageIndex);
		uint8_t* blocks = (uint8_t*)dstData;
		size_t blockSize = dstFormat.getMemorySize();
	
		// iterate in z-direction
		for (int z = 0; z < size.z; ++z)
		{
			for (int y = 0; y < numBlocksY; ++y)
			{
				int numRows = 4;
				int mask = 0xffff;
				if (y == partialBlockY)
				{
					numRows = size.y & 3;
					mask = ~(~0 << numRows * 4);
				}
		
				ubyte4* sourceLine = sourceRow;
				for (int x = 0; x < numBlocksX; ++x)
				{
					int numColumns = 4;
					if (x == partialBlockX)
					{
						numColumns = size.x & 3;
						mask &= ~(~0 << numColumns) * 0x1111;
					}
											
					// get block
					ubyte4* sourcePixel = sourceLine;
					ubyte4 pixels[16];
					ubyte4* p = pixels;
					int j = 0;
					for (; j < numRows; ++j)
					{
						int i = 0;
						for (; i < numColumns; ++i)
							p[i] = sourcePixel[i];
					
						// next row
						sourcePixel += size.x;
						p += 4;
					}
				
					// convert block
					squish::CompressMasked((const squish::u8*)pixels, mask, blocks, flags);
				
					// next block
					sourceLine += 4;
					blocks += blockSize;
				}

				// next block row
				sourceRow += size.x * 4;
			}
		}
	}
#endif

#ifdef HAVE_PVRTC
	void convertMipmapToPVRTC(ImageConverter* imageConverter,
		Pointer<Image> currentImage, int3 size, int currentMipmapIndex, int currentImageIndex,
		ImageFormat dstFormat, void* dstData)
	{
		// create source texture
		PVRTextureHeaderV3 header;
		header.u64PixelFormat = pvrtexture::PVRStandard8PixelType.PixelTypeID;
		header.u32ChannelType = ePVRTVarTypeUnsignedByteNorm;
		header.u32Width = size.x;
		header.u32Height = size.y;
		pvrtexture::CPVRTexture texture(header);//, currentImage->getData<void>(currentMipmapIndex, currentImageIndex));

		// convert into source texture
		ImageFormat rgba8Format = ImageFormat(ImageFormat::XYZW8, ImageFormat::UNORM, ImageFormat::RGBA);
		imageConverter->convertPixels(
			currentImage->getFormat(), currentImage->getData<void>(currentMipmapIndex, currentImageIndex),
			rgba8Format, texture.getDataPtr(),
			Image::calcNumPixels(size));

		// convert to pvrtc
		pvrtexture::PixelType pvrtc4(dstFormat.mapping == ImageFormat::PVRTC2 ? ePVRTPF_PVRTCI_2bpp_RGBA : ePVRTPF_PVRTCI_4bpp_RGBA);
		bool result = pvrtexture::Transcode(texture, pvrtc4, ePVRTVarTypeUnsignedInteger, ePVRTCSpacelRGB);

		// copy
		size_t size1 = Image::calcMemorySize(dstFormat, size);
		size_t size2 = texture.getDataSize();
		memcpy(dstData, texture.getDataPtr(), size1);
	}

	void convertMipmapFromPVRTC(ImageConverter* imageConverter,
		Pointer<Image> currentImage, int3 size, int currentMipmapIndex, int currentImageIndex,
		ImageFormat dstFormat, void* dstData)
	{
		// source texture
		PVRTextureHeaderV3 header;
		header.u64PixelFormat = pvrtexture::PixelType(dstFormat.mapping == ImageFormat::PVRTC2 ? ePVRTPF_PVRTCI_2bpp_RGBA : ePVRTPF_PVRTCI_4bpp_RGBA).PixelTypeID;
		header.u32ChannelType = ePVRTVarTypeUnsignedInteger;
		header.u32Width = size.x;
		header.u32Height = size.y;
		pvrtexture::CPVRTexture texture(header, currentImage->getData<void>(currentMipmapIndex, currentImageIndex));

		// convert to RGBA8
		bool result = pvrtexture::Transcode(texture, pvrtexture::PVRStandard8PixelType, ePVRTVarTypeUnsignedByteNorm, ePVRTCSpacelRGB);

		// convert into destination
		ImageFormat rgba8Format = ImageFormat(ImageFormat::XYZW8, ImageFormat::UNORM, ImageFormat::RGBA);
		imageConverter->convertPixels(
			rgba8Format, texture.getDataPtr(),
			dstFormat, dstData,
			Image::calcNumPixels(size));
	}
#endif

// ImageConverter

ImageConverter::~ImageConverter()
{
}

Pointer<DataConverter> ImageConverter::getPixelConverter(ImageFormat srcFormat, ImageFormat dstFormat,
	DataConverter::Mode dstMode)
{
	uint8_t buffer[7];
	buffer[0] = srcFormat.layout;
	buffer[1] = srcFormat.type;
	buffer[2] = srcFormat.mapping;
	buffer[3] = dstFormat.layout;
	buffer[4] = dstFormat.type;
	buffer[5] = dstFormat.mapping;
	buffer[6] = dstMode;
	uint32_t crc = calcCRC32(buffer, boost::size(buffer));

	// look in chache for converter
	Pointer<DataConverter>& converter = this->converters[crc];
	
	// create new converter if not found
	if (converter == null)
	{
		std::string code;
		CodeWriter w(new StringRefDevice(code));
		ConverterWriter cw(w);
		
		cw.beginConverter();
		cw.load(srcFormat);
		if (srcFormat != dstFormat)
		{
			cw.depackToFloat(srcFormat);
			ImageConverter::buildConversion(cw, srcFormat, dstFormat);
			cw.packFromFloat(dstFormat);
		}
		cw.store(dstFormat, dstMode);
		cw.endConverter();
		
		converter = DataConverter::create(this->context, code);
	}

	return converter;	
}

void ImageConverter::convertPixels(
	ImageFormat srcFormat, void* srcData,
	ImageFormat dstFormat, void* dstData,
	size_t numPixels)
{	
	this->getPixelConverter(srcFormat, dstFormat, DataConverter::NATIVE)->convert(
		srcData, srcFormat.getMemorySize(),
		NULL,
		dstData, dstFormat.getMemorySize(),
		numPixels);
}

void ImageConverter::convertPixels(
	ImageFormat srcFormat, void* srcData,
	ImageFormat dstFormat, DataConverter::Mode dstMode, void* dstData, size_t dstStride,
	size_t numPixels)
{
	this->getPixelConverter(srcFormat, dstFormat, dstMode)->convert(
		srcData, srcFormat.getMemorySize(),
		NULL,
		dstData, dstStride,
		numPixels);
}

/*
void ImageConverter::convertImage(
	Pointer<Image> srcImage, int srcNumMipmaps, int srcImageIndex,
	ImageFormat dstFormat, void* dstData)
{
	ImageFormat srcFormat = srcImage->getFormat();
	int3 size = srcImage->getSize();
	
	if (!srcFormat.isCompressed() && !dstFormat.isCompressed())
	{
		this->convertPixels(srcFormat, srcImage->getData<void>(srcImageIndex),
			dstFormat, dstData, Image::calcNumPixels(size, srcNumMipmaps));
	}
	else
	{
		// one format is compressed
		if (srcFormat == dstFormat)
		{
			memcpy(dstData, srcImage->getData<void>(srcImageIndex), Image::calcMemorySize(srcFormat, size, srcNumMipmaps));
			return;
		}
		
		// iterate over mipmaps
		for (int mipmapIndex = 0; mipmapIndex < srcNumMipmaps; ++mipmapIndex)
		{
			Pointer<Image> currentImage = srcImage;
			int currentMipmapIndex = mipmapIndex;
			int currentImageIndex = srcImageIndex;
			if (srcFormat.mapping >= ImageFormat::DXT1 && srcFormat.mapping <= ImageFormat::DXT5)
			{
				// source is DXT1, DXT3 or DXT5: decompress
				#ifdef HAVE_S3TC
					ImageFormat tmpFormat = ImageFormat(ImageFormat::XYZW8, ImageFormat::UNORM, ImageFormat::RGBA);
					Pointer<Image> tmpImage = new Image(srcImage->getType(), tmpFormat, size);
				
					//!
				
					// set current
					currentImage = tmpImage;
					currentMipmapIndex = 0;
					currentImageIndex = 0;
				#endif
			}
			else if (srcFormat.mapping >= ImageFormat::PVRTC2 && srcFormat.mapping <= ImageFormat::PVRTC4)
			{
				// source is PVRTC with 2 or 4 bits per pixel: decompress
				#ifdef HAVE_PVRTC
					ImageFormat tmpFormat = ImageFormat(ImageFormat::XYZW8, ImageFormat::UNORM, ImageFormat::RGBA);
					Pointer<Image> tmpImage = new Image(srcImage->getType(), tmpFormat, size);

					convertMipmapFromPVRTC(this, currentImage, size, currentMipmapIndex, currentImageIndex,
						tmpFormat, tmpImage->getData<void>());

					// set current
					currentImage = tmpImage;
					currentMipmapIndex = 0;
					currentImageIndex = 0;
				#endif
			}
			
			if (dstFormat.mapping >= ImageFormat::DXT1 && dstFormat.mapping <= ImageFormat::DXT5)
			{
				// destination is DXT1, DXT3 or DXT5
				#ifdef HAVE_S3TC
					convertMipmapToS3TC(this, currentImage, size, currentMipmapIndex, currentImageIndex,
						dstFormat, dstData);
				#endif
			}
			else if (dstFormat.mapping >= ImageFormat::PVRTC2 && dstFormat.mapping <= ImageFormat::PVRTC4)
			{
				// destination is PVRTC with 2 or 4 bits per pixel: compress
				#ifdef HAVE_PVRTC
					convertMipmapToPVRTC(this, currentImage, size, currentMipmapIndex, currentImageIndex,
						dstFormat, dstData);
				#endif				
			}
			else
			{
				// destination is pixel format: convert from current image into destination data
				this->convertPixels(currentImage->getFormat(), currentImage->getData<void>(currentMipmapIndex, currentImageIndex),
					dstFormat, dstData, Image::calcNumPixels(size));			
			}
			
			// next mipmap level
			dstData = (uint8_t*)dstData + Image::calcMemorySize(dstFormat, size);
			size = max(size >> 1, 1);
		}
	}	
}
*/

void ImageConverter::convertImage(
	Pointer<Image> srcImage, int srcNumMipmaps, int srcImageIndex,
	ImageFormat dstFormat, DataConverter::Mode dstMode, void* dstData)
{
	ImageFormat srcFormat = srcImage->getFormat();
	int3 size = srcImage->getSize();
	
	if (!srcFormat.isCompressed() && !dstFormat.isCompressed())
	{
		// both formats are uncompressed
		this->convertPixels(srcFormat, srcImage->getData<void>(srcImageIndex),
			dstFormat, dstMode, dstData, dstFormat.getMemorySize(), Image::calcNumPixels(size, srcNumMipmaps));
	}
	else
	{
		// one format is compressed
		if (srcFormat == dstFormat)
		{
			memcpy(dstData, srcImage->getData<void>(srcImageIndex), Image::calcMemorySize(srcFormat, size, srcNumMipmaps));
			return;
		}

		// iterate over mipmaps
		for (int mipmapIndex = 0; mipmapIndex < srcNumMipmaps; ++mipmapIndex)
		{
			Pointer<Image> currentImage = srcImage;
			int currentMipmapIndex = mipmapIndex;
			int currentImageIndex = srcImageIndex;
			
			if (srcFormat.mapping >= ImageFormat::DXT1 && srcFormat.mapping <= ImageFormat::DXT5)
			{
				// source is DXT1, DXT3 or DXT5: decompress
			#ifdef HAVE_S3TC
				ImageFormat byteFormat = ImageFormat(ImageFormat::XYZW8, ImageFormat::UNORM, ImageFormat::RGBA);
				Pointer<Image> byteImage = new Image(srcImage->getType(), byteFormat, size);

				//!
			
				// set current
				currentImage = byteImage;
				currentMipmapIndex = 0;
				currentImageIndex = 0;
			#endif
			}
			else if (srcFormat.mapping >= ImageFormat::PVRTC2 && srcFormat.mapping <= ImageFormat::PVRTC4)
			{
				// source is PVRTC with 2 or 4 bits per pixel: decompress
			#ifdef HAVE_PVRTC
				ImageFormat tmpFormat = ImageFormat(ImageFormat::XYZW8, ImageFormat::UNORM, ImageFormat::RGBA);
				Pointer<Image> tmpImage = new Image(srcImage->getType(), tmpFormat, size);

				convertMipmapFromPVRTC(this, currentImage, size, currentMipmapIndex, currentImageIndex,
					tmpFormat, tmpImage->getData<void>());

				// set current
				currentImage = tmpImage;
				currentMipmapIndex = 0;
				currentImageIndex = 0;
			#endif
			}
			
			if (dstFormat.mapping >= ImageFormat::DXT1 && dstFormat.mapping <= ImageFormat::DXT5)
			{
				// destination is DXT1, DXT3 or DXT5: compress
			#ifdef HAVE_S3TC
				convertMipmapToS3TC(this, currentImage, size, currentMipmapIndex, currentImageIndex,
					dstFormat, dstData);
			#endif
			}
			else if (dstFormat.mapping >= ImageFormat::PVRTC2 && dstFormat.mapping <= ImageFormat::PVRTC4)
			{
				// destination is PVRTC with 2 or 4 bits per pixel: compress
			#ifdef HAVE_PVRTC
				convertMipmapToPVRTC(this, currentImage, size, currentMipmapIndex, currentImageIndex,
					dstFormat, dstData);
			#endif				
			}
			else
			{
				// destination is pixel format: convert from current image into destination data
				this->convertPixels(currentImage->getFormat(), currentImage->getData<void>(currentMipmapIndex, currentImageIndex),
					dstFormat, dstMode, dstData, dstFormat.getMemorySize(), Image::calcNumPixels(size));
			}
			
			// next mipmap level
			dstData = (uint8_t*)dstData + Image::calcMemorySize(dstFormat, size);
			size = max(size >> 1, 1);
		}
	}	
}

void ImageConverter::convert(
	Pointer<Image> srcImage,
	Pointer<Image> dstImage,
	int srcStartImage, int numImages,
	int dstStartImage)
{
	ImageFormat dstFormat = dstImage->getFormat();
	int3 size = dstImage->getSize();
	int numMipmaps = dstImage->getNumMipmaps();
	
	// clamp numImages
	numImages = std::min(numImages, std::min(
		srcImage->getNumImages() - srcStartImage,
		dstImage->getNumImages() - dstStartImage));

	// chck if rescale necessary
	bool rescale = any(size != srcImage->getSize());
	
	// check if we can convert directly
	if (!rescale && (numMipmaps <= srcImage->getNumMipmaps()))
	{
		// convert directly if size is equal and source image has enough mipmaps
		
		// convert one image at a time since the source image may have mipmaps while the destination image doesn't have mipmaps
		for (int imageIndex = 0; imageIndex < numImages; ++imageIndex)
		{
			this->convertImage(srcImage, numMipmaps, srcStartImage + imageIndex,
				dstFormat, DataConverter::NATIVE, dstImage->getData<void>(dstStartImage + imageIndex));
		}
	}
	else
	{
		// a temporary float4 image is needed
		ImageFormat floatFormat = ImageFormat(ImageFormat::XYZW32, ImageFormat::FLOAT, ImageFormat::RGBA);
		Pointer<Image> floatImage = new Image(srcImage->getType(), floatFormat, srcImage->getSize());
		
		// convert one image at a time since the source image may have mipmaps while the destination image doesn't have mipmaps
		for (int imageIndex = 0; imageIndex < numImages; ++imageIndex)
		{
			// convert source image to float format
			this->convertImage(srcImage, 1, srcStartImage + imageIndex,
				floatFormat, DataConverter::NATIVE, floatImage->getData<void>());
			
			// check if rescaling of the top-level image is necessary
			//Pointer<Image> mipmap = rescale ? scaleFiltered(floatImage, size) : floatImage;
			Pointer<Image> mipmap = floatImage;
			if (rescale)
			{
				// scale down by no more than factor of 2
				int3 currentSize = srcImage->getSize();
				do
				{
					currentSize = max(size, currentSize >> 1);
					mipmap = scaleFiltered(floatImage, currentSize);
				} while (any(currentSize != size));
			}

			// convert toplevel float image to destination format
			this->convertImage(mipmap, 1, 0,
				dstFormat, DataConverter::NATIVE, dstImage->getData<void>(dstStartImage + imageIndex));

			// generate mipmaps
			for (int mipmapIndex = 1; mipmapIndex < numMipmaps; ++mipmapIndex)
			{
				size = max(size >> 1, 1);
				mipmap = scaleFiltered(mipmap, size);
				
				// convert scaled mipmap float image to destination format
				this->convertImage(mipmap, 1, 0,
					dstFormat, DataConverter::NATIVE, dstImage->getData<void>(mipmapIndex, dstStartImage + imageIndex));
			}
		}
	}
}

Pointer<Image> ImageConverter::convert(Pointer<Image> srcImage,
	ImageFormat dstFormat, int3 size, bool mipmaps, int startImage, int numImages)
{
	int numMipmaps = mipmaps ? Image::calcNumMipmaps(size) : 1;
	Pointer<Image> dstImage = new Image(srcImage->getType(), dstFormat, size, numMipmaps, numImages);
	
	this->convert(srcImage, dstImage, startImage, numImages);
	
	return dstImage;
}

void ImageConverter::convert(Pointer<Image> srcImage,
	ImageFormat dstFormat, int3 size, bool mipmaps, DataConverter::Mode dstMode, uint8_t* dstData,
	int startImage, int numImages)
{
	int numMipmaps = mipmaps ? Image::calcNumMipmaps(size) : 1;
	bool rescale = any(size != srcImage->getSize());

	// check if we can convert directly
	if (!rescale && (!mipmaps || srcImage->checkMipmaps()))
	{
		// convert directly if size is equal and either no mipmaps or mipmaps are ok
		
		// convert one image at a time since the source image may have mipmaps while the destination image doesn't have mipmaps
		for (int imageIndex = 0; imageIndex < numImages; ++imageIndex)
		{
			// convert image with mipmaps to destination format
			this->convertImage(srcImage, numMipmaps, imageIndex, dstFormat, dstMode, dstData);
			dstData += Image::calcMemorySize(dstFormat, size, numMipmaps);
		}
	}
	else
	{
		// a temporary float4 image is needed
		ImageFormat floatFormat = ImageFormat(ImageFormat::XYZW32, ImageFormat::FLOAT, ImageFormat::RGBA);
		Pointer<Image> floatImage = new Image(srcImage->getType(), floatFormat, srcImage->getSize());
		
		// convert one image at a time since the source image may have mipmaps while the destination image doesn't have mipmaps
		for (int imageIndex = 0; imageIndex < numImages; ++imageIndex)
		{
			// convert source image to float format
			this->convertPixels(srcImage->getFormat(), srcImage->getData<void>(startImage + imageIndex),
				floatFormat, floatImage->getData<void>(), floatImage->getImageNumPixels());
			
			// check if rescaling of the top-level image is necessary
			Pointer<Image> mipmap = rescale ? scaleFiltered(floatImage, size) : floatImage;

			// convert toplevel float image to destination format
			this->convertImage(mipmap, 1, 0, dstFormat, dstMode, dstData);
			dstData += Image::calcMemorySize(dstFormat, size);

			// generate mipmaps
			for (int mipmapIndex = 1; mipmapIndex < numMipmaps; ++mipmapIndex)
			{
				size = max(size >> 1, 1);
				mipmap = scaleFiltered(mipmap, size);
				
				// convert scaled mipmap float image to destination format
				this->convertImage(mipmap, 1, 0, dstFormat, dstMode, dstData);
				dstData += Image::calcMemorySize(dstFormat, size);
			}
		}
	}
}


void ImageConverter::buildConversion(ConverterWriter& cw, ImageFormat srcFormat, ImageFormat dstFormat)
{
	if (srcFormat.mapping == dstFormat.mapping)
		return;
		
	int swizzle1 = 0x3210;
	bool matrixUsed = false;
	float4x4 matrix = float4x4Identity();
	
	switch (srcFormat.mapping)
	{
	case ImageFormat::R:
		// set unused channels
		cw.setValue(1, 0.0f);
		cw.setValue(3, 1.0f);
		swizzle1 = 0x3110;
		break;
	case ImageFormat::G:
		// set unused channels
		cw.setValue(1, 0.0f);
		cw.setValue(3, 1.0f);
		swizzle1 = 0x3101;
		break;
	case ImageFormat::B:
		// set unused channels
		cw.setValue(1, 0.0f);
		cw.setValue(3, 1.0f);
		swizzle1 = 0x3011;
		break;
	case ImageFormat::A:
		// set unused channels
		cw.setValue(1, 0.0f);
		swizzle1 = 0x0111;
		break;
	case ImageFormat::RGB:
		// set alpha to 1.0
		cw.setValue(3, 1.0f);
		break;
	case ImageFormat::RGBA:
		break;
	case ImageFormat::BGR:
		// set alpha to 1.0
		cw.setValue(3, 1.0f);
		swizzle1 = 0x3012;
		break;
	case ImageFormat::BGRA:
		swizzle1 = 0x3012;
		break;
	case ImageFormat::Y:
		// set alpha to 1.0
		cw.setValue(3, 1.0f);
		swizzle1 = 0x3000;
		break;
	case ImageFormat::YA:
		swizzle1 = 0x1000;
		break;
	/*case ImageFormat::YUV:
		// set alpha to 1.0
		cw.setValue(3, 1.0f);
		matrixUsed = true;
		matrix = matrix4x4(
				1.0f,     1.0f,      1.0f,     0.0f,
				0.0f,     -0.39465f, 2.03211f, 0.0f,
				1.13983f, -0.58060f, 0.0f,     0.0f,
				0.0f,     0.0f,      0.0f,     1.0f);
		break;*/
	default:
		// compressed formats not allowed
		break;
	}
	
	int swizzle2 = 0x3210;
	switch (dstFormat.mapping)
	{
	case ImageFormat::R:
		swizzle2 = 0x3210;
		break;
	case ImageFormat::G:
		swizzle2 = 0x3021;
		break;
	case ImageFormat::B:
		swizzle2 = 0x3102;
		break;
	case ImageFormat::A:
		swizzle2 = 0x0123;
		break;
	case ImageFormat::RGB:
	case ImageFormat::RGBA:
		swizzle2 = 0x3210;
		break;
	case ImageFormat::BGR:
	case ImageFormat::BGRA:
		swizzle2 = 0x3012;
		break;
	case ImageFormat::Y:
	case ImageFormat::YA:
		matrixUsed = true;
		matrix = matrix4x4(
				0.299f, 0.0f, 0.0f, 0.0f,
				0.587f, 0.0f, 0.0f, 0.0f,
				0.114f, 0.0f, 0.0f, 0.0f,
				0.0f,   1.0f, 0.0f, 0.0f
			) * matrix;
			break;
	/*case ImageFormat::YUV:
		matrixUsed = true;
		matrix = matrix4x4(
				0.299f, -0.14713f, 0.615f,    0.0f,
				0.587f, -0.28886f, -0.51499f, 0.0f,
				0.114f, 0.436f,    -0.10001f, 0.0f,
				0.0f,   0.0f,      0.0f,      1.0f
			) * matrix;
		break;*/
	default:
		// compressed formats not allowed
		break;
	}
	
	cw.permute(swizzle1);
	if (matrixUsed)
		cw.transform(matrix);	
	cw.permute(swizzle2);
}

} // namespace digi
