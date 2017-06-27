/*
	Auto-generated from InitLibraries.h.cmake.
	Call initLibraries() to call all init functions that were registered with ADD_INIT_FUNCTION.
*/

#include <digi/Base/VersionInfo.h>
	

// this saves us from including header files manually
void digiCodeGeneratorInit(); void digiSystemInit(); void digiUtilityInit(); 
void digiCodeGeneratorDone(); void digiSystemDone(); void digiUtilityDone(); 

static void initLibraries()
{
	digiCodeGeneratorInit();
	digiSystemInit();
	digiUtilityInit();
	
}

static void doneLibraries()
{
	digiCodeGeneratorDone();
	digiSystemDone();
	digiUtilityDone();
	
}
