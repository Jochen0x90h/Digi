//typedef void (*ResetShader)();

struct RenderJob
{
	// transform matrix
	float4x4 matrix;

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

struct RenderQueues
{
	// the available (unused) render jobs
	RenderJob* begin;
	RenderJob* end;
	
	// alpha sorting render queue
	RenderJob* alphaSort;
};
