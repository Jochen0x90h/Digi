// test if everything compiles and extract mangled function names from TestIntermediate.ll
// clang -cc1 TestIntermediate.cpp -emit-llvm

#include "Intermediate.h"


int main()
{
	float f;
	float2 f2;
	float3 f3;
	float4 f4;
	Texture2D t2;
	Texture3D t3;
	TextureCube tc;
	
	sample(t2, f2);
	sample(t3, f3);
	sample(tc, f3);
	
	sample(t2, f2, 0.0f);
	sample(t3, f3, 0.0f);
	sample(tc, f3, 0.0f);
	
	sample(t2, f2, f2, f2);
	sample(t3, f3, f3, f3);
	sample(tc, f3, f3, f3);

	dFdx(f);
	dFdy(f);

	dFdx(f2);
	dFdy(f2);

	dFdx(f3);
	dFdy(f3);

	dFdx(f4);
	dFdy(f4);

	return 0;
}
