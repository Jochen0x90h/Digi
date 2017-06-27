#ifndef digi_Engine_RenderJob_h
#define digi_Engine_RenderJob_h

#include <digi/Utility/Standard.h>
#include <digi/Math/All.h>
#include <digi/OpenGL/GLWrapper.h>


namespace digi {

/// @addtogroup Engine
/// @{

//typedef void (*ResetShader)();

// render job. keep in sync with RenderJob.h in virtual machine
struct RenderJob
{
	// transform matrix
	ALIGN(16) float4x4 matrix;

	// distance of object, for alpha-sorting
	float distance;

	// id of object, for picking
	int id;

	// render function
	void (*render)(RenderJob* renderJob);
	
	// draw function
	void (*draw)(void* pInstance, void* pShader);

	// pointer to instance ("this")
	void* instance;

	// next in queue
	RenderJob* next;
};

// render queues. keep in sync with RenderJob.h in virtual machine
struct RenderQueues
{
	// the available (unused) render jobs
	RenderJob* begin;
	RenderJob* end;
	
	// alpha sorting render queue
	RenderJob* alphaSort;
};

void renderSorted(RenderJob* renderJobs);

//void resetShaderDummy();

/// @}

} // namespace digi

#endif
