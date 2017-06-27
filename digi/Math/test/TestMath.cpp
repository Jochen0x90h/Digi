#define __STDC_CONSTANT_MACROS
#define __STDC_LIMIT_MACROS

#include <gtest/gtest.h>

#include <digi/Base/VersionInfo.h>
#include <digi/Math/All.h>
#include <digi/Math/GTestHelpers.h>

#include "InitLibraries.h"


using namespace digi;
//using digi::ushort;
//using digi::uint;


// ----------------------------------------------------------------------------
/// eigenvectors

template <typename Type>
Vector3<Type> positiveSymmetricEig(const DIGI_MATRIX3X3(Type)& M)
{
	// http://en.wikipedia.org/wiki/Eigenvalue_algorithm
	
	Type m = trace(M) / 3.0f;
	DIGI_MATRIX3X3(Type) K = M - diag(vector3(m, m, m));
	Type q = det(K) / 2.0f;
	 
	Type p = sum(K.x * K.x + K.y * K.y + K.z * K.z) / 6.0f;
	 
	Type phi = atan2(sqrt(p*p*p - q*q), q) / 3.0f;
	if (phi < 0)
		phi = phi + Type(pi / 3.0);
	 
	return vector3(
		m + 2.0f * sqrt(p) * cos(phi),
		m - sqrt(p) * (cos(phi) + sqrt(3.0f) * sin(phi)),
		m - sqrt(p) * (cos(phi) - sqrt(3.0f) * sin(phi)));
}

// ----------------------------------------------------------------------------

TEST(Math, InitLibraries)
{
	initLibraries();
	std::string versionInfo = VersionInfo::get();
}

TEST(Math, half)
{
	// round-trip test: half -> float -> half
	for (int i = 0; i < 65536; ++i)
	{
		half h1 = as<half>(int16_t(i));
		float f = h1;
		half h2 = half(f);

		EXPECT_EQ(h1, h2);
	}

	// test some values
	EXPECT_EQ(float(half()), 0.0f);
	EXPECT_EQ(float(half(0.0f)), 0.0f);
	EXPECT_EQ(float(half(1.0f)), 1.0f);
	EXPECT_EQ(float(half(-1.0f)), -1.0f);
	EXPECT_EQ(float(half(1)), 1.0f);

	// test constructor and assign
	half h;
	h = half(half(1.0f));
	EXPECT_EQ(float(h), 1.0f);
}

TEST(Math, ScalarFunctions)
{
	EXPECT_EQ(abs(1.8f), 1.8f);
	EXPECT_EQ(abs(-1.8f), 1.8f);

	EXPECT_EQ(floor(0.8f), 0.0f);
	EXPECT_EQ(ceil(0.8f), 1.0f);
	EXPECT_EQ(round(0.8f), 1.0f);
	EXPECT_EQ(trunc(0.8f), 0.0f);

	EXPECT_EQ(floor(0.5f), 0.0f);
	EXPECT_EQ(ceil(0.5f), 1.0f);
	EXPECT_EQ(round(0.5f), 1.0f);
	EXPECT_EQ(trunc(0.5f), 0.0f);

	EXPECT_EQ(floor(0.2f), 0.0f);
	EXPECT_EQ(ceil(0.2f), 1.0f);
	EXPECT_EQ(round(0.2f), 0.0f);
	EXPECT_EQ(trunc(0.2f), 0.0f);

	EXPECT_EQ(floor(-0.8f), -1.0f);
	EXPECT_EQ(ceil(-0.8f), 0.0f);
	EXPECT_EQ(round(-0.8f), -1.0f);
	EXPECT_EQ(trunc(-0.8f), 0.0f);

	EXPECT_EQ(floor(-0.5f), -1.0f);
	EXPECT_EQ(ceil(-0.5f), 0.0f);
	EXPECT_EQ(round(-0.5f), -1.0f);
	EXPECT_EQ(trunc(-0.5f), 0.0f);

	EXPECT_EQ(floor(-0.2f), -1.0f);
	EXPECT_EQ(ceil(-0.2f), 0.0f);
	EXPECT_EQ(round(-0.2f), 0.0f);
	EXPECT_EQ(trunc(-0.2f), 0.0f);
}

// enable vector swizzle, e.g. v.xyz
#include <digi/Math/SwizzleDefine.inc.h>

