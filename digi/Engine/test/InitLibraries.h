/*
	Auto-generated from InitLibraries.h.cmake.
	Call initLibraries() to call all init functions that were registered with ADD_INIT_FUNCTION.
*/

#include <digi/Base/VersionInfo.h>
	

// this saves us from including header files manually
void digiEngineInit(); void digiSystemInit(); void digiOpenGLInit(); void digiUtilityInit(); 
void digiEngineDone(); void digiSystemDone(); void digiOpenGLDone(); void digiUtilityDone(); 

static void initLibraries()
{
	digiEngineInit();
	digiSystemInit();
	digiOpenGLInit();
	digiUtilityInit();
	
}

static void doneLibraries()
{
	digiEngineDone();
	digiSystemDone();
	digiOpenGLDone();
	digiUtilityDone();
	
}
