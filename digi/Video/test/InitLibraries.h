/*
	Auto-generated from InitLibraries.h.cmake.
	Call initLibraries() to call all init functions that were registered with ADD_INIT_FUNCTION.
*/

#include <digi/Base/VersionInfo.h>
	

// this saves us from including header files manually
void digiDisplayInit(); void digiSystemInit(); void digiOpenGLInit(); void digiUtilityInit(); void digiVideoInit(); void digiDataInit(); void digiAudioInit(); 
void digiDisplayDone(); void digiSystemDone(); void digiOpenGLDone(); void digiUtilityDone(); void digiVideoDone(); void digiDataDone(); void digiAudioDone(); 

static void initLibraries()
{
	digiDisplayInit();
	digiSystemInit();
	digiOpenGLInit();
	digiUtilityInit();
	digiVideoInit();
	digiDataInit();
	digiAudioInit();
	
}

static void doneLibraries()
{
	digiDisplayDone();
	digiSystemDone();
	digiOpenGLDone();
	digiUtilityDone();
	digiVideoDone();
	digiDataDone();
	digiAudioDone();
	
}
