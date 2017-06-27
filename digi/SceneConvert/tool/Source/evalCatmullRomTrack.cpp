float keys[100];
float x;

float result;

inline float catmullRom(float p0, float p1, float p2, float p3, float a)
{
	float a2 = a * a;
	float a3 = a2 * a;

	return 0.5f * (
		+ p0 * (-a3 + 2.0f * a2 - a)
		+ p1 * (3.0f * a3 - 5.0f * a2 + 2.0f)
		+ p2 * (-3.0f * a3 + 4.0f * a2 + a)
		+ p3 * (a3 - a2));
}

extern "C" void main()
{
	float ix = floor(x);
	float a = x - ix;
	
	int i = int(ix);
	result = catmullRom(
		keys[i + 0],
		keys[i + 1],
		keys[i + 2], 
		keys[i + 3],
		a);
}
