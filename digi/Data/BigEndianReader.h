#ifndef digi_Data_BigEndianReader_h
#define digi_Data_vEndianReader_h

#include "BufferedReader.h"
#include "ReadFunctions.h"


namespace digi {

/// @addtogroup Video
/// @{

class BigEndianReader : public BufferedReader
{
public:
	BigEndianReader(Pointer<IODevice> dev, int bufferSize = 1024)
		: BufferedReader(dev, bufferSize) {}
	BigEndianReader(const fs::path& path, int bufferSize = 1024)
		: BufferedReader(path, bufferSize) {}

	
	template <typename Type>
	Type read()
	{
		ReadBE<Type> convert;
		return this->readInternal<Type>(convert);
	}
	
	template <typename Type>
	void read(Type& value)
	{
		ReadBE<Type> convert;
		value = this->readInternal<Type>(convert);
	}

	template <typename Type, typename Iterator>
	void read(Iterator it, size_t numValues)
	{
		ReadBE<Type> convert;
		this->readInternal<typename std::iterator_traits<Iterator>::value_type>(it, it + numValues, convert);
	}
};

/// @}

} // namespace digi

#endif
