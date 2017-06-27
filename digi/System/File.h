/*
	file access using IODevice and the native operating system api.
	this allows e.g. accessing files > 4GB on 32bit Windows.
*/

#ifndef digi_System_File_h
#define digi_System_File_h

#include "IODevice.h"
#include "FileSystem.h"


namespace digi {

/// @addtogroup System
/// @{

class File : public IODevice
{
public:

	enum OpenMode
	{
		/// create new file if it does not exist
		CREATE = 1,
        
		/// truncate existing file
		TRUNCATE = 2,

		/// open with read permission
		READ = 4,

		/// open with write permission
		WRITE = 8,

		/// open with read and write permission
		READ_WRITE = 4 | 8,
		
		/// create new file or truncate existing one and open with read permission
		CREATE_TRUNCATE_WRITE = 1 | 2 | 8,
	};

	/// open a file with given name and open mode
	static Pointer<File> open(const fs::path& path, int mode = READ);

	/// create/truncate a file and open for writing and additional open mode
	static Pointer<File> create(const fs::path& path, int mode = 0)
	{
		return open(path, CREATE_TRUNCATE_WRITE | mode);
	}
	
	virtual ~File();
	

	/// flush file
	virtual void flush();


	// mainly for internal usage. this gets the system error code (GetLastError()/errno) and
	// throws an IOException if an error is present
	static void throwException(Pointer<File> file);

	// get a dummy file that you can throw with an IOException
	static Pointer<File> getDummyFile(const fs::path& path);

protected:

	File() {}
};

/// @}

} // namespace digi

#endif
