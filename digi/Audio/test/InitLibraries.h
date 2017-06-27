/*
	Auto-generated from InitLibraries.h.cmake.
	Call initLibraries() to call all init functions that were registered with ADD_INIT_FUNCTION.
*/

#include <digi/Base/VersionInfo.h>
	

// this saves us from including header files manually
void digiAudioInit(); void digiDataInit(); void digiSystemInit(); void digiUtilityInit(); 
void digiAudioDone(); void digiDataDone(); void digiSystemDone(); void digiUtilityDone(); 

static void initLibraries()
{
	digiAudioInit();
	digiDataInit();
	digiSystemInit();
	digiUtilityInit();
	
}

static void doneLibraries()
{
	digiAudioDone();
	digiDataDone();
	digiSystemDone();
	digiUtilityDone();
	
}
