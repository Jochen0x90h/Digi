float xValues[100];
float yValues[100];
int numKeys;
float x;

int index;

float result;

extern "C" void main()
{
	int i3 = index * 3;

	const float* x_ = xValues + i3;
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
