#ifdef _WIN32
	#ifndef NO_BOOST_FILESYSTEM
		#define BOOST_FILESYSTEM_VERSION 3
		#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>
	#endif
#else
	#include <signal.h>
#endif

#include "Version.h"
#include "FileSystem.h"
#include "Log.h"

void digiSystemInit()
{
	digi::VersionInfo::add(versionInfo);
	
	#ifdef _WIN32
		#ifndef NO_BOOST_FILESYSTEM
			// set utf8 locale to boost::filesystem::path
			std::locale standardLocale = std::locale();
			std::locale loc(standardLocale, new boost::filesystem::detail::utf8_codecvt_facet);
			boost::filesystem::path::imbue(loc);
		#endif
	#else
		// ignore SIGPIPE
		signal(SIGPIPE, SIG_IGN);
	#endif
	
	digi::Log::init();
}

void digiSystemDone()
{
	digi::Log::close();
}
