#ifndef digi_Utility_ConvertFunctions_h
#define digi_Utility_ConvertFunctions_h

#include <string>
#include <sstream>

#include "StringUtility.h"


namespace digi {

/// @addtogroup Utility
/// @{

// convert 64 bit unsigned long integer to string with base 10 (internal use only)
char* toString(char* str, unsigned long long v);

// convert 64 bit signed long integer to string with base 10 (internal use only)
char* toString(char* str, long long v);

// convert signed integer to string with base 10 (internal use only)
inline char* toString(char* str, int v) {return toString(str, (long long)v);}

// convert double to string (internal use only)
char* toString(char* str, double d, int numDigits);


/// convert a string to 64 bit unsigned long integer with given base
unsigned long long toULongLong(StringRef str, int base = 10);


/// convert a value to string using operator <<
template <typename Type>
std::string toString(Type v)
{
	std::stringstream s;
	s << v;
	return s.str();
}

/// convert c-string to std::string. str may be NULL
static inline std::string toString(const char* str)
{
	return std::string(str, str + (str == NULL ? 0 : strlen(str)));
}

/// convert a number to a hexadecimal string using operator <<
template <typename Type>
std::string toHexString(Type v)
{
	std::stringstream s;
	s << std::hex << v;
	return s.str();
}

/// convert a string to bool. Returns false if str is "false" or "0", true otherwise
static inline bool toBool(const std::string& str)
{
	return str != "false" && str != "0";
}

/// convert a string to int using operator >>
static inline int toInt(const std::string& str)
{
	int v = 0;
	std::stringstream s(str);
	s >> v;
	return v;
}

/// convert a string to float using operator >>
static inline float toFloat(const std::string& str)
{
	float v = 0.0f;
	std::stringstream s(str);
	s >> v;
	return v;
}
	
/// convert a string to double using operator >>
static inline double toDouble(const std::string& str)
{
	double v = 0.0;
	std::stringstream s(str);
	s >> v;
	return v;
}


/// replaces %<number> with the lowest number by the argument
template <typename Type>
std::string arg(StringRef str, Type v)
{
	return arg(str, StringRef(toString(v)));
}
	
/// @}

} // namespace digi

#endif
