#ifndef digi_Base_VersionInfo_h
#define digi_Base_VersionInfo_h

#include <string>

namespace digi
{

/**
	Version info provides version information per library.
	
	To use svn on windows see http://www.sliksvn.com/en/download
*/
struct VersionInfo
{
	// name of library/executable
	const char* name;
	
	// revision of version control system (e.g. "250M")
	const char* revision;

	// pointer to next info
	VersionInfo* next;

	// add a version info
	static void add(VersionInfo& versionInfo);
	
	// get a string containing all version infos
	static std::string get();
	
	// global list of version infos
	static VersionInfo* first;
};

} // namespace digi

#endif
