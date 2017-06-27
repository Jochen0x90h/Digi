#ifndef digi_Engine_Track_h
#define digi_Engine_Track_h

#include <digi/Math/All.h>


namespace digi {

/// @addtogroup Engine
/// @{

// step track
float evalStepTrack(const float* xValues, const float* yValues, int numKeys, float x);

// hermite spline track
float evalHermiteTrack(const float* xValues, const float* yValues, int numKeys, float x);

// weighted hermite spline track
float evalWeightedHermiteTrack(const float* xValues, const float* yValues, int numKeys, float x);

/*
// equidistant spline (catmull rom) track
float sCRT(const uint16_t* keys, float x);
inline float sampleCatmullRomTrack(const uint16_t* keys, float x)
{
	return sCRT(keys, x) * 1.525902e-5f; // 1/65535
}
*/

// bezier spline track
float evalBezierTrack(const float* xValues, const float* yValues, int numKeys, float x);

// weighted bezier spline track
float evalWeightedBezierTrack(const float* xValues, const float* yValues, int numKeys, float x);

// equidistant catmull rom track
float evalCatmullRomTrack(const float* yValues, float x);


// step track
float evalStepTrack(const uint16_t* xValues, const uint16_t* yValues, int numKeys, float x);

// bezier spline track
float evalBezierTrack(const uint16_t* xValues, const uint16_t* yValues, int numKeys, float x);

// weighted bezier spline track
float evalWeightedBezierTrack(const uint16_t* xValues, const uint16_t* yValues, int numKeys, float x);

// equidistant catmull rom track
float evalCatmullRomTrack(const uint16_t* yValues, float x);


/// @}

} // namespace digi

#endif
