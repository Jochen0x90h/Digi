#ifndef digi_Data_RiffReader_h
#define digi_Data_RiffReader_h

#include "BufferedReader.h"
#include "DataException.h"
#include "ReadFunctions.h"


namespace digi {

/// @addtogroup Data
/// @{

/// RIFF reader (little endian, e.g. WAV)
class RiffReader : public BufferedReader
{
public:
	RiffReader(Pointer<IODevice> dev)
		: BufferedReader(dev), byteCount() {}
	RiffReader(const fs::path& path)
		: BufferedReader(path), byteCount() {}

	/// read a value
	template <typename Type>
	Type read()
	{
		ReadLE<Type> convert;
		Type value = this->readInternal<Type>(convert);
		this->byteCount += ReadLE<Type>::SIZE;
		return value;
	}

	/// read IFF ID
	uint32_t readId() {return this->read<uint32_t>();}
	
	/// read IFF chunk size with given maximum. DataException is thrown if maximum is exceeded
	size_t readSize(size_t maxSize)
	{
		size_t size = this->read<uint32_t>();
		if (size > maxSize)
			throw DataException(this->dev, DataException::DATA_CORRUPT);
		return size;		
	}

	/// read byte data with given size
	void read(std::vector<uint8_t>& value, size_t size)
	{
		value.resize(size);
		this->readData(value.data(), size);
		this->byteCount += size;
	}

	/// read data in big endian byte order (e.g. samples)
	template <typename Type, typename Iterator>
	void read(Iterator it, size_t numValues)
	{
		ReadLE<Type> convert;
		this->readInternal<typename std::iterator_traits<Iterator>::value_type>(it, it + numValues, convert);
		this->byteCount += numValues * ReadLE<Type>::SIZE;
	}
	
	/// read data in big endian byte order (e.g. samples)
	template <typename Converter, typename Iterator>
	void read(Converter convert, Iterator it, size_t numValues)
	{
		this->readInternal<typename std::iterator_traits<Iterator>::value_type>(it, it + numValues, convert);
		this->byteCount += numValues * Converter::SIZE;
	}

	/// skip given number of bytes by reading from the device, support for seek() is not needed
	void skip(size_t numBytes)
	{
		this->BufferedReader::skip(numBytes);
		this->byteCount += numBytes;
	}
	
	/// seek
	int64_t seek(int64_t position, IODevice::PositionMode mode)
	{
		if (mode == IODevice::CURRENT)
			this->byteCount += position;
		return this->BufferedReader::seek(position, mode);
	}

	/// get number of bytes read so far
	int64_t getByteCount() {return this->byteCount;}

protected:

	int64_t byteCount;
};

/// @}

} // namespace digi

#endif
