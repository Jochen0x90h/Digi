/*
	Auto-generated from InitLibraries.h.cmake.
	Call initLibraries() to call all init functions that were registered with ADD_INIT_FUNCTION.
*/

#include <digi/Base/VersionInfo.h>
	

// this saves us from including header files manually
void digiCompressInit(); void digiDataInit(); void digiSystemInit(); void digiUtilityInit(); 
void digiCompressDone(); void digiDataDone(); void digiSystemDone(); void digiUtilityDone(); 

static void initLibraries()
{
	digiCompressInit();
	digiDataInit();
	digiSystemInit();
	digiUtilityInit();
	
}

static void doneLibraries()
{
	digiCompressDone();
	digiDataDone();
	digiSystemDone();
	digiUtilityDone();
	
}
