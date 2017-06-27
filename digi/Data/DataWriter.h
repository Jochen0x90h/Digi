#ifndef digi_Data_DataWriter_h
#define digi_Data_DataWriter_h

#include "BufferedWriter.h"
#include "WriteFunctions.h"


namespace digi {

/// @addtogroup Data
/// @{

/// data writer that can be configured to little endian or big endian at runtime
class DataWriter : public BufferedWriter
{
public:

	/// construct DataWriter on a device
	DataWriter(Pointer<IODevice> dev, bool bigEndian = false, int bufferSize = 1024)
		: BufferedWriter(dev, bufferSize), bigEndian(bigEndian) {}

	/// construct DataWriter on a file and open it
	DataWriter(const fs::path& path, bool bigEndian = false, int bufferSize = 1024)
		: BufferedWriter(path, bufferSize), bigEndian(bigEndian) {}


	/// returns true if in big endian mode, false otherwise
	bool isBigEndian() {return this->bigEndian;}
	
	/// set big endian (true) or little endian (false)
	void setBigEndian(bool bigEndian) {this->bigEndian = bigEndian;} 
	
		
	/// write a value
	template <typename Type>
	void write(Type value)
	{
		if (sizeof(Type) == 1 || !this->bigEndian)
		{
			WriteLE<Type> convert;
			this->writeInternal<Type>(value, convert);
		}
		else
		{
			WriteBE<Type> convert;
			this->writeInternal<Type>(value, convert);
		}
	}
	
	/// write array of values
	template <typename Type, typename Iterator>
	void write(Iterator it, size_t numValues)
	{
		if (sizeof(Type) == 1 || !this->bigEndian)
		{
			WriteLE<Type> convert;
			this->writeInternal<Type>(it, it + numValues, convert);
		}
		else
		{
			WriteBE<Type> convert;
			this->writeInternal<Type>(it, it + numValues, convert);
		}
	}

protected:
	
	bool bigEndian;
};

/// @}

} // namespace digi

#endif