TEST(Math, Vector2)
{	
	// test creators
	float2 a = make_float2(42, -1);
	EXPECT_EQ(a.x, 42.0f);
	EXPECT_EQ(a.y, -1.0f);
	float2 b = vector2(33.f, 17.0f);
	EXPECT_VECTOR_EQ(b, make_float2(33, 17));

	// test zero initializer
	float2 c = {};
	EXPECT_VECTOR_EQ(c, splat2(0.0f));

	// test =
	c = a;
	EXPECT_VECTOR_EQ(c, a);
	c = float2();
	EXPECT_VECTOR_EQ(c, 0.0f);
	c = 55.0f;
	EXPECT_VECTOR_EQ(c, 55.0f);
	c = b.xy;
	EXPECT_VECTOR_EQ(c, b);
	c.xy = a;
	EXPECT_VECTOR_EQ(c, a);
	c.xy = 55.0f;
	EXPECT_VECTOR_EQ(c, 55.0f);
	c.xy = b.xy;
	EXPECT_VECTOR_EQ(c, b);
	c.xy.xy = a;
	EXPECT_VECTOR_EQ(c, a);

	// test +=
	c = a;
	c += b;
	EXPECT_VECTOR_EQ(c, a + b);
	c = a;
	c += 1.0f;
	EXPECT_VECTOR_EQ(c, a + 1.0f);
	c = a;
	c.xy += b;
	EXPECT_VECTOR_EQ(c, a + b);
	c = a;
	c += b.xy;
	EXPECT_VECTOR_EQ(c, a + b);
	c = a;
	c.xy += b.xy;
	EXPECT_VECTOR_EQ(c, a + b);
	c = a;
	c.xy += 1.0f;
	EXPECT_VECTOR_EQ(c, a + 1.0f);

	// test -=
	c = a;
	c -= b;
	EXPECT_VECTOR_EQ(c, a - b);
	c = a;
	c -= 1.0f;
	EXPECT_VECTOR_EQ(c, a - 1.0f);
	c = a;
	c.xy -= b;
	EXPECT_VECTOR_EQ(c, a - b);
	c = a;
	c -= b.xy;
	EXPECT_VECTOR_EQ(c, a - b);
	c = a;
	c.xy -= b.xy;
	EXPECT_VECTOR_EQ(c, a - b);
	c = a;
	c.xy -= 1.0f;
	EXPECT_VECTOR_EQ(c, a - 1.0f);

	// test *=
	c = a;
	c *= b;
	EXPECT_VECTOR_EQ(c, a * b);
	c = a;
	c *= 2.0f;
	EXPECT_VECTOR_EQ(c, a * 2.0f);
	c = a;
	c.xy *= b;
	EXPECT_VECTOR_EQ(c, a * b);
	c = a;
	c *= b.xy;
	EXPECT_VECTOR_EQ(c, a * b);
	c = a;
	c.xy *= b.xy;
	EXPECT_VECTOR_EQ(c, a * b);
	c = a;
	c.xy *= 1.0f;
	EXPECT_VECTOR_EQ(c, a * 1.0f);

	// test /=
	c = a;
	c /= b;
	EXPECT_VECTOR_EQ(c, a / b);
	c = a;
	c /= 2.0f;
	EXPECT_VECTOR_EQ(c, a / 2.0f);
	c = a;
	c.xy /= b;
	EXPECT_VECTOR_EQ(c, a / b);
	c = a;
	c /= b.xy;
	EXPECT_VECTOR_EQ(c, a / b);
	c = a;
	c.xy /= b.xy;
	EXPECT_VECTOR_EQ(c, a / b);
	c = a;
	c.xy /= 2.0f;
	EXPECT_VECTOR_EQ(c, a / 2.0f);

	// test index operator
	EXPECT_EQ(a[0], 42.0f);
	EXPECT_EQ(a[1], -1.0f);

	// test unary +/-
	EXPECT_VECTOR_EQ(-a, 0.0f - a);
	EXPECT_VECTOR_EQ(+a, a);
	EXPECT_VECTOR_EQ(-a.xy, 0.0f - a);
	EXPECT_VECTOR_EQ(+a.xy, a);

	// test +
	EXPECT_VECTOR_EQ(a + b, make_float2(75, 16));
	EXPECT_VECTOR_EQ(a + 1.0f, make_float2(43, 0));
	EXPECT_VECTOR_EQ(1.0f + b, make_float2(34, 18));
	EXPECT_VECTOR_EQ(a.xy + b.xy, a + b);
	EXPECT_VECTOR_EQ(a.xy + 1.0f, a + 1.0f);
	EXPECT_VECTOR_EQ(1.0f + b.xy, 1.0f + b);
		
	// test -
	EXPECT_VECTOR_EQ(a - b, make_float2(9, -18));
	EXPECT_VECTOR_EQ(a - 1.0f, make_float2(41, -2));
	EXPECT_VECTOR_EQ(1.0f - b, make_float2(-32, -16));
	EXPECT_VECTOR_EQ(a.xy - b.xy, a - b);
	EXPECT_VECTOR_EQ(a.xy - 1.0f, a - 1.0f);
	EXPECT_VECTOR_EQ(1.0f - b.xy, 1.0f - b);

	// test *
	EXPECT_VECTOR_EQ(a * b, vector2(1386.0f, -17.0f));
	EXPECT_VECTOR_EQ(a * 5.0f, vector2(210.0f, -5.0f));
	EXPECT_VECTOR_EQ(5.0f * b, vector2(165.0f, 85.0f));
	EXPECT_VECTOR_EQ(a.xy * b.xy, a * b);
	EXPECT_VECTOR_EQ(a.xy * 5.0f, a * 5.0f);
	EXPECT_VECTOR_EQ(5.0f * b.xy, 5.0f * b);

	// test /
	EXPECT_EPSILON_EQ(a / b, vector2(1.27273f, -0.0588235f));
	EXPECT_VECTOR_EQ(a / 2.0f, vector2(21.0f, -0.5f));
	EXPECT_EPSILON_EQ(2.0f / b, vector2(0.0606061f, 0.117647f));
	EXPECT_EPSILON_EQ(a.xy / b.xy, a / b);
	EXPECT_VECTOR_EQ(a.xy / 2.0f, a / 2.0f);
	EXPECT_EPSILON_EQ(2.0f / b.xy, 2.0f / b);

	// test ==
	EXPECT_VECTOR_EQ(a == a, vector2(true, true));
	EXPECT_VECTOR_EQ(a == b, vector2(false, false));
		
	// test epsilonEquals
	EXPECT_EQ(epsilonEquals(a, b, 100.0f), true);
	EXPECT_EQ(epsilonEquals(a, b, 0.01f), false);

	// test normalize()
	c = normalize(b);
	EXPECT_EPSILON_EQ(lengthSquared(c), 1.0f);

	// test min
	EXPECT_VECTOR_EQ(min(a, b), make_float2(33, -1));
	EXPECT_VECTOR_EQ(min(a, 0.0f), make_float2(0, -1));
	EXPECT_VECTOR_EQ(min(a.yx, b.yx), make_float2(-1, 33));
		
	// test max
	EXPECT_VECTOR_EQ(max(a, b), make_float2(42, 17));
	EXPECT_VECTOR_EQ(max(a, 0.0f), make_float2(42, 0));
	EXPECT_VECTOR_EQ(max(a.yx, b.yx), make_float2(17, 42));

	// test dot
	EXPECT_EQ(dot(a, b), 1369.0f);

	// distance
	EXPECT_EPSILON_EQ(distance(a, b), 20.124611f);

	// swizzle		
	EXPECT_VECTOR_EQ(a.xx, splat2(42.0f));		
}

