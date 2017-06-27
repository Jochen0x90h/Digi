#ifndef digi_Data_EBMLWriter_h
#define digi_Data_EBMLWriter_h

#include <boost/type_traits.hpp>

#include "BufferedWriter.h"
#include "EbmlHeader.h"


namespace digi {

/// @addtogroup Data
/// @{

/// EBML writer
/// http://matroska.org/technical/specs/rfc/index.html
class EbmlWriter : public BufferedWriter
{
public:
	EbmlWriter(Pointer<IODevice> dev)
		: BufferedWriter(dev) {}
	EbmlWriter(const fs::path& path)
		: BufferedWriter(path) {}

	/// write EBML header
	void writeHeader(const EbmlHeader& header);

	/// write EBML ID
	void writeId(uint id)
	{
		uint8_t buffer[4];
		uint8_t* end = buffer + 4;
		uint8_t* begin = EbmlWriter::writeId(end, id);
		this->writeData(begin, end - begin);
	}

	/// write EBML variable length integer
	void writeVarInt(uint64_t value)
	{
		uint8_t buf[8];
		uint8_t* end = buf + 8;
		uint8_t* begin = EbmlWriter::writeVarInt(end, value);
		this->writeData(begin, end - begin);
	}

	/// write EBML variable length integer with given size (must be in the range 1 - 8)
	void writeVarInt(uint64_t value, int size);


	/// write a signed or unsigned integral value
	template <typename Type>
	void write(uint id, Type value,
		typename boost::enable_if_c<boost::is_integral<Type>::value>::type* dummy = 0)
	{
		uint8_t buffer[16];
		uint8_t* end = buffer + 16;
		uint8_t* begin = end;
		
		// value
		uint8_t size = 0x80;
		if (boost::is_signed<Type>::value)
		{
			// signed
			Type sign = 0;
			while ((value ^ sign) != 0)
			{
				--begin;
				*begin = uint8_t(value);
				sign = (value & 0x80) != 0 ? -1 : 0;
				value >>= 8;
				++size;
			}
		}
		else
		{
			// unsigned
			while (value > 0)
			{
				--begin;
				*begin = uint8_t(value);
				value >>= 8;
				++size;
			}
		}
		
		// size
		--begin;
		*begin = size;
		
		// id
		begin = EbmlWriter::writeId(begin, id);
		
		// write
		this->writeData(begin, end - begin);
	}

	/// write a signed or unsigned integral value with given size (must be in the range 0 - 8)
	template <typename Type>
	void write(uint id, Type value, size_t size,
		typename boost::enable_if_c<boost::is_integral<Type>::value>::type* dummy = 0)
	{
		uint8_t buffer[16];
		uint8_t* end = buffer + 16;
		uint8_t* begin = end;
		
		// value (big endian, therefore start with last byte)
		for (size_t i = 0; i < size; ++i)
		{
			--begin;
			*begin = uint8_t(value);
			value >>= 8;
		}
		
		// size
		--begin;
		*begin = uint8_t(0x80 + size);
		
		// id
		begin = EbmlWriter::writeId(begin, id);
		
		// write
		this->writeData(begin, end - begin);
	}
	
	/// write a bool
	void write(uint id, bool value)
	{
		this->write(id, 1u, value ? 1 : 0);
	}

	/// write a bool with given size (must be in the range 0 - 8)
	void write(uint id, bool value, size_t size)
	{
		this->write(id, value ? 1u : 0u, size);
	}

	/// write a float
	void write(uint id, float value)
	{
		this->write(id, value, value == 0.0f ? 0 : 4);
	}

	/// write a float with given size (must be 0, 4 or 8)
	void write(uint id, float value, size_t size)
	{
		this->write(id, *(uint32_t*)&value, size);
	}

	/// write a double
	void write(uint id, double value)
	{
		this->write(id, value, value == 0.0 ? 0 : 8);
	}

	/// write a double with given size (must be 0, 4 or 8)
	void write(uint id, double value, size_t size)
	{
		this->write(id, *(uint64_t*)&value, size);
	}

	/// write a string
	void write(uint id, StringRef value);
	void write(uint id, const char* value) {return this->write(id, StringRef(value));}
	void write(uint id, const std::string& value) {return this->write(id, StringRef(value));}
	void write(uint id, const fs::path& value) {return this->write(id, StringRef(value.string()));}
	
	/// write a string with given size. gets padded with zeros if string is shorter
	void write(uint id, StringRef value, size_t size);
	void write(uint id, const char* value, size_t size) {return this->write(id, StringRef(value), size);}
	void write(uint id, const std::string& value, size_t size) {return this->write(id, StringRef(value), size);}
	void write(uint id, const fs::path& value, size_t size) {return this->write(id, StringRef(value.string()), size);}

	/// write data
	void write(uint id, const std::vector<uint8_t>& value);
	
	static size_t calcSizeVarInt(uint64_t value)
	{
		// values with all data bits set are reserved (e.g. 0x7f, 0x3fff, 0x1fffff), therefore store one more byte
		uint64_t value1 = value + 1;
		size_t size = 0;
		while (value1 > 0)
		{
			value1 >>= 7;
			++size;
		}
		return size;
	}

	template <typename Type>
	static size_t calcSize(Type value,
		typename boost::enable_if_c<boost::is_integral<Type>::value>::type* dummy = 0)
	{
		size_t size = 1; // one byte for length
		if (boost::is_signed<Type>::value)
		{
			// signed
			Type sign = 0;
			while ((value ^ sign) != 0)
			{
				sign = (value & 0x80) != 0 ? -1 : 0;
				value >>= 8;
				++size;
			}
		}
		else
		{
			// unsigned
			while (value > 0)
			{
				value >>= 8;
				++size;
			}
		}
		return size;
	}

	static size_t calcSize(bool value)
	{
		return value ? 2 : 1;
	}

	static size_t calcSize(float value)
	{
		return value == 0.0f ? 1 : 5;
	}

	static size_t calcSize(double value)
	{
		return value == 0.0 ? 1 : 9;
	}

	static size_t calcSize(StringRef value)
	{
		size_t size = value.size();
		return EbmlWriter::calcSizeVarInt(size) + size;
	}
	static size_t calcSize(const char* value) {return EbmlWriter::calcSize(StringRef(value));}
	static size_t calcSize(const std::string& value) {return EbmlWriter::calcSize(StringRef(value));}
	static size_t calcSize(const fs::path& value) {return EbmlWriter::calcSize(StringRef(value.string()));}
	
	static size_t calcSize(const std::vector<uint8_t>& value)
	{
		size_t size = value.size();
		return EbmlWriter::calcSizeVarInt(size) + size;
	}

protected:

	static uint8_t* writeId(uint8_t* begin, uint id);

	static uint8_t* writeVarInt(uint8_t* begin, uint64_t value);
};

/// @}

} // namespace digi

#endif
