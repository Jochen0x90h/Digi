#include <digi/Utility/foreach.h>

#include "Log.h"


namespace digi {

// Log

Log::Log()
{
}

Log::~Log()
{
}

void Log::init()
{
	if (Log::log == NULL)
		Log::log = new Log();
}

bool Log::write(const std::string& message, Priority priority, const char* fileName, int lineNumber)
{
	//PROFILE("Log::write - all printed output");	

	Log* log = Log::log;

	if (log != NULL)
	{
		// lock mutex
		// Lock lock(log->mutex);
								
		// call the channels
		foreach (Pointer<LogChannel> channel, log->channels)
			channel->write(message, priority, fileName, lineNumber);
	}

	return false;
}

void Log::close()
{
	Log* log = Log::log;
	if (log != NULL)
	{
		// close log channels
		foreach (Pointer<LogChannel> channel, log->channels)
			channel->close();
	
		delete Log::log;
		Log::log = NULL;
	}
}

void Log::addChannel(Pointer<LogChannel> channel)
{
	// don't forget to call Log::init() before addChannel

	Log::log->channels.insert(channel);
}

void Log::removeChannel(Pointer<LogChannel> channel)
{
	Log::log->channels.erase(channel);
}

Log* Log::log = NULL;



// LogChannel

LogChannel::~LogChannel()
{
}

void LogChannel::close()
{
}

} // namespace digi
