#include "All.h"


namespace digi {

// http://staffwww.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf

// permutation table
static const int perm[512] = 
{
	151,160,137,91,90,15,
	131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
	190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
	88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
	77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
	102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
	135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
	5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
	223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
	129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
	251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
	49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
	
	// repeat to prevent overflow
	151,160,137,91,90,15,
	131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
	190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
	88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
	77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
	102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
	135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
	5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
	223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
	129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
	251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
	49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

static const float grad[32 * 4] =
{
	// 2D - 4D
	 1,  1,  0,  1,
	-1,  1,  0,  1,
	 1, -1,  0,  1,
	-1, -1,  0,  1,
	 1,  0,  1,  1,
	-1,  0,  1,  1,
	 1,  0, -1,  1,
	-1,  0, -1,  1,
	 0,  1,  1,  1,
	 0, -1,  1,  1,
	 0,  1, -1,  1,
	 0, -1, -1,  1,
	
	// 4D
	 1,  1,  0, -1,
	-1,  1,  0, -1,
	 1, -1,  0, -1,
	-1, -1,  0, -1,
	 1,  0,  1, -1,
	-1,  0,  1, -1,
	 1,  0, -1, -1,
	-1,  0, -1, -1,
	 0,  1,  1, -1,
	 0, -1,  1, -1,
	 0,  1, -1, -1,
	 0, -1, -1, -1,
	 1,  1,  1,  0,
	 1,  1, -1,  0,
	 1, -1,  1,  0,
	 1, -1, -1,  0,
	-1,  1,  1,  0,
	-1,  1, -1,  0,
	-1, -1,  1,  0,
	-1, -1, -1,  0
};

static const float F2 = 0.3660f; // 0.5 * (sqrt(3.0) - 1.0)
static const float G2 = 0.2113f; // (3.0 - sqrt(3.0)) / 6.0


float noise(float2 v)
{
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
	return 70.0f * dot(t0, n);
}

static const float F3 = 1.0f/3.0f;
static const float G3 = 1.0f/6.0f; // Very nice and simple unskew factor, too

float noise(float3 v)
{
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
	return 32.0f * dot(t0, n);
}

} // namespace digi
