#ifndef digi_System_FileCatcher_h
#define digi_System_FileCatcher_h

#include "File.h"
#include "IOException.h"


namespace digi {

/// @addtogroup System
/// @{

/*
	wrapper for IODevice that catches and stores the reason of an IOException.
	this is mainly for passing exceptions through C code such as image and audio libraries.
*/
struct IOCatcher
{
	Pointer<IODevice> dev;
	IOException::Reason reason;
	
	IOCatcher(Pointer<IODevice> dev)
		: dev(dev), reason(IOException::NO_ERROR)
	{
	}
		
	size_t read(void* data, size_t size)
	{
		try
		{
			return this->dev->read(data, size);
		}
		catch (IOException& e)
		{
			this->reason = e.getReason();
		}
		return -1;
	}
	
	size_t write(void* data, size_t size)
	{
		try
		{
			return this->dev->write(data, size);
		}
		catch (IOException& e)
		{
			this->reason = e.getReason();
		}
		return -1;
	}

	int64_t seek(int64_t position, File::PositionMode positionMode)
	{
		try
		{
			return this->dev->seek(position, positionMode);
		}
		catch (IOException& e)
		{
			this->reason = e.getReason();
		}
		return -1;
	}

	int64_t getPosition() {return this->seek(0, File::CURRENT);}

	int64_t getSize()
	{
		try
		{
			return this->dev->getSize();
		}
		catch (IOException& e)
		{
			this->reason = e.getReason();
		}
		return -1;
	}
	
	// check in an IOException was catched and throw it again
	void checkState()
	{
		if (this->reason != IOException::NO_ERROR)
			throw IOException(this->dev, this->reason);
	}
};

/// @}

} // namespace digi

#endif
