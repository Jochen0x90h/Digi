#include "../SerialPort.h"
#include "../IOException.h"

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#undef PARITY_NONE
#undef PARITY_ODD
#undef PARITY_EVEN

#define CHECK(condition) \
	if (!(condition)) { \
		if (this->handle == INVALID_HANDLE_VALUE) throw IOException(this, IOException::CLOSED); \
		SerialPort::throwException(this); \
	}

#define SERIALPORT_BUFSIZE 4096


namespace digi {


// win32 implementation of SerialPort

class Win32SerialPort : public SerialPort
{
public:

	Win32SerialPort(HANDLE handle, const fs::path& path)
		: handle(handle), path(path) {}

	virtual ~Win32SerialPort()
	{
		if (this->handle != INVALID_HANDLE_VALUE)
		{
			// handle still open in the destructor may happen in these two cases:
			// 1. an exception is thrown and the destructor is called while stack unrolling
			// 2. the user forgot to call close() which is a bug
			CloseHandle(this->handle);
		}	
	}

	virtual int getState()
	{
		return this->handle != INVALID_HANDLE_VALUE ? OPEN : 0;
	}

	virtual void close()
	{
		BOOL result = CloseHandle(this->handle);
		CHECK(result != 0);

		this->handle = INVALID_HANDLE_VALUE;
	}

	virtual std::string getResource()
	{
		return this->path.string();
	}
	
	virtual size_t read(void* data, size_t length)
	{
		DWORD toRead = DWORD(length);
		DWORD nr;
		BOOL result = ReadFile(
			this->handle,   // handle to file
			data,           // data buffer
			toRead,         // number of bytes to read
			&nr,            // number of bytes read
			NULL            // overlapped buffer
		);
		CHECK(result != 0)
		return nr;
	}

	virtual size_t write(const void* data, size_t length)
	{
		DWORD toWrite = DWORD(length);
		DWORD nw;		
		BOOL result = WriteFile(
			this->handle,   // handle to file
			data,           // data buffer
			toWrite,        // number of bytes to write
			&nw,            // number of bytes written
			NULL            // overlapped buffer
		);
		CHECK(result != 0)
		return nw;

	}

	virtual void purge(int direction)
	{
		DWORD flags = 0;
		if (direction & PURGE_RX)
			flags |= PURGE_RXCLEAR;
		if (direction & PURGE_TX)
			flags |= PURGE_TXCLEAR;
		BOOL result = PurgeComm(this->handle, flags);
		CHECK(result != 0);	
	}


