/*
	log channel for output on text console
*/

#ifndef digi_System_ConsoleLogChannel_h
#define digi_System_ConsoleLogChannel_h

#include "Log.h"



namespace digi {

/// @addtogroup System
/// @{


class ConsoleLogChannel : public LogChannel
{
public:

	virtual ~ConsoleLogChannel();
	
	virtual void write(const std::string& message, Log::Priority priority,
		const char* fileName, int lineNumber);
};


/// @}

} // namespace digi

#endif
