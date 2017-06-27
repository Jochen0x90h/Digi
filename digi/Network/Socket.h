#ifndef digi_Network_Socket_h
#define digi_Network_Socket_h

#include <digi/Utility/StringRef.h>
#include <digi/System/IODevice.h>


namespace digi {

/// @addtogroup Network
/// @{


class Socket : public IODevice
{
public:

	enum Protocol
	{
		IP,
		UDP
	};
	
	enum Mode
	{
		// non-blocing IO
		NON_BLOCKING = 1,
	};

	enum Shutdown
	{
		RECEIVE = 0,
		SEND = 1,
		BOTH = 2
	};

	/// create a socket
	static Pointer<Socket> create(Protocol protocol, int mode = 0);
	
	virtual ~Socket();

	/// connect to a server with given address or name (e.g. "127.0.0.1" or "localhost")
	virtual void connect(StringRef serverName, int port) = 0;
	
	/// sets socket to broadcast mode on given port. note that receiving data is not possible in this mode
	virtual void setBroadcast(int port) = 0;
	
	/// bind socket to a port of this machine, now it can receive e.g. udp packets
	virtual void bind(int port) = 0;
	
	/// listen to incoming connections (only for stream protocols like IP)
	virtual void listen(int backlog) = 0;
	
	/// accept an incoming connection (only for stream protocols like IP)
	virtual Pointer<IODevice> accept() = 0;

	/// accept an incoming connection and return address and port of remote host (only for stream protocols like IP)
	virtual Pointer<IODevice> accept(std::string& address, int& port) = 0;

	/// indicate that no more receive or send will occur. the remote host reads zero bytes and can detect the shutdown by this.
	virtual void shutdown(Shutdown mode) = 0;
	

	// mainly for internal usage. this gets the system error code (WSAGetLastError()/errno) and
	// throws an IOException if an error is present
	static void throwException(Pointer<Socket> socket);

protected:

	Socket() {}
};

/// @}

} // namespace digi

#endif
