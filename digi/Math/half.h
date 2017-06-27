#ifndef digi_Math_half_h
#define digi_Math_half_h

#include <digi/Base/Platform.h>
#include <digi/Utility/as.h>


namespace digi {

/// @addtogroup Math
/// @{

/// half precision floating point number with 16 bits: 1s, 5e, 10m
/// reference: Jeroen van der Zijp. "Fast Half Float Conversions"
struct half
{
	half()
		: value(0) {}
			
	template <typename Type>
	explicit half(Type value)
	{
		// cast to float
		float f = float(value);
	
		// bit-cast to int
		uint32_t i = as<uint32_t>(f);
		
		// get sign and exponent
		int se = i >> 23;
		
		// convert
		this->value = basetable[se] + ((i & 0x007fffff) >> shifttable[se]);
	}

	operator float()
	{
		uint16_t h = this->value;
		
		// get sign and exponent
		int se = h >> 10;

		// convert
		uint32_t f = mantissatable[offsettable[se] + (h & 0x3ff)] + exponenttable[se];
		
		// bit-cast to float
		return *(float*)&f;
	}

	static void initTables();
	
	// the half value
	int16_t value;

private:	

	// half -> float
	static int32_t mantissatable[2048];
	static int32_t exponenttable[64];
	static int16_t offsettable[64];

	// float -> half
	static uint16_t basetable[512];
	static uint8_t shifttable[512];
};

inline bool operator ==(const half& a, const half& b)
{
	return a.value == b.value;
}

inline bool operator !=(const half& a, const half& b)
{
	return a.value != b.value;
}

} // namespace Digi

#endif
