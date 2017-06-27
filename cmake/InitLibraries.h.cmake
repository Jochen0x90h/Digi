/*
	Auto-generated from InitLibraries.h.cmake.
	Call initLibraries() to call all init functions that were registered with ADD_INIT_FUNCTION.
*/

#include <digi/Base/VersionInfo.h>
	

// this saves us from including header files manually
${INIT_FUNCTION_PROTOTYPES}
${DONE_FUNCTION_PROTOTYPES}

static void initLibraries()
{
	${INIT_FUNCTIONS}
}

static void doneLibraries()
{
	${DONE_FUNCTIONS}
}
