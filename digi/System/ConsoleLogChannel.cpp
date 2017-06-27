#include <iostream>

#include <digi/Utility/UTFTranscode.h>
#include "ConsoleLogChannel.h"

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN 1
	#include <windows.h>
	#undef NO_ERROR
	#undef ERROR
#endif

namespace digi {

ConsoleLogChannel::~ConsoleLogChannel()
{
}

void ConsoleLogChannel::write(const std::string& message, Log::Priority priority,
	const char* fileName, int lineNumber)
{
#ifdef _WIN32

	std::wstring wMessage = utfString<wchar_t>(message);	
	if (priority == Log::LOG_NOTIFY)
		std::wcout << wMessage << std::endl;
	else
		std::wcerr << wMessage << std::endl;	
		
//#ifndef NDEBUG
	OutputDebugStringW(wMessage.c_str());
	OutputDebugStringW(L"\n");
//#endif

#else

	if (priority == Log::LOG_NOTIFY)
		std::cout << message << std::endl;
	else
		std::cerr << message << std::endl;	

#endif
}
			
} // namespace digi
