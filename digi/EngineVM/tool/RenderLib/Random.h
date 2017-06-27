const int rand_a = 1103515245;
const int rand_c = 12345;

inline float random(float maxNumber, int& seed)
{
	int s = (rand_a * seed + rand_c) & 0x7fffffff;
	seed = s;
	
	return float(s) * 4.656612875245797e-010f * maxNumber;
}

inline float3 random(float3 maxNumber, int& seed)
{
	return vector3(random(maxNumber.x, seed), random(maxNumber.y, seed), random(maxNumber.z, seed));
}

inline float random(float minNumber, float maxNumber, int& seed)
{
	int s = (rand_a * seed + rand_c) & 0x7fffffff;
	seed = s;

	return float(s) * 4.656612875245797e-010f * (maxNumber - minNumber) + minNumber;
}

inline float3 random(float3 minNumber, float3 maxNumber, int& seed)
{
	return vector3(random(minNumber.x, maxNumber.x, seed), random(minNumber.y, maxNumber.y, seed), random(minNumber.z, maxNumber.z, seed));
}


inline float3 sphereSurfaceRandom(int& seed)
{
	float z = random(-1.0f, 1.0f, seed);
	float t = random(6.2832f, seed);
	float r = sqrt(1.0f - z * z);
	return vector3(r * cos(t), r * sin(t), z);
}

inline float3 sphereSurfaceRandom(float radius, int& seed) {return sphereSurfaceRandom(seed) * radius;}

inline float3 sphereSurfaceRandom(float3 radius, int& seed) {return sphereSurfaceRandom(seed) * radius;}

inline float3 sphereVolumeRandom(float radius, int& seed)
{
	float3 r = sphereSurfaceRandom(seed) * radius;
	return r * pow(random(1.0f, seed), 0.333333f);
}

inline float3 sphereVolumeRandom(float3 radius, int& seed)
{
	float3 r = sphereSurfaceRandom(seed) * radius;
	return r * pow(random(1.0f, seed), 0.333333f);
}


inline float gauss(float stdDev, int& seed)
{
	float u1 = random(1.0f, seed);
	float u2 = random(1.0f, seed);
	
	// box-muller method
	float r = sqrt(-2.0f * log(u1 + 1e-30f));
	float phi = 6.283185f * u2;
	return stdDev * r * cos(phi);
}

inline float2 gauss(float2 stdDev, int& seed)
{
	float u1 = random(1.0f, seed);
	float u2 = random(1.0f, seed);
	
	// box-muller method
	float r = sqrt(-2.0f * log(u1 + 1e-30f));
	float phi = 6.283185f * u2;
	return stdDev * vector2(r * cos(phi), r * sin(phi));
}

inline float3 gauss(float3 stdDev, int& seed)
{
	return vector3(gauss(stdDev.xy, seed), gauss(stdDev.z, seed));
}
