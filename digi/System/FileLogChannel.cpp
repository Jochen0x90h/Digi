#include "FileLogChannel.h"


namespace digi {


FileLogChannel::FileLogChannel(const fs::path& path)
{
	this->s.open(path.c_str(), std::ios_base::out | std::ios_base::trunc);
}

void FileLogChannel::write(const std::string& message, Log::Priority priority,
	const char* fileName, int lineNumber)
{
	this->s << message << std::endl;
}
			
void FileLogChannel::close()
{
	this->s.close();
}


} // namespace digi
