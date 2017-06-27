#include <vector>
#include <boost/detail/endian.hpp>

#include <digi/Utility/IntUtility.h>
#include <digi/Utility/VectorUtility.h>
#include <digi/System/File.h>
#include <digi/System/IOCatcher.h>
#include <digi/Data/DataException.h>

#include <png.h>
#include <libimagequant.h>

#include "PNGWrapper.h"

#ifdef _WIN32
	#define fopen(path, mode) _wfopen(path, L##mode)
#endif

// http://www.libpng.org/pub/png/libpng-1.4.0-manual.pdf
// http://www.libpng.org/pub/png/pngsuite.html


namespace digi {

namespace
{
	// custom read/write functions
	void pngRead(png_structp png_ptr, png_bytep data, png_size_t size)
	{
		static_cast<IOCatcher*>(png_get_io_ptr(png_ptr))->read(data, size);
	}

	void pngWrite(png_structp png_ptr, png_bytep data, png_size_t size)
	{
		static_cast<IOCatcher*>(png_get_io_ptr(png_ptr))->write(data, size);
	}

	void pngFlush(png_structp png_ptr)
	{
	}

	// guard that does cleanup on exception
	struct ReadGuard
	{
		png_structp png_ptr;
		png_infop info_ptr;

		ReadGuard(png_structp png_ptr, png_infop info_ptr)
			: png_ptr(NULL), info_ptr(NULL)
		{
		}

		~ReadGuard()
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		}
	};

	// guard that does cleanup on exception
	struct WriteGuard
	{
		png_structp png_ptr;
		png_infop info_ptr;

		WriteGuard(png_structp png_ptr, png_infop info_ptr)
			: png_ptr(png_ptr), info_ptr(info_ptr)
		{
		}

		~WriteGuard()
		{
			png_destroy_write_struct(&png_ptr, &info_ptr);
		}
	};
	
	void yCallback(liq_color row_out[], int row_index, int width, void *user_info)
	{
		uint8_t* src = (uint8_t*)user_info + row_index * width;
		liq_color* dst = row_out;
		for (int i = 0; i < width; ++i)
		{
			dst->r = dst->g = dst->b = src[0];
			dst->a = 255;
			++src;
			++dst;
		}
	}

	void yaCallback(liq_color row_out[], int row_index, int width, void *user_info)
	{
		uint8_t* src = (uint8_t*)user_info + row_index * width * 2;
		liq_color* dst = row_out;
		for (int i = 0; i < width; ++i)
		{
			dst->r = dst->g = dst->b = src[0];
			dst->a = src[1];
			src += 2;
			++dst;
		}
	}

	void rgbCallback(liq_color row_out[], int row_index, int width, void *user_info)
	{
		uint8_t* src = (uint8_t*)user_info + row_index * width * 3;
		liq_color* dst = row_out;
		for (int i = 0; i < width; ++i)
		{
			dst->r = src[0];
			dst->g = src[1];
			dst->b = src[2];
			dst->a = 255;
			src += 3;
			++dst;
		}
	}
/*
	void bgrCallback(liq_color row_out[], int row_index, int width, void *user_info)
	{
		uint8_t* src = (uint8_t*)user_info + row_index * width * 3;
		liq_color* dst = row_out;
		for (int i = 0; i < width; ++i)
		{
			dst->r = src[2];
			dst->g = src[1];
			dst->b = src[0];
			dst->a = 255;
			src += 3;
			++dst;
		}
	}

	void bgraCallback(liq_color row_out[], int row_index, int width, void *user_info)
	{
		uint8_t* src = (uint8_t*)user_info + row_index * width * 4;
		liq_color* dst = row_out;
		for (int i = 0; i < width; ++i)
		{
			dst->r = src[2];
			dst->g = src[1];
			dst->b = src[0];
			dst->a = src[3];
			src += 4;
			++dst;
		}
	}
*/
} // anonymous namespace


Pointer<Image> loadPNG(const fs::path& path)
{
	Pointer<File> f = File::open(path, File::READ);		
	Pointer<Image> image = loadPNG(f);
	f->close();
	return image;
}

