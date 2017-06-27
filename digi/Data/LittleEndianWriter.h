#ifndef digi_Data_LittleEndianWriter_h
#define digi_Data_LittleEndianWriter_h

#include "BufferedWriter.h"
#include "WriteFunctions.h"


namespace digi {

/// @addtogroup Data
/// @{

class LittleEndianWriter : public BufferedWriter
{
public:
	LittleEndianWriter(Pointer<IODevice> dev, int bufferSize = 1024)
		: BufferedWriter(dev, bufferSize) {}
	LittleEndianWriter(const fs::path& path, int bufferSize = 1024)
		: BufferedWriter(path, bufferSize) {}


	template <typename Type>
	void write(Type value)
	{
		WriteLE<Type> convert;
		this->writeInternal<Type>(value, convert);
	}
	
	template <typename Type, typename Iterator>
	void write(Iterator it, size_t numValues)
	{
		WriteLE<Type> convert;
		this->writeInternal<Type>(it, it + numValues, convert);
	}
};

/// @}

} // namespace digi

#endif