TEST(Math, Vector3)
{
	// test creators
	float3 a = make_float3(42, -1, 9);
	EXPECT_VECTOR_EQ(a.x, 42.0f);
	EXPECT_VECTOR_EQ(a.y, -1.0f);
	float3 b = vector3(33.f, 17.0f, -7.0f);
	EXPECT_VECTOR_EQ(b, make_float3(33, 17, -7));

	// test zero initializer
	float3 c = {};
	EXPECT_VECTOR_EQ(c, splat3(0.0f));

	// test =
	c = a;
	EXPECT_VECTOR_EQ(c, a);
	c = float3();
	EXPECT_VECTOR_EQ(c, splat3(0.0f));
	c = 55.0f;
	EXPECT_VECTOR_EQ(c, splat3(55.0f));
	c = b.xyz;
	EXPECT_VECTOR_EQ(c, b);
	c.xyz = a;
	EXPECT_VECTOR_EQ(c, a);
	c.xyz = 55.0f;
	EXPECT_VECTOR_EQ(c, splat3(55.0f));
	c.xyz = b.xyz;
	EXPECT_VECTOR_EQ(c, b);
	c.xyz.xyz = a;
	EXPECT_VECTOR_EQ(c, a);

	// test +=
	c = a;
	c += b;
	EXPECT_VECTOR_EQ(c, a + b);
	c = a;
	c += 1.0f;
	EXPECT_VECTOR_EQ(c, a + 1.0f);
	c = a;
	c.xyz += b;
	EXPECT_VECTOR_EQ(c, a + b);
	c = a;
	c += b.xyz;
	EXPECT_VECTOR_EQ(c, a + b);
	c = a;
	c.xyz += b.xyz;
	EXPECT_VECTOR_EQ(c, a + b);
	c = a;
	c.xyz += 1.0f;
	EXPECT_VECTOR_EQ(c, a + 1.0f);

	// test -=
	c = a;
	c -= b;
	EXPECT_VECTOR_EQ(c, a - b);
	c = a;
	c -= 1.0f;
	EXPECT_VECTOR_EQ(c, a - 1.0f);
	c = a;
	c.xyz -= b;
	EXPECT_VECTOR_EQ(c, a - b);
	c = a;
	c -= b.xyz;
	EXPECT_VECTOR_EQ(c, a - b);
	c = a;
	c.xyz -= b.xyz;
	EXPECT_VECTOR_EQ(c, a - b);
	c = a;
	c.xyz -= 1.0f;
	EXPECT_VECTOR_EQ(c, a - 1.0f);

	// test *=
	c = a;
	c *= b;
	EXPECT_VECTOR_EQ(c, a * b);
	c = a;
	c *= 2.0f;
	EXPECT_VECTOR_EQ(c, a * 2.0f);
	c = a;
	c.xyz *= b;
	EXPECT_VECTOR_EQ(c, a * b);
	c = a;
	c *= b.xyz;
	EXPECT_VECTOR_EQ(c, a * b);
	c = a;
	c.xyz *= b.xyz;
	EXPECT_VECTOR_EQ(c, a * b);
	c = a;
	c.xyz *= 1.0f;
	EXPECT_VECTOR_EQ(c, a * 1.0f);

	// test /=
	c = a;
	c /= b;
	EXPECT_VECTOR_EQ(c, a / b);
	c = a;
	c /= 2.0f;
	EXPECT_VECTOR_EQ(c, a / 2.0f);
	c = a;
	c.xyz /= b;
	EXPECT_VECTOR_EQ(c, a / b);
	c = a;
	c /= b.xyz;
	EXPECT_VECTOR_EQ(c, a / b);
	c = a;
	c.xyz /= b.xyz;
	EXPECT_VECTOR_EQ(c, a / b);
	c = a;
	c.xyz /= 2.0f;
	EXPECT_VECTOR_EQ(c, a / 2.0f);

	// test index operator
	EXPECT_EQ(a[0], 42.0f);
	EXPECT_EQ(a[1], -1.0f);
	EXPECT_EQ(a[2], 9.0f);

	// test unary +/-
	EXPECT_VECTOR_EQ(-a, 0.0f - a);
	EXPECT_VECTOR_EQ(+a, a);
	EXPECT_VECTOR_EQ(-a.xyz, 0.0f - a);
	EXPECT_VECTOR_EQ(+a.xyz, a);

	// test +
	EXPECT_VECTOR_EQ(a + b, make_float3(75, 16, 2));
	EXPECT_VECTOR_EQ(a + 1.0f, make_float3(43, 0, 10));
	EXPECT_VECTOR_EQ(1.0f + b, make_float3(34, 18, -6));
	EXPECT_VECTOR_EQ(a.xyz + b.xyz, a + b);
	EXPECT_VECTOR_EQ(a.xyz + 1.0f, a + 1.0f);
	EXPECT_VECTOR_EQ(1.0f + b.xyz, 1.0f + b);
		
	// test -
	EXPECT_VECTOR_EQ(a - b, make_float3(9, -18, 16));
	EXPECT_VECTOR_EQ(a - 1.0f, make_float3(41, -2, 8));
	EXPECT_VECTOR_EQ(1.0f - b, make_float3(-32, -16, 8));
	EXPECT_VECTOR_EQ(a.xyz - b.xyz, a - b);
	EXPECT_VECTOR_EQ(a.xyz - 1.0f, a - 1.0f);
	EXPECT_VECTOR_EQ(1.0f - b.xyz, 1.0f - b);

	// test *
	EXPECT_VECTOR_EQ(a * b, vector3(1386.0f, -17.0f, -63.0f));
	EXPECT_VECTOR_EQ(a * 5.0f, vector3(210.0f, -5.0f, 45.0f));
	EXPECT_VECTOR_EQ(5.0f * b, vector3(165.0f, 85.0f, -35.0f));
	EXPECT_VECTOR_EQ(a.xyz * b.xyz, a * b);
	EXPECT_VECTOR_EQ(a.xyz * 5.0f, a * 5.0f);
	EXPECT_VECTOR_EQ(5.0f * b.xyz, 5.0f * b);

	// test /
	EXPECT_EPSILON_EQ(a / b, vector3(1.27273f, -0.0588235f, -1.285714f));
	EXPECT_VECTOR_EQ(a / 2.0f, vector3(21.0f, -0.5f, 4.5f));
	EXPECT_EPSILON_EQ(2.0f / b, vector3(0.0606061f, 0.117647f, -0.285714f));
	EXPECT_EPSILON_EQ(a.xyz / b.xyz, a / b);
	EXPECT_VECTOR_EQ(a.xyz / 2.0f, a / 2.0f);
	EXPECT_EPSILON_EQ(2.0f / b.xyz, 2.0f / b);

	// test ==
	EXPECT_VECTOR_EQ(a == a, vector3(true, true, true));
	EXPECT_VECTOR_EQ(a == b, vector3(false, false, false));
		
	// test epsilonEquals
	EXPECT_EQ(epsilonEquals(a, b, 100.0f), true);
	EXPECT_EQ(epsilonEquals(a, b, 0.01f), false);

	// test normalize()
	c = normalize(b);
	EXPECT_EPSILON_EQ(lengthSquared(c), 1.0f);

	// test min
	EXPECT_VECTOR_EQ(min(a, b), make_float3(33, -1, -7));
	EXPECT_VECTOR_EQ(min(a, 0.0f), make_float3(0, -1, 0));
		
	// test max
	EXPECT_VECTOR_EQ(max(a, b), make_float3(42, 17, 9));
	EXPECT_VECTOR_EQ(max(a, 0.0f), make_float3(42, 0, 9));

	// test dot
	EXPECT_EQ(dot(a, b), 1306.0f);

	// distance
	EXPECT_EPSILON_EQ(distance(a, b), 25.709921f);

	// swizzle		
	EXPECT_VECTOR_EQ(a.xxx, splat3(42.0f));		
}

