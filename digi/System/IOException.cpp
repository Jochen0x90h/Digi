#include "IOException.h"


namespace digi {

IOException::~IOException() throw()
{
	// protect against double destruction, see http://www.boost.org/community/error_handling.html
	this->device = null;
}

const char* IOException::what() const throw()
{
	static const char* reasons[] =
	{
		"input/output error",
		"invalid handle",
		"access denied",
		"closed",
		"file not found",
		"disk full",
		"seek error",
		"unknown host",
		"address in use",
		"unreachable",
		"timed out",
		"connection refused",
		"connection aborted",
		"connection closed by peer",
		"shutdown",
	};
	return reasons[this->reason];
}

} // namespace digi
