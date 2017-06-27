/*
	calculate the CRC (cyclic redundancy check) value for given buffer
*/
#ifndef digi_CRC32_h
#define digi_CRC32_h

#include <string>
#include <Digi/Base/Platform.h>


namespace digi {


/// returns the CRC32 of a buffer
uint32_t calcCRC32(const uint8_t* buffer, size_t count, uint32_t seed = 0);
inline uint32_t calcCRC32(const char* buffer, size_t count, uint32_t seed = 0)
{
	return calcCRC32((const uint8_t*)buffer, count, seed);
}

/// returns the CRC32 of a string
inline uint32_t calcCRC32(const std::string& s, uint32_t seed = 0)
{
	return calcCRC32((const uint8_t*)s.c_str(), s.size(), seed);
}

} // namespace digi

#endif
