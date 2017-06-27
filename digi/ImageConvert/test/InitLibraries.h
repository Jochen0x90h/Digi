/*
	Auto-generated from InitLibraries.h.cmake.
	Call initLibraries() to call all init functions that were registered with ADD_INIT_FUNCTION.
*/

#include <digi/Base/VersionInfo.h>
	

// this saves us from including header files manually
void digiImageConvertInit(); void digiChecksumInit(); void digiCodeGeneratorInit(); void digiImageInit(); void digiEngineVMInit(); void digiUtilityInit(); void digiSystemInit(); void digiDataInit(); void digiMathInit(); void digiEngineInit(); void digiLLVMJitInit(); void digiOpenGLInit(); 
void digiImageConvertDone(); void digiChecksumDone(); void digiCodeGeneratorDone(); void digiImageDone(); void digiEngineVMDone(); void digiUtilityDone(); void digiSystemDone(); void digiDataDone(); void digiMathDone(); void digiEngineDone(); void digiLLVMJitDone(); void digiOpenGLDone(); 

static void initLibraries()
{
	digiImageConvertInit();
	digiChecksumInit();
	digiCodeGeneratorInit();
	digiImageInit();
	digiEngineVMInit();
	digiUtilityInit();
	digiSystemInit();
	digiDataInit();
	digiMathInit();
	digiEngineInit();
	digiLLVMJitInit();
	digiOpenGLInit();
	
}

static void doneLibraries()
{
	digiImageConvertDone();
	digiChecksumDone();
	digiCodeGeneratorDone();
	digiImageDone();
	digiEngineVMDone();
	digiUtilityDone();
	digiSystemDone();
	digiDataDone();
	digiMathDone();
	digiEngineDone();
	digiLLVMJitDone();
	digiOpenGLDone();
	
}
