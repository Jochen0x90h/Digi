#include <digi/Utility/Ascii.h>
#include <digi/Utility/StringUtility.h>

#include "CodeWriterFunctions.h"


namespace digi {


char* convertIntToIdentifier(char* str, int v)
{
	// first char is a-z, A-Z
	{
		int digit = char(v % (26+26));
		v /= (26+26);
		if (digit < 26)
			*str = digit + 'a';
		else
			*str = digit-26 + 'A';
		++str;
	}
	
	// second to last chars are a-z, A-Z, 0-9
	while (v > 0)
	{
		int digit = char(v % (26+26+10));
		v /= (26+26+10);
		if (digit < 26)
			*str = digit + 'a';
		else if (digit < 26+26)
			*str = digit-26 + 'A';
		else
			*str = digit-26-26 + '0';
		++str;
	}

	return str;
}
	
std::string createVariableName(const std::string& str)
{
	int len = int(str.length());

	// begin variable name with "_"
	std::string escaped = "_";
	
	for (int i = 0; i < len; ++i)
	{
		char ch = str[i];
		switch (ch)
		{
		case '_':
			escaped += "__";
			break;
		case ' ':
			escaped += "_s";
			break;
		case '/':
		case '\\':
			escaped += "_7";
			break;
		case '.':
			escaped += "_p";
			break;
		case ':':
			escaped += "_c";
			break;
		case '!':
			escaped += "_i";
			break;
		case '-':
			escaped += "_m";
			break;
		case '=':
			escaped += "_e";
			break;
		case '&':
			escaped += "_8";
			break;
		case '|':
			escaped += "_l";
			break;
		case '^':
			escaped += "_v";
			break;
		case '#':
			escaped += "_n";
			break;
		default:
			escaped += ch;
		}
	}
	return escaped;		
}


} // namespace digi
