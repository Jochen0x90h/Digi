#ifndef digi_ImageConvert_BufferConverter_h
#define digi_ImageConvert_BufferConverter_h

#include <map>

#include <digi/Utility/StringRef.h>
#include <digi/Image/Buffer.h>
#include "DataConverter.h"


namespace digi {

/// @addtogroup ImageConvert
/// @{

class BufferConverter : public Object
{
public:

	BufferConverter(Pointer<ConverterContext> context)
		: context(context) {}
	virtual ~BufferConverter();
	
	/// get a converter that converts elements of srcFormat to elements of dstFormat
	Pointer<DataConverter> getElementConverter(BufferFormat srcFormat,
		BufferFormat dstFormat, DataConverter::Mode dstMode);

	/// get a converter that converts elements of srcFormat to elements of dstFormat with processing code
	Pointer<DataConverter> getElementConverter(BufferFormat srcFormat,
		StringRef code, BufferFormat dstFormat, DataConverter::Mode dstMode);

	/// get a processor that processes elements of srcFormat with given processing code
	Pointer<DataConverter> getElementProcessor(BufferFormat srcFormat, StringRef code);

	/// convert from source buffer to destination buffer
	void convert(
		BufferFormat srcFormat, void* srcData,
		BufferFormat dstFormat, void* dstData,
		size_t numElements);

	/// convert from source buffer to destination buffer
	void convert(
		BufferFormat srcFormat, void* srcData,
		BufferFormat dstFormat, DataConverter::Mode dstMode, void* dstData, size_t dstStride,
		size_t numElements);

	/// load from source buffer, do some processing and write to destination buffer
	void convert(
		BufferFormat srcFormat, void* srcData,
		StringRef code, void* global,
		BufferFormat dstFormat, DataConverter::Mode dstMode, void* dstData, size_t dstStride,
		size_t numElements);

	/// convert source buffer buffer into new destination buffer
	Pointer<Buffer> convert(Pointer<Buffer> srcBuffer, BufferFormat dstFormat);

	/// process buffer
	void process(
		BufferFormat srcFormat, void* srcData,
		StringRef code, void* global,
		size_t numElements);

protected:

	// context (the llvm jit)
	Pointer<ConverterContext> context;

	// converter cache (crc32 of converter parameters -> converter)
	std::map<uint32_t, Pointer<DataConverter> > converters;
};	

/// @}

} // namespace Digi

#endif
