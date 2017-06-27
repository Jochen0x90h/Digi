#include "../File.h"
#include "../IOException.h"

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#define CHECK(condition) \
	if (!(condition)) { \
		if (this->handle == INVALID_HANDLE_VALUE) throw IOException(this, IOException::CLOSED); \
		File::throwException(this); \
	}


namespace digi {

// helpers
inline uint32_t getLow(int64_t x)
{
	return uint32_t(x);
}

inline int32_t getHigh(int64_t x)
{
	return int32_t(x >> 32);
}

inline int64_t makeLong(uint32_t low, int32_t high)
{
	return (int64_t(high) << 32) | low;
}


// win32 implementation of File
class Win32File : public File
{
public:

	Win32File(HANDLE handle, const fs::path& path)
		: handle(handle), path(path)
	{
	}

	virtual ~Win32File()
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
		return (this->handle != INVALID_HANDLE_VALUE ? OPEN : 0);
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
		size_t numRead = 0;
		while (numRead < length)
		{
			DWORD toRead = DWORD(std::min(size_t(0x40000000), length - numRead));

			DWORD nr;
			BOOL result = ReadFile(
				this->handle,   // handle to file
				data,           // data buffer
				toRead,         // number of bytes to read
				&nr,            // number of bytes read
				NULL            // overlapped buffer
			);
			CHECK(result != 0)

			if (nr == 0)
				break;
				
			numRead += nr;
			data = (uint8_t*)data + nr;
		}
		return numRead;
	}
	
	virtual size_t write(const void* data, size_t length)
	{
		size_t numWritten = 0;
		while (numWritten < length)
		{
			DWORD toWrite = DWORD(std::min(size_t(0x40000000), length - numWritten));

			DWORD nw;
			BOOL result = WriteFile(
				this->handle,   // handle to file
				data,           // data buffer
				toWrite,        // number of bytes to write
				&nw,            // number of bytes written
				NULL            // overlapped buffer
			);
			CHECK(result != 0)

			if (nw == 0)
				break;
				
			numWritten += nw;
			data = (const uint8_t*)data + nw;
		}

		return numWritten;	
	}
	
	virtual int64_t seek(int64_t position, PositionMode positionMode)
	{
		DWORD pos = (DWORD)getLow(position);
		LONG highPos = (LONG)getHigh(position);
		DWORD mode = (DWORD)positionMode;
		
		pos = SetFilePointer(
			this->handle, // handle to file
			pos,          // number of bytes to move file pointer
			&highPos,     // address of high-order word of distance to move
			mode          // starting point (FILE_BEGIN = 0, FILE_CURRENT = 1, FILE_END = 2)
		);
		
		// note: pos may be 0xFFFFFFFF without an error, in this case GetLastError() is ERROR_SUCCESS
		CHECK(pos != 0xFFFFFFFF);

		return makeLong(pos, highPos);
	}

	virtual void setSize(int64_t size)
	{
		// get current position
		int64_t position = this->getPosition();
		
		// set position to size
		this->setPosition(size);
		
		// set file end here
		BOOL result = SetEndOfFile(this->handle);
		CHECK(result != 0);
		
		// restore position
		this->setPosition(position);
	}
	
	virtual int64_t getSize()
	{
		DWORD size;
		DWORD highSize;
		size = GetFileSize(
			(HANDLE)this->handle, // handle to file
			&highSize             // file size
		);

		// note: pos may be 0xFFFFFFFF without an error, in this case GetLastError() is ERROR_SUCCESS
		CHECK(size != 0xFFFFFFFF);

		return makeLong(size, highSize);
	}

	virtual void flush()
	{
		BOOL result = FlushFileBuffers(this->handle);
		CHECK(result != 0);
	}
	
	
	HANDLE handle;
	fs::path path;
};


Pointer<File> File::open(const fs::path& path, int mode)
{
	DWORD disposition = OPEN_EXISTING;
	if ((mode & (CREATE | TRUNCATE)) == (CREATE | TRUNCATE)) // create a new empty file
		disposition = CREATE_ALWAYS;
	else if ((mode & CREATE) != 0) // create the file if it does not exist
		disposition = OPEN_ALWAYS;
	else if ((mode & TRUNCATE) != 0) // truncate the file. fails if it does not exist
		disposition = TRUNCATE_EXISTING;

	DWORD access = 0;
	if ((mode & READ) != 0)
		access = GENERIC_READ;
	if ((mode & WRITE) != 0)
		access |= GENERIC_WRITE;

#ifdef _WIN32_WCE
	DWORD share = 0;
#else
	// posix compatible: share read, write and delete
	DWORD share = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
#endif

	DWORD attribs = FILE_ATTRIBUTE_NORMAL;

	HANDLE handle = CreateFileW(
		path.c_str(), // file name
		access,       // access mode
		share,        // share mode
		NULL,         // SD
		disposition,  // how to create
		attribs,      // file attributes
		NULL);        // handle to template file

	Win32File* f = new Win32File(handle, path);	
	if (handle == INVALID_HANDLE_VALUE)
		File::throwException(f);
	return f;
}

void File::throwException(Pointer<File> file)
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
	case ERROR_PATH_NOT_FOUND:
		reason = IOException::FILE_NOT_FOUND;
		break;	

	case ERROR_HANDLE_DISK_FULL:
	case ERROR_DISK_FULL:
		reason = IOException::DISK_FULL;
		break;
	
	case ERROR_SEEK:
		reason = IOException::SEEK_ERROR;
		break;
	}
	throw IOException(file, reason);
}

Pointer<File> File::getDummyFile(const fs::path& path)
{
	return new Win32File(INVALID_HANDLE_VALUE, path);
}

} // namespace digi
