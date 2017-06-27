#include "half.h"


namespace digi {

namespace
{
	int32_t convertmantissa(int i)
	{
		int32_t m = i << 13; // Zero pad mantissa bits
		int32_t e = 0; // Zero exponent
		while (!(m & 0x00800000)) // While not normalized
		{
			e -= 0x00800000; // Decrement exponent (1<<23)
			m <<= 1; // Shift mantissa
		}
		m &= ~0x00800000; // Clear leading 1 bit
		e += 0x38800000; // Adjust bias ((127-14)<<23)
		return m | e; // Return combined number
	}		
}
	
// half -> float
int32_t half::mantissatable[2048];
int32_t half::exponenttable[64];
int16_t half::offsettable[64];

// float -> half
uint16_t half::basetable[512];
uint8_t half::shifttable[512];

void half::initTables()
{
	// half -> float
	mantissatable[0] = 0;
	for (int i = 1; i < 1024; ++i)
		mantissatable[i] = convertmantissa(i);
	for (int i = 1024; i < 2048; ++i)
		mantissatable[i] = 0x38000000 + ((i - 1024) << 13);

	for (int i = 0; i < 31; ++i)
	{
		exponenttable[i] = i << 23;
		exponenttable[i + 32] = 0x80000000 + (i << 23);
	}
	exponenttable[31] = 0x47800000;
	exponenttable[63] = 0xC7800000;

	offsettable[0] = 0;
	offsettable[32] = 0;
	for (int i = 1; i < 32; ++i)
	{
		offsettable[i] = 1024;
		offsettable[i + 32] = 1024;
	}
		
	// float -> half
	for (int i = 0 ; i < 256; ++i)
	{
		int e = i - 127;
		if (e < -24)
		{
			// Very small numbers map to zero
			basetable[i | 0x000] = 0x0000;
			basetable[i | 0x100] = 0x8000;
			shifttable[i | 0x000] = 24;
			shifttable[i | 0x100] = 24;
		}
		else if(e < -14)
		{
			// Small numbers map to denorms
			basetable[i | 0x000] = (0x0400 >> (-e-14));
			basetable[i | 0x100] = (0x0400 >> (-e-14)) | 0x8000;
			shifttable[i | 0x000] = -e - 1;
			shifttable[i | 0x100] = -e - 1;
		}
		else if(e <= 15)
		{
			// Normal numbers just lose precision
			basetable[i | 0x000] = ((e + 15) << 10);
			basetable[i | 0x100] = ((e + 15) << 10) | 0x8000;
			shifttable[i | 0x000] = 13;
			shifttable[i | 0x100] = 13;
		}
		else if(e < 128)
		{
			// Large numbers map to Infinity
			basetable[i | 0x000] = 0x7C00;
			basetable[i | 0x100] = 0xFC00;
			shifttable[i | 0x000] = 24;
			shifttable[i | 0x100] = 24;
		}
		else
		{
			// Infinity and NaN's stay Infinity and NaN's
			basetable[i | 0x000]=0x7C00;
			basetable[i | 0x100]=0xFC00;
			shifttable[i | 0x000]=13;
			shifttable[i | 0x100]=13;
		}
	}
}
	
} // namespace digi
