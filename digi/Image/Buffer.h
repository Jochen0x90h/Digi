#ifndef digi_Image_Buffer_h
#define digi_Image_Buffer_h

#include <limits>

#include <boost/range/iterator_range.hpp>

#include <digi/Utility/Object.h>
#include "BufferFormat.h"


namespace digi {

/// @addtogroup Image
/// @{


/// data buffer containing elements of format defined by BufferFormat
class Buffer : public Object
{
	friend size_t copyData(Pointer<Buffer> dstBuffer, size_t dstIndex, Pointer<Buffer> srcBuffer, size_t srcIndex, size_t numElements);

public:
	
	// construct data buffer with format and number of elements
	Buffer(BufferFormat format, size_t numElements);
	
	~Buffer();
	
	/// get format of data buffer
	const BufferFormat& getFormat() {return this->format;}
		
	/// get size of an element
	size_t getElementMemorySize() {return this->format.getMemorySize();}

	/// get number of elements (as defined by format) in data buffer
	size_t getNumElements() {return this->numElements;}

	/// get size of data buffer in bytes
	size_t getMemorySize() {return this->numElements * this->format.getMemorySize();}		


	/// set data (warning: no type checking)
	void setData(const void* data) {memcpy(this->data, data, this->getMemorySize());}

	/// get data as pointer of given type (warning: no type checking)
	template <typename Type>
	Type* getData() {return (Type*)this->data;}

	/// get data as boost::range for use with boost::forach (warning: no type checking)
	template <typename Type>
	boost::iterator_range<Type*> getRange() {return boost::iterator_range<Type*>((Type*)this->data, (Type*)this->data + this->numElements);}

protected:

	// format of elements
	BufferFormat format;
	
	// number of elements
	size_t numElements;
	 
	// data
	void* data;
};



// helper functions

// create data buffer from float data
static inline Pointer<Buffer> createDataBuffer(float* data, size_t numElements)
{
	Buffer* dataBuffer = new Buffer(BufferFormat(BufferFormat::X32, BufferFormat::FLOAT), numElements);
	dataBuffer->setData(data);
	return dataBuffer;
}

// create data buffer from float3 data
static inline Pointer<Buffer> createDataBuffer(float3* data, size_t numElements)
{
	Buffer* dataBuffer = new Buffer(BufferFormat(BufferFormat::XYZ32, BufferFormat::FLOAT), numElements);
	dataBuffer->setData(data);
	return dataBuffer;
}

// copy elements from srcImage to dstImage
size_t copyData(Pointer<Buffer> dstBuffer, size_t dstIndex, Pointer<Buffer> srcBuffer, size_t srcIndex, size_t numElements);

// copy all elements from srcImage to dstImage
static inline size_t copyData(Pointer<Buffer> dstBuffer, Pointer<Buffer> srcBuffer)
{
	return copyData(dstBuffer, 0, srcBuffer, 0, std::numeric_limits<size_t>::max());
}


/// named data buffer
struct NamedBuffer
{
	// name of data buffer
	std::string name;

	// the data buffer
	Pointer<Buffer> buffer;
	
	NamedBuffer() {}
	
	NamedBuffer(const std::string& name, Pointer<Buffer> buffer)
		: name(name), buffer(buffer) {}
};

/// @}

} // namespace digi

#endif
