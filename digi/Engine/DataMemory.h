#ifndef digi_Engine_DataMemory_h
#define digi_Engine_DataMemory_h

#include <digi/Utility/malloc16.h>


namespace digi {

/// @addtogroup Engine
/// @{

/// data memory, aligned to 16
class DataMemory
{
	friend void swap(DataMemory& a, DataMemory& b);
	
public:

	DataMemory()
		: d(NULL), s(0) {}
	
	DataMemory(size_t numBytes)
	{
		this->d = (uint8_t*)malloc16(numBytes);
		memset(this->d, 0, numBytes);
		this->s = numBytes;
	}

	~DataMemory()
	{
		free16(this->d);
	}

	operator uint8_t* () {return this->d;}
	operator const uint8_t* () const {return this->d;}

	size_t size() {return this->s;}
	uint8_t* begin() {return this->d;}
	uint8_t* end() {return this->d + this->s;}

protected:
	
	uint8_t* d;
	size_t s;
};

inline void swap(DataMemory& a, DataMemory& b)
{
	std::swap(a.d, b.d);
	std::swap(a.s, b.s);
}

/// @}

} // namespace digi

#endif
