/*
	Auto-generated from InitLibraries.h.cmake.
	Call initLibraries() to call all init functions that were registered with ADD_INIT_FUNCTION.
*/

#include <digi/Base/VersionInfo.h>
	

// this saves us from including header files manually
void digiImageInit(); void digiDataInit(); void digiMathInit(); void digiSystemInit(); void digiUtilityInit(); 
void digiImageDone(); void digiDataDone(); void digiMathDone(); void digiSystemDone(); void digiUtilityDone(); 

static void initLibraries()
{
	digiImageInit();
	digiDataInit();
	digiMathInit();
	digiSystemInit();
	digiUtilityInit();
	
}

static void doneLibraries()
{
	digiImageDone();
	digiDataDone();
	digiMathDone();
	digiSystemDone();
	digiUtilityDone();
	
}
