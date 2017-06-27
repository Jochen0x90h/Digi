#include <vector>

#include <digi/Utility/VectorUtility.h>
#include <digi/System/IOCatcher.h>
#include <digi/Data/DataException.h>

#include <tiff.h>
#include <tiffio.h>

#include "TIFFWrapper.h"


// http://www.libtiff.org


namespace digi {

namespace
{
	// custom read/write functions
	tmsize_t tiffRead(thandle_t handle, void* data, tmsize_t size)
	{
		return static_cast<IOCatcher*>(handle)->read(data, size);
	}

	tmsize_t tiffWrite(thandle_t handle, void* data, tmsize_t size)
	{
		return static_cast<IOCatcher*>(handle)->write(data, size);
	}

	toff_t tiffSeek(thandle_t handle, toff_t off, int whence)
	{
		return static_cast<IOCatcher*>(handle)->seek(off, File::PositionMode(whence));
	}

	int tiffClose(thandle_t fd)
	{
		// the tiff library is not responsible for closing the file
		return 0;
	}

	toff_t tiffSize(thandle_t handle)
	{
		return static_cast<IOCatcher*>(handle)->getSize();
	}

	int tiffMap(thandle_t, void** base, toff_t* size)
	{
		return 0;
	}

	void tiffUnmap(thandle_t, void* base, toff_t size)
	{
	}

	TIFF* open(IOCatcher& ioCatcher, const char *mode)
	{		
		// open and set io functions
		TIFF* tif = TIFFClientOpen("tiff", mode, &ioCatcher,
				tiffRead, tiffWrite, tiffSeek, tiffClose, tiffSize, tiffMap, tiffUnmap);
		return tif;
	} 

	// guard that does cleanup on exception
	struct Guard
	{
		TIFF* tif;

		Guard(TIFF* tif)
			: tif(tif)
		{
		}

		~Guard()
		{
			TIFFCleanup(this->tif);
		}
	};

} // anonymous namespace


Pointer<Image> loadTIFF(const fs::path& path)
{
	Pointer<File> f = File::open(path, File::READ);		
	Pointer<Image> image = loadTIFF(f);
	f->close();
	return image;
}

Pointer<Image> loadTIFF(Pointer<IODevice> dev)
{
	// init and set io functions
	IOCatcher ioCatcher(dev);
	TIFF* tif = open(ioCatcher, "r");
	
	// check if a read error occured
	ioCatcher.checkState();
	
	// check if format error occured (or other error e.g. out of memory which is unlikely)
	if (tif == NULL)
		throw DataException(dev, DataException::FORMAT_ERROR);

	// automatic cleanup on exception
	Guard guard(tif);

	// get parameters
	uint32_t width;
	uint32_t height;
	uint16_t numBits;
	uint16_t sampleFormat = SAMPLEFORMAT_UINT;
	uint16_t numChannels;
	uint16_t config;
	uint32_t rowsPerStrip;
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
	TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &numBits);
	TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT, &sampleFormat);
	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &numChannels);
	TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &config);
	TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowsPerStrip);
	int numStrips = TIFFNumberOfStrips(tif);

	// determine image format
	ImageFormat format;
	switch (numBits)
	{
	case 8:
		format.layout = ImageFormat::Layout(ImageFormat::X8 + numChannels - 1);
		break;
	case 16:
		format.layout = ImageFormat::Layout(ImageFormat::X16 + numChannels - 1);
		break;
	case 32:
		format.layout = ImageFormat::Layout(ImageFormat::X32 + numChannels - 1);
		break;
	}
	switch (sampleFormat)
	{
	case SAMPLEFORMAT_UINT:
		format.type = ImageFormat::UNORM;
		break;
	case SAMPLEFORMAT_INT:
		format.type = ImageFormat::NORM;
		break;
	case SAMPLEFORMAT_IEEEFP:
		format.type = ImageFormat::FLOAT;
		break;
	}
	switch (numChannels)
	{
	case 1:
		format.mapping = ImageFormat::Y;
		break;
	case 2:
		format.mapping = ImageFormat::YA;
		break;
	case 3:
		format.mapping = ImageFormat::RGB;
		break;
	case 4:
		format.mapping = ImageFormat::RGBA;
		break;
	}
	if (format.layout == ImageFormat::INVALID_LAYOUT || format.type == ImageFormat::INVALID_TYPE
		|| format.mapping == ImageFormat::INVALID_MAPPING)
		throw DataException(dev, DataException::FORMAT_NOT_SUPPORTED);

	Pointer<Image> image = new Image(Image::IMAGE, format, width, height, 1, 1, 1);

	if (config == PLANARCONFIG_CONTIG)
	{
		// interleaved channels
		uint8_t* data = image->getData<uint8_t>();
		//size_t stripSize = rowsPerStrip * width * format.getMemorySize();
		size_t stripSize = TIFFStripSize(tif);
		size_t size = width * height * format.getMemorySize();
		for (int i = 0; i < numStrips; ++i)
		{
			// read strip
			TIFFReadEncodedStrip(tif, i, data, min(stripSize, size));
			data += stripSize;
			size -= stripSize;

			// check if a read error occured
			ioCatcher.checkState();
		}
	}
	else
	{
		// channels organized in planes

		//std::vector<uint8_t> strip(TIFFStripSize(tif));
		throw DataException(dev, DataException::FORMAT_NOT_SUPPORTED);


	}

	// the guard closes the tiff image, also when an exception is thrown
	return image;
}

