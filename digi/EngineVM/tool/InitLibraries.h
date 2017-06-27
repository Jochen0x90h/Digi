/*
	Auto-generated from InitLibraries.h.cmake.
	Call initLibraries() to call all init functions that were registered with ADD_INIT_FUNCTION.
*/

#include <digi/Base/VersionInfo.h>
	

// this saves us from including header files manually
void digiEngineVMInit(); void digiEngineInit(); void digiLLVMJitInit(); void digiSystemInit(); void digiOpenGLInit(); void digiUtilityInit(); 
void digiEngineVMDone(); void digiEngineDone(); void digiLLVMJitDone(); void digiSystemDone(); void digiOpenGLDone(); void digiUtilityDone(); 

static void initLibraries()
{
	digiEngineVMInit();
	digiEngineInit();
	digiLLVMJitInit();
	digiSystemInit();
	digiOpenGLInit();
	digiUtilityInit();
	
}

static void doneLibraries()
{
	digiEngineVMDone();
	digiEngineDone();
	digiLLVMJitDone();
	digiSystemDone();
	digiOpenGLDone();
	digiUtilityDone();
	
}