TEST(Math, Vector4)
{
	// test creators
	float4 a = make_float4(42, -1, 9, 3);
	EXPECT_VECTOR_EQ(a.x, 42.0f);
	EXPECT_VECTOR_EQ(a.y, -1.0f);
	float4 b = vector4(33.f, 17.0f, -7.0f, -2.0f);
	EXPECT_VECTOR_EQ(b, make_float4(33, 17, -7, -2));

	// test zero initializer
	float4 c = {};
	EXPECT_VECTOR_EQ(c, splat4(0.0f));

	// test =
	c = a;
	EXPECT_VECTOR_EQ(c, a);
	c = float4();
	EXPECT_VECTOR_EQ(c, splat4(0.0f));
	c = 55.0f;
	EXPECT_VECTOR_EQ(c, splat4(55.0f));
	c = b.xyzw;
	EXPECT_VECTOR_EQ(c, b);
	c.xyzw = a;
	EXPECT_VECTOR_EQ(c, a);
	c.xyzw = 55.0f;
	EXPECT_VECTOR_EQ(c, splat4(55.0f));
	c.xyzw = b.xyzw;
	EXPECT_VECTOR_EQ(c, b);
	c.xyzw.xyzw = a;
	EXPECT_VECTOR_EQ(c, a);

	// test +=
	c = a;
	c += b;
	EXPECT_VECTOR_EQ(c, a + b);
	c = a;
	c += 1.0f;
	EXPECT_VECTOR_EQ(c, a + 1.0f);
	c = a;
	c.xyzw += b;
	EXPECT_VECTOR_EQ(c, a + b);
	c = a;
	c += b.xyzw;
	EXPECT_VECTOR_EQ(c, a + b);
	c = a;
	c.xyzw += b.xyzw;
	EXPECT_VECTOR_EQ(c, a + b);
	c = a;
	c.xyzw += 1.0f;
	EXPECT_VECTOR_EQ(c, a + 1.0f);

	// test -=
	c = a;
	c -= b;
	EXPECT_VECTOR_EQ(c, a - b);
	c = a;
	c -= 1.0f;
	EXPECT_VECTOR_EQ(c, a - 1.0f);
	c = a;
	c.xyzw -= b;
	EXPECT_VECTOR_EQ(c, a - b);
	c = a;
	c -= b.xyzw;
	EXPECT_VECTOR_EQ(c, a - b);
	c = a;
	c.xyzw -= b.xyzw;
	EXPECT_VECTOR_EQ(c, a - b);
	c = a;
	c.xyzw -= 1.0f;
	EXPECT_VECTOR_EQ(c, a - 1.0f);

	// test *=
	c = a;
	c *= b;
	EXPECT_VECTOR_EQ(c, a * b);
	c = a;
	c *= 2.0f;
	EXPECT_VECTOR_EQ(c, a * 2.0f);
	c = a;
	c.xyzw *= b;
	EXPECT_VECTOR_EQ(c, a * b);
	c = a;
	c *= b.xyzw;
	EXPECT_VECTOR_EQ(c, a * b);
	c = a;
	c.xyzw *= b.xyzw;
	EXPECT_VECTOR_EQ(c, a * b);
	c = a;
	c.xyzw *= 1.0f;
	EXPECT_VECTOR_EQ(c, a * 1.0f);

	// test /=
	c = a;
	c /= b;
	EXPECT_VECTOR_EQ(c, a / b);
	c = a;
	c /= 2.0f;
	EXPECT_VECTOR_EQ(c, a / 2.0f);
	c = a;
	c.xyzw /= b;
	EXPECT_VECTOR_EQ(c, a / b);
	c = a;
	c /= b.xyzw;
	EXPECT_VECTOR_EQ(c, a / b);
	c = a;
	c.xyzw /= b.xyzw;
	EXPECT_VECTOR_EQ(c, a / b);
	c = a;
	c.xyzw /= 2.0f;
	EXPECT_VECTOR_EQ(c, a / 2.0f);

	// test index operator
	EXPECT_EQ(a[0], 42.0f);
	EXPECT_EQ(a[1], -1.0f);
	EXPECT_EQ(a[2], 9.0f);
	EXPECT_EQ(a[3], 3.0f);

	// test unary +/-
	EXPECT_VECTOR_EQ(-a, 0.0f - a);
	EXPECT_VECTOR_EQ(+a, a);
	EXPECT_VECTOR_EQ(-a.xyzw, 0.0f - a);
	EXPECT_VECTOR_EQ(+a.xyzw, a);

	// test +
	EXPECT_VECTOR_EQ(a + b, make_float4(75, 16, 2, 1));
	EXPECT_VECTOR_EQ(a + 1.0f, make_float4(43, 0, 10, 4));
	EXPECT_VECTOR_EQ(1.0f + b, make_float4(34, 18, -6, -1));
	EXPECT_VECTOR_EQ(a.xyzw + b.xyzw, a + b);
	EXPECT_VECTOR_EQ(a.xyzw + 1.0f, a + 1.0f);
	EXPECT_VECTOR_EQ(1.0f + b.xyzw, 1.0f + b);
		
	// test -
	EXPECT_VECTOR_EQ(a - b, make_float4(9, -18, 16, 5));
	EXPECT_VECTOR_EQ(a - 1.0f, make_float4(41, -2, 8, 2));
	EXPECT_VECTOR_EQ(1.0f - b, make_float4(-32, -16, 8, 3));
	EXPECT_VECTOR_EQ(a.xyzw - b.xyzw, a - b);
	EXPECT_VECTOR_EQ(a.xyzw - 1.0f, a - 1.0f);
	EXPECT_VECTOR_EQ(1.0f - b.xyzw, 1.0f - b);

	// test *
	EXPECT_VECTOR_EQ(a * b, vector4(1386.0f, -17.0f, -63.0f, -6.0f));
	EXPECT_VECTOR_EQ(a * 5.0f, vector4(210.0f, -5.0f, 45.0f, 15.0f));
	EXPECT_VECTOR_EQ(5.0f * b, vector4(165.0f, 85.0f, -35.0f, -10.0f));
	EXPECT_VECTOR_EQ(a.xyzw * b.xyzw, a * b);
	EXPECT_VECTOR_EQ(a.xyzw * 5.0f, a * 5.0f);
	EXPECT_VECTOR_EQ(5.0f * b.xyzw, 5.0f * b);

	// test /
	EXPECT_EPSILON_EQ(a / b, vector4(1.27273f, -0.0588235f, -1.285714f, -1.5f));
	EXPECT_VECTOR_EQ(a / 2.0f, vector4(21.0f, -0.5f, 4.5f, 1.5f));
	EXPECT_EPSILON_EQ(2.0f / b, vector4(0.060606f, 0.11765f, -0.28571f, -1.0f));
	EXPECT_EPSILON_EQ(a.xyzw / b.xyzw, a / b);
	EXPECT_VECTOR_EQ(a.xyzw / 2.0f, a / 2.0f);
	EXPECT_EPSILON_EQ(2.0f / b.xyzw, 2.0f / b);

	// test ==
	EXPECT_VECTOR_EQ(a == a, vector4(true, true, true, true));
	EXPECT_VECTOR_EQ(a == b, vector4(false, false, false, false));
		
	// test epsilonEquals
	EXPECT_EQ(epsilonEquals(a, b, 100.0f), true);
	EXPECT_EQ(epsilonEquals(a, b, 0.01f), false);

	// test normalize()
	c = normalize(b);
	EXPECT_EPSILON_EQ(lengthSquared(c), 1.0f);

	// test min
	EXPECT_VECTOR_EQ(min(a, b), make_float4(33, -1, -7, -2));
	EXPECT_VECTOR_EQ(min(a, 0.0f), make_float4(0, -1, 0, 0));
		
	// test max
	EXPECT_VECTOR_EQ(max(a, b), make_float4(42, 17, 9, 3));
	EXPECT_VECTOR_EQ(max(a, 0.0f), make_float4(42, 0, 9, 3));

	// test dot
	EXPECT_EQ(dot(a, b), 1300.0f);

	// distance
	EXPECT_EPSILON_EQ(distance(a, b), 26.191602f);

	// swizzle		
	EXPECT_VECTOR_EQ(a.xxxx, splat4(42.0f));
}

