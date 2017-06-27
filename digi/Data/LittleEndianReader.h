#ifndef digi_Data_LittleEndianReader_h
#define digi_Data_LittleEndianReader_h

#include "BufferedReader.h"
#include "ReadFunctions.h"


namespace digi {

/// @addtogroup Video
/// @{

class LittleEndianReader : public BufferedReader
{
public:
	LittleEndianReader(Pointer<IODevice> dev, int bufferSize = 1024)
		: BufferedReader(dev, bufferSize) {}
	LittleEndianReader(const fs::path& path, int bufferSize = 1024)
		: BufferedReader(path, bufferSize) {}

	
	template <typename Type>
	Type read()
	{
		ReadLE<Type> convert;
		return this->readInternal<Type>(convert);
	}
	
	template <typename Type>
	void read(Type& value)
	{
		ReadLE<Type> convert;
		value = this->readInternal<Type>(convert);
	}

	template <typename Type, typename Iterator>
	void read(Iterator it, size_t numValues)
	{
		ReadLE<Type> convert;
		this->readInternal<typename std::iterator_traits<Iterator>::value_type>(it, it + numValues, convert);
	}
};

/// @}

} // namespace digi

#endif
