#ifndef digi_ImageConvert_ImageConverter_h
#define digi_ImageConvert_ImageConverter_h

#include <map>

#include <digi/Image/Image.h>
#include "DataConverter.h"


namespace digi {

/// @addtogroup ImageConvert
/// @{

/**
	image converter. use like this:

	// create context and converter (can be used for multiple conversions)
	Pointer<ConverterContext> context = new ConverterContext();
	Pointer<ImageConverter> converter = new ImageConverter(context);

	// convert to float luminance of size 256x256
	image = converter->convert(image,
		ImageFormat(ImageFormat::X32, ImageFormat::FLOAT, ImageFormat::Y),
		vector3(256, 256, 1), false);

*/
class ImageConverter : public Object
{
public:

	ImageConverter(Pointer<ConverterContext> context)
		: context(context) {}
	virtual ~ImageConverter();
	
	/// get a converter that converts pixels of srcFormat to a byte buffer
	Pointer<DataConverter> getPixelConverter(ImageFormat srcFormat, ImageFormat dstFormat, DataConverter::Mode dstMode);

	/// convert pixels. no blocky image/compressed formats allowed.
	void convertPixels(
		ImageFormat srcFormat, void* srcData,
		ImageFormat dstFormat, void* dstData,
		size_t numPixels);

	/// convert pixels into byte buffer. no blocky/compressed image formats allowed.
	void convertPixels(
		ImageFormat srcFormat, void* srcData,
		ImageFormat dstFormat, DataConverter::Mode dstMode, void* dstData, size_t dstStride,
		size_t numPixels);


	/// convert one image (with mipmaps). blocky/compressed image formats allowed.
	void convertImage(
		Pointer<Image> srcImage, int srcNumMipmaps, int srcImageIndex,
		ImageFormat dstFormat, DataConverter::Mode dstMode, void* dstData);


	/// convert an image to destination format and size and optionally create mipmaps
	void convert(
		Pointer<Image> srcImage,
		Pointer<Image> dstImage,
		int srcStartImage = 0, int numImages = 1,
		int dstStartImage = 0);

	/// convert an image to destination format and size and optionally create mipmaps
	Pointer<Image> convert(
		Pointer<Image> srcImage,
		ImageFormat dstFormat, int3 size, bool mipmaps,
		int startImage = 0, int numImages = 1);
	
	/// convert an image into byte buffer with given destination format and size and optionally create mipmaps
	void convert(
		Pointer<Image> srcImage,
		ImageFormat dstFormat, int3 size, bool mipmaps, DataConverter::Mode dstMode, uint8_t* dstData,
		int startImage = 0, int numImages = 1);

protected:

	static void buildConversion(ConverterWriter& cw, ImageFormat srcFormat, ImageFormat dstFormat);

	// context (the llvm jit)
	Pointer<ConverterContext> context;

	// converter cache (crc32 of converter parameters -> converter)
	std::map<uint32_t, Pointer<DataConverter> > converters;
};	

/// @}

} // namespace Digi

#endif