TEST(Math, VectorTypes)
{
	// convert from positive double with _sat
	{
		double4 a = vector4(1000.0, 100000.0, 10000000000.0, 1e100);
			
		bool4 b = a != 0.0;
		byte4 y = convert_byte4_sat(a.xyzw);
		ubyte4 uy = convert_ubyte4_sat(a);
		short4 s = convert_short4_sat(a);
		ushort4 us = convert_ushort4_sat(a);
		int4 i = convert_int4_sat(a);
		uint4 ui = convert_uint4_sat(a);
		long4 l = convert_long4_sat(a);
		ulong4 ul = convert_ulong4_sat(a);
		//float4 f = convert_float4_sat(a);

		EXPECT_VECTOR_EQ(b, splat4(true));
		EXPECT_VECTOR_EQ(y, splat4(std::numeric_limits<int8_t>::max()));
		EXPECT_VECTOR_EQ(uy, splat4(std::numeric_limits<uint8_t>::max()));
		EXPECT_VECTOR_EQ(s, vector4(int16_t(1000), std::numeric_limits<int16_t>::max(), std::numeric_limits<int16_t>::max(), std::numeric_limits<int16_t>::max()));
		EXPECT_VECTOR_EQ(us, vector4(uint16_t(1000), std::numeric_limits<uint16_t>::max(), std::numeric_limits<uint16_t>::max(), std::numeric_limits<uint16_t>::max()));
		EXPECT_VECTOR_EQ(i, vector4(int32_t(1000), int32_t(100000), std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max()));
		EXPECT_VECTOR_EQ(ui, vector4(uint32_t(1000), uint32_t(100000), std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::max()));
		EXPECT_VECTOR_EQ(l, vector4(INT64_C(1000), INT64_C(100000), INT64_C(10000000000), std::numeric_limits<int64_t>::max()));
		EXPECT_VECTOR_EQ(ul, vector4(UINT64_C(1000), UINT64_C(100000), UINT64_C(10000000000), std::numeric_limits<uint64_t>::max()));
		//EXPECT_VECTOR_EQ(f, vector4(1000.0f, 100000.0f, 10000000000.0f, std::numeric_limits<float>::max()));
	}
		
	// convet from negative double with _sat
	{
		double4 a = vector4(-1000.0, -100000.0, -10000000000.0, -1e100);
			
		bool4 b = a != 0.0;
		byte4 y = convert_byte4_sat(a.xyzw);
		ubyte4 uy = convert_ubyte4_sat(a);
		short4 s = convert_short4_sat(a);
		ushort4 us = convert_ushort4_sat(a);
		int4 i = convert_int4_sat(a);
		uint4 ui = convert_uint4_sat(a);
		long4 l = convert_long4_sat(a);
		ulong4 ul = convert_ulong4_sat(a);
		//float4 f = convert_float4_sat(a);
			
		EXPECT_VECTOR_EQ(b, splat4(true));
		EXPECT_VECTOR_EQ(y, splat4(std::numeric_limits<int8_t>::min()));
		EXPECT_VECTOR_EQ(uy, splat4(std::numeric_limits<uint8_t>::min()));
		EXPECT_VECTOR_EQ(s, make_short4(-1000, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::min()));
		EXPECT_VECTOR_EQ(us, make_ushort4(0, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::min()));
		EXPECT_VECTOR_EQ(i, make_int4(-1000, -100000, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::min()));
		EXPECT_VECTOR_EQ(ui, make_uint4(0, 0, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::min()));
		EXPECT_VECTOR_EQ(l, vector4(INT64_C(-1000), INT64_C(-100000), INT64_C(-10000000000), std::numeric_limits<int64_t>::min()));
		EXPECT_VECTOR_EQ(ul, vector4(UINT64_C(0), UINT64_C(0), UINT64_C(0), std::numeric_limits<uint64_t>::min()));
		//EXPECT_VECTOR_EQ(f, vector4(-1000.0f, -100000.0f, -10000000000.0f, -std::numeric_limits<float>::max()));
	}		
		
	// convert from float with _sat_rte
	{
		float4 f = vector4(-1.1f, 0.5f, 1.5f, 300.3f);
		
		byte4 y = convert_byte4_sat_rte(f);
		int4 i = convert_int4_sat_rte(f);
		uint4 ui = convert_uint4_sat_rte(f);
		EXPECT_VECTOR_EQ(y, make_byte4(-1, 0, 2, 127));
		EXPECT_VECTOR_EQ(i, make_int4(-1, 0, 2, 300));
		EXPECT_VECTOR_EQ(ui, make_uint4(0, 0, 2, 300));
	}
		
	// convert from double with _sat_rte
	{
		double4 d = vector4(-1.1, 0.5, 1.5, 300.3);
		
		byte4 y = convert_byte4_sat_rte(d);
		int4 i = convert_int4_sat_rte(d);
		uint4 ui = convert_uint4_sat_rte(d);
		EXPECT_VECTOR_EQ(y, make_byte4(-1, 0, 2, 127));
		EXPECT_VECTOR_EQ(i, make_int4(-1, 0, 2, 300));
		EXPECT_VECTOR_EQ(ui, make_uint4(0, 0, 2, 300));
	}		
}
	
