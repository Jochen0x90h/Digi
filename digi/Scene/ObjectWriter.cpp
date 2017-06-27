#include <digi/Utility/StringUtility.h>
#include <digi/Utility/VectorUtility.h>

#include "ObjectWriter.h"


namespace digi {

// ObjectWriter

void save(ObjectWriter& w, Pointer<Buffer> buffer)
{
	// write parameters
	BufferFormat format = buffer->getFormat();
	size_t numElements = buffer->getNumElements();
	w & wrapUInt(format.layout);
	w & wrapUInt(format.type);
	w & numElements;
	
	// write data
	size_t numValues = numElements * format.getNumComponents();
	switch (format.getComponentSize())
	{
	case 1:
		w.write<uint8_t>(buffer->getData<uint8_t>(), numValues);
		break;
	case 2:
		w.write<uint16_t>(buffer->getData<uint16_t>(), numValues);
		break;
	case 4:
		w.write<uint32_t>(buffer->getData<uint32_t>(), numValues);
		break;
	}
}

void save(ObjectWriter& w, Pointer<Image> image)
{
	// read parameters
	Image::Type type = image->getType();
	ImageFormat format = image->getFormat();
	uint width = image->getWidth();
	uint height = image->getHeight();
	uint depth = image->getDepth();
	uint numMipmaps = image->getNumMipmaps();
	uint numImages = image->getNumImages();
	w & wrapUInt(type);
	w & wrapUInt(format.layout);
	w & wrapUInt(format.type);
	w & wrapUInt(format.mapping);
	w & width;
	w & height;
	w & depth;
	w & numMipmaps;
	w & numImages;
	
	// read data
	size_t numValues = image->getNumPixels() * format.getNumComponents();
	switch (format.getComponentSize())
	{
	case 1:
		w.write<uint8_t>(image->getData<uint8_t>(), numValues);
		break;
	case 2:
		w.write<uint16_t>(image->getData<uint16_t>(), numValues);
		break;
	case 4:
		w.write<uint32_t>(image->getData<uint32_t>(), numValues);
		break;
	}			
}

} // namespace digi
