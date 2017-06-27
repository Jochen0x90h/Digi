#ifdef _WIN32
	#include <windows.h>
#endif

#include "Version.h"


void digiNetworkInit()
{
	digi::VersionInfo::add(versionInfo);
	
	#ifdef _WIN32
		// init Win32 sockets
		WORD versionRequested = MAKEWORD(2, 2);
		WSADATA wsaData;
		WSAStartup(versionRequested, &wsaData);	
	#endif
}

void digiNetworkDone()
{
}
