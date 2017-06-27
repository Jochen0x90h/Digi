#include "AudioException.h"


namespace digi {

const char* AudioException::what() const throw()
{
	static const char* reasons[] =
	{
		"audio error",
		"format not supported",
	};
	return reasons[this->reason];
}

} // namespace digi
