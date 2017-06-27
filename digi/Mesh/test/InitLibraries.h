/*
	Auto-generated from InitLibraries.h.cmake.
	Call initLibraries() to call all init functions that were registered with ADD_INIT_FUNCTION.
*/

#include <digi/Base/VersionInfo.h>
	

// this saves us from including header files manually
void digiMeshInit(); void digiUtilityInit(); void digiMathInit(); 
void digiMeshDone(); void digiUtilityDone(); void digiMathDone(); 

static void initLibraries()
{
	digiMeshInit();
	digiUtilityInit();
	digiMathInit();
	
}

static void doneLibraries()
{
	digiMeshDone();
	digiUtilityDone();
	digiMathDone();
	
}
