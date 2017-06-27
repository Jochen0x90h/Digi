// projection matrix
float4x4 p;

// position in projection space (-1 to 1)
float x;
float y;

// result
float4x4 r;

extern "C" void main()
{
	// pick area
	float sx = 10000.0f;
	float sy = 10000.0f;
	float4x4 area = matrix4x4(
		sx, 0.0f, 0.0f, 0.0f,
		0.0f, sy, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-x * sx, -y * sy, 0.0f, 1.0f);

	// area * projectionMatrix
	r = area * p;
}
