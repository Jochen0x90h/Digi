#ifndef digi_Utility_IntUtility_h
#define digi_Utility_IntUtility_h

#include <digi/Base/Platform.h>
#include "Standard.h"


namespace digi {

/// @addtogroup Utility
/// @{

inline uint32_t intCeil(uint32_t value, uint32_t step)
{
	return value + uint32_t(-int32_t(value)) % step;
}

inline uint64_t intCeil(uint64_t value, uint64_t step)
{
	return value + uint64_t(-int64_t(value)) % step;
}

/// swaps the bytes of a 16 bit integer
inline uint16_t byteSwap(uint16_t value)
{
	return (value >> 8) | value << 8;
}

/// swaps the bytes of a 32 bit integer
inline uint32_t byteSwap(uint32_t value)
{
	return (value >> 24) | ((value >> 8) & 0xff00) | ((value << 8) & 0xff0000) | value << 24;
}

/// returns the first clear bit as a set bit (starting from least significant bit) of the integer, or 0 if none is clear
inline int firstClearBit(int value)
{
	return ~value & (value + 1);
}

/// returns the first set bit as a set bit (starting from least significant bit) of the integer, or 0 if none is set
inline int firstSetBit(int value)
{
	return value & ~(value - 1);
}


/// checks if an int is power of 2
inline bool isPowerOfTwo(int x)
{
	return !(x & ((x - 1) | ~(x - 1) << 1));
}

/// returns a number that is a power of two and less or equal to given value
inline int downToPowerOfTwo(int value)
{
	int result;
	for (result = 1; result <= value; result <<= 1);
	return result >> 1;
}

/// returns a number that is a power of two and greater or equal to given value
inline int upToPowerOfTwo(int value)
{
	int result;
	for (result = 1; result < value; result <<= 1);
	return result;
}

/// log2 for integer
inline int log2(int value)
{
	int result = 0;
	while (value > 1)
	{
		value >>= 1;
		++result;
	}
	return result;
}


/// sets a bit in \a value to the state of \a set
inline void setBit(int& value, int bit, bool set)
{
	if (set)
		value |= bit;
	else
		value &= ~bit;
}

/// returns the number of set bits
inline int countSetBits(uint value)
{
	int numBits = 0;
	while (value != 0)
	{
		if (value & 1)
			++numBits;
		value >>= 1;
	}
	return numBits;
}

/// @}

} // namespace digi

#endif