TEST(Math, VectorUtility)
{
	float4 x = {-0.5f, 0.2f, 0.5f, 1.8f};
		
	EXPECT_VECTOR_EQ(abs(x), vector4(0.5f, 0.2f, 0.5f, 1.8f));
	//EXPECT_VECTOR_EQ(abs(x.xyzw), vector4(0.5f, 0.2f, 0.5f, 1.8f));
		
	EXPECT_VECTOR_EQ(round(x), vector4(-1.0f, 0.0f, 1.0f, 2.0f));
	EXPECT_VECTOR_EQ(round(x.xyzw), vector4(-1.0f, 0.0f, 1.0f, 2.0f));
		
}
#include <digi/Math/SwizzleUndef.inc.h>
	
TEST(Math, Matrix)
{
	// inverse of matrix2x2
	{
		float2x2 m = {
			{10.0f, 1.0f},
			{5.0f, 20.0f}};

		EXPECT_EPSILON_EQ(make_float2x2(m.x.x, m.x.y, m.y.x, m.y.y), m);
			
		float2x2 invm = {
			{ 0.1025641f, -0.0051282f},
			{-0.0256410f,  0.0512821f}};
		 
		EXPECT_EPSILON_EQ(inv(m), invm);
	}

	// inverse of matrix3x3
	{
		float3x3 m = {
			{10.0f, 1.0f, 2.0f},
			{5.0f, 20.0f, -1.0f},
			{0.0f, -3.0f, 15.0f}};
			
		float3x3 invm = {
			{ 0.1036649f, -0.0073298f, -0.0143106f},
			{-0.0261780f,  0.0523560f,  0.0069808f},
			{-0.0052356f,  0.0104712f,  0.0680628f}};
		 
		EXPECT_EPSILON_EQ(inv(m), invm);
	}
		
	// inverse of matrix4x4
	{
		float4x4 m = {
			{10.0f, 1.0f, 2.0f, 3.0f},
			{5.0f, 20.0f, -1.0f, -2.0f},
			{0.0f, -3.0f, 15.0f, -4.0f},
			{1.0f, -1.0f, 7.0f, 22.0f}};
			
		float4x4 invm = {
			{1.0518e-001f,  -7.1087e-003f, -6.9162e-003f, -1.6246e-002f},
			{-2.7013e-002f,  5.2234e-002f,  2.9027e-003f,  8.9599e-003f},
			{-6.4571e-003f,  1.0293e-002f,  6.2097e-002f,  1.3107e-002f},
			{-3.9542e-003f, -5.7758e-004f, -1.9312e-002f,  4.2430e-002f}};
		 
		EXPECT_EPSILON_EQ(inv(m), invm);
	}	
}
	
// MatrixFunctions
void helperTestProjection(float4x4 matrix, float4 cameraSpacePosition, float3 expectedNormalizedDevicePosition)
{
	/*
		camera space (view frustum, z-range is -znear to -zfar)
			|
			|<- projection matrix	
			|
		homogenous clip space (conditions: -w <= x <= w, -w <= y <= w, -w <= z <= w)
			|
			|<- perspective divide
			|
		normalized device coordinates (-1 to 1 in all dimensions)
			|
			|<- viewport transform
			|
		viewport coordinates
	*/

	// calc position of point on near clipping plane in homogenous clip space
	float4 homogenousClipSpacePosition = matrix * cameraSpacePosition;

	// calc z value in normalized device coordinates (must be -1.0)
	float3 normalizedDevicePosition = getXYZ(homogenousClipSpacePosition) / homogenousClipSpacePosition.w;
	EXPECT_EPSILON_EQ(normalizedDevicePosition, expectedNormalizedDevicePosition);
}

TEST(Math, MatrixFunctions)
{		
	// test matrix4x4Perspective
	{
		// near = 1, far = 100
		float4x4 matrix = matrix4x4Perspective(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 100.0f);
			
		// near
		helperTestProjection(matrix, make_float4(0, 0, -1, 1), make_float3(0, 0, -1));
			
		// far
		helperTestProjection(matrix, make_float4(0, 0, -100, 1), make_float3(0, 0, 1));

		// near upper left
		helperTestProjection(matrix, make_float4(1, 1, -1, 1), make_float3(1, 1, -1));
	}

	// test matrix4x4Perspective
	{
		// near = 0.01, far = 1000
		float4x4 matrix = matrix4x4Perspective(splat2(2.0f), splat2(0.0f), 0.01f, 1000.0f);

		// near
		helperTestProjection(matrix, make_float4(0, 0, -0.01f, 1), make_float3(0, 0, -1));

		// far
		helperTestProjection(matrix, make_float4(0, 0, -1000, 1), make_float3(0, 0, 1));
	}
		
	// test matrix4x4PerspectiveY
	{
		// near = 1, far = 100
		float4x4 matrix = matrix4x4PerspectiveY(45.0f, 1.0f, 1.0f, 100.0f);
			
		// near
		helperTestProjection(matrix, make_float4(0, 0, -1, 1), make_float3(0, 0, -1));
			
		// far
		helperTestProjection(matrix, make_float4(0, 0, -100, 1), make_float3(0, 0, 1));
	}
}

