#include "Pointer.h"


namespace digi {

BadCast::~BadCast() throw()
{
}

const char* BadCast::what() const throw()
{
	return "bad cast";
}

} // namespace digi
