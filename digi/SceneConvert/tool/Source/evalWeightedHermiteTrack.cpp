float xValues[100];
float keys[100];
int numKeys;
float x;

int index;

float result;

extern "C" void main()
{
	float x0 = xValues[index];
	float x1 = xValues[index + 1];
	
	const float4x4 bm = {
		{ 2, -3,  0,  1},
		{ 1, -2,  1,  0},
		{ 1, -1,  0,  0},
		{-2,  3,  0,  0}};

	// keys are organized as follows:
	//                               y(0), yOutTangent(0), xOutTangent(0),
	// xInTangent(1), yInTangent(1), y(1), yOutTangent(1), xOutTangent(1),
	// xInTangent(2), yInTangent(2), y(2), ...
	const float* key = keys + index * 5;
		
	float4 px = vector4(
		x0,      // x(index)
		key[2],  // xOutTangent(index)
		key[3],  // xInTangent(index + 1)
		x1);     // x(index + 1)
	float4 py = vector4(
		key[0],  // y(index)
		key[1],  // yOutTangent(index)
		key[4],  // yInTangent(index + 1)
		key[5]); // y(index + 1)

	// initial guess for spline parameter
	float u = (x - x0) / (x1 - x0);

	// part of x-spline
	float4 h = bm * px;
	
	// iteratively solve for u (so that spline(u).x = x)
	{
		float u2 = u * u;
		float4 uv = vector4(u2 * u, u2, u, 1.0f);
		
		// derivative
		float4 uv_ = vector4(3.0f * u2, 2.0f * u, 1.0f, 0.0f);
				
		// newton step
		u = u - (dot(uv, h) - x) / dot(uv_, h);
	}
	{
		float u2 = u * u;
		float4 uv = vector4(u2 * u, u2, u, 1.0f);
		
		// derivative
		float4 uv_ = vector4(3.0f * u2, 2.0f * u, 1.0f, 0.0f);
				
		// newton step
		u = u - (dot(uv, h) - x) / dot(uv_, h);
	}
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
	result = dot(uv * bm, py);
}
