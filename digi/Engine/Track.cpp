#include "Track.h"


namespace digi {

namespace
{
	// find index of key
	int findIndex(const float* xValues, int numKeys, float x)
	{
		int low = 0;
		int high = numKeys; 
	
		int index = (low + high) >> 1;
		while (low < high - 1)
		{
			if (xValues[index] > x)
				high = index;
			else
				low = index;
		
			index = (low + high) >> 1;
		}
	
		return index;
	}

	int findWeightedIndex(const float* xValues, int numKeys, float x)
	{
		int low = 0;
		int high = numKeys; 
	
		int index = (low + high) >> 1;
		while (low < high - 1)
		{
			if (xValues[index * 3] > x)
				high = index;
			else
				low = index;
		
			index = (low + high) >> 1;
		}
	
		return index;
	}

	int findIndex(const uint16_t* xValues, int numKeys, int x)
	{
		int low = 0;
		int high = numKeys; 
	
		int index = (low + high) >> 1;
		while (low < high - 1)
		{
			if (xValues[index] > x)
				high = index;
			else
				low = index;
		
			index = (low + high) >> 1;
		}
	
		return index;
	}

	int findWeightedIndex(const uint16_t* xValues, int numKeys, int x)
	{
		int low = 0;
		int high = numKeys; 
	
		int index = (low + high) >> 1;
		while (low < high - 1)
		{
			if (xValues[index * 3] > x)
				high = index;
			else
				low = index;
		
			index = (low + high) >> 1;
		}
	
		return index;
	}
} // anonymous namespace

float evalStepTrack(const float* xValues, const float* yValues, int numKeys, float x)
{
	int index = findIndex(xValues, numKeys, x);
	
	return yValues[index];
}

float evalHermiteTrack(const float* xValues, const float* yValues, int numKeys, float x)
{
	// find index. the last key cannot be found
	int index = findIndex(xValues, numKeys - 1, x);
	
	float x0 = xValues[index];
	float x1 = xValues[index + 1];

	const float4x4 bm = {
		{ 2, -3,  0,  1},
		{ 1, -2,  1,  0},
		{ 1, -1,  0,  0},
		{-2,  3,  0,  0}};
	
	// yValues are organized as follows:
	//                y(0), yOutTangent(0),
	// yInTangent(1), y(1), yOutTangent(1),
	// yInTangent(2), y(2), ...
	const float* y = yValues + index * 3;
	
	float4 py = vector4(
		y[0],  // y(index)
		y[1],  // yOutTangent(index)
		y[2],  // yInTangent(index + 1)
		y[3]); // y(index + 1)

	float u = (x - x0) / (x1 - x0);	
	float u2 = u * u;
	float4 uv = vector4(u2 * u, u2, u, 1.0f);
	return dot(uv * bm, py);
}

float evalWeightedHermiteTrack(const float* xValues, const float* yValues, int numKeys, float x)
{
	// find index. the last key cannot be found
	// xValues are organized as follows:
	//                x(0), xOutTangent(0),
	// xInTangent(1), x(1), xOutTangent(1),
	// xInTangent(2), x(2), ...
	int index = findWeightedIndex(xValues, numKeys - 1, x);
	int i3 = index * 3;

	const float* x_ = xValues + i3;
	float x0 = x_[0];
	float x1 = x_[3];
	
	const float4x4 bm = {
		{ 2, -3,  0,  1},
		{ 1, -2,  1,  0},
		{ 1, -1,  0,  0},
		{-2,  3,  0,  0}};

	// yValues are organized as follows:
	//                y(0), yOutTangent(0),
	// yInTangent(1), y(1), yOutTangent(1),
	// yInTangent(2), y(2), ...
	const float* y = yValues + i3;
		
	float4 px = vector4(
		x0,    // x(index)
		x_[1], // xOutTangent(index)
		x_[2], // xInTangent(index + 1)
		x1);   // x(index + 1)
	float4 py = vector4(
		y[0],  // y(index)
		y[1],  // yOutTangent(index)
		y[2],  // yInTangent(index + 1)
		y[3]); // y(index + 1)

	// initial guess for spline parameter
	float u = (x - x0) / (x1 - x0);

	// part of x-spline
	float4 h = bm * px;
	
	// iteratively solve for u (so that spline(u).x = x)
	for (int i = 0; i < 3; ++i)
	{
		float u2 = u * u;
		float4 uv = vector4(u2 * u, u2, u, 1.0f);
		
		// derivative
		float4 uv_ = vector4(3.0f * u2, 2.0f * u, 1.0f, 0.0f);
				
		// newton step
		u = u - (dot(uv, h) - x) / dot(uv_, h);
	}

	// calc y value (spline(u).y)
	float u2 = u * u;
	float4 uv = vector4(u2 * u, u2, u, 1.0f);
	return dot(uv * bm, py);
}

float evalBezierTrack(const float* xValues, const float* yValues, int numKeys, float x)
{
	// find index of key. the last key cannot be found
	int index = findIndex(xValues, numKeys - 1, x);
	
	float x0 = xValues[index];
	float x1 = xValues[index + 1];

	const float4x4 bm = {
		{-1,  3, -3,  1},
		{ 3, -6,  3,  0},
		{-3,  3,  0,  0},
		{ 1,  0,  0,  0}};
	
	// yValues are organized as follows:
	//                y(0), yOutControl(0),
	// yInControl(1), y(1), yOutControl(1),
	// yInControl(2), y(2), ...
	const float* y = yValues + index * 3;
	
	float4 py = vector4(
		y[0],  // y(index)
		y[1],  // yOutControl(index)
		y[2],  // yInControl(index + 1)
		y[3]); // y(index + 1)

	float u = (x - x0) / (x1 - x0);	
	float u2 = u * u;
	float4 uv = vector4(u2 * u, u2, u, 1.0f);
	return dot(uv * bm, py);
}

float evalWeightedBezierTrack(const float* xValues, const float* yValues, int numKeys, float x)
{
	// find index of key. the last key cannot be found
	// xValues are organized as follows:
	//                x(0), xOutControl(0),
	// xInControl(1), x(1), xOutControl(1),
	// xInControl(2), x(2), ...
	int index = findWeightedIndex(xValues, numKeys - 1, x);
	int i3 = index * 3;

	const float* x_ = xValues + i3;
	float x0 = x_[0];
	float x1 = x_[3];
	
	const float4x4 bm = {
		{-1,  3, -3,  1},
		{ 3, -6,  3,  0},
		{-3,  3,  0,  0},
		{ 1,  0,  0,  0}};

	// yValues are organized as follows:
	//                y(0), yOutControl(0),
	// yInControl(1), y(1), yOutControl(1),
	// yInControl(2), y(2), ...
	const float* y = yValues + i3;
		
	float4 px = vector4(
		x0,    // x(index)
		x_[1], // xOutControl(index)
		x_[2], // xInControl(index + 1)
		x1);   // x(index + 1)
	float4 py = vector4(
		y[0],  // y(index)
		y[1],  // yOutControl(index)
		y[2],  // yInControl(index + 1)
		y[3]); // y(index + 1)

	// initial guess for spline parameter
	float u = (x - x0) / (x1 - x0);

	// part of x-spline
	float4 h = bm * px;
	
	// iteratively solve for u (so that spline(u).x = x)
	for (int i = 0; i < 3; ++i)
	{
		float u2 = u * u;
		float4 uv = vector4(u2 * u, u2, u, 1.0f);
		
		// derivative
		float4 uv_ = vector4(3.0f * u2, 2.0f * u, 1.0f, 0.0f);
				
		// newton step
		u = u - (dot(uv, h) - x) / dot(uv_, h);
	}

	// calc y value (spline(u).y)
	float u2 = u * u;
	float4 uv = vector4(u2 * u, u2, u, 1.0f);
	return dot(uv * bm, py);
}

float evalCatmullRomTrack(const float* yValues, float x)
{
	float ix = floor(x);
	float a = x - ix;
	
	int i = int(ix);
	return catmullRom(
		yValues[i + 0],
		yValues[i + 1],
		yValues[i + 2],
		yValues[i + 3],
		a);
}

float evalStepTrack(const uint16_t* xValues, const uint16_t* yValues, int numKeys, float x)
{
	int index = findIndex(xValues, numKeys, int(x));
	
	return float(yValues[index]);
}

float evalBezierTrack(const uint16_t* xValues, const uint16_t* yValues, int numKeys, float x)
{
	// find index of key. the last key cannot be found
	int index = findIndex(xValues, numKeys - 1, int(x));

	float x0 = float(xValues[index]);
	float x1 = float(xValues[index + 1]);
	
	const float4x4 bm = {
		{-1,  3, -3,  1},
		{ 3, -6,  3,  0},
		{-3,  3,  0,  0},
		{ 1,  0,  0,  0}};
	
	// keys are organized as follows:
	//                y(0), yOutControl(0),
	// yInControl(1), y(1), yOutControl(1),
	// yInControl(2), y(2), ...
	const uint16_t* key = yValues + index * 3;
	
	float4 py = vector4(
		float(key[0]),  // y(index)
		float(key[1]),  // yOutControl(index)
		float(key[2]),  // yInControl(index + 1)
		float(key[3])); // y(index + 1)

	float u = (x - x0) / (x1 - x0);
	float u2 = u * u;
	float4 uv = vector4(u2 * u, u2, u, 1.0f);
	return dot(uv * bm, py);
}

float evalWeightedBezierTrack(const uint16_t* xValues, const uint16_t* yValues, int numKeys, float x)
{
	// find index of key. the last key cannot be found
	// xValues are organized as follows:
	//                x(0), xOutControl(0),
	// xInControl(1), x(1), xOutControl(1),
	// xInControl(2), x(2), ...
	int index = findWeightedIndex(xValues, numKeys - 1, int(x));
	int i3 = index * 3;
	
	const uint16_t* x_ = xValues + i3;
	float x0 = x_[0];
	float x1 = x_[3];
	
	const float4x4 bm = {
		{-1,  3, -3,  1},
		{ 3, -6,  3,  0},
		{-3,  3,  0,  0},
		{ 1,  0,  0,  0}};

	// keys are organized as follows:
	//                y(0), yOutControl(0),
	// yInControl(1), y(1), yOutControl(1),
	// yInControl(2), y(2), ...
	const uint16_t* y = yValues + i3;
		
	float4 px = vector4(
		x0,           // x(index)
		float(x_[1]), // xOutControl(index)
		float(x_[2]), // xInControl(index + 1)
		x1);          // x(index + 1)
	float4 py = vector4(
		float(y[0]),  // y(index)
		float(y[1]),  // yOutControl(index)
		float(y[2]),  // yInControl(index + 1)
		float(y[3])); // y(index + 1)

	// initial guess for spline parameter
	float u = (x - x0) / (x1 - x0);

	// part of x-spline
	float4 h = bm * px;
	
	// iteratively solve for u (so that spline(u).x = x)
	for (int i = 0; i < 3; ++i)
	{
		float u2 = u * u;
		float4 uv = vector4(u2 * u, u2, u, 1.0f);
		
		// derivative
		float4 uv_ = vector4(3.0f * u2, 2.0f * u, 1.0f, 0.0f);
				
		// newton step
		u = u - (dot(uv, h) - x) / dot(uv_, h);
	}

	// calc y value (spline(u).y)
	float u2 = u * u;
	float4 uv = vector4(u2 * u, u2, u, 1.0f);
	return dot(uv * bm, py);
}

float evalCatmullRomTrack(const uint16_t* yValues, float x)
{
	float ix = floor(x);
	float a = x - ix;
	
	int i = int(ix);
	return catmullRom(
		float(yValues[i + 0]),
		float(yValues[i + 1]),
		float(yValues[i + 2]),
		float(yValues[i + 3]),
		a);
}

}
