#include <digi/Utility/Ascii.h>
#include <digi/Utility/StringUtility.h>

#include "NameMangler.h"


namespace digi {

// NameMangler

void NameMangler::addFunction(StringRef name)
{

	this->s << "_Z" << name.length() << name;
}

void NameMangler::addArgument(VectorInfo::Type type)
{
	// letters for bool, signed char, unsigned char, short, unsigned short, int, unsigned int,
	// long, unsigned long, float, double
	this->s << "bahstijlmfd"[type];
}

void NameMangler::addArgument(VectorInfo type)
{
	if (type.numRows > 1)
		this->s << "Dv" << type.numRows << '_';

	this->addArgument(type.type);
}


// NameDemangler

std::string NameDemangler::demangle(StringRef mangled)
{
	if (startsWith(mangled, "_Z"))
	{
		int length = 0;
		int pos = 2;
		while (Ascii::isDecimal(mangled[pos]))
		{
			length *= 10;
			length += mangled[pos] - '0';
			++pos;
		}
		return substring(mangled, pos, pos + length);
	}
	
	// assume name is unmangled
	return mangled;
}


} // namespace digi
