#include <sys/stat.h>
#include <fcntl.h>

#include "../File.h"
#include "../IOException.h"


#define INVALID_HANDLE_VALUE -1

#define CHECK(condition) \
	if (!(condition)) { \
		if (this->handle == INVALID_HANDLE_VALUE) throw IOException(this, IOException::CLOSED); \
		File::throwException(this); \
	}


namespace digi {

	
// posix implementation of File
class POSIXFile : public File
{
public:

	POSIXFile(int handle, const fs::path& path)
		: handle(handle), path(path)
	{
	}

	virtual ~POSIXFile()
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
		return (this->handle != INVALID_HANDLE_VALUE ? OPEN : 0);
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
	
	virtual int64_t seek(int64_t position, PositionMode positionMode)
	{
		int64_t newPosition = lseek(this->handle, position, int(positionMode));
		CHECK(newPosition != -1);

		return newPosition;
	}

	virtual void setSize(int64_t size)
	{
		int result = ftruncate(this->handle, size);
		CHECK(result != -1);
	}
	
	virtual int64_t getSize()
	{
		struct stat st;
		int result = fstat(this->handle, &st);
		CHECK(result != -1);
		
		return st.st_size;
	}

	virtual void flush()
	{
		fsync(this->handle);
	}
	
	
	int handle;
	fs::path path;
};


Pointer<File> File::open(const fs::path& path, int mode)
{
	int oflag = 0;
	if ((mode & CREATE) != 0)
		oflag |= O_CREAT;
	if ((mode & TRUNCATE) != 0)
		oflag |= O_TRUNC;
	if ((mode & READ_WRITE) == READ_WRITE)
		oflag |= O_RDWR;
	else if ((mode & READ) != 0)
		oflag |= O_RDONLY;
	else if ((mode & WRITE) != 0)
		oflag |= O_WRONLY;

	mode_t access = 0666; // rw-rw-rw-
	
	int handle = ::open(path.c_str(), oflag, access);

	POSIXFile* f = new POSIXFile(handle, path);
	if (handle == INVALID_HANDLE_VALUE)
		File::throwException(f);
	return f;
}

void File::throwException(Pointer<File> file)
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

	case ENOSPC:
		reason = IOException::DISK_FULL;
		break;
	
	case ESPIPE:
		reason = IOException::SEEK_ERROR;
		break;
	}
	throw IOException(file, reason);
}

Pointer<File> File::getDummyFile(const fs::path& path)
{
	return new POSIXFile(INVALID_HANDLE_VALUE, path);
}

} // namespace digi
