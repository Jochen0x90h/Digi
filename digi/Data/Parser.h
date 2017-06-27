#ifndef digi_Data_Parser_h
#define digi_Data_Parser_h

#include "BufferedReader.h"


namespace digi {

/// @addtogroup Data
/// @{

/// parser for use with BufferedReader that eats white spaces
struct WhiteSpaceParser
{
	WhiteSpaceParser()
		: lineIndex(1) {}
	
	int parse(char ch)
	{
		if (ch == '\n')
			++lineIndex;
		return uint8_t(ch) <= 32 ? -1 : 0;
	}

	void add(char* begin, char* end)
	{
	}
	
	// line counter
	int lineIndex;
};

///  parser for use with BufferedReader that parses one char and sets success to true if it was the given char
struct CharParser
{
	CharParser(char ch)
		: ch(ch), success() {}

	int parse(char ch)
	{
		return (this->success = ch == this->ch) ? 1 : 0;
	}

	void add(char* begin, char* end)
	{
	}

	char ch;
	bool success;
};

/// @}

} // namespace digi

#endif
