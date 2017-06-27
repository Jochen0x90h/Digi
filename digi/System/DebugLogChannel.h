/*
	debug channel for OS logging system. uses OutputDebugStringA on windows.
*/

#ifndef digi_System_DebugLogChannel_h
#define digi_System_DebugLogChannel_h

#include "Log.h"



namespace digi {

/// @addtogroup System
/// @{


class DebugLogChannel : public LogChannel
{
	public:

		virtual void write(const std::string& message, Log::Priority priority,
			const char* fileName, int lineNumber);
};


/// @}

} // namespace digi

#endif
