#include "DataException.h"


namespace digi {

// DataException

DataException::~DataException() throw()
{
}

const char* DataException::what() const throw()
{
	static const char* reasons[] =
	{
		"unexpected end of data",
		"format not supported",
		"format error",
		"unknown version",
		"data incomplete",
		"data inconsistent",
		"data corrupt",
		"bad value",
	};
	return reasons[this->reason];
}

// TextDataException

TextDataException::~TextDataException() throw()
{
}

} // namespace digi
