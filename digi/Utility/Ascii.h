#ifndef digi_Utility_Ascii_h
#define digi_Utility_Ascii_h

#include "StringRef.h"


namespace digi {

/// @addtogroup Utility
/// @{

/*
	helper functions for ASCII characters and strings.
	They only know about the lower 7 bit of each character and therefore do not work for unicode.
*/
namespace Ascii
{
	inline bool isSpace(char ch)
	{
		return ch == ' ' || (ch >= 9 && ch <= 13);
	}

	inline bool isLetter(char ch)
	{
		return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
	}

	inline bool isDecimal(char ch)
	{
		return ch >= '0' && ch <= '9';
	}

	inline int getDecimalValue(char ch)
	{
		if (ch >= '0' && ch <= '9')
			return ch - '0';
		return 0;
	}

	inline bool isHex(char ch)
	{
		return (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f');
	}

	inline int getHexValue(char ch)
	{
		if (ch >= '0' && ch <= '9')
			return ch - '0';
		if (ch >= 'A' && ch <= 'F')
			return ch - 'A' + 10;
		if (ch >= 'a' && ch <= 'f')
			return ch - 'a' + 10;
		return 0;
	}
	
	inline bool isFloat(char ch)
	{
		return (ch >= '0' && ch <= '9') || ch == '.' || ch == 'e' || ch == 'E' || ch == '+' || ch == '-';
	}
	
	inline bool isIdentifier(char ch)
	{
		return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_' || (ch >= '0' && ch <= '9') || (ch & 0x80) != 0;
	}

	/// check if character is upper case ('A' - 'Z')
	inline char isUpperCase(char ch)
	{
		return ch >= 'A' && ch <= 'Z';
	}
	
	/// convert to upper case
	inline char toUpperCase(char ch)
	{
		if (ch >= 'a' && ch <= 'z')
			return ch - 32;
		return ch;
	}

	/// check if character is upper case ('a' - 'z')
	inline char isLowerCase(char ch)
	{
		return ch >= 'a' && ch <= 'z';
	}

	/// convert to lower case
	inline char toLowerCase(char ch)
	{
		if (ch >= 'A' && ch <= 'Z')
			return ch + 32;
		return ch;
	}
	
		
	/// convert string to upper case
	std::string toUpperCase(StringRef str);

	/// convert string to lower case
	std::string toLowerCase(StringRef str);

	/// convert first letter of each word to uppercase, the rest to lowercase
	std::string capitalize(StringRef str);
};

} // namespace digi

#endif
