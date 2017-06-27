#include <digi/System/IOException.h>
#include "../Socket.h"

#include <windows.h>

#undef HOST_NOT_FOUND


#define CHECK(condition) \
	if (!(condition)) { \
		if (this->socket == INVALID_SOCKET) throw IOException(this, IOException::CLOSED); \
		Socket::throwException(this); \
	}


namespace digi {

typedef int socklen_t;


// Win32 implementation of Socket
class Win32Socket : public Socket
{
public:

	Win32Socket(SOCKET socket)
		: socket(socket)
	{
	}
	
	virtual ~Win32Socket()
	{
		if (this->socket != INVALID_SOCKET)
		{
			// handle still open in the destructor may happen in these two cases:
			// 1. an exception is thrown and the destructor is called while stack unrolling
			// 2. the user forgot to call close() which is a bug
			closesocket(this->socket);
		}
	}

	virtual int getState()
	{
		return (this->socket != INVALID_SOCKET ? OPEN : 0);
	}

	virtual void close()
	{
		int result = closesocket(this->socket);
		CHECK(result != SOCKET_ERROR);
		
		this->socket = INVALID_SOCKET;
	}

	virtual size_t read(void* data, size_t length)
	{
		int numRead = recv(this->socket, (char*)data, int(length), 0);		
		if (numRead == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
				return 0;

			// error
			CHECK(false);
		}
		return numRead;
	}

	virtual size_t write(const void* data, size_t length)
	{
		int numWritten = send(this->socket, (const char*)data, int(length), 0);
		if (numWritten == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
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
				throw IOException(this, IOException::UNKNOWN_HOST);
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
		
		return new Win32Socket(socket);
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
		return new Win32Socket(socket);
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
	
	Socket* s = new Win32Socket(socket);
	if (socket == INVALID_SOCKET)
		Socket::throwException(s);

	// set nonblocking if requested
	u_long nonblocking = mode & NON_BLOCKING;
	if (ioctlsocket(socket, FIONBIO, &nonblocking) == SOCKET_ERROR)
		Socket::throwException(s);
	
	return s;
}

void Socket::throwException(Pointer<Socket> socket)
{
	DWORD e = WSAGetLastError();
	IOException::Reason reason = IOException::IO_ERROR;
	switch (e)
	{
	case ERROR_SUCCESS:
		// do not throw on success
		return;

	case WSAEBADF:
		reason = IOException::INVALID_HANDLE;
		break;

	case WSAEACCES:
		reason = IOException::ACCESS_DENIED;
		break;
	
	case WSAEADDRINUSE:
		reason = IOException::ADDRESS_IN_USE;
		break;
	
	case WSAENETUNREACH:
		reason = IOException::UNREACHABLE;
		break;

	case WSAETIMEDOUT:
		reason = IOException::TIMEDOUT;
		break;

	case WSAECONNREFUSED:
		reason = IOException::CONNECTION_REFUSED;
		break;
	
	case WSAECONNABORTED:
		reason = IOException::CONNECTION_ABORTED;
		break;
	
	case WSAECONNRESET:		
		reason = IOException::CONNECTION_CLOSED_BY_PEER;
		break;
	
	case WSAESHUTDOWN:
		reason = IOException::SHUTDOWN;
		break;
	}
	throw IOException(socket, reason);
}


} // namespace digi
