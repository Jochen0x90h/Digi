// we need standard constant macros
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif

#include <algorithm>
#include <cmath>

#include "StringUtility.h"
#include "ArrayUtility.h"
#include "UTFTranscode.h"
#include "Convert.h"


namespace digi {

	
char* toString(char* str, unsigned long long v)
{
	char* begin = str;
	do
	{
		char digit = char(v % 10);
		v /= 10;
		*str = digit + '0';
		++str;
	} while (v > 0);
	
	// reverse string
	char* end = str - 1;
	while (end > begin)
	{
		std::swap(*end, *begin);
		++begin;
		--end;
	}
	return str;
}

char* toString(char* str, long long v)
{
	if (v < 0)
	{
		v = -v;
		*str = '-';
		++str;
	}
	return toString(str, uint64_t(v));
}

char* toString(char* str, double d, int numDigits)
{
	int64_t l = *(int64_t*)&d;

	// check for zero
	if ((l & INT64_C(0x7fffffffffffffff)) == 0)
	{
		*str = '0';
		++str;
		*str = '.';
		++str;
		*str = '0';
		++str;
		return str;
	}
	
	// check for sign
	if ((l & INT64_C(0x8000000000000000)) != 0)
	{
		l &= INT64_C(0x7fffffffffffffff);
		*str = '-';
		++str;
	}
	
	//! check for infinity
	
	//! check for nan

	int binaryExponent = int(l >> 52) - 1023 - 52;
	
	l = (l & INT64_C(0x000fffffffffffff)) + ((int64_t(1023 + 52)) << 52);
	
	d = *(double*)&l;
	
	double dDecimalExponent = double(binaryExponent) * log10(2.0);
	
	double floorDecimalExponent = floor(dDecimalExponent);
	int decimalExponent = int(floorDecimalExponent);
	
	d *= pow(10.0, (dDecimalExponent - floorDecimalExponent) + (numDigits - 16.0));
		
	l = int64_t(d + 0.5);
	
	if (l >= int64_t(pow(10.0, double(numDigits)) + 0.5))
	{
		l /= 10;
		++decimalExponent;
	}
	
	decimalExponent += 15;

	int numDigitsBeforePoint = 1;
	int numZerosAfterPoint = 0;

	if (decimalExponent >= -numDigits / 2 && decimalExponent < numDigits - 1)
	{
		// can represent number without exponent, e.g. 123.4 or 0.01234
		if (decimalExponent >= 0)
		{
			numDigitsBeforePoint = 1 + decimalExponent;
			numZerosAfterPoint = 0;
			decimalExponent = 0;
		}
		else
		{
			numDigitsBeforePoint = 0;
			numZerosAfterPoint = -decimalExponent - 1; 
			decimalExponent = 0;
		}
	}
	
	// build number in reverse order. start with digits after point
	char* begin = str;
	bool first = true;
	for (int i = numDigitsBeforePoint; i < numDigits; ++i)
	{
		char digit = char(l % 10);
		l /= 10;
		if (!first || digit > 0 || i == numDigits - 1)
		{
			*str = digit + '0';
			++str;
			first = false;
		}
	}
		
	for (int i = 0; i < numZerosAfterPoint; ++i)
	{
		*str = '0';
		++str;
	}			
	
	*str = '.';
	++str;

	// digits before point
	if (numDigitsBeforePoint > 0)
	{
		for (int i = 0; i < numDigitsBeforePoint; ++i)
		{
			char digit = char(l % 10);
			l /= 10;
			*str = digit + '0';
			++str;
		}
	}
	else
	{
		*str = '0';
		++str;
	}
		
	// reverse string
	char* end = str - 1;
	while (end > begin)
	{
		std::swap(*end, *begin);
		++begin;
		--end;
	}	

	// add exponent
	if (decimalExponent != 0)
	{
		*str = 'e';
		++str;
		return toString(str, decimalExponent);
	}

	return str;
}


unsigned long long toULongLong(StringRef str, int base)
{
	int len = int(str.length());
	unsigned long long value = 0;
	for (int i = 0; i < len; ++i)
	{
		uint8_t digit = str[i];
		if (digit >= '0' && digit <= '9')
			digit -= '0';
		else if (digit >= 'a')
			digit -= 'a' - 10;
		else if (digit >= 'A')
			digit -= 'A' - 10;
		else
			break;
		if (digit >= base)
			break;
			
		value *= base;
		value += digit;
	}
	return value;
}


} // namespace digi
