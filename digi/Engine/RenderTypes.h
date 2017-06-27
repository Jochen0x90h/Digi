#ifndef digi_Engine_RenderTypes_h
#define digi_Engine_RenderTypes_h

#include <digi/Math/All.h>


namespace digi {

/// @addtogroup Engine
/// @{

struct Transform
{
	bool visible;
	float4x4 matrix;
};

// axis aligned bounding box
struct BoundingBox
{
	// center of bounding box
	float3 center;
	
	// size of bounding box is corner - center, i.e. half diameter in each dimension
	float3 size;
};


/// Projection for camera or orthographic view
struct Projection
{
	// note: this struct must be synced with RenderLib and care must be taken of the member alignment

	// perspective fit mode: 1.0: fill, 2.0: horizontal, 3.0: vertical, 4.0: overscan
	// orthographic fit mode: -1.0: fill, -2.0: horizontal, -3.0: vertical, -4.0: overscan
	float mode;

	// perspective: focal length
	// orthographic: orthographic width
	float scale;

	float2 filmSize;
	float2 filmOffset;
	float nearClipPlane;
	float farClipPlane;
};

static inline float4x4 matrix4x4Projection(const Projection& p, float viewAspect)
{
	// see wikipedia article on focal length.
	// angleOfView = atan(filmSize/2 / focalLength) * 2 * (180 / pi)

	float2 filmSize = p.filmSize;
	
	bool fovX;
	float mode = p.mode;
	float m = mode * mode;
	if (m < 2.0f)
	{
		// fill
		// x aligned fov when the film matches the window in x-direction and exceeds it in y-direction
		// y aligned fov when the film matches the window in y-direction and exceeds it in x-direction
		fovX = viewAspect * p.filmSize.y > p.filmSize.x;
	}
	else if (m < 5.0f)
	{
		// horizontal
		// always x aligned fov
		fovX = true;
	}
	else if (m < 10.0f)
	{
		// vertical
		// always y aligned fov
		fovX = false;
	}
	else
	{	
		// overscan
		// x aligned fov when the film matches the window in y-direction and does not fill it in x-direction
		// y aligned fov when the film matches the window in x-direction and does not fill it in y-direction
		fovX = viewAspect * p.filmSize.y < p.filmSize.x;
	}
	
	if (fovX)
	{
		// fov is fixed in x-direction
		filmSize.y = filmSize.x / viewAspect;
	}
	else
	{
		// fov is fixed in y-direction
		filmSize.x = filmSize.y * viewAspect;
	}

	float2 f = 2.0f * p.scale / filmSize;
	float2 o = 2.0f * p.filmOffset / filmSize;

	// create orthographic or perspective matrix
	if (mode < 0)
		return matrix4x4Orthographic(f, o, p.nearClipPlane, p.farClipPlane);
	else
		return matrix4x4Perspective(f, o, p.nearClipPlane, p.farClipPlane);
}

/// @}

} // namespace digi

#endif
