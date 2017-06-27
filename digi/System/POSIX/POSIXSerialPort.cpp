#include <fcntl.h>

#include "../SerialPort.h"
#include "../IOException.h"

#include <termios.h>

#define INVALID_HANDLE_VALUE -1

#define CHECK(condition) \
	if (!(condition)) { \
		if (this->handle == INVALID_HANDLE_VALUE) throw IOException(this, IOException::CLOSED); \
		SerialPort::throwException(this); \
	}


namespace digi {

// POSIX implementation of SerialPort
class POSIXSerialPort : public SerialPort
{
public:

	POSIXSerialPort(int handle, const fs::path& path)
		: handle(handle), path(path)
	{
	}

	virtual ~POSIXSerialPort()
	{
		if (this->handle != INVALID_HANDLE_VALUE)
		{
			// handle still open in the destructor may happen in these two cases:
			// 1. an exception is thrown and the destructor is called while stack unrolling
			// 2. the user forgot to call close() which is a bug
			::close(this->handle);
		}	
	}

	virtual int getState()
	{
		return this->handle != INVALID_HANDLE_VALUE ? OPEN : 0;
	}

	virtual void close()
	{
		int result = ::close(this->handle);
		CHECK(result != -1);

		this->handle = INVALID_HANDLE_VALUE;
	}

	virtual std::string getResource()
	{
		return this->path.string();
	}
	
	virtual size_t read(void* data, size_t length)
	{
		ssize_t numRead = ::read(this->handle, data, length);
		CHECK(numRead != -1);

		return numRead;
	}
	
	virtual size_t write(const void* data, size_t length)
	{
		ssize_t numWritten = ::write(this->handle, data, length);
		CHECK(numWritten != -1);

		return numWritten;
	}
	
	virtual void purge(int direction)
	{
		direction &= PURGE_RX | PURGE_TX;
		if (direction == 0)
			return;
		
		int action = TCIOFLUSH;
		if (direction == PURGE_RX)
			action = TCIFLUSH;
		else if (direction == PURGE_TX)
			action = TCOFLUSH;

		int result = tcflush(this->handle, action);
		CHECK(result != -1);
	}
	
	
	int handle;
	fs::path path;
};


Pointer<SerialPort> SerialPort::open(const fs::path& path, int speed, int mode, int timeout)
{
	int wordLength = mode & 0x000F;
	int parity = mode & 0x00F0;
	int stopBits = mode & 0x0F00;

	int handle = ::open(path.c_str(), // device name
		// O_RDWR - read and write access
		// O_NOCTTY - open() shall not cause the terminal device to become the controlling terminal for the process
		// O_NDELAY - we don’t care if the other side is connected (some devices don’t explicitly connect)
		O_RDWR | O_NOCTTY | O_NDELAY);
	if (handle == INVALID_HANDLE_VALUE)
		SerialPort::throwException(new POSIXSerialPort(handle, path));
	
	// clear non-blocking flag
	fcntl(handle, F_SETFL, 0);
	
	// terminal options
	struct termios options;
	memset(&options, 0, sizeof(options));
	
	// CLOCAL - don't allow control of the port to be changed
	// CREAD - enable the receiver
	options.c_cflag = CLOCAL | CREAD;
	
	// set word length
	if (wordLength == LENGTH_5)
		options.c_cflag |= CS5;
	else if (wordLength == LENGTH_6)
		options.c_cflag |= CS6;
	else if (wordLength == LENGTH_7)
		options.c_cflag |= CS7;
	else
		options.c_cflag |= CS8;

	// set parity
	if (parity == PARITY_EVEN)
		options.c_cflag |= PARENB;
	else if (parity == PARITY_ODD)
		options.c_cflag |= PARENB | PARODD;

	// set number of stop bits
	if (stopBits == STOP_2)
		options.c_cflag |= CSTOPB;
	
	// input options
	// IGNPAR - ignore parity errors
	options.c_iflag = IGNPAR;
	
	// output options
	options.c_oflag = 0;
	
	// line options
	options.c_lflag = 0; 

	// baud rate at which the communications device operates (may modify c_cflag)
	speed_t s;
	switch (speed)
	{
	case 300:
		s = B300;
		break;
	case 600:
		s = B600;
		break;
	case 1200:
		s = B1200;
		break;
	case 2400:
		s = B2400;
		break;
	case 4800:
		s = B4800;
		break;
	case 9600:
		s = B9600;
		break;
	case 19200:
		s = B19200;
		break;
	case 38400:
		s = B38400;
		break;
	#ifdef B57600
	case 57600:
		s = B57600;
		break;
	#endif
	#ifdef B76800
	case 76800:
		s = B76800;
		break;
	#endif
	#ifdef B115200
	case 115200:
		s = B115200;
		break;
	#endif
	default:
		//! custom bit rate
		s = B9600;
	}
	cfsetispeed(&options, s);
	cfsetospeed(&options, s);
	
	// control characters
	options.c_cc[VMIN] = 0; // minimum amount of characters to read
	options.c_cc[VTIME] = (timeout + 50) / 100; // time to wait until exiting read (tenths of a second)
	
	// set options
	tcsetattr(handle, TCSANOW, &options); 
	 
	// create and return the serial port object
	return new POSIXSerialPort(handle, path);
}

void SerialPort::throwException(Pointer<SerialPort> serialPort)
{
	int e = errno;
	IOException::Reason reason = IOException::IO_ERROR;
	switch (e)
	{
	case 0:
		// do not throw on success
		return;

	case EBADF: // invalid file handle, write on file opened for read, read on file opened for write
		reason = IOException::INVALID_HANDLE;
		break;

	case EACCES:
	case ENOLCK: // no lock available
	case EROFS: // read-only file system
		reason = IOException::ACCESS_DENIED;
		break;

	case ENOENT:
		reason = IOException::FILE_NOT_FOUND;
		break;

	}
	throw IOException(serialPort, reason);
}

} // namespace digi
