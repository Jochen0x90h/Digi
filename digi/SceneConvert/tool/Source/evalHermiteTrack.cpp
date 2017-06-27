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
	//                y(0), yOutTangent(0),
	// yInTangent(1), y(1), yOutTangent(1),
	// yInTangent(2), y(2), ...
	const float* key = keys + index * 3;
	
	float4 py = vector4(
		key[0],  // y(index)
		key[1],  // yOutTangent(index)
		key[2],  // yInTangent(index + 1)
		key[3]); // y(index + 1)

	float u = (x - x0) / (x1 - x0);	
	float u2 = u * u;
	float4 uv = vector4(u2 * u, u2, u, 1.0f);
	result = dot(uv * bm, py);
}
