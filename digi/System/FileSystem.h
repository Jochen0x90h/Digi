#ifndef digi_System_FileSystem_h
#define digi_System_FileSystem_h

/*
	wrapper for boost::filesystem. namespace boost::filesystem is mapped to digi::fs.
	for embedded systems a reduced implementation is provided.

	usefil classes:
	path                     - represents a filesystem path (e.g. "/foo/bar")

	useful functions:
	exists(path)             - check if file or directory exists
	remove(path)             - remove file
	create_directory(path)   - create directory
	create_directories(path) - create nested directories
*/


#ifndef NO_BOOST_FILESYSTEM

#include <boost/version.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>

#include <digi/Utility/StringRef.h>


// auto conversion from StringRef to boost::filesystem::path
namespace boost {

#if BOOST_VERSION >= 105000
namespace filesystem
#else
namespace filesystem3
#endif
{
	namespace path_traits
	{
		template<> struct is_pathable<digi::StringRef> { static const bool value = true; };
	}
}

} // namespace boost

#else // NO_BOOST_FILESYSTEM

#include <cassert>
#include <string>
#include <boost/type_traits/decay.hpp>
#include <boost/range.hpp>
#include <boost/range/as_array.hpp>
#include <boost/range/as_literal.hpp>

#include <digi/Utility/UTFTranscode.h>

namespace boost {

namespace filesystem
{
	namespace path_traits
	{
		template <class T>
		struct is_pathable { static const bool value = false; };

		template<> struct is_pathable<char*>                  { static const bool value = true; };
		template<> struct is_pathable<const char*>            { static const bool value = true; };
		template<> struct is_pathable<wchar_t*>               { static const bool value = true; };
		template<> struct is_pathable<const wchar_t*>         { static const bool value = true; };
		template<> struct is_pathable<std::string>            { static const bool value = true; };
		template<> struct is_pathable<std::wstring>           { static const bool value = true; };
		template<> struct is_pathable<digi::StringRef>        { static const bool value = true; };		
	}


	// very simple implementation of boost::filesystem::path. use for embedded devices where boost::filesystem does not compile
	class path
	{
	public:

		#ifdef _WIN32
			typedef wchar_t value_type;
			typedef std::wstring string_type;
		#else
			typedef char value_type;
			typedef std::string string_type;
		#endif

		typedef string_type::size_type size_type;
		typedef string_type::iterator iterator;
		typedef string_type::const_iterator const_iterator;

		bool empty() const {return m_pathname.empty();}
		size_type size() const {return m_pathname.size();}
		iterator begin() {return m_pathname.begin();}
		iterator end() {return m_pathname.end();}
		const_iterator begin() const {return m_pathname.begin();}
		const_iterator end() const {return m_pathname.end();}

	
		//  -----  constructors  -----

		path() {}                                          

		path(const path& p)
			: m_pathname(p.m_pathname)
		{
		}

		template <typename String>
		path(const String& s,
			typename boost::enable_if<path_traits::is_pathable<typename boost::decay<String>::type> >::type* dummy = 0)
			: m_pathname(digi::utfString<value_type>(s))
		{
		}


	    //  -----  assignments  -----
		
		path& operator =(const path& p)
		{
			m_pathname = p.m_pathname;
			return *this;
		}

		template <typename String>
		path& operator =(const String& s)
		{
			m_pathname = digi::utfString<value_type>(s);
			return *this;
		}


		//  -----  appends  -----

		path& operator /=(const path& p)
		{
		#ifdef _WIN32
			m_pathname += L'\\';
		#else
			m_pathname += '/';
		#endif
			m_pathname += p.m_pathname;
			return *this;
		}
	
		template <typename String>
		path& operator /=(const String& s)
		{
		#ifdef _WIN32
			m_pathname += L'\\';
		#else
			m_pathname += '/';
		#endif
			m_pathname += digi::utfString<value_type>(s);
			return *this;
		}


	    //  -----  concatenation  -----

		path& operator +=(const path& p)
		{
			m_pathname += p.m_pathname;
			return *this;
		}
		
		template <typename String>
		path& operator +=(const String& s)
		{
			m_pathname += digi::utfString<value_type>(s);
			return *this;
		}


		//  -----  native format observers  -----

		const string_type& native() const {return m_pathname;}
		const value_type* c_str() const  {return m_pathname.c_str();}

		#ifdef _WIN32
			const std::string string() const {return digi::utfString<char>(m_pathname);}
		#else
			const std::string& string() const {return this->p;}
		#endif


	    //  -----  decomposition  -----
	    
		path filename() const;
		path extension() const;

	protected:

		string_type m_pathname;
	};


	template <typename String>
	path operator /(const path& a, const String& b) {return path(a) /= b;}

	inline bool operator==(const path& a, const path& b) {return a.native() == b.native();}

	template <typename String>
	bool operator==(const path& a, const String& b) {return a.native() == digi::utfString<path::value_type>(b);}

	template <typename String>
	bool operator==(const String& a, const path& b) {return digi::utfString<path::value_type>(a) == b.native();}

	inline bool operator!=(const path& a, const path& b) {return a.native() != b.native();}

	template <typename String>
	bool operator!=(const path& a, const String& b) {return a.native() != digi::utfString<path::value_type>(b);}

	template <typename String>
	bool operator!=(const String& a, const path& b) {return digi::utfString<path::value_type>(a) != b.native();}
}

} // namespace boost

#endif // NO_BOOST_FILESYSTEM


namespace digi {

/// @addtogroup System
/// @{

namespace fs = boost::filesystem;

/// add path and character. the character has to be ASCII (e.g. '.' or '_'), i.e. the high bit has to be clear
inline fs::path operator +(const fs::path& a, char b)
{
	assert((b & 0x80) == 0 && "char appended to path must be ASCII");
	return a.native() + fs::path::value_type(b);
}

/// add two paths (no '/' gets inserted)
inline fs::path operator +(const fs::path& a, const fs::path& b)
{
	return a.native() + b.native();
}


// converts 8bit to 16bit string using the global path locale
/*
static inline std::wstring toWString(StringRef str)
{
	#ifndef NO_BOOST_FILESYSTEM
		std::wstring result;

		#if BOOST_VERSION >= 105000
			using namespace boost::filesystem;
		#else
			using namespace boost::filesystem3;
		#endif

		path_traits::convert(str.begin(), str.end(), result, path::codecvt());
		return result;
	#else
		return transcode<wchar_t>(str);
	#endif
}
*/

#ifdef _WIN32

// use this for 3rd party libs that only take 8bit (const char*) paths. breaks unicode compatibility on windows
std::string to8BitPath(const fs::path& path);
	
#else

static inline const std::string& to8BitPath(const fs::path& path)
{
	return path.string();
}

#endif

/// @}

} // namespace digi

#endif