	HANDLE handle;
	fs::path path;
};

Pointer<SerialPort> SerialPort::open(const fs::path& path, int speed, int mode, int timeout)
{
	int wordLength = mode & 0x000F;
	int parity = mode & 0x00F0;
	int stopBits = mode & 0x0F00;

	HANDLE handle = CreateFileW(path.c_str(), // device name
		GENERIC_READ | GENERIC_WRITE, // read/write
		0, // not shared
		NULL, // default value for object security ?!?
		OPEN_EXISTING, // file (device) exists
		FILE_ATTRIBUTE_NORMAL, // flags
		NULL); // no template	
	if (handle == INVALID_HANDLE_VALUE)
		SerialPort::throwException(new Win32SerialPort(handle, path));

	// device control block
	DCB dcb;
	memset(&dcb, 0, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);

	// baud rate at which the communications device operates
	dcb.BaudRate = speed;

	// if this member is TRUE, binary mode is enabled. Win32 does not support
	// nonbinary mode transfers, so this member must be TRUE.
	dcb.fBinary = TRUE;

	// if this member is TRUE, the CTS (clear-to-send) signal is monitored for
	// output flow control. If this member is TRUE and CTS is turned off, output
	// is suspended until CTS is sent again.
	dcb.fOutxCtsFlow = FALSE;

	// if this member is TRUE, the DSR (data-set-ready) signal is monitored for
	// output flow control. If this member is TRUE and DSR is turned off, output
	// is suspended until DSR is sent again.
	dcb.fOutxDsrFlow = FALSE;

	// DTR (data-terminal-ready) flow control. This member can be one of the
	// following values:
	// DTR_CONTROL_DISABLE   Disables the DTR line when the device is opened
	//                       and leaves it disabled.
	// DTR_CONTROL_ENABLE    Enables the DTR line when the device is opened
	//                       and leaves it on.
	// DTR_CONTROL_HANDSHAKE Enables DTR handshaking. If handshaking is enabled,
	//                       it is an error for the application to adjust the line
	//                       by using the EscapeCommFunction function.
	dcb.fDtrControl = DTR_CONTROL_DISABLE;

	// if this member is TRUE, the communications driver is sensitive to the state
	// of the DSR signal. The driver ignores any bytes received, unless the DSR modem
	// input line is high.
	dcb.fDsrSensitivity = FALSE;

	// RTS (request-to-send) flow control. This member can be one of the following
	// values:
	// RTS_CONTROL_DISABLE   Disables the RTS line when the device is opened and
	//                       leaves it disabled.
	// RTS_CONTROL_ENABLE    Enables the RTS line when the device is opened and
	//                       leaves it on.
	// RTS_CONTROL_HANDSHAKE Enables RTS handshaking. The driver raises the RTS line
	//                       when the "type-ahead" (input) buffer is less than one-half
	//                       full and lowers the RTS line when the buffer is more than
	//                       three-quarters full. If handshaking is enabled, it is an
	//                       error for the application to adjust the line by using the
	//                       EscapeCommFunction function.
	// RTS_CONTROL_TOGGLE    Specifies that the RTS line will be high if bytes are
	//                       available for transmission. After all buffered bytes have
	//                       been sent, the RTS line will be low.
	dcb.fRtsControl = RTS_CONTROL_DISABLE;

	// set word length
	dcb.ByteSize = 8 - wordLength;

	// set parity
	if (parity == PARITY_EVEN)
		dcb.fParity = TRUE, dcb.Parity = EVENPARITY;
	else if (parity == PARITY_ODD)
		dcb.fParity = TRUE, dcb.Parity = ODDPARITY;

	// set number of stop bits
	dcb.StopBits = ONESTOPBIT;
	if (stopBits == STOP_2)
		dcb.StopBits = TWOSTOPBITS;

	// set communication state
	SetCommState(handle, &dcb);

	// set comm timeouts
	COMMTIMEOUTS cto;
	cto.ReadIntervalTimeout = MAXDWORD;
	cto.ReadTotalTimeoutMultiplier = 0;
	cto.ReadTotalTimeoutConstant = timeout;
	cto.WriteTotalTimeoutMultiplier = 0;
	cto.WriteTotalTimeoutConstant = timeout;
	SetCommTimeouts(handle, &cto);

	// set buffer sizes for a better performance with win95/98.
	// (normal size is 1024, but this can be a little bit to small,
	// if you use a higher baudrate like 115200)
	SetupComm(handle, SERIALPORT_BUFSIZE, SERIALPORT_BUFSIZE);

	// create and return the serial port object
	return new Win32SerialPort(handle, path);
}

void SerialPort::throwException(Pointer<SerialPort> serialPort)
{
	DWORD e = GetLastError();
	IOException::Reason reason = IOException::IO_ERROR;
	switch (e)
	{
	case ERROR_SUCCESS:
		// do not throw on success
		return;

	case ERROR_INVALID_HANDLE:
		reason = IOException::INVALID_HANDLE;
		break;

	case ERROR_ACCESS_DENIED:
	case ERROR_SHARING_VIOLATION:
	case ERROR_LOCK_VIOLATION:
		reason = IOException::ACCESS_DENIED;
		break;

	case ERROR_FILE_NOT_FOUND:
		reason = IOException::FILE_NOT_FOUND;
		break;	
	}
	throw IOException(serialPort, reason);
}

/*
// implementation of functions inside namespace SerialPort in SerialPort.h

void SerialPort::GetDevices(std::vector<std::string> &devices)
{
	static const char *comList[] = 
	{
		"com1",
		"com2",
		"com3",
		"com4",
		"com5",
		"com6",
		"com7",
		"com8",
		"com9",
		"\\\\.\\com10",
		"\\\\.\\com11",
		"\\\\.\\com12",
		"\\\\.\\com13",
		"\\\\.\\com14",
		"\\\\.\\com15",
		"\\\\.\\com16"
	};

	for (int i = 0; i < ArrayLength(comList); ++i)
	{
		/ *WIN32_FIND_DATA findData;
		HANDLE handle = FindFirstFile(
			comList[i],
			&findData);* /
			
		HANDLE handle = CreateFile(
			comList[i], // device name
			GENERIC_READ | GENERIC_WRITE, // read/write
			0, // not shared
			NULL, // default value for object security ?!?
			OPEN_EXISTING, // file (device) exists
			0, // flags
			NULL); // no template
	
		if (handle != INVALID_HANDLE_VALUE)
		{
			devices.push_back(comList[i]);
			//FindClose(handle);
			CloseHandle(handle);
		}
		else if (GetLastError() == ERROR_ACCESS_DENIED)
		{
			devices.push_back(comList[i]);
		}
	}
}
*/


} // namespace digi
