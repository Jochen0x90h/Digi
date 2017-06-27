#include <digi/System/IOException.h>
#include "../Socket.h"

#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#define CHECK(condition) \
	if (!(condition)) { \
		if (this->socket == INVALID_SOCKET) throw IOException(this, IOException::CLOSED); \
		Socket::throwException(this); \
	}

// prevent SIGPIPE (linux)
#ifndef MSG_NOSIGNAL
	#define MSG_NOSIGNAL 0
#endif

namespace digi {

typedef int SOCKET;
typedef int BOOL;
const int INVALID_SOCKET = -1;
const int SOCKET_ERROR = -1;
	

// POSIX implementation of Socket
class POSIXSocket : public Socket
{
public:

	POSIXSocket(SOCKET socket)
		: socket(socket)
	{
	}
	
	virtual ~POSIXSocket()
	{
		if (this->socket != INVALID_SOCKET)
		{
			// handle still open in the destructor may happen in these two cases:
			// 1. an exception is thrown and the destructor is called while stack unrolling
			// 2. the user forgot to call close() which is a bug
			::close(this->socket);
		}
	}
	
	virtual int getState()
	{
		return (this->socket != INVALID_SOCKET ? OPEN : 0);
	}

	virtual void close()
	{
		int result = ::close(this->socket);
		CHECK(result != SOCKET_ERROR);
		
		this->socket = INVALID_SOCKET;
	}

	virtual size_t read(void* data, size_t length)
	{
		int numRead = recv(this->socket, (char*)data, int(length), 0);		
		if (numRead == SOCKET_ERROR)
		{
			if (errno == EWOULDBLOCK || errno == EAGAIN)
				return 0;

			// error
			CHECK(false);
		}
		return numRead;
	}

	virtual size_t write(const void* data, size_t length)
	{
		int numWritten = send(this->socket, (const char*)data, int(length), MSG_NOSIGNAL);
		if (numWritten == SOCKET_ERROR)
		{
			if (errno == EWOULDBLOCK || errno == EAGAIN)
				return 0;
			
			// error
			CHECK(false);
		}
		return numWritten;
	}

	virtual void connect(StringRef serverName, int port)
	{
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));

		// check if this is a numeric address (e.g. "127.0.0.1")
		unsigned long a = inet_addr(serverName.data());
		if (a != INADDR_NONE)
		{
			// is numeric
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = a;		
		}
		else
		{
			// is a name (e.g. "localhost")
			struct hostent* hp = gethostbyname(serverName.data());
			if (hp == NULL)
			{
				// note: error code is in h_errno, see man gethostbyname
				throw digi::IOException(this, IOException::UNKNOWN_HOST);
			}
			addr.sin_family = hp->h_addrtype;
			memcpy(&addr.sin_addr, hp->h_addr, hp->h_length);
		}
		addr.sin_port = htons(uint16_t(port));

		// connect
		int result = ::connect(this->socket, (sockaddr*)&addr, sizeof(addr));
		CHECK(result != SOCKET_ERROR);
	}

	virtual void setBroadcast(int port)
	{
		BOOL value = 1;
		setsockopt(this->socket, SOL_SOCKET, SO_BROADCAST, (char*)&value, sizeof(value));

		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(struct sockaddr_in));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_BROADCAST;
		addr.sin_port = htons(port);

		// connect to broadcast address
		int result = ::connect(this->socket, (sockaddr*)&addr, sizeof(addr));
		CHECK(result != SOCKET_ERROR);
	}

	virtual void bind(int port)
	{
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));

		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_port = htons(uint16_t(port));

		// bind
		int result = ::bind(this->socket, (sockaddr*)&addr, sizeof(addr));
		CHECK(result != SOCKET_ERROR);
	}

	virtual void listen(int backlog)
	{
		int result = ::listen(this->socket, backlog);
		CHECK(result != SOCKET_ERROR);
	}

	virtual Pointer<IODevice> accept()
	{
		SOCKET socket = ::accept(this->socket, NULL, NULL);
		CHECK(socket != INVALID_SOCKET);
		
		return new POSIXSocket(socket);
	}

	virtual Pointer<IODevice> accept(std::string& address, int& port)
	{
		struct sockaddr addr;
		socklen_t addrLen = sizeof(addr);
		
		SOCKET socket = ::accept(this->socket, &addr, &addrLen);
		CHECK(socket != INVALID_SOCKET);
		
		sockaddr_in* inetAddr = (sockaddr_in*)&addr;
		address = inet_ntoa(inetAddr->sin_addr);
		port = ntohs(inetAddr->sin_port);
		return new POSIXSocket(socket);
	}

	virtual void shutdown(Shutdown mode)
	{
		int result =::shutdown(this->socket, mode);
		CHECK(result != SOCKET_ERROR);
	}


	SOCKET socket;
};

Pointer<Socket> Socket::create(Protocol protocol, int mode)
{
	SOCKET socket = INVALID_SOCKET;
	switch (protocol)
	{
	case IP:
		socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
		break;
	case UDP:
		socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		break;		
	}
	
	Socket* s = new POSIXSocket(socket);	
	if (socket == INVALID_SOCKET)
		Socket::throwException(s);

	// prevent SIGPIPE (apple)
	#ifdef SO_NOSIGPIPE
		int set = 1;
		setsockopt(socket, SOL_SOCKET, SO_NOSIGPIPE, &set, sizeof(int));
	#endif
	
	// set nonblocking if requested
	u_long nonblocking = mode & NON_BLOCKING;
	if (ioctl(socket, FIONBIO, &nonblocking) == SOCKET_ERROR)
		Socket::throwException(s);
	
	return s;
}

void Socket::throwException(Pointer<Socket> socket)
{
	int e = errno;
	IOException::Reason reason = IOException::IO_ERROR;
	switch (e)
	{
	case 0:
		// do not throw on success
		return;

	case EBADF:
		reason = IOException::INVALID_HANDLE;
		break;

	case EACCES:
		reason = IOException::ACCESS_DENIED;
		break;
	
	case EADDRINUSE:
		reason = IOException::ADDRESS_IN_USE;
		break;
	
	case ENETUNREACH:
		reason = IOException::UNREACHABLE;
		break;

	case ETIMEDOUT:
		reason = IOException::TIMEDOUT;
		break;

	case ECONNREFUSED:
		reason = IOException::CONNECTION_REFUSED;
		break;

	case EPIPE:
	case ECONNABORTED:
		reason = IOException::CONNECTION_ABORTED;
		break;
	
	case ECONNRESET:		
		reason = IOException::CONNECTION_CLOSED_BY_PEER;
		break;
	
	case ESHUTDOWN:
		reason = IOException::SHUTDOWN;
		break;
	}
	throw IOException(socket, reason);
}

} // namespace digi
