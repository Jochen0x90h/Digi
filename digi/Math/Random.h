#ifndef Digi_Math_Random_h
#define Digi_Math_Random_h

#include "Vector3.h"


namespace digi {

/// @addtogroup Math
/// @{

/**
	these very simple pseudo-random functions are only intended for graphics as their
	randomness ist not very good. for better pseudo-random generators see Boost.Random

	random(maxNumber, seed) 1D, 2D or 3D random numbers in the range 0 to maxNumber
	random(minNumber, maxNumber, seed) 1D, 2D or 3D random numbers in the range minNumber to maxNumber
	
	sphereSurfaceRandom(radius, seed) random numbers on the surface of a sphere
	sphereVolumeRandom(radius, seed) random numbers in the volume of a sphere
	
	gauss(stdDev, seed) 1D, 2D or 3D gaussian random numbers with given standard deviation and mean of zero
	gauss(stdDevX, stdDevY, seed) 2D gaussian random numbers with given standard deviation and mean of zero
	gauss(stdDevX, stdDevY, stdDevZ, seed) 3D gaussian random numbers with given standard deviation and mean of zero
*/

// random number in the range [0, maxNumber]
float random(float maxNumber, int& seed);

// 2D random number in the range [0, maxNumber]
inline float2 random(float2 maxNumber, int& seed)
{
	return vector2(random(maxNumber.x, seed), random(maxNumber.y, seed));
}

// 3D random number in the range [0, maxNumber]
inline float3 random(float3 maxNumber, int& seed)
{
	return vector3(random(maxNumber.x, seed), random(maxNumber.y, seed), random(maxNumber.z, seed));
}

// random number in the range [minNumber, maxNumber]
float random(float minNumber, float maxNumber, int& seed);

// 2D random number in the range [minNumber, maxNumber]
inline float2 random(float2 minNumber, float2 maxNumber, int& seed)
{
	return vector2(random(minNumber.x, maxNumber.x, seed), random(minNumber.y, maxNumber.y, seed));
}

// 3D random number in the range [minNumber, maxNumber]
inline float3 random(float3 minNumber, float3 maxNumber, int& seed)
{
	return vector3(random(minNumber.x, maxNumber.x, seed), random(minNumber.y, maxNumber.y, seed), random(minNumber.z, maxNumber.z, seed));
}


// random numbers on the surface of a unit sphere
float3 sphereSurfaceRandom(int& seed);

// random numbers on the surface of a sphere
inline float3 sphereSurfaceRandom(float radius, int& seed) {return sphereSurfaceRandom(seed) * radius;}

// random numbers on the surface of an ellipsoid
inline float3 sphereSurfaceRandom(float3 radius, int& seed) {return sphereSurfaceRandom(seed) * radius;}

// random numbers in the volume of a sphere
float3 sphereVolumeRandom(float radius, int& seed);

// random numbers in the volume of an ellipsoid
float3 sphereVolumeRandom(float3 radius, int& seed);


// gaussian random numbers with given standard deviation and mean of zero
float gauss(float stdDev, int& seed);

// 2D gaussian
float2 gauss(float2 stdDev, int& seed);

// 3D gaussian
float3 gauss(float3 stdDev, int& seed);

/// @}

} // namespace Digi

#endif
