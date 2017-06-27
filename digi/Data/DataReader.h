#ifndef digi_Data_DataReader_h
#define digi_Data_DataReader_h

#include "BufferedReader.h"
#include "ReadFunctions.h"


namespace digi {

/// @addtogroup Video
/// @{

class DataReader : public BufferedReader
{
public:

	/// construct DataReader on a device
	DataReader(Pointer<IODevice> dev, bool bigEndian = false, int bufferSize = 1024)
		: BufferedReader(dev, bufferSize), bigEndian(bigEndian) {}

	/// construct DataReader on a file and open it
	DataReader(const fs::path& path, bool bigEndian = false, int bufferSize = 1024)
		: BufferedReader(path, bufferSize), bigEndian(bigEndian) {}

	
	/// returns true if in big endian mode, false otherwise
	bool isBigEndian() {return this->bigEndian;}
	
	/// set big endian (true) or little endian (false)
	void setBigEndian(bool bigEndian) {this->bigEndian = bigEndian;} 


	/// read a value
	template <typename Type>
	Type read()
	{
		if (sizeof(Type) == 1 || !this->bigEndian)
		{
			ReadLE<Type> convert;
			return this->readInternal<Type>(convert);
		}
		else
		{
			ReadBE<Type> convert;
			return this->readInternal<Type>(convert);
		}
	}
	
	/// read a value
	template <typename Type>
	void read(Type& value)
	{
		if (sizeof(Type) == 1 || !this->bigEndian)
		{
			ReadLE<Type> convert;
			value = this->readInternal<Type>(convert);
		}
		else
		{
			ReadBE<Type> convert;
			value = this->readInternal<Type>(convert);
		}
	}

	/// read array of values
	template <typename Type, typename Iterator>
	void read(Iterator it, size_t numValues)
	{
		if (sizeof(Type) == 1 || !this->bigEndian)
		{
			ReadLE<Type> convert;
			this->readInternal<typename std::iterator_traits<Iterator>::value_type>(it, it + numValues, convert);
		}
		else
		{
			ReadBE<Type> convert;
			this->readInternal<typename std::iterator_traits<Iterator>::value_type>(it, it + numValues, convert);
		}
	}

	/// read data and return as string
	std::string readString(size_t length)
	{
		std::string s;
		s.resize(length);
		this->readData((void*)s.data(), length);
		return s;
	}
	
protected:

	bool bigEndian;
};

/// @}

} // namespace digi

#endif
