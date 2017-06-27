// arguments and result of noise2f(float x, float y)
float x;
float y; 
float result;

// gradient table
float grad[32 * 4];

// permutation table
int perm[512];



static const float F2 = 0.3660f; // 0.5 * (sqrt(3.0) - 1.0)
static const float G2 = 0.2113f; // (3.0 - sqrt(3.0)) / 6.0


extern "C" void main() 
{
	float2 v = vector2(x, y);
	
	// Skew the input space to determine which simplex cell we're in
	float s = (v.x + v.y) * F2; // Hairy factor for 2D
	float2 i = floor(v + s);
	float t = (i.x + i.y) * G2;
	float2 X0 = i - t; // Unskew the cell origin back to (x,y) space
	float2 x0 = v - X0; // The x,y distances from the cell origin
	
	// For the 2D case, the simplex shape is an equilateral triangle.
	// Determine which simplex we are in.
	// Offsets for second (middle) corner of simplex in (i,j) coords
	// lower triangle, XY order: (0,0)->(1,0)->(1,1)
	// upper triangle, YX order: (0,0)->(0,1)->(1,1)
	int2 i1 = x0.x > x0.y ? vector2(1, 0) : vector2(0, 1);
	
	// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
	// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
	// c = (3-sqrt(3))/6
	float2 x1 = x0 - convert_float2(i1) + G2; // Offsets for middle corner in (x,y) unskewed coords
	float2 x2 = x0 + (-1.0f + 2.0f * G2); // Offsets for last corner in (x,y) unskewed coords
	
	// Work out the hashed gradient indices of the three simplex corners
	int ii = (int)i.x & 255;
	int jj = (int)i.y & 255;
	int gi0 = (perm[ii        + perm[jj       ]] % 12) * 4;
	int gi1 = (perm[ii + i1.x + perm[jj + i1.y]] % 12) * 4;
	int gi2 = (perm[ii    + 1 + perm[jj +    1]] % 12) * 4;
	float3 n = vector3(
		dot(vector2(grad[gi0], grad[gi0 + 1]), x0), // (x,y) of grad3 used for 2D gradient
		dot(vector2(grad[gi1], grad[gi1 + 1]), x1),
		dot(vector2(grad[gi2], grad[gi2 + 1]), x2));
	
	// Calculate the contribution from the three corners
	float3 t0 = 0.5f - vector3(dot(x0, x0), dot(x1, x1), dot(x2, x2));
	t0 = max(t0, 0.0f);
	t0 *= t0;
	t0 *= t0;
	
	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the interval [-1,1].
	result = 70.0f * dot(t0, n);
}
