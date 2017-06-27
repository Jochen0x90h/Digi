#ifndef digi_Data_NativeWriter_h
#define digi_Data_NativeWriter_h

#include "BufferedWriter.h"


namespace digi {

/// @addtogroup Data
/// @{

class NativeWriter : public BufferedWriter
{
public:
	NativeWriter(Pointer<IODevice> dev, int bufferSize = 1024)
		: BufferedWriter(dev, bufferSize) {}
	NativeWriter(const fs::path& path, int bufferSize = 1024)
		: BufferedWriter(path, bufferSize) {}

	
	template <typename Type>
	void write(Type value)
	{
		Write<Type> convert;
		this->writeInternal<Type>(value, convert);
	}
	
	template <typename Type, typename Iterator>
	void write(Iterator it, size_t numValues)
	{
		Write<Type> convert;
		this->writeInternal<Type>(it, it + numValues, convert);
	}

protected:

	template <typename Type>
	struct Write
	{
		enum {SIZE = sizeof(Type)};
		void operator ()(Type value, uint8_t* buffer)
		{
			uint8_t* v = (uint8_t*)&value;
			for (size_t i = 0; i < sizeof(Type); ++i)
			{
				*buffer = *v;
				++v;
				++buffer;
			}
		}
	};
};

/// @}

} // namespace digi

#endif
