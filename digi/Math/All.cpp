#include "Version.h"
#include "half.h"


void digiMathInit()
{
	digi::VersionInfo::add(versionInfo);
	digi::half::initTables();
}

void digiMathDone()
{
}
