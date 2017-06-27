#ifndef digi_System_IODevice_h
#define digi_System_IODevice_h

#include <ios>

#include <digi/Base/Platform.h>
#include <digi/Utility/Standard.h>
#include <digi/Utility/Object.h>


namespace digi {

/// @addtogroup System
/// @{

/// abstraction for all kinds of input/output devices, e.g. files, network sockets, serial ports
class IODevice : public Object
{
public:

	/// destructor
	virtual ~IODevice();

// state

	enum State
	{
		OPEN = 1,
		//END_OF_INPUT = 2,
	};

	/// get state of device
	virtual int getState() = 0;

	/// returns true if the device is open
	bool isOpen() {return (this->getState() & OPEN) != 0;}

	/// returns true if the device is at end of input (eof = end of file)
	//bool isEndOfInput() {return (this->getState() & END_OF_INPUT) != 0;}

	/// closes the device
	///
	/// note: close can produce an error e.g. if the disk is full and cached data can not be written.
	/// therefore always close a device explicitly inside a try/catch block
	virtual void close() = 0;

	/// get underlying resource (path, device, address etc.)
	virtual std::string getResource();

// stream access

	/// read data with given length. returns -1 if nothing could be read after timeout
	virtual size_t read(void* data, size_t length) = 0;
	
	/// write data with given length. returns -1 if nothing could be written after timeout
	virtual size_t write(const void* data, size_t length) = 0;

// random access

	enum PositionMode
	{
		BEGIN = SEEK_SET,
		CURRENT = SEEK_CUR,
		END	= SEEK_END
	};

	/// set position relative to begin, current position or end of file and return resulting position relative to begin
	virtual int64_t seek(int64_t position, PositionMode mode);

	/// set position of file pointer
	void setPosition(int64_t position) {this->seek(position, BEGIN);}

	/// get position of file pointer
	int64_t getPosition() {return this->seek(0, CURRENT);}

	/// set size of file
	virtual void setSize(int64_t size);
	
	/// get size of file
	virtual int64_t getSize();
};

/// @}

} // namespace digi

#endif
