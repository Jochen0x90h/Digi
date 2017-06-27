#include "Platform.h"
#include "VersionInfo.h"


namespace digi {

// global list of version infos
VersionInfo* VersionInfo::first = NULL;

void VersionInfo::add(VersionInfo& versionInfo)
{
	if (versionInfo.next != NULL || VersionInfo::first == &versionInfo)
	{
		// error: version info already added
		return;
	}
	
	// link to global list
	versionInfo.next = VersionInfo::first;
	VersionInfo::first = &versionInfo;
}

std::string VersionInfo::get()
{
	VersionInfo* current = VersionInfo::first;
	
	std::string versionInfo;
	while (current != NULL)
	{
		versionInfo += current->name;
		versionInfo += ':';
		versionInfo += current->revision;
		current = current->next;
		if (current == NULL)
			break;
		versionInfo += ", ";
	}
	return versionInfo;	
}


} // namespace digi
