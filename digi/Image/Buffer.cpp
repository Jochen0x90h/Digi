#include <digi/Utility/malloc16.h>

#include "Buffer.h"


namespace digi {


Buffer::Buffer(BufferFormat format, size_t numElements)
	: format(format), numElements(numElements)
{
	size_t numBytes = format.getMemorySize() * numElements;
	this->data = malloc16(numBytes);
}

Buffer::~Buffer()
{
	free16(this->data);
}


// helper functions

size_t copyData(Pointer<Buffer> dstBuffer, size_t dstIndex, Pointer<Buffer> srcBuffer, size_t srcIndex,
	size_t numElements)
{
	// check format
	if (dstBuffer->format != srcBuffer->format)
		return 0;
		
	// clamp number of elements
	numElements = min(numElements, dstBuffer->numElements - dstIndex);
	numElements = min(numElements, srcBuffer->numElements - srcIndex);

	// get size of element
	size_t elementSize = dstBuffer->format.getMemorySize();
	
	// copy pages
	memcpy(dstBuffer->getData<uint8_t>() + dstIndex * elementSize,
		srcBuffer->getData<uint8_t>() + srcIndex * elementSize,
		numElements * elementSize);
	
	// return number of images copied
	return numElements;
}


} // namespace digi