TEST(Math, QuaternionFunctions)
{
	// exp, log
	{
		floatQuaternion q = quaternionEulerX(1.0f);
		floatQuaternion v = log(q);
		floatQuaternion q2 = exp(v);
		EXPECT_EPSILON_EQ(q, q2);
	}
	{
		floatQuaternion q = quaternionEulerX(0.0001f);
		floatQuaternion v = log(q);
		floatQuaternion q2 = exp(v);
		EXPECT_EPSILON_EQ(q, q2);
	}

	// expUnit, logUnit
	{
		floatQuaternion q = quaternionEulerX(1.0f);
		float3 v = logUnit(q);
		floatQuaternion q2 = expUnit(v);
		EXPECT_EPSILON_EQ(q, q2);
	}
	{
		floatQuaternion q = quaternionEulerX(0.0001f);
		float3 v = logUnit(q);
		floatQuaternion q2 = expUnit(v);
		EXPECT_EPSILON_EQ(q, q2);
	}
}

TEST(Math, Intersection)
{
	// test calcHomogenousIntersection
	{
		float4x4 matrix1 = matrix4x4Perspective(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 100.0f);		
		float4x4 matrix2 = matrix4x4Perspective(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 100.0f);

		// no intersection
		EXPECT_EQ(intersectBoxBox(matrix1 * matrix4x4Translate(vector3(0.0f, 0.0f, 100.0f)) * inv(matrix2)), -1);

		// no intersection
		EXPECT_EQ(intersectBoxBox(matrix1 * matrix4x4Translate(vector3(0.0f, 0.0f, -100.0f)) * inv(matrix2)), -1);

		// intersect all but far
		EXPECT_EQ(intersectBoxBox(matrix1 * matrix4x4Translate(vector3(0.0f, 0.0f, 1.0f)) * inv(matrix2)), 0x1f);

		// intersect only far
		EXPECT_EQ(intersectBoxBox(matrix1 * matrix4x4Translate(vector3(0.0f, 0.0f, -1.0f)) * inv(matrix2)), 0x20);

		// intersect right
		EXPECT_EQ(intersectBoxBox(matrix1 * matrix4x4Translate(vector3(10.0f, 0.0f, 0.0f)) * inv(matrix2)), 0x02);
	}
}

// TransformFunctions
void helperLookAt(float3 position, float3 target, float3 up)
{
	float4x4 viewMatrix1 = matrix4x4LookAt(position, target, up);

	float3 z = normalize(position - target);
	float3 x = normalize(cross(up, z));
	float3 y = cross(z, x);
	float4x4 viewMatrix2 = inv(matrix4x4TranslateMatrix3x3(position, matrix3(x, y, z)));

	EXPECT_EPSILON_EQ(viewMatrix1, viewMatrix2);
}

void helperGetRotation(floatQuaternion q1)
{
	float3x3 m = matrix3x3Rotate(q1);
	if (q1.v.w < 0)
		q1 = -q1;
	floatQuaternion q2 = getRotation(m);
	EXPECT_EPSILON_EQ(q1, q2);
}

TEST(Math, TransformFunctions)
{
	// test matrix4x4 functions
	{
		helperLookAt(vector3(-10.0f, 0.0f, 0.0f), splat3(0.0f), vector3(0.0f, 1.0f, 0.0f));
		helperLookAt(vector3(0.0f, 0.0f, 10.0f), splat3(0.0f), vector3(0.0f, 1.0f, 0.0f));
		helperLookAt(vector3(0.0f, 0.0f, 10.0f), vector3(1.0f, 2.0f, 3.0f), vector3(0.0f, 1.0f, 0.0f));
	}

	// test matrix3x3 functions
	float3x3 m = matrix3x3TranslateRotateScale(float2(), 0.5f * pif, vector2(2.0f, 2.0f));
	float3 v = m * vector3(1.0f, 0.0f, 1.0f);
	EXPECT_EPSILON_EQ(v, vector3(0.0f, 2.0f, 1.0f));
		
	// test getRotation
	helperGetRotation(quaternionEulerXYZ(vector3(0.2f, 0.4f, 0.6f)));
	helperGetRotation(quaternionEulerXYZ(vector3(-0.2f, 0.4f, 0.6f)));
	helperGetRotation(quaternionEulerXYZ(vector3(0.2f, 0.4f, -0.6f)));

	helperGetRotation(quaternionEulerXYZ(vector3(2.2f, 0.4f, 4.6f)));
	helperGetRotation(quaternionEulerXYZ(vector3(-2.2f, 0.4f, 4.6f)));
	helperGetRotation(quaternionEulerXYZ(vector3(2.2f, 0.4f, -4.6f)));
}
	
TEST(Math, RotationFunctions)
{
	float3 angles = vector3(0.2f, 0.6f, 0.44f);

	// test angles -> quaternion -> angles for all rotation orders
	{
		floatQuaternion q = quaternionEulerZ(angles.z) * quaternionEulerY(angles.y) * quaternionEulerX(angles.x);
		EXPECT_EPSILON_EQ(q, quaternionEulerXYZ(angles));
		EXPECT_EPSILON_EQ(angles, getEulerXYZ(q));
	}
	{
		floatQuaternion q = quaternionEulerX(angles.x) * quaternionEulerZ(angles.z) * quaternionEulerY(angles.y);
		EXPECT_EPSILON_EQ(q, quaternionEulerYZX(angles));
		EXPECT_EPSILON_EQ(angles, getEulerYZX(q));
	}
	{
		floatQuaternion q = quaternionEulerY(angles.y) * quaternionEulerX(angles.x) * quaternionEulerZ(angles.z);
		EXPECT_EPSILON_EQ(q, quaternionEulerZXY(angles));
		EXPECT_EPSILON_EQ(angles, getEulerZXY(q));
	}
	{
		floatQuaternion q = quaternionEulerY(angles.y) * quaternionEulerZ(angles.z) * quaternionEulerX(angles.x);
		EXPECT_EPSILON_EQ(q, quaternionEulerXZY(angles));
		EXPECT_EPSILON_EQ(angles, getEulerXZY(q));
	}
	{
		floatQuaternion q = quaternionEulerZ(angles.z) * quaternionEulerX(angles.x) * quaternionEulerY(angles.y);
		EXPECT_EPSILON_EQ(q, quaternionEulerYXZ(angles));
		EXPECT_EPSILON_EQ(angles, getEulerYXZ(q));
	}
	{
		floatQuaternion q = quaternionEulerX(angles.x) * quaternionEulerY(angles.y) * quaternionEulerZ(angles.z);
		EXPECT_EPSILON_EQ(q, quaternionEulerZYX(angles));
		EXPECT_EPSILON_EQ(angles, getEulerZYX(q));
	}
}
	
void helperGetEuler(floatQuaternion q1)
{
	float3 angles = getEulerXYZ(q1);
	floatQuaternion q2 = quaternionEulerXYZ(angles);

	EXPECT_EPSILON_EQ(q1, q2);
}
	
