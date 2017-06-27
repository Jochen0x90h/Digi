/*
	log channel for output into a file
*/

#ifndef digi_System_FileLogChannel_h
#define digi_System_FileLogChannel_h

#include <fstream>

#include "Log.h"
#include "FileSystem.h"


namespace digi {

/// @addtogroup System
/// @{


class FileLogChannel : public LogChannel
{
	public:

		FileLogChannel(const fs::path& path);

		virtual void write(const std::string& message, Log::Priority priority,
			const char* fileName, int lineNumber);

		virtual void close();

		std::fstream s;
};


/// @}

} // namespace digi

#endif
