#include <map>
#include <fstream>

#include <boost/range.hpp>

#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/Ascii.h>
#include <digi/Utility/foreach.h>
#include <digi/Utility/VectorUtility.h>


using namespace digi;


static void addForbiddenVariable(std::vector<int>& forbiddenVariables, const char* variableName)
{
	const char* str = variableName;
		
	// convert variable to int
	int variable = 0;
	int base = 1;
	while (*str != 0)
	{
		int digit;
		char ch = *str;
		if (ch >= 'a' && ch <= 'z')
			digit = ch - 'a';
		else if (ch >= 'A' && ch <= 'Z')
			digit = ch - 'A' + 26;
		else
			digit = ch - '0' + 52;
				
		variable += digit * base;
		base *= base == 1 ? 52 : 62;
			
		++str;
	}

	forbiddenVariables += variable;

	// check if the int gets converted back to the same string
	//char buf[10];
	//*convertIntToIdentifier(buf, variable) = 0;
	//return strcmp(buf, variableName) != 0;
}

// variables for digi/CodeGenerator/NameGenerator.cpp
void genNameGenerator()
{
	// reserved words with up to 3 letters (200000 vars). words ending with 'a' are not generated anyway
	static const char* forbiddenVariableNames[] =
	{
		// keywords
		"do",
		"for",
		"if",
		"in",
		"is",		
		"out",
		"ref",

		// types
		"int",

		// functions
		"abs",
		"min",
		"max",
		"sin",
		"cos",
		"tan",
	};
	
	// list of words
	std::vector<int> forbiddenVariables;
	for (size_t i = 0; i < boost::size(forbiddenVariableNames); ++i)
	{
		addForbiddenVariable(forbiddenVariables, forbiddenVariableNames[i]);
	}	

	// two component swizzle (e.g. .xw)
	for (int i = 0; i < 4*4; ++i)
	{
		char buf[3];
		buf[0] = 'w' + (i & 3);
		buf[1] = 'w' + (i >> 2);
		buf[2] = 0;
		addForbiddenVariable(forbiddenVariables, buf);
	}
	
	// three component swizzle (e.g. .xyz)
	for (int i = 0; i < 4*4*4; ++i)
	{
		char buf[4];
		buf[0] = 'w' + (i & 3);
		buf[1] = 'w' + ((i >> 2) & 3);
		buf[2] = 'w' + (i >> 4);
		buf[3] = 0;
		addForbiddenVariable(forbiddenVariables, buf);
	}

	sort(forbiddenVariables);

	// write array of forbidden variables
	std::ofstream w("forbiddenVariables.h", std::ios::out | std::ios::binary);
	w << "static const int forbiddenVariables[] = \n";
	w << "{\n";
	int numValues = int(forbiddenVariables.size());
	std::vector<int>::iterator values = forbiddenVariables.begin();
	while (numValues > 0)
	{
		int num = std::min(numValues, 16);
		w << "\t";
		for (int i = 0; i < num; ++i)
		{
			w << *values << ", ";
			++values;
		}
		w << "\n";
		numValues -= num;
	}
	w << "};\n";
	w.close();
}

int main()
{
	// generate forbiddenVariables.h for NameGenerator
	genNameGenerator();
}
