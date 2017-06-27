#ifndef digi_Data_EbmlReader_h
#define digi_Data_EbmlReader_h

#include <boost/type_traits.hpp>

#include "BufferedReader.h"
#include "DataException.h"
#include "EbmlHeader.h"


namespace digi {

/// @addtogroup Data
/// @{

/// EBML reader
/// http://matroska.org/technical/specs/rfc/index.html
class EbmlReader : public BufferedReader
{
public:
	EbmlReader(Pointer<IODevice> dev)
		: BufferedReader(dev), byteCount() {}
	EbmlReader(const fs::path& path)
		: BufferedReader(path), byteCount() {}

	/// read EBML header
	void readHeader(EbmlHeader& header);

	/// read EBML ID
	uint readId();
	
	/// read variable size integer
	uint64_t readVarInt();

	/// read EBML element size (variable size integer) with given maximum. DataException is thrown if maximum is exceeded
	size_t readSize(size_t maxSize);

	
	void readData(void* data, size_t numBytes)
	{
		this->BufferedReader::readData(data, numBytes);
		this->byteCount += numBytes;
	}
	
	/// read a signed or unsigned integral value
	template <typename Type>
	void read(Type& value,
		typename boost::enable_if_c<boost::is_integral<Type>::value>::type* dummy = 0)
	{
		uint8_t length;
		this->readData(&length, 1);
		length ^= 0x80;
		
		// length must be in the range 0-8
		if (length > 8)
			throw DataException(this->dev, DataException::DATA_CORRUPT);
		
		value = 0;
		if (length > 0)
		{
			uint8_t buffer[8];
			this->readData(buffer, length);
			
			uint8_t* it = buffer;
			uint8_t* end = buffer + length;
			if (boost::is_signed<Type>::value)
			{
				// signed
				value = (*it & 0x80) != 0 ? -1 : 0;
				do
				{
					value <<= 8;
					value |= *it;
					++it;
				} while (it != end);
			}
			else
			{
				// unsigned
				do
				{
					value <<= 8;
					value |= *it;
					++it;
				} while (it != end);
			}
		}
	}

	/// read a boolean
	void read(bool& value);

	/// read a float
	void read(float& value)
	{
		double d;
		this->read(d);
		value = float(d);
	}
	
	/// read a double
	void read(double& value);

	/// read a string with given size limit
	void read(std::string& value, size_t maxSize = std::numeric_limits<size_t>::max());

	/// read a path with given size limit
	void read(fs::path& value, size_t maxSize = std::numeric_limits<size_t>::max())
	{
		std::string str;
		this->read(str, maxSize);
		value = str;
	}

	/// read data with given size limit
	void read(std::vector<uint8_t>& value, size_t maxSize = std::numeric_limits<size_t>::max());

	template <typename Type>
	Type read()
	{
		Type value;
		this->read(value);
		return value;
	}

	template <typename Type>
	Type read(size_t maxSize)
	{
		Type value;
		this->read(value, maxSize);
		return value;
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
	size_t getByteCount() {return this->byteCount;}

protected:

	size_t byteCount;
};

/// @}

} // namespace digi

#endif
