#ifndef digi_Utility_StringUtility_h
#define digi_Utility_StringUtility_h

#include <stddef.h>
#include <limits>
#include <cstring>
#include <cwchar>
#include <algorithm>
#include <assert.h>

#include "StringRef.h"

#undef max


namespace digi {

/// returns length of the c-string
inline std::size_t length(const char* s)
{
	return strlen(s);
}

/// returns length of the wide c-string
inline std::size_t length(const wchar_t* s)
{
	return wcslen(s);
}

/// returns a pointer to the string data
static inline char* getData(std::string& str)
{
	return (char*)str.data();
}

static inline const char* getData(StringRef str)
{
	return str.data();
}


/// find index of character in a string, returns the default result if not found
inline ptrdiff_t find(StringRef str, char ch, ptrdiff_t start = 0, ptrdiff_t defaultResult = -1)
{
	StringRef::iterator end = str.end();
	StringRef::iterator it = std::find(str.begin() + start, end, ch);
	return it != end ? it - str.begin() : defaultResult;
}

/// find index of string in a string, returns the length of the string if not found
inline ptrdiff_t find(StringRef str, StringRef str2, ptrdiff_t start = 0, ptrdiff_t defaultResult = -1)
{
	StringRef::iterator end = str.end();
	StringRef::iterator it = std::search(str.begin() + start, end, str2.begin(), str2.end());
	return it != end ? it - str.begin() : defaultResult;
}


/// reverse find a character in a string, returns -1 if not found
ptrdiff_t findRev(StringRef str, char ch, ptrdiff_t start = std::numeric_limits<ptrdiff_t>::max());

/// find one of the characters in a string, returns the length of the string if not found
static inline ptrdiff_t findOneOf(StringRef str, StringRef charSet, ptrdiff_t start = 0, ptrdiff_t defaultResult = -1)
{
	StringRef::iterator end = str.end();
	StringRef::iterator it = std::find_first_of(str.begin() + start, str.end(), charSet.begin(), charSet.end());
	if (it >= end)
		return int(defaultResult);
	return it == end ? defaultResult : it - str.begin();
}

/// find one of the characters in a string, returns -1 if not found
static inline ptrdiff_t findOneOfRev(const std::string& str, const char* charSet, ptrdiff_t start = std::numeric_limits<ptrdiff_t>::max())
{
	return str.find_last_of(charSet, start);
}


/// checks if a string starts with another string (e.g. startsWith("Hello World", "Hello") is true)
static inline bool startsWith(StringRef str, StringRef start)
{
	return start.length() <= str.length() && memcmp(str.data(), start.data(), start.length()) == 0;
}

/// chcks if a string ends with another string (e.g. startsWith("Hello World", "World") is true)
static inline bool endsWith(StringRef str, StringRef end)
{
	return end.length() <= str.length() && memcmp(str.data() + str.length() - end.length(), end.data(), end.length()) == 0;
}

/// checks if a string contains a given character
static inline bool contains(StringRef str, char ch)
{
	return find(str, ch) != -1;
}

/// checks if a string contains a given substring
static inline bool contains(StringRef str, StringRef substring)
{
	return find(str, substring) != -1;
}

/// checks if a string contains a given substring as whole word (using Ascii::isIdentifier)
bool containsWord(StringRef str, StringRef substring);

	
/// get substring of string
std::string substring(StringRef str, ptrdiff_t start, ptrdiff_t end = std::numeric_limits<ptrdiff_t>::max());

/// replace character a by character b in given string str in-place
void replaceString(std::string& str, char a, char b);

/// replace character a by character b in given string str
inline std::string replace(const std::string& str, char a, char b)
{
	std::string tmp = str;
	replaceString(tmp, a, b);
	return tmp;
}

/// replace in-place string a by string b in given string str in-place
void replaceString(std::string& str, StringRef a, StringRef b);

/// replace string a by string b in given string str
inline std::string replace(const std::string& str, StringRef a, StringRef b)
{
	std::string tmp = str;
	replaceString(tmp, a, b);
	return tmp;
}

/// remove white spaces at beginning and end
std::string trim(StringRef str);

/// replaces %<number> with the lowest number by the argument. see Convert.h for a generic version
std::string arg(StringRef str, StringRef value);

/// returns true if the string matches the wildcard which has special characters '?' for any character
/// and '*' for 0-n times any character
bool wildcardMatch(StringRef wildcard, StringRef str);

} // namespace digi

#endif
