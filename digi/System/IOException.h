#ifndef digi_System_IOException_h
#define digi_System_IOException_h

#include <exception>

#include <digi/Utility/Pointer.h>
#include "IODevice.h"

#undef NO_ERROR


namespace digi {

/// @addtogroup System
/// @{

/// exception for all kinds of IO operations
class IOException : public std::exception
{
public:

	enum Reason
	{
		NO_ERROR = -1,
	
		// unspecified input/output error
		IO_ERROR = 0,
				
		// invalid file handle
		INVALID_HANDLE,

		// access denied, e.g. trying to write on a read-only file or file locked by another process
		ACCESS_DENIED,

		// device is already closed
		CLOSED,

	// file specific

		// file or directory not found
		FILE_NOT_FOUND,
		
		// disk is full
		DISK_FULL,
		
		// error while seeking
		SEEK_ERROR,

	// network or pipe specific

		// host not found by gethosbyname()
		UNKNOWN_HOST,
		
		// network address/port is already in use
		ADDRESS_IN_USE,
		
		// network is unreachable, no route known
		UNREACHABLE,
		
		// no response after connect, e.g. because a firewall swallows the packets instead of replying "unreachable"
		TIMEDOUT,

		// connection refused by remote host
		CONNECTION_REFUSED,
		
		// the virtual circuit was aborted because of a timeout or other failure
		CONNECTION_ABORTED,
		
		// the virtual circuit was reset by the remote host executing a hard or abortive close
		CONNECTION_CLOSED_BY_PEER,
		
		// trying to send/receive on socket that was shut down
		SHUTDOWN,
	};

	IOException(Pointer<IODevice> device, Reason reason)
		: device(device), reason(reason) {}
	
	virtual ~IOException() throw();
			
	/// get device where error occured. use getResource() on the device to find out the file, device, address etc.
	Pointer<IODevice> getDevice() {return this->device;}

	/// get exception reason
	Reason getReason() {return this->reason;}
	
	/// get exception reason as text
	virtual const char* what() const throw();

protected:
	
	Pointer<IODevice> device;
	Reason reason;
};

/// @}

} // namespace digi

#endif
