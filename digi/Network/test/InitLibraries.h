/*
	Auto-generated from InitLibraries.h.cmake.
	Call initLibraries() to call all init functions that were registered with ADD_INIT_FUNCTION.
*/

#include <digi/Base/VersionInfo.h>
	

// this saves us from including header files manually
void digiNetworkInit(); void digiSystemInit(); void digiUtilityInit(); 
void digiNetworkDone(); void digiSystemDone(); void digiUtilityDone(); 

static void initLibraries()
{
	digiNetworkInit();
	digiSystemInit();
	digiUtilityInit();
	
}

static void doneLibraries()
{
	digiNetworkDone();
	digiSystemDone();
	digiUtilityDone();
	
}
