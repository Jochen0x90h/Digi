#include <vector>
#include <boost/detail/endian.hpp>

#include <digi/Utility/VectorUtility.h>
#include <digi/System/File.h>
#include <digi/System/IOCatcher.h>
#include <digi/Data/DataException.h>

#include <webp/decode.h>
#include <webp/encode.h>

#include "WebPWrapper.h"

// https://developers.google.com/speed/webp/docs/api


namespace digi {

Pointer<Image> loadWebP(const fs::path& path)
{
	Pointer<File> f = File::open(path, File::READ);		
	Pointer<Image> image = loadWebP(f);
	f->close();
	return image;
}

Pointer<Image> loadWebP(Pointer<IODevice> dev)
{

	return null;
}

void saveWebP(const fs::path& path, Pointer<Image> image, int quality, int mipmapIndex, int imageIndex)
{
	Pointer<File> f = File::create(path);
	saveWebP(f, image, quality, mipmapIndex, imageIndex);
	f->close();
}

void saveWebP(Pointer<IODevice> dev, Pointer<Image> image, int quality, int mipmapIndex, int imageIndex)
{
	if (mipmapIndex < 0)
		mipmapIndex = 0;

	// determine size
	int width = max(image->getWidth() >> mipmapIndex, 1);
	int height = max(image->getHeight() >> mipmapIndex, 1);

	// get image format
	ImageFormat format = image->getFormat();
	const BufferFormat::LayoutInfo& info = format.getLayoutInfo();
	int numChannels = info.numChannels;
	
	// get image data
	uint8_t* data = image->getData<uint8_t>(mipmapIndex, imageIndex);
	int stride = width * numChannels * info.componentSize;

	uint8_t* output = NULL;
	size_t size = 0;
	if ((format.type == ImageFormat::UNORM || format.type == ImageFormat::UINT)
		&& info.type == BufferFormat::STANDARD)
	{
		if (numChannels == 1) // write one component as gray independent of mapping
		{
			// gray
			int numPixels = width * height;
			uint8_t* rgb = new uint8_t[numPixels * 3];
			uint8_t* src = data;
			uint8_t* dst = rgb;
			for (int i = 0; i < numPixels; ++i)
			{
				uint8_t y = src[0];
				dst[0] = y;
				dst[1] = y;
				dst[2] = y;
				++src;
				dst += 3;
			}
			size = WebPEncodeRGB(rgb, width, height, width * 3, quality, &output);
			delete [] rgb;
		}
		else if (numChannels == 2 && format.mapping == ImageFormat::YA)
		{
			// gray, alpha
			int numPixels = width * height;
			uint8_t* rgba = new uint8_t[numPixels * 4];
			uint8_t* src = data;
			uint8_t* dst = rgba;
			for (int i = 0; i < numPixels; ++i)
			{
				uint8_t y = src[0];
				uint8_t a = src[1];
				dst[0] = y;
				dst[1] = y;
				dst[2] = y;
				dst[3] = a;
				src += 2;
				dst += 4;
			}
			size = WebPEncodeRGBA(rgba, width, height, width * 4, quality, &output);
			delete [] rgba;
		}
		else if (numChannels == 3)
		{
			// color
			if (format.mapping == ImageFormat::RGB)
				size = WebPEncodeRGB(data, width, height, stride, quality, &output);
			//else if (format.mapping == ImageFormat::BGR)
			//	size = WebPEncodeBGR(data, width, height, stride, quality, &output);
		}
		else if (numChannels == 4)
		{
			// color, alpha
			if (format.mapping == ImageFormat::RGBA)
				size = WebPEncodeRGBA(data, width, height, stride, quality, &output);
			//else if (format.mapping == ImageFormat::BGRA)
			//	size = WebPEncodeBGRA(data, width, height, stride, quality, &output);
		}
	}
	
	// check if valid
	if (size == 0)
		throw DataException(dev, DataException::FORMAT_NOT_SUPPORTED);

	// write
	dev->write(output, size);
}

} // namespace digi
