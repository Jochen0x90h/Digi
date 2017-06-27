#ifndef digi_System_SerialPort_h
#define digi_System_SerialPort_h

#include <digi/Utility/StringRef.h>

#include "IODevice.h"
#include "FileSystem.h"

#undef PARITY_NONE
#undef PARITY_ODD
#undef PARITY_EVEN


namespace digi {

/// @addtogroup System
/// @{

/// abstraction of a serial port device
class SerialPort : public IODevice
{
public:

	enum OpenMode
	{
		// word length
		LENGTH_8 = 0x0000,
		LENGTH_7 = 0x0001,
		LENGTH_6 = 0x0002,
		LENGTH_5 = 0x0003,
		
		// parity
		PARITY_NONE = 0x0000,
		PARITY_ODD  = 0x0010,
		PARITY_EVEN = 0x0020,
		
		// number of stop bits
		STOP_1 = 0x0000,
		STOP_2 = 0x0100,
		
		FORMAT_8N1 = LENGTH_8 | PARITY_NONE | STOP_1,
		FORMAT_8E2 = LENGTH_8 | PARITY_EVEN | STOP_2,
	};

	enum PurgeDirection
	{
		// purge receive buffer
		PURGE_RX = 1,
		
		// purge transmit buffer
		PURGE_TX = 2
	};

	/**
		open a serial port with given name.
		on windows the first 9 serial ports are "com1" to "com9",
		then "\\\\.\\com10" "\\\\.\\com11" and so on
		on linux builtin serial ports are "/dev/ttyS0", "dev/ttyS1" and so on,
		usb serial ports (e.g. FTDI) are "/dev/ttyUSB0", "dev/ttyUSB1" and so on.
	*/
	static Pointer<SerialPort> open(const fs::path& path, int speed, int mode, int timeout);

	virtual ~SerialPort();

	/// purge buffers (i.e. clear without send/receive)
	virtual void purge(int direction) = 0;

	// mainly for internal usage. this gets the system error code (GetLastError()/errno) and
	// throws an IOException if an error is present
	static void throwException(Pointer<SerialPort> serialPort);

protected:

	SerialPort() {}
};

/// @}

} // namespace digi

#endif
