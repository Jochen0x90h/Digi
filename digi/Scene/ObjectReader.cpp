#include <digi/Utility/StringUtility.h>
#include <digi/Utility/VectorUtility.h>

#include "ObjectReader.h"


namespace digi {

// ObjectReader

void load(ObjectReader& r, Pointer<Buffer>& buffer)
{
	// read parameters
	BufferFormat format;
	size_t numElements;
	r & wrapUInt(format.layout);
	r & wrapUInt(format.type);
	r & numElements;
	
	// read data
	buffer = new Buffer(format, numElements);
	size_t numValues = numElements * format.getNumComponents();
	switch (format.getComponentSize())
	{
	case 1:
		r.read<uint8_t>(buffer->getData<uint8_t>(), numValues);
		break;
	case 2:
		r.read<uint16_t>(buffer->getData<uint16_t>(), numValues);
		break;
	case 4:
		r.read<uint32_t>(buffer->getData<uint32_t>(), numValues);
		break;
	}			
}

void load(ObjectReader& r, Pointer<Image>& image)
{
	// read parameters
	Image::Type type;
	ImageFormat format;
	uint width;
	uint height;
	uint depth;
	uint numMipmaps;
	uint numImages;
	r & wrapUInt(type);
	r & wrapUInt(format.layout);
	r & wrapUInt(format.type);
	r & wrapUInt(format.mapping);
	r & width;
	r & height;
	r & depth;
	r & numMipmaps;
	r & numImages;
	
	// read data
	image = new Image(type, format, width, height, depth, numMipmaps, numImages);
	size_t numValues = image->getNumPixels() * format.getNumComponents();
	switch (format.getComponentSize())
	{
	case 1:
		r.read<uint8_t>(image->getData<uint8_t>(), numValues);
		break;
	case 2:
		r.read<uint16_t>(image->getData<uint16_t>(), numValues);
		break;
	case 4:
		r.read<uint32_t>(image->getData<uint32_t>(), numValues);
		break;
	}			
}

} // namespace digi
