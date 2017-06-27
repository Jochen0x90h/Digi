#include <digi/Base/Platform.h>

#include "Ascii.h"
#include "Convert.h"
#include "StringUtility.h"


namespace digi {

ptrdiff_t findRev(StringRef str, char ch, ptrdiff_t start)
{
	ptrdiff_t i = std::min(ptrdiff_t(str.length() - 1), start);
	while (i >= 0 && str[i] != ch)
		--i;
	return i;
}

bool containsWord(StringRef str, StringRef word)
{
	if (word.empty())
		return false;

	ptrdiff_t len = str.length();
	ptrdiff_t s = 0;
	while (s < len)
	{
		while (s < len && !Ascii::isIdentifier(str[s]))
			++s;
		
		ptrdiff_t e = s;
		while (e < len && Ascii::isIdentifier(str[e]))
			++e;
		
		if (e - s == word.length() && memcmp(str.data() + s, word.data(), word.length()) == 0)
			return true;
	}
	return false;
}

std::string substring(StringRef str, ptrdiff_t start, ptrdiff_t end)
{
	if (end > ptrdiff_t(str.length()))
		end = str.length();

	if (start >= end)
		return std::string();
	return std::string(str.data() + start, end - start);
}

void replaceString(std::string& str, char a, char b)
{
	for (std::string::iterator it = str.begin(); it != str.end(); ++it)
	{
		if (*it == a)
			*it = b;
	}
}

void replaceString(std::string& str, StringRef a, StringRef b)
{
	ptrdiff_t pos = 0;
	
	while (true)
	{
		pos = str.find(a.data(), pos, a.length());
		if (pos == std::string::npos)
			break;
		str.replace(pos, a.length(), b.data(), b.length());
		pos += b.length();
	}
}
/*
std::string replace(const std::string& str, char a, char b)
{
	std::string tmp = str;
	for (std::string::iterator it = tmp.begin(); it != tmp.end(); ++it)
	{
		if (*it == a)
			*it = b;
	}
	return tmp;
}

std::string replace(const std::string& str, const std::string& a, const std::string& b)
{
	std::string tmp = str;
	ptrdiff_t pos = 0;
	
	while (true)
	{
		pos = tmp.find(a, pos);
		if (pos == std::string::npos)
			break;
		tmp.replace(pos, a.length(), b);
		pos += b.length();
	}
	return tmp;
}
*/
std::string trim(StringRef str)
{
	size_t s = 0;
	size_t e = str.length();
	
	while (s < e && (uint8_t)str[s] <= 32)
		++s;
	while (e > s && (uint8_t)str[e - 1] <= 32)
		--e;

	return substring(str, s, e);
}

/*
std::string getFileStem(const std::string& str)
{
	int dotPos = findOneOfRev(str, "./\\");
	if (dotPos < 0 || str[dotPos] != '.')		
		dotPos = int(str.length());
	int slashPos = findOneOfRev(str, "/\\", dotPos);
	return str.substr(slashPos + 1, dotPos - (slashPos + 1));
}

std::string getFilePathAndStem(const std::string& str)
{
	int dotPos = findOneOfRev(str, "./\\");
	if (dotPos < 0 || str[dotPos] != '.')		
		dotPos = int(str.length());
	return str.substr(0, dotPos);
}

std::string getFileExtension(const std::string& str)
{
	int dotPos = findOneOfRev(str, "./\\");
	if (dotPos >= 0 && str[dotPos] == '.')
		return str.substr(dotPos);
	return std::string();
}
*/

std::string arg(StringRef str, StringRef value)
{
	std::string st = str;

	size_t len = st.length();
	
	// step 1: determine lowest number
	int minNum = 0x7fffffff;
	size_t e = 0;
	while (e < len)
	{
		if (st[e] == '%')
		{
			size_t s = e;
			++e;
			int num = 0;
			while (e < len && Ascii::isDecimal(st[e]))
			{
				num *= 10;
				num += st[e] - '0';
				++e;
			}
				
			if (e >= s + 2)
				minNum = std::min(num, minNum);
		}
		else
		{
			++e;
		}
	}
	
	if (minNum == 0x7fffffff)
		return st;
	
	// step 2: replace all occurences of lowest number
	e = 0;
	while (e < len)
	{
		if (st[e] == '%')
		{
			size_t s = e;
			++e;
			int num = 0;
			while (e < len && Ascii::isDecimal(st[e]))
			{
				num *= 10;
				num += st[e] - '0';
				++e;
			}
				
			if (e >= s + 2 && num == minNum)
			{
				st.replace(s, e - s, value.data(), value.length());
				len = st.length();
				e = s + value.length();
			}
		}
		else
		{
			++e;
		}
	}
	
	return st;
}

bool wildcardMatch(StringRef wildcard, StringRef str)
{
	size_t iw = 0;
	size_t is = 0;
	size_t lw = wildcard.size();
	size_t ls = str.size();
	while (iw < lw)
	{
		char ch = wildcard[iw];
		++iw;
		if (ch == '?')
		{
			if (is == ls)
				return false;

			// skip to the head of the next utf-8 character
			do {
				++is;
			} while (is < ls && (str[is] & 0xC0) == 0x80);
		}
		else if (ch == '*')
		{
			StringRef wildcardTail(wildcard.data() + iw, lw - iw);
			do
			{
				if (wildcardMatch(wildcardTail, StringRef(str.data() + is, ls - is)))
					return true;
				if (is == ls)
					return false;
				
				// skip to the head of the next utf-8 character
				do {
					++is;
				} while (is < ls && (str[is] & 0xC0) == 0x80);
			} while (true);
		}
		else
		{
			if (is == ls)
				return false;
			if (ch != str[is])
				return false;
			++is;
		}
	}
	return iw == lw && is == ls;
}

} // namespace digi