void saveTIFF(const fs::path& path, Pointer<Image> image, int mipmapIndex, int imageIndex)
{
	Pointer<File> f = File::create(path);
	saveTIFF(f, image, mipmapIndex, imageIndex);
	f->close();
}

void saveTIFF(Pointer<IODevice> dev, Pointer<Image> image, int mipmapIndex, int imageIndex)
{
	if (mipmapIndex < 0)
		mipmapIndex = 0;

	// determine size
	int width = max(image->getWidth() >> mipmapIndex, 1);
	int height = max(image->getHeight() >> mipmapIndex, 1);
		
	// determine tiff format
	ImageFormat format = image->getFormat();
	const BufferFormat::LayoutInfo& info = format.getLayoutInfo();
	int numBits = info.componentSize * 8;
	int sampleFormat = -1;
	int numChannels = info.numChannels;
	int photometric = -1;
	bool alpha = false;
	if (info.type == BufferFormat::STANDARD)
	{
		switch (format.type)
		{
		case ImageFormat::UNORM:
		case ImageFormat::UINT:
			sampleFormat = SAMPLEFORMAT_UINT;
			break;
		case ImageFormat::NORM:
		case ImageFormat::INT:
			sampleFormat = SAMPLEFORMAT_INT;
			break;
		case ImageFormat::FLOAT:
			sampleFormat = SAMPLEFORMAT_IEEEFP;
			break;
		}
		switch (format.mapping)
		{
		case ImageFormat::Y:
			photometric = PHOTOMETRIC_MINISBLACK;
			break;
		case ImageFormat::YA:
			photometric = PHOTOMETRIC_MINISBLACK;
			alpha = true;
			break;
		case ImageFormat::RGB:
			photometric = PHOTOMETRIC_RGB;
			break;
		case ImageFormat::RGBA:
			photometric = PHOTOMETRIC_RGB;
			alpha = true;
			break;
		}
	}

	// check if valid
	if (sampleFormat == -1 || photometric == -1)
		throw DataException(dev, DataException::FORMAT_NOT_SUPPORTED);

	// init and set io functions
	IOCatcher ioCatcher(dev);
	TIFF* tif = open(ioCatcher, "w");
		
	// automatic cleanup on exception
	Guard guard(tif);

	// check if a write error occured
	ioCatcher.checkState();
		
	TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
	TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
	TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, numBits);
	TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, sampleFormat);
	TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, numChannels);
	TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
	if (alpha)
	{
		uint16_t data[1];
		data[0] = EXTRASAMPLE_ASSOCALPHA;
		TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, 1, &data);
	}
	TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
		
	int defaultStripSize = TIFFDefaultStripSize(tif, width * numChannels);
	TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, defaultStripSize);

	uint8_t* data = image->getData<uint8_t>(mipmapIndex, imageIndex);
	int stride = width * numChannels * info.componentSize;
	for (int row = 0; row < height; ++row)
	{
		int result = TIFFWriteScanline(tif, data, row, 0);
		data += stride;

		// check if a write error occured
		ioCatcher.checkState();
	}

	// the guard closes the tiff image, also when an exception is thrown
}

} // namespace digi
