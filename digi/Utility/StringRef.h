#ifndef digi_Utility_StringRef_h
#define digi_Utility_StringRef_h

#include <assert.h>
#include <string.h>
#include <ostream>
#include <iterator>

#include "Standard.h"


namespace digi {

/// @addtogroup Utility
/// @{

struct StringRef
{
	typedef char value_type;
	typedef const char* iterator;
	typedef const char* const_iterator;
	
	/// constructs empty StringRef
	StringRef()
		: d(emptyString), s(0) {}

	/// constructs empty StringRef (e.g. StringRef(null))
	StringRef(NullType)
		: d(emptyString), s(0) {}

	/// constructs StringRef from data and length. data must not be NULL
	StringRef(const char* data, size_t length)
		: d(data), s(length) {}

	/// constructs StringRef from const char*. str must not be NULL
	StringRef(const char* str)
		: d(str), s(strlen(str)) {}
	
	/// constructs StringRef from std::string
	StringRef(const std::string& str)
		: d(str.c_str()), s(str.size()) {}

	bool empty() const {return this->s == 0;}
	size_t size() const {return this->s;}
	size_t length() const {return this->s;}
	iterator begin() const {return this->d;}
	iterator end() const {return this->d + this->s;}
	const char* data() const {return this->d;}

	char operator[](size_t index) const
	{
		assert(index < this->s && "invalid index");
		return this->d[index];
	}
		
	std::string getString() const
	{
		return std::string(this->d, this->s);
	}

	operator std::string() const
	{
		return std::string(this->d, this->s);
	}

	// an empty StringRef points to this empty string
	static const char* emptyString;

protected:

	const char* d;
	size_t s;
};

inline std::string operator +(StringRef a, StringRef b)
{
	std::string str(a.size() + b.size(), 0);
	std::copy(a.begin(), a.end(), str.begin());
	std::copy(b.begin(), b.end(), str.begin() + a.size());
	return str;
}

inline std::string operator +(char a, StringRef b)
{
	std::string str(1 + b.size(), a);
	std::copy(b.begin(), b.end(), str.begin() + 1);
	return str;
}

inline std::string operator +(StringRef a, char b)
{
	std::string str(a.size() + 1, b);
	std::copy(a.begin(), a.end(), str.begin());
	return str;
}

inline bool operator ==(StringRef a, StringRef b)
{
	if (a.size() != b.size())
		return false;
		
	const char* end = a.end();
	for (const char* it = a.begin(), * it2 = b.begin(); it != end; ++it, ++it2)
	{
		if (*it != *it2)
			return false;
	}
	return true;
}

inline bool operator !=(StringRef a, StringRef b)
{
	return !(a == b);
}

inline bool operator <(StringRef a, StringRef b)
{
	size_t minLength = std::min(a.size(), b.size());
	const char* end = a.begin() + minLength;
	for (const char* it = a.begin(), * it2 = b.begin(); it != end; ++it, ++it2)
	{
		if (*it < *it2)
			return true;
		if (*it > *it2)
			return false;
	}
	return a.size() < b.size();
}

inline bool operator >(StringRef a, StringRef b)
{
	return b < a;
}

template <typename Element, typename Traits>
std::basic_ostream<Element, Traits>& operator <<(std::basic_ostream<Element, Traits>& w, StringRef str)
{
	std::copy(str.begin(), str.end(), std::ostream_iterator<char, Element, Traits>(w));
	return w;
}

/// @}

} // namespace digi

#endif
