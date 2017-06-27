/*
	Log system for notifications, warnings and errors
*/

#ifndef digi_System_Log_h
#define digi_System_Log_h

#include <string>
#include <sstream>
#include <set>

#include <digi/Base/Platform.h>
#include <digi/Utility/Object.h>



/// logs with priority NOTIFY
#define dNotify(message) \
	do { std::stringstream _temp_; _temp_ << message; digi::Log::write(_temp_.str(), digi::Log::LOG_NOTIFY, __FILE__, __LINE__); } while (false)

/// logs with priority WARNING
#define dWarning(message) \
	do { std::stringstream _temp_; _temp_ << message; digi::Log::write(_temp_.str(), digi::Log::LOG_WARNING, __FILE__, __LINE__); } while (false)

/// logs with priority ERROR
#define dError(message) \
	do { std::stringstream _temp_; _temp_ << message; digi::Log::write(_temp_.str(), digi::Log::LOG_ERROR, __FILE__, __LINE__); } while (false)



namespace digi {

/// @addtogroup System
/// @{

class LogChannel;


/// this is a simple logging system that can output log, warning and error messages
class Log
{
	friend class LogChannel;
		
protected:
	
	/// constructor protected: should not get created by user
	Log();
	
	~Log();

public:
	
	enum Priority
	{
		/// priority level notify is for messages that are not too important
		LOG_NOTIFY = 1,

		/// warning is for messages that are interesting, but require no user interaction
		LOG_WARNING,

		/// error messages normally pop up a message dialog and request for program termination
		LOG_ERROR
	};


	/// init log system
	static void init();

	/// get static instance of log system. returns NULL if init() was not called.
	static Log* get() { return Log::log; }
	
	/// logs the message to the output channels
	static bool write(const std::string& message, Priority priority, const char* fileName, int lineNumber);

	/// close log system and all currently added channel objects.
	static void close();

	/// adds a channel object which can output the log messages
	static void addChannel(Pointer<LogChannel> channel);

	/// removes a channel object. does not close it.
	static void removeChannel(Pointer<LogChannel> channel);

protected:

	// log channels, this class has ownership
	std::set<Pointer<LogChannel> > channels;

	static Log* log;
};


/// log channel
///
/// derive from it to log to console, files or other logging api's
class LogChannel : public Object
{
	friend class Log;
	
public:

	/// destructor
	virtual ~LogChannel();

	/// this function has to be implemented by the debug channel
	virtual void write(const std::string& message, Log::Priority priority, 
		const char* fileName, int lineNumber) = 0;
		
	/// close any output file handles
	virtual void close();
};



/// @}

} // namespace digi

#endif
