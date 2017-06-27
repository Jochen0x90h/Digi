// arguments and result of noise2f(float x, float y)
float x;
float y; 
float z; 
float result;

// gradient table
float grad[32 * 4];

// permutation table
int perm[512];



static const float F3 = 1.0f/3.0f;
static const float G3 = 1.0f/6.0f; // Very nice and simple unskew factor, too


extern "C" void main() 
{
	float3 v = vector3(x, y, z);
	// Skew the input space to determine which simplex cell we're in
	float s = (v.x + v.y + v.z) * F3; // Very nice and simple skew factor for 3D
	float3 i = floor(v + s);
	float t = (i.x + i.y + i.z) * G3;
	float3 X0 = i - t; // Unskew the cell origin back to (x,y,z) space
	float3 x0 = v - X0; // The x,y,z distances from the cell origin

	// For the 3D case, the simplex shape is a slightly irregular tetrahedron.
	// Determine which simplex we are in.
	int3 i1; // Offsets for second corner of simplex in (i,j,k) coords
	int3 i2; // Offsets for third corner of simplex in (i,j,k) coords
	if (x0.x >= x0.y)
	{
		if (x0.y >= x0.z)
		{
			// X Y Z order
			i1 = vector3(1, 0, 0);
			i2 = vector3(1, 1, 0);
		}
		else if (x0.x >= x0.z)
		{
			// X Z Y order
			i1 = vector3(1, 0, 0);
			i2 = vector3(1, 0, 1); 
		}
		else
		{
			// Z X Y order
			i1 = vector3(0, 0, 1);
			i2 = vector3(1, 0, 1); 
		}
	}
	else // x0 < y0
	{
		if (x0.y < x0.z)
		{
			// Z Y X order
			i1 = vector3(0, 0, 1);
			i2 = vector3(0, 1, 1); 
		}
		else if (x0.x < x0.z)
		{
			// Y Z X order
			i1 = vector3(0, 1, 0);
			i2 = vector3(0, 1, 1); 
		}
		else
		{
			// Y X Z order
			i1 = vector3(0, 1, 0);
			i2 = vector3(1, 1, 0);
		}
	}
	
	// A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
	// a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
	// a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
	// c = 1/6.
	float3 x1 = x0 - convert_float3(i1) + G3; // Offsets for second corner in (x,y,z) coords
	float3 x2 = x0 - convert_float3(i2) + 2.0f * G3; // Offsets for third corner in (x,y,z) coords
	float3 x3 = x0 + (-1.0f + 3.0f * G3); // Offsets for last corner in (x,y,z) coords

	// Work out the hashed gradient indices of the four simplex corners
	int ii = (int)i.x & 255;
	int jj = (int)i.y & 255;
	int kk = (int)i.z & 255;
	int gi0 = (perm[ii        + perm[jj        + perm[kk       ]]] % 12) * 4;
	int gi1 = (perm[ii + i1.x + perm[jj + i1.y + perm[kk + i1.z]]] % 12) * 4;
	int gi2 = (perm[ii + i2.x + perm[jj + i2.y + perm[kk + i2.z]]] % 12) * 4;
	int gi3 = (perm[ii +    1 + perm[jj +    1 + perm[kk +    1]]] % 12) * 4;
	float4 n = vector4(
		dot(vector3(grad[gi0], grad[gi0 + 1], grad[gi0 + 2]), x0), // (x,y) of grad3 used for 2D gradient
		dot(vector3(grad[gi1], grad[gi1 + 1], grad[gi1 + 2]), x1),
		dot(vector3(grad[gi2], grad[gi2 + 1], grad[gi2 + 2]), x2),
		dot(vector3(grad[gi3], grad[gi3 + 1], grad[gi3 + 2]), x3));

	// Calculate the contribution from the four corners
	float4 t0 = 0.6f - vector4(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3));
	t0 = max(t0, 0.0f);
	t0 *= t0;
	t0 *= t0;

	// Add contributions from each corner to get the final noise value.
	// The result is scaled to stay just inside [-1,1]
	result = 32.0f * dot(t0, n);
}
