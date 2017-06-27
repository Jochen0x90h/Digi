#include "FileSystem.h"


#ifndef NO_BOOST_FILESYSTEM

namespace digi
{

#ifdef _WIN32

std::string to8BitPath(const fs::path& path)
{
/*
	/// this is the ultimate hack for unicode filename support for 3rd party libraries on windows.
	/// it uses GetShortPathNameW to get an ansi 8.3 filename for the unicode name which can be
	/// passed to a 3rd party library that takes only 8bit (const char*) filenames (e.g. ffmpeg).
	/// set parameter create to true if you want to write a file. in this case the file is created
	/// if it does not exist and then overwritten by the 3rd party library.

	if (create)
	{
		DWORD access = GENERIC_WRITE;
		DWORD share = FILE_SHARE_READ | FILE_SHARE_WRITE;
		DWORD disposition = OPEN_ALWAYS;
		DWORD attribs = FILE_ATTRIBUTE_NORMAL;
		
		HANDLE handle = CreateFileW(
			path.c_str(), // file name
			access,       // access mode
			share,        // share mode
			NULL,         // SD
			disposition,  // how to create
			attribs,      // file attributes
			NULL);        // handle to template file
		if (handle == INVALID_HANDLE_VALUE)
			return std::string();
		CloseHandle(handle);
	}
	
	const wchar_t* wPath = path.c_str();
	
	// determine needed short path buffer size
	DWORD wBufferSize = GetShortPathNameW(wPath, NULL, 0);	
	
	// get short path
	std::wstring wBuffer(wBufferSize, 0);
	GetShortPathNameW(wPath, &wBuffer[0], wBufferSize);
	
	// determine needed 8bit buffer size
	int bufferSize = WideCharToMultiByte(
		CP_ACP, // codepage
		0, // flags
		wBuffer.c_str(), // in buffer
		wBufferSize, // in size
		NULL, // out buffer
		0, // out size
		NULL,
		NULL);
		
	std::string buffer(bufferSize, 0);
	WideCharToMultiByte(
		CP_ACP, // codepage
		0, // flags
		wBuffer.c_str(), // in buffer
		wBufferSize, // in size
		&buffer[0], // out buffer
		bufferSize, // out size
		NULL,
		NULL);
	
	return buffer;
*/
	const wchar_t* str = path.c_str();
	std::string dst;

	mbstate_t state;
	memset(&state, 0, sizeof(state));
	while (str != NULL)
	{	
		char buffer[128];
		size_t numConverted = wcsrtombs(buffer, &str, sizeof(buffer), &state);
		if (numConverted == size_t(-1))
			return std::string();
		dst.append(buffer, numConverted);
	}

	return dst;
}

#endif

} // namespace digi

#else // NO_BOOST_FILESYSTEM

namespace boost {

namespace filesystem
{
	namespace
	{
	#ifdef _WIN32
		const wchar_t dot = L'.';
		const wchar_t colon = L':';
		const wchar_t* separators = L"/\\";
		const path dot_path = L".";
		const path dot_dot_path = L"..";

		inline bool is_separator(wchar_t ch) {return ch == L'/' || ch == '\\';}
		inline bool is_letter(wchar_t ch) {return ch >= L'A' && ch <= L'Z' || ch >= L'a' && ch <= L'z';}
	#else
		const char dot = '.';
		const char* separators = "/";
		const path dot_path = ".";
		const path dot_dot_path = "..";

		inline bool is_separator(char ch) {return ch == '/';}
	#endif

		typedef path::string_type string_type;
		typedef path::size_type size_type;

		bool is_root_separator(const string_type& str, size_type pos)
			// pos is position of the separator
		{
			assert(!str.empty() && is_separator(str[pos]) && "precondition violation");

			// subsequent logic expects pos to be for leftmost slash of a set
			while (pos > 0 && is_separator(str[pos-1]))
				--pos;

			// "/" [...]
			if (pos == 0)  
				return true;

			#ifdef _WIN32
				//  "c:/" [...]
				if (pos == 2 && is_letter(str[0]) && str[1] == colon)  
				return true;
			#endif

			//  "//" name "/"
			if (pos < 3 || !is_separator(str[0]) || !is_separator(str[1]))
				return false;

			return str.find_first_of(separators, 2) == pos;
		}

		size_type filename_pos(const string_type& str, size_type end_pos) // end_pos is past-the-end position
			// return 0 if str itself is filename (or empty)
		{
			// case: "//"
			if (end_pos == 2 
				&& is_separator(str[0])
				&& is_separator(str[1])) return 0;

			// case: ends in "/"
			if (end_pos && is_separator(str[end_pos-1]))
				return end_pos-1;
    
			// set pos to start of last element
			size_type pos(str.find_last_of(separators, end_pos-1));

		#ifdef _WIN32
			if (pos == string_type::npos)
				pos = str.find_last_of(colon, end_pos-2);
		#endif

			return (pos == string_type::npos // path itself must be a filename (or empty)
				|| (pos == 1 && is_separator(str[0]))) // or net
				? 0 // so filename is entire string
				: pos + 1; // or starts after delimiter
		}
	}

	path path::filename() const
	{
		size_type pos(filename_pos(m_pathname, m_pathname.size()));
		return (m_pathname.size()
			&& pos
			&& is_separator(m_pathname[pos])
			&& !is_root_separator(m_pathname, pos))
			? dot_path
			: path(m_pathname.c_str() + pos);
	}

	path path::extension() const
	{
		path name(this->filename());
		if (name == dot_path || name == dot_dot_path)
			return path();
		size_t pos(name.m_pathname.rfind(dot));
		return pos == string_type::npos ? path() : path(name.m_pathname.substr(pos));
	}
	
	
	/*
	bool exists(const path& path)
	{
		return GetFileAttributesW(path.c_str()) != 0xffffffff;
	}
	*/
}

}

#endif // NO_BOOST_FILESYSTEM
