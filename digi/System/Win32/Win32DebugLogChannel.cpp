#include "../DebugLogChannel.h"

#include <windows.h>
#undef min
#undef max
#undef NO_ERROR
#undef ERROR


namespace digi {

void DebugLogChannel::write(const std::string& message, Log::Priority priority,
	const char* fileName, int lineNumber)
{
	std::stringstream s;
	s << fileName << '(' << lineNumber << ") : " << message << std::endl;
	OutputDebugStringA(s.str().c_str());
}
			
} // namespace digi
