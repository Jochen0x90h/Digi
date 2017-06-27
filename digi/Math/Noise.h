#ifndef digi_Math_Noise_h
#define digi_Math_Noise_h


namespace digi {

/// @addtogroup Math
/// @{

// 2D simplex noise
float noise(float2 v);

// 1D simplex noise (x-axis of 2D simplex noise)
inline float noise(float v) {return noise(vector2(v, 0.0f));}

// 3D simplex noise
float noise(float3 v);

// 3D simplex noise returning a vector
inline float3 noise3(float3 v)
{
	return vector3(
		noise(v),
		noise(v + vector3(17.0f, 58.0f, 111.0f)),
		noise(v + vector3(99.0f, 43.0f, 102.0f)));
}

//float noise(float4 v);	

/// @}

} // namespace digi

#endif
