#include <FreeImage.h>

#include <digi/System/File.h>
#include <digi/Data/DataException.h>
#include <digi/Image/DDS.h>
#include <digi/Image/JPEGWrapper.h>
#include <digi/Image/PNGWrapper.h>
#include <digi/Image/TIFFWrapper.h>

#include "ImageIO.h"


namespace digi {

enum Origin
{
	// origin top-left (e.g. directx)
	TOP_LEFT,
	
	// origin bottom-left (e.g. opengl)
	BOTTOM_LEFT
};


// use unicode versions of freeimage file functions if on windows
#if defined(_WIN32)
	#define FreeImage_GetFIFFromFilename FreeImage_GetFIFFromFilenameU
	#define FreeImage_Load               FreeImage_LoadU
	#define FreeImage_Save               FreeImage_SaveU
#endif

Pointer<Image> loadImage(const fs::path& path)
{	
	Origin origin = TOP_LEFT;

	// guess file format
	FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(path.c_str());
	if (fif == FIF_UNKNOWN)
		throw DataException(File::getDummyFile(path), DataException::FORMAT_NOT_SUPPORTED);
	
	// do some formats directly
	switch (fif)
	{
	case FIF_DDS:
		return loadDDS(path);
	case FIF_PNG:
		return loadPNG(path);
	case FIF_JPEG:
		return loadJPEG(path);
	case FIF_TIFF:
		return loadTIFF(path);
	}

	if (FreeImage_FIFSupportsReading(fif))
	{
		// load using FreeImage

		FIBITMAP* freeImage = FreeImage_Load(fif, path.c_str(), 0);
		if (freeImage == NULL)
		{
			Pointer<File> dummy = File::getDummyFile(path);

			// throw IOException if a low level error occured
			File::throwException(dummy);
			
			// otherwise throw DataException
			throw DataException(dummy, DataException::DATA_CORRUPT);
		}
		
		// convert pixel format
		FREE_IMAGE_TYPE type = FreeImage_GetImageType(freeImage);
		ImageFormat format;
		switch (type)
		{
			case FIT_BITMAP: // Standard freeImage: 1-, 4-, 8-, 16-, 24-, 32-bit
				{
					int bpp = FreeImage_GetBPP(freeImage);
					
					if (bpp == 8)
					{
						// gray
						format = ImageFormat(ImageFormat::X8, ImageFormat::UNORM, ImageFormat::Y);
					}
					else if (bpp == 16)
					{
						int red_mask = FreeImage_GetRedMask(freeImage);
						int green_mask = FreeImage_GetGreenMask(freeImage);
						int blue_mask = FreeImage_GetBlueMask(freeImage);
					
						if (red_mask == FI16_565_RED_MASK && green_mask == FI16_565_GREEN_MASK && blue_mask == FI16_565_BLUE_MASK)
							format = ImageFormat(ImageFormat::X5Y6Z5, ImageFormat::UNORM, ImageFormat::BGR);
						else
							format = ImageFormat(ImageFormat::X5Y5Z5W1, ImageFormat::UNORM, ImageFormat::BGR);
					}
					else if (bpp == 24)
					{
						format = ImageFormat(ImageFormat::XYZ8, ImageFormat::UNORM,
							FI_RGBA_RED == 0 ? ImageFormat::RGB : ImageFormat::BGR);
					}
					else if (bpp == 32)
					{
						format = ImageFormat(ImageFormat::XYZW8, ImageFormat::UNORM,
							FI_RGBA_RED == 0 ? ImageFormat::RGBA : ImageFormat::BGRA);
					}
				}
				break;
			case FIT_UINT16:
				format = ImageFormat(ImageFormat::X16, ImageFormat::UNORM, ImageFormat::Y);
				break;
			case FIT_INT16:
				format = ImageFormat(ImageFormat::X16, ImageFormat::NORM, ImageFormat::Y);
				break;
			case FIT_UINT32:
				format = ImageFormat(ImageFormat::X32, ImageFormat::UINT, ImageFormat::Y);
				break;
			case FIT_INT32:
				format = ImageFormat(ImageFormat::X32, ImageFormat::INT, ImageFormat::Y);
				break;
			case FIT_FLOAT:
				format = ImageFormat(ImageFormat::X32, ImageFormat::FLOAT, ImageFormat::Y);
				break;
			case FIT_DOUBLE:
				break;
			case FIT_COMPLEX: // 2 x 64-bit IEEE floating point
				break;
			case FIT_RGB16:
				format = ImageFormat(ImageFormat::XYZ16, ImageFormat::UNORM, ImageFormat::RGB);
				break;
			case FIT_RGBA16:
				format = ImageFormat(ImageFormat::XYZW16, ImageFormat::UNORM, ImageFormat::RGBA);
				break;
			case FIT_RGBF:
				format = ImageFormat(ImageFormat::XYZ32, ImageFormat::FLOAT, ImageFormat::RGB);
				break;
			case FIT_RGBAF:
				format = ImageFormat(ImageFormat::XYZW32, ImageFormat::FLOAT, ImageFormat::RGBA);
				break;
			default:
				break;
		}
		
		// check if format supported
		if (format.layout == ImageFormat::INVALID_LAYOUT)
			throw DataException(File::getDummyFile(path), DataException::FORMAT_NOT_SUPPORTED);
		
		// freeImage is source
		const uint8_t *srcData = (uint8_t*)FreeImage_GetBits(freeImage);
		int width = FreeImage_GetWidth(freeImage);
		int height = FreeImage_GetHeight(freeImage);
		size_t pitch = FreeImage_GetPitch(freeImage);
		
		// our image is destination
		Pointer<Image> image = new Image(Image::IMAGE, format, width, height);
		//Image image(Image::DIM_2D, dataType, format, width, height);
		uint8_t* dstData = image->getData<uint8_t>();
		size_t lineSize = width * format.getMemorySize();
		if (origin == TOP_LEFT)
			dstData += image->getMemorySize() - lineSize;

		// copy lines
		for (int i = 0; i < height; i++)
		{
			memcpy(dstData, srcData, lineSize);

			// next line
			srcData += pitch;
			if (origin == TOP_LEFT)
				dstData -= lineSize;
			else
				dstData += lineSize;
		}

		FreeImage_Unload(freeImage);
		
		return image;
	}

	throw DataException(File::getDummyFile(path), DataException::FORMAT_NOT_SUPPORTED);
}

void saveImage(const fs::path& path, Pointer<Image> image, int mipmapIndex, int imageIndex)
{
	Origin origin = TOP_LEFT;

	// guess file format
	FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(path.c_str());
	if (fif == FIF_UNKNOWN)
		throw DataException(File::getDummyFile(path), DataException::FORMAT_NOT_SUPPORTED);

	if (fif == FIF_DDS)
	{
		// save dds
		saveDDS(path, image, mipmapIndex, imageIndex);
		return;
	}
	else if (fif == FIF_PNG)
	{
		// save png
		savePNG(path, image, mipmapIndex, imageIndex);
		return;
	}
	else if (fif == FIF_JPEG)
	{
		// save jpeg
		saveJPEG(path, image, 80, mipmapIndex, imageIndex);
		return;
	}
	else if (fif == FIF_TIFF)
	{
		// save jpeg
		saveTIFF(path, image, mipmapIndex, imageIndex);
		return;
	}

	if (FreeImage_FIFSupportsWriting(fif))
	{
		// load using FreeImage
		mipmapIndex = max(mipmapIndex, 0);
		int width = max(image->getWidth() >> mipmapIndex, 1);
		int height = max(image->getHeight() >> mipmapIndex, 1);
		
		// tetermine freeimage format
		ImageFormat format = image->getFormat();
		FREE_IMAGE_TYPE type;
		int bpp = 0;
		if (format.type == ImageFormat::UNORM)
		{
			if (format.layout == ImageFormat::X8 && format.mapping == ImageFormat::Y)
			{
				// gray
				type = FIT_BITMAP;
				bpp = 8;
			}
			else if (format.layout == ImageFormat::XYZ8)
			{
				type = FIT_BITMAP;
				bpp = 24;
			}
			else if (format.layout == ImageFormat::XYZW8)
			{
				type = FIT_BITMAP;
				bpp = 32;
			}
		}

		// check if format is supported
		if (bpp == 0)
			throw DataException(File::getDummyFile(path), DataException::FORMAT_NOT_SUPPORTED);

		// our image is source
		const uint8_t* srcData = image->getData<uint8_t>(mipmapIndex, imageIndex);
		size_t lineSize = width * format.getMemorySize();
		if (origin == TOP_LEFT)
			srcData += image->getMipmapMemorySize(mipmapIndex) - lineSize;

		// freeImage is destination
		FIBITMAP* freeImage = FreeImage_AllocateT(type, width, height, bpp);
		uint8_t *dstData = (uint8_t*)FreeImage_GetBits(freeImage);
		size_t pitch = FreeImage_GetPitch(freeImage);

		// copy lines
		for (int i = 0; i < height; i++)
		{
			memcpy(dstData, srcData, lineSize);

			// next line
			if (origin == TOP_LEFT)
				srcData -= lineSize;
			else
				srcData += lineSize;
			dstData += pitch;
		}

		// save image
		bool result = FreeImage_Save(fif, freeImage, path.c_str()) != 0;
		
		FreeImage_Unload(freeImage);
		
		// throw exception if save failed
		if (!result)
		{
			Pointer<File> dummy = File::getDummyFile(path);

			// throw IOException if a low level error occured
			File::throwException(dummy);
			
			// otherwise throw DataException
			throw DataException(dummy, DataException::DATA_CORRUPT);			
		}
		return;
	}

	throw DataException(File::getDummyFile(path), DataException::FORMAT_NOT_SUPPORTED);
}

} // namespace digi
