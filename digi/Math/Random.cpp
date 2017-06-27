#include "All.h"


namespace digi {

// http://en.wikipedia.org/wiki/Multiply-with-carry

static const int rand_a = 1103515245;
static const int rand_c = 12345;

float random(float maxNumber, int& seed)
{
	int s = (rand_a * seed + rand_c) & 0x7fffffff;
	seed = s;
	
	return float(s) * 4.656612875245797e-010f * maxNumber;
}

float random(float minNumber, float maxNumber, int& seed)
{
	int s = (rand_a * seed + rand_c) & 0x7fffffff;
	seed = s;

	return float(s) * 4.656612875245797e-010f * (maxNumber - minNumber) + minNumber;
}

// http://www.advancedmcode.org/randsphere-generating-random-points-on-the-surface-of-a-sphere.html

float3 sphereSurfaceRandom(int& seed)
{
	float z = random(-1.0f, 1.0f, seed);
	float t = random(6.2832f, seed);
	float r = sqrt(1.0f - z * z);
	return vector3(r * cos(t), r * sin(t), z);
}

float3 sphereVolumeRandom(float radius, int& seed)
{
	float3 r = sphereSurfaceRandom(seed) * radius;
	
	// density of points in sphere is d = 1/r^2 for uniformly distributed r
	// to counteract this we need random with pdf = 3r^2
	// integrate and inverse gives mapping from uniform random to desired pdf
	return r * pow(random(1.0f, seed), 0.333333f);
}

float3 sphereVolumeRandom(float3 radius, int& seed)
{
	float3 r = sphereSurfaceRandom(seed) * radius;
	
	// density of points in sphere is d = 1/r^2 for uniformly distributed r
	// to counteract this we need random with pdf = 3r^2
	// integrate and inverse gives mapping from uniform random to desired pdf
	return r * pow(random(1.0f, seed), 0.333333f);
}

float gauss(float stdDev, int& seed)
{
	float u1 = random(1.0f, seed);
	float u2 = random(1.0f, seed);
	
	// box-muller method
	float r = sqrt(-2.0f * log(u1 + 1e-30f));
	float phi = 6.283185f * u2;
	return stdDev * r * cos(phi);
}

float2 gauss(float2 stdDev, int& seed)
{
	float u1 = random(1.0f, seed);
	float u2 = random(1.0f, seed);
	
	// box-muller method
	float r = sqrt(-2.0f * log(u1 + 1e-30f));
	float phi = 6.283185f * u2;
	return stdDev * vector2(r * cos(phi), r * sin(phi));
}

float3 gauss(float3 stdDev, int& seed)
{
	return vector3(gauss(getXY(stdDev), seed), gauss(stdDev.z, seed));
}

} // namespace digi