TEST(Math, GetEuler)
{
	// test quaternion -> angles -> quaternion
		
	helperGetEuler(quaternion(-0.5f, -0.5f, -0.5f, 0.5f));
	helperGetEuler(quaternion(-0.68404281f, -0.069696546f, 0.014538541f, 0.72595894f));
}
		
TEST(Math, Decompose)
{
	// polar decomposition
	{
		float3x3 Q1 = matrix3x3Rotate(quaternionEulerXYZ(vector3(1.0f, 0.5f, 0.2f)));
		float3x3 S1 = matrix3x3Scale(vector3(1.0f, 2.0f, 3.0f));
			
		float3x3 Q2;
		float3x3 S2;
			
		polarDecomposition(Q1 * S1, Q2, S2);
			
		EXPECT_EPSILON_EQ(Q2, Q1);
		EXPECT_EPSILON_EQ(S2, S1);
	}
		
	// QR decomposition
	{
		floatQuaternion q = quaternionEulerXYZ(vector3(0.5f, 0.0f, 0.0f));
		float4x4 m = matrix4x4TranslateRotate(float3(), q)
			* matrix4x4TranslateShearScale(float3(), vector3(1.0f, 1.0f, 1.0f), vector3(1.0f, 1.0f, 1.0f));
				

		float3x3 Q;
		float3x3 R;
		float3x3 A = getMatrix3x3(m);
		qrDecomposition(A, Q, R);
		float3x3 QR = Q*R;
			
		EXPECT_EPSILON_EQ(A, QR);
			
		floatQuaternion q2 = getRotation(Q);
		float3 angles = getEulerXYZ(q2) * 180.0f / pif;
		floatQuaternion q3 = quaternionEulerXYZ(angles * pif / 180.0f);
			
		floatQuaternion qmaya = quaternionEulerXYZ(vector3(0.0f, 180.0f, 0.0f) * pif / 180.0f);
		float3x3 Qmaya = matrix3x3Rotate(qmaya);
	}
		
	// find eigen values of positive definite symmetrix matrix
	{
		float3x3 A = float3x3Identity();
		float3 eigenVectors = positiveSymmetricEig(A);
		EXPECT_EPSILON_EQ(eigenVectors, vector3(1.0f, 1.0f, 1.0f));
			
		A = make_float3x3(
				0,  1, -1,
				1,  1,  0,
			-1,  0,  1);
		eigenVectors = positiveSymmetricEig(A);
		EXPECT_EPSILON_EQ(eigenVectors, make_float3(2, -1, 1));
	}
		
	// find maximum eigen value and its eigen vector using power iteration
	{
		float3x3 A = make_float3x3(
				0,  1, -1,
				1,  1,  0,
			-1,  0,  1);
			
		// http://de.wikipedia.org/wiki/Potenzmethode
		float3 lengths = vector3(
			length(A.x),
			length(A.y),
			length(A.z));
/*				
		int index = argMax(lengths.x, lengths.y, lengths.z);
			
		float3 r = A[index];
		for (int i = 0; i < 20; ++i)
		{
			r = normalize(A * r);
		}
			
		float maxEigenValue = dot(r, A * r);
		EXPECT_EPSILON_EQ(maxEigenValue, 2.0f);
*/
	}
}

TEST(Math, Utility)
{
	// saturate
	{
		float v = 2.0f;
		float s = saturate(v);
		EXPECT_EQ(s, 1.0f);
	}
	{
		float3 v = {-2.0f, 0.5f, 2.0f};
		float3 s = saturate(v);
		EXPECT_VECTOR_EQ(s, vector3(0.0f, 0.5f, 1.0f));
	}
		
	// step
	{
		float v = -1.0f;
		EXPECT_EQ(step(0.0f, v), 0.0f);
	}
	{
		float2 v = {-1.0f, 1.0f};
		EXPECT_VECTOR_EQ(step(splat2(0.0f), v), vector2(0.0f, 1.0f));
		EXPECT_VECTOR_EQ(step(0.0f, v), vector2(0.0f, 1.0f));
	}
	{
		float3 v = {-1.0f, 1.0f, -2.0f};
		EXPECT_VECTOR_EQ(step(splat3(0.0f), v), vector3(0.0f, 1.0f, 0.0f));
		EXPECT_VECTOR_EQ(step(0.0f, v), vector3(0.0f, 1.0f, 0.0f));
	}
	{
		float4 v = {-1.0f, 1.0f, -2.0f, 2.0f};
		EXPECT_VECTOR_EQ(step(splat4(0.0f), v), vector4(0.0f, 1.0f, 0.0f, 1.0f));
		EXPECT_VECTOR_EQ(step(0.0f, v), vector4(0.0f, 1.0f, 0.0f, 1.0f));
	}
		
	// lerp
	EXPECT_EQ(lerp(-1.0f, 3.0f, 0.25f), 0.0f);
	EXPECT_VECTOR_EQ(lerp(make_float2(-1, 1), make_float2(3, -3), 0.25f), splat2(0.0f));
	EXPECT_VECTOR_EQ(lerp(make_double2(-1, 1), make_double2(3, -3), splat2(0.25)), splat2(0.0));
}
	
TEST(Math, Noise)
{
	float n1 = noise(1.0f);
	float n2 = noise(vector2(1.0f, 2.0f));
	float n3 = noise(vector3(2.0f, 2.5f, 2.0f));
}
	
// Operators
template <typename Type>
void helperOperators(Type v, const char* result)
{
	// write to string stream
	std::stringstream w;
	#ifdef HAVE_LOCALE
		w.imbue(std::locale::classic());
	#endif

	w << v;
	std::string str = w.str();
	EXPECT_EQ(str, result);
		
	// read from string stream
	std::stringstream r(str);
	#ifdef HAVE_LOCALE
		r.imbue(std::locale::classic());
	#endif

	Type u = {};
	r >> u;
	EXPECT_VECTOR_EQ(u, v);
}

TEST(Math, Operators)
{
	helperOperators(vector2(100.0f, -2.2f), "[100,-2.2]");
	helperOperators(vector3(100.0f, -2.2f, 0.003f), "[100,-2.2,0.003]");
	#ifdef _WIN32
		helperOperators(vector4(100.0f, -2.2f, 0.003f, -1e10f), "[100,-2.2,0.003,-1e+010]");
	#else
		helperOperators(vector4(100.0f, -2.2f, 0.003f, -1e10f), "[100,-2.2,0.003,-1e+10]");
	#endif
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