Pointer<Image> loadPNG(Pointer<IODevice> dev)
{
	// init
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
		throw std::bad_alloc();		
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		throw std::bad_alloc();

	// set io functions
	IOCatcher ioCatcher(dev);
	png_set_read_fn(png_ptr, &ioCatcher, pngRead);

	// automatic cleanup on exception
	ReadGuard guard(png_ptr, info_ptr);
	
	// declare variables before setjmp so that their constructor gets called in setjmp
	ImageFormat format;
	Pointer<Image> image;
	std::vector<png_bytep> rowPointers;

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		// check if a write error occured
		ioCatcher.checkState();

		// other error, assume data corrupt
		throw DataException(dev, DataException::DATA_CORRUPT);
	}

	// read header
	png_read_info(png_ptr, info_ptr);
	png_uint_32 width;
	png_uint_32 height;
	int numBits;
	int colorType;
	int interlaceType;
	int compressionType;
	int filterMethod;
	png_get_IHDR(png_ptr,  info_ptr,  &width,  &height, &numBits,  &colorType,
		&interlaceType, &compressionType,  &filterMethod);
	int numBytes = (numBits + 7) >> 3;
	bool tRNS = png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS) != 0;

	// set transforms
	#ifdef BOOST_LITTLE_ENDIAN
		if (numBits > 8)
			png_set_swap(png_ptr);
	#endif
	if (numBits < 8)
		png_set_expand(png_ptr);
	if (tRNS)
		png_set_tRNS_to_alpha(png_ptr);

	// determine image format
	switch (colorType)
	{
	case PNG_COLOR_TYPE_PALETTE:
		{
			// get palette
			png_color* palette;
			int numPalette;
			png_get_PLTE(png_ptr, info_ptr, &palette,
				&numPalette);

			// check if palette is gray
			bool gray = true;
			png_color* end = palette + numPalette;
			for (png_color* col = palette; col != end; ++col)
			{
				gray &= (col->red == col->green) & (col->red == col->blue);
			}

			// set conversions
			png_set_palette_to_rgb(png_ptr);
			if (gray)
				png_set_rgb_to_gray_fixed(png_ptr, 1, 100000, 0);

			// determine mapping
			if (gray)
				format.mapping = tRNS ? ImageFormat::YA : ImageFormat::Y;
			else
				format.mapping = tRNS ? ImageFormat::RGBA : ImageFormat::RGB;
		}
		break;
	case PNG_COLOR_TYPE_GRAY:
		format.mapping = tRNS ? ImageFormat::YA : ImageFormat::Y;
		break;
	case PNG_COLOR_TYPE_GRAY_ALPHA:
		format.mapping = ImageFormat::YA;
		break;
	case PNG_COLOR_TYPE_RGB:
		format.mapping = tRNS ? ImageFormat::RGBA : ImageFormat::RGB;
		break;
	case PNG_COLOR_TYPE_RGB_ALPHA:
		format.mapping = ImageFormat::RGBA;
		break;
	}

	switch (format.mapping)
	{
	case ImageFormat::Y:
		format.layout = numBits > 8 ? ImageFormat::X16 : ImageFormat::X8;
		break;
	case ImageFormat::YA:
		format.layout = numBits > 8 ? ImageFormat::XY16 : ImageFormat::XY8;
		break;
	case ImageFormat::RGB:
		format.layout = numBits > 8 ? ImageFormat::XYZ16 : ImageFormat::XYZ8;
		break;
	case ImageFormat::RGBA:
		format.layout = numBits > 8 ? ImageFormat::XYZW16 : ImageFormat::XYZW8;
		break;
	}
	format.type = ImageFormat::UNORM;
	int channelCount = format.getNumChannels();

	// read data
	image = new Image(Image::IMAGE, format, width, height);
	uint8_t* data = image->getData<uint8_t>();
	int stride = width * channelCount * numBytes;
	rowPointers.resize(height);
	for (uint row = 0; row < height; ++row)
		rowPointers[row] = (png_bytep)(data + row * stride);
	png_read_image(png_ptr, rowPointers.data());
	png_read_end(png_ptr, NULL);

	// the guard closes the png image, also when an exception is thrown
	return image;
}

