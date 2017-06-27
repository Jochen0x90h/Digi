#ifndef digi_Engine_CodeMemory_h
#define digi_Engine_CodeMemory_h

#include <algorithm>
#include <digi/Base/Platform.h>


namespace digi {

/// @addtogroup Engine
/// @{

/// code memory, permission is readable, writable and executable
class CodeMemory
{
	friend void swap(CodeMemory& a, CodeMemory& b);
	
public:

	CodeMemory()
		: d(NULL), s(0) {}
	
	CodeMemory(size_t numBytes);

	~CodeMemory();

	operator uint8_t* () {return this->d;}
	operator const uint8_t* () const {return this->d;}

	size_t size() {return this->s;}
	uint8_t* begin() {return this->d;}
	uint8_t* end() {return this->d + this->s;}

protected:
	
	uint8_t* d;
	size_t s;
};

inline void swap(CodeMemory& a, CodeMemory& b)
{
	std::swap(a.d, b.d);
	std::swap(a.s, b.s);
}

/// @}

} // namespace digi

#endif
