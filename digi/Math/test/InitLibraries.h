/*
	Auto-generated from InitLibraries.h.cmake.
	Call initLibraries() to call all init functions that were registered with ADD_INIT_FUNCTION.
*/

#include <digi/Base/VersionInfo.h>
	

// this saves us from including header files manually
void digiMathInit(); 
void digiMathDone(); 

static void initLibraries()
{
	digiMathInit();
	
}

static void doneLibraries()
{
	digiMathDone();
	
}
