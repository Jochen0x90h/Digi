#include "Ascii.h"


namespace digi {

std::string Ascii::toUpperCase(StringRef str)
{
	size_t length = str.length();
	std::string result(length, ' ');
	for (size_t i = 0; i < length; ++i)
	{
		result[i] = Ascii::toUpperCase(str[i]);
	}
	return result;
}

std::string Ascii::toLowerCase(StringRef str)
{
	size_t length = str.length();
	std::string result(length, ' ');
	for (size_t i = 0; i < length; ++i)
	{
		result[i] = Ascii::toLowerCase(str[i]);
	}	
	return result;
}

std::string Ascii::capitalize(StringRef str)
{
	size_t length = str.length();
	std::string result(length, ' ');
	bool startOfWord = true;
	for (size_t i = 0; i < length; ++i)
	{
		char ch = str[i];
		result[i] = startOfWord ? Ascii::toUpperCase(ch) : Ascii::toLowerCase(ch);
		
		// if character is not an identifier a new word starts
		startOfWord = !isIdentifier(ch);
	}
	return result;
}

} // namespace digi
