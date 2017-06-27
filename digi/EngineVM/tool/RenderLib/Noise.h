#include "NoiseC.h"

inline float noise(float2 v) {return noise2f(v.x, v.y);}

inline float noise(float v) {return noise2f(v, 0.0f);}

inline float noise(float3 v) {return noise3f(v.x, v.y, v.z);}

inline float3 noise3(float3 v)
{
	return vector3(
		noise(v),
		noise(v + vector3(17.0f, 58.0f, 111.0f)),
		noise(v + vector3(99.0f, 43.0f, 102.0f)));
}
