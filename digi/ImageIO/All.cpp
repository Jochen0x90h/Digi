#include <FreeImage.h>

#include "Version.h"

void digiImageIOInit()
{
	digi::VersionInfo::add(versionInfo);
	
	FreeImage_Initialise(true);
}

void digiImageIODone()
{
	FreeImage_DeInitialise();
}
