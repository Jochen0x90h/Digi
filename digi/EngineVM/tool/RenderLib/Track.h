#include "TrackC.h"

inline float evalStepTrack(const float* xValues, const float* keys, int numKeys, float x)
{
	return eSTF(xValues, keys, numKeys, x);
}

inline float evalBezierTrack(const float* xValues, const float* keys, int numKeys, float x)
{
	return eBTF(xValues, keys, numKeys, x);
}

inline float evalWeightedBezierTrack(const float* xValues, const float* keys, int numKeys, float x)
{
	return eWBTF(xValues, keys, numKeys, x);
}

inline float evalCatmullRomTrack(const float* keys, float x)
{
	return eCTF(keys, x);
}

inline float evalStepTrack(const ushort* xValues, const ushort* keys, int numKeys, float x)
{
	return eSTS(xValues, keys, numKeys, x);
}

inline float evalBezierTrack(const ushort* xValues, const ushort* keys, int numKeys, float x)
{
	return eBTS(xValues, keys, numKeys, x);
}

inline float evalWeightedBezierTrack(const ushort* xValues, const ushort* keys, int numKeys, float x)
{
	return eWBTS(xValues, keys, numKeys, x);
}

inline float evalCatmullRomTrack(const ushort* keys, float x)
{
	return eCTS(keys, x);
}
