#include <boost/range.hpp>

#include <digi/Utility/ArrayUtility.h>
#include <digi/System/MemoryDevices.h>
#include <digi/Checksum/CRC32.h>

#include "BufferConverter.h"


namespace digi {


// BufferConverter

BufferConverter::~BufferConverter()
{
}

Pointer<DataConverter> BufferConverter::getElementConverter(BufferFormat srcFormat,
	BufferFormat dstFormat, DataConverter::Mode dstMode)
{
	uint8_t buffer[5];
	buffer[0] = srcFormat.layout;
	buffer[1] = srcFormat.type;
	buffer[2] = dstFormat.layout;
	buffer[3] = dstFormat.type;
	buffer[4] = dstMode;
	uint32_t crc = calcCRC32(buffer, boost::size(buffer));

	// look in chache for converter
	Pointer<DataConverter>& converter = this->converters[crc];

	// create new converter if not found
	if (converter == null)
	{
		std::string code;
		CodeWriter w(new StringRefDevice(code));
		ConverterWriter cw(w);
		
		cw.beginConverter();
		cw.load(srcFormat);
		if (srcFormat != dstFormat)
		{
			/*if (srcFormat.isInt() && dstFormat.isInt())
			{
				cw.depackToInt(srcFormat);
				cw.packFromInt(dstFormat);
			}
			else*/
			{
				cw.depackToFloat(srcFormat);
				cw.packFromFloat(dstFormat);
			}
		}
		cw.store(dstFormat, dstMode);
		cw.endConverter();
		
		converter = DataConverter::create(this->context, code);
	}
	return converter;
}

Pointer<DataConverter> BufferConverter::getElementConverter(BufferFormat srcFormat,
	StringRef code, BufferFormat dstFormat, DataConverter::Mode dstMode)
{
	uint8_t buffer[5];
	buffer[0] = srcFormat.layout;
	buffer[1] = srcFormat.type;
	buffer[2] = dstFormat.layout;
	buffer[3] = dstFormat.type;
	buffer[4] = dstMode;
	uint32_t crc = calcCRC32(buffer, boost::size(buffer));
	crc = calcCRC32(code.data(), code.length(), crc);

	// look in chache for converter
	Pointer<DataConverter>& converter = this->converters[crc];

	// create new converter if not found
	if (converter == null)
	{
		std::string code2 = code;
		CodeWriter w(new StringRefDevice(code2, code2.size()));
		ConverterWriter cw(w);
		
		cw.beginConverter();
		cw.load(srcFormat);
		cw.depackToFloat(srcFormat);
		cw.callFunc();
		cw.packFromFloat(dstFormat);
		cw.store(dstFormat, dstMode);
		cw.endConverter();
		
		converter = DataConverter::create(this->context, code2);
	}
	return converter;
}

Pointer<DataConverter> BufferConverter::getElementProcessor(BufferFormat srcFormat, StringRef code)
{
	uint8_t buffer[2];
	buffer[0] = srcFormat.layout;
	buffer[1] = srcFormat.type;
	uint32_t crc = calcCRC32(buffer, boost::size(buffer));
	crc = calcCRC32(code.data(), code.length(), crc);

	// look in chache for converter
	Pointer<DataConverter>& converter = this->converters[crc];

	// create new converter if not found
	if (converter == null)
	{
		std::string code2 = code;
		CodeWriter w(new StringRefDevice(code2, code2.size()));
		ConverterWriter cw(w);
		
		cw.beginConverter();
		cw.load(srcFormat);
		cw.depackToFloat(srcFormat);
		cw.callProc();
		cw.endConverter();
		
		converter = DataConverter::create(this->context, code2);
	}

	return converter;
}

void BufferConverter::convert(
	BufferFormat srcFormat, void* srcData,
	BufferFormat dstFormat, void* dstData,
	size_t numElements)
{
	this->getElementConverter(srcFormat, dstFormat, DataConverter::NATIVE)->convert(
		srcData, srcFormat.getMemorySize(),
		NULL,
		dstData, dstFormat.getMemorySize(),
		numElements);
}

void BufferConverter::convert(
	BufferFormat srcFormat, void* srcData,
	BufferFormat dstFormat, DataConverter::Mode dstMode, void* dstData, size_t dstStride,
	size_t numElements)
{
	this->getElementConverter(srcFormat, dstFormat, dstMode)->convert(
		srcData, srcFormat.getMemorySize(),
		NULL,
		dstData, dstStride,
		numElements);
}

void BufferConverter::convert(
	BufferFormat srcFormat, void* srcData,
	StringRef code, void* global,
	BufferFormat dstFormat, DataConverter::Mode dstMode, void* dstData, size_t dstStride,
	size_t numElements)
{
	this->getElementConverter(srcFormat, code, dstFormat, dstMode)->convert(
		srcData, srcFormat.getMemorySize(),
		global,
		dstData, dstStride,
		numElements);
}

Pointer<Buffer> BufferConverter::convert(Pointer<Buffer> srcBuffer, BufferFormat dstFormat)
{
	BufferFormat srcFormat = srcBuffer->getFormat();
	void* srcData = srcBuffer->getData<void>();
	size_t numElements = srcBuffer->getNumElements();
	Pointer<Buffer> dstBuffer = new Buffer(dstFormat, numElements);
	void* dstData = dstBuffer->getData<void>();
	
	this->getElementConverter(srcFormat, dstFormat, DataConverter::NATIVE)->convert(
		srcData, srcFormat.getMemorySize(),
		NULL,
		dstData,
		dstFormat.getMemorySize(),
		numElements);
	
	return dstBuffer;
}

void BufferConverter::process(
	BufferFormat srcFormat, void* srcData,
	StringRef code, void* global,
	size_t numElements)
{
	this->getElementProcessor(srcFormat, code)->convert(
		srcData, srcFormat.getMemorySize(),
		global,
		NULL, 0,
		numElements);
}

} // namespace digi
