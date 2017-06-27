/*
	Auto-generated from InitLibraries.h.cmake.
	Call initLibraries() to call all init functions that were registered with ADD_INIT_FUNCTION.
*/

#include <digi/Base/VersionInfo.h>
	

// this saves us from including header files manually
void digiDisplayInit(); void digiSystemInit(); void digiOpenGLInit(); void digiUtilityInit(); 
void digiDisplayDone(); void digiSystemDone(); void digiOpenGLDone(); void digiUtilityDone(); 

static void initLibraries()
{
	digiDisplayInit();
	digiSystemInit();
	digiOpenGLInit();
	digiUtilityInit();
	
}

static void doneLibraries()
{
	digiDisplayDone();
	digiSystemDone();
	digiOpenGLDone();
	digiUtilityDone();
	
}
