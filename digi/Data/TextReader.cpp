#include "TextReader.h"


namespace digi {

namespace
{
	struct TextParser
	{
		TextParser(std::string& line)
			: line(line) {}
		
		int parse(char ch)
		{
			return ch == '\r' || ch == '\n' ? 0 : -1;
		}

		void add(char* begin, char* end)
		{
			this->line.append(begin, end);
		}

		std::string& line;
	};

	struct NewLineParser
	{
		NewLineParser()
			: last() {}

		int parse(char ch)
		{
			// parse "\n" (unix), "\r" (mac), "\r\n" (windows)
			if (this->last == '\r')
				return ch == '\n' ? 1 : 0;
			if (this->last == '\n')
				return 0;

			this->last = ch;
			return -1;
		}

		void add(char* begin, char* end)
		{
		}

		char last;
	};
} // anonymous namespace

std::string TextReader::read()
{
	// parse text line
	std::string line;
	TextParser textParser(line);
	this->e = !this->parse(textParser, true); // noThrow

	// parse unix, mac or windows line ending
	NewLineParser newLineParser;
	this->parse(newLineParser, true); // noThrow
	
	return line;
}

} // namespace digi