void savePNG(const fs::path& path, Pointer<Image> image, int quality, int mipmapIndex, int imageIndex)
{
	Pointer<File> f = File::create(path);
	savePNG(f, image, quality, mipmapIndex, imageIndex);
	f->close();
}

void savePNG(Pointer<IODevice> dev, Pointer<Image> image, int quality, int mipmapIndex, int imageIndex)
{
	if (mipmapIndex < 0)
		mipmapIndex = 0;

	// determine size
	size_t width = max(image->getWidth() >> mipmapIndex, 1);
	size_t height = max(image->getHeight() >> mipmapIndex, 1);
	
	// determine png format
	ImageFormat format = image->getFormat();
	int componentSize = format.getComponentSize();
	int channelCount = format.getNumChannels();
	int colorType = -1;
	bool filler = false;
	if ((format.type == ImageFormat::UNORM || format.type == ImageFormat::UINT)
		&& format.getLayoutType() == BufferFormat::STANDARD)
	{
		if (channelCount == 1) // write one component as gray independent of mapping
		{
			// gray
			colorType = PNG_COLOR_TYPE_GRAY;
		}
		else if (channelCount == 2 && format.mapping == ImageFormat::YA)
		{
			// gray, alpha
			colorType = PNG_COLOR_TYPE_GRAY_ALPHA;
		}
		else if (channelCount >= 3 && (format.mapping == ImageFormat::RGB))// || format.mapping == ImageFormat::BGR))
		{
			// color
			colorType = PNG_COLOR_TYPE_RGB;
			filler = channelCount == 4;
		}
		else if (channelCount == 4 && (format.mapping == ImageFormat::RGBA))// || format.mapping == ImageFormat::BGRA))
		{
			// color, alpha
			colorType = PNG_COLOR_TYPE_RGB_ALPHA;
		}
	}
	
	// check if valid (only up to 16 bit supported)
	if (componentSize > 2 || colorType == -1)
		throw DataException(dev, DataException::FORMAT_NOT_SUPPORTED);
		
	// reduce to 8 bit
	if (quality < 95 && componentSize > 1)
	{
		ImageFormat format8(ImageFormat::X8, channelCount, format.type, format.mapping);
		Pointer<Image> image8 = new Image(Image::IMAGE, format8, width, height);
		
		// convert from 16 bit to 8 bit
		uint16_t* src = image->getData<uint16_t>(mipmapIndex, imageIndex);
		uint8_t* dst = image8->getData<uint8_t>();
		size_t valueCount = width * height * channelCount;
		for (int i = 0; i < valueCount; ++i)
		{
			*dst = *src >> 8;
			++src;
			++dst;
		}
		
		// use reduced image
		image = image8;
		mipmapIndex = 0;
		imageIndex = 0;
		componentSize = 1;
	}

	int bitCount = componentSize * 8;

	// quantize to palette http://pngquant.org/lib/
	std::vector<png_color> colorPalette;
	std::vector<uint8_t> alphaPalette;
	if (quality < 95)
	{
		liq_attr* attr = liq_attr_create();
		liq_image* qImage;
		if (format.mapping == ImageFormat::Y)
			qImage = liq_image_create_custom(attr, yCallback, image->getData<uint8_t>(), width, height, 0);
		else if (format.mapping == ImageFormat::YA)
			qImage = liq_image_create_custom(attr, yaCallback, image->getData<uint8_t>(), width, height, 0);
		else if (format.mapping == ImageFormat::RGB)
			qImage = liq_image_create_custom(attr, rgbCallback, image->getData<uint8_t>(), width, height, 0);
		//else if (format.mapping == ImageFormat::BGR)
		//	qImage = liq_image_create_custom(attr, bgrCallback, image->getData<uint8_t>(), width, height, 0);
		//else if (format.mapping == ImageFormat::BGRA)
		//	qImage = liq_image_create_custom(attr, bgraCallback, image->getData<uint8_t>(), width, height, 0);
		else
			qImage = liq_image_create_rgba(attr, image->getData<uint8_t>(), width, height, 0);
		
		// set quality
		liq_set_quality(attr, quality, quality + 5);
		
		// quantize
		liq_result* result = liq_quantize_image(attr, qImage);
		
		if (result != NULL)
		{
			ImageFormat format8(ImageFormat::X8, ImageFormat::UINT, ImageFormat::Y);
			Pointer<Image> image8 = new Image(Image::IMAGE, format8, width, height);
			
			size_t pixelCount = width * height;
			liq_set_dithering_level(result, 1.0f);
			if (liq_write_remapped_image(result, qImage, image8->getData<uint8_t>(), pixelCount) == LIQ_OK)
			{
				const liq_palette* palette = liq_get_palette(result);
			
				colorPalette.resize(palette->count);
				alphaPalette.resize(palette->count);
				int alphaCount = 0;
				for (int i = 0; i < palette->count; ++i)
				{
					liq_color entry = palette->entries[i];
					if (entry.a < 255)
						alphaCount = i + 1;
					png_color& color = colorPalette[i];
					color.red = entry.r; color.green = entry.g; color.blue = entry.b;
					alphaPalette[i] = entry.a;
				}
				alphaPalette.resize(alphaCount);
				
				// check if palette image has less bits than original image
				size_t imageBitCount = pixelCount * channelCount * 8;
				size_t paletteBitCount = pixelCount * log2(int(palette->count)) + (palette->count * 3 + alphaCount) * 8;
				if (paletteBitCount < imageBitCount)
				{
					// use palette image
					image = image8;
					mipmapIndex = 0;
					imageIndex = 0;
					channelCount = 1;
					colorType = PNG_COLOR_TYPE_PALETTE;
					
					// reduce bit count if possible
					if (palette->count <= 2)
						bitCount = 1;
					else if (palette->count <= 4)
						bitCount = 2;
					else if (palette->count <= 16)
						bitCount = 4;
				}
			}
			liq_result_destroy(result);
		}
		liq_image_destroy(qImage);
		liq_attr_destroy(attr);
	}
	
	
	// init
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
		throw std::bad_alloc();		
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		throw std::bad_alloc();

	// set io functions
	IOCatcher ioCatcher(dev);
	png_set_write_fn(png_ptr, &ioCatcher, pngWrite, pngFlush);

	// automatic cleanup on exception
	WriteGuard guard(png_ptr, info_ptr);
	
	// declare variables before setjmp so that their destructor gets called in setjmp
	std::vector<png_bytep> rowPointers(height);

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		// check if a write error occured
		ioCatcher.checkState();

		// other error, assume data corrupt
		throw DataException(dev, DataException::DATA_CORRUPT);
	}
			
	// write header
	png_set_IHDR(png_ptr, info_ptr, width, height, bitCount, colorType,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	
	// set palette
	if (colorType == PNG_COLOR_TYPE_PALETTE)
	{
		png_set_PLTE(png_ptr, info_ptr, colorPalette.data(), colorPalette.size());
		if (!alphaPalette.empty())
			png_set_tRNS(png_ptr, info_ptr, alphaPalette.data(), alphaPalette.size(), NULL);
	}

	// write info and check if a write error occured
	png_write_info(png_ptr, info_ptr);
	ioCatcher.checkState();

	// set transforms
#ifdef BOOST_LITTLE_ENDIAN
	if (componentSize > 1)
		png_set_swap(png_ptr);
#endif
	//if (format.mapping == ImageFormat::BGR || format.mapping == ImageFormat::BGRA)
	//	png_set_bgr(png_ptr);
	if (filler)
		png_set_filler(png_ptr, 0, PNG_FILLER_AFTER);
	png_set_packing(png_ptr);
	
	// write data
	uint8_t* data = image->getData<uint8_t>(mipmapIndex, imageIndex);
	size_t stride = width * channelCount * componentSize;
	for (size_t row = 0; row < height; ++row)
		rowPointers[row] = (png_bytep)(data + row * stride);
	png_write_image(png_ptr, rowPointers.data());
	png_write_end(png_ptr, NULL);

	// check if a write error occured
	ioCatcher.checkState();

	// the guard closes the png image, also when an exception is thrown
}

} // namespace digi
