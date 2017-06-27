/*
	Auto-generated from InitLibraries.h.cmake.
	Call initLibraries() to call all init functions that were registered with ADD_INIT_FUNCTION.
*/

#include <digi/Base/VersionInfo.h>
	

// this saves us from including header files manually
void digiSceneConvertInit(); void digiImageConvertInit(); void digiMeshInit(); void digiSceneInit(); void digiCompressInit(); void digiChecksumInit(); void digiCodeGeneratorInit(); void digiImageInit(); void digiEngineVMInit(); void digiUtilityInit(); void digiMathInit(); void digiSystemInit(); void digiDataInit(); void digiEngineInit(); void digiLLVMJitInit(); void digiOpenGLInit(); 
void digiSceneConvertDone(); void digiImageConvertDone(); void digiMeshDone(); void digiSceneDone(); void digiCompressDone(); void digiChecksumDone(); void digiCodeGeneratorDone(); void digiImageDone(); void digiEngineVMDone(); void digiUtilityDone(); void digiMathDone(); void digiSystemDone(); void digiDataDone(); void digiEngineDone(); void digiLLVMJitDone(); void digiOpenGLDone(); 

static void initLibraries()
{
	digiSceneConvertInit();
	digiImageConvertInit();
	digiMeshInit();
	digiSceneInit();
	digiCompressInit();
	digiChecksumInit();
	digiCodeGeneratorInit();
	digiImageInit();
	digiEngineVMInit();
	digiUtilityInit();
	digiMathInit();
	digiSystemInit();
	digiDataInit();
	digiEngineInit();
	digiLLVMJitInit();
	digiOpenGLInit();
	
}

static void doneLibraries()
{
	digiSceneConvertDone();
	digiImageConvertDone();
	digiMeshDone();
	digiSceneDone();
	digiCompressDone();
	digiChecksumDone();
	digiCodeGeneratorDone();
	digiImageDone();
	digiEngineVMDone();
	digiUtilityDone();
	digiMathDone();
	digiSystemDone();
	digiDataDone();
	digiEngineDone();
	digiLLVMJitDone();
	digiOpenGLDone();
	
}
