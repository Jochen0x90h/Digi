#ifndef digi_Math_Utility_h
#define digi_Math_Utility_h

#include "Vector4.h"


namespace digi {

/// @addtogroup Math
/// @{


/*
	this file contains some utility functions:

	clamp(x, minValue, maxValue)	
	saturate(x) clamp(x, 0, 1)

	step(edge, x)
	linstep(start, end, x)
	smoothstep(start, end, x)

	lerp(p0, p1, a)
	hermite(p0, t0, t1, p1, a)
	hermiteTangent(p0, t0, t1, p1, a)
	bezier(p0, p1, p2, p3, a)
	catmullRom(p0, p1, p2, p3, a)
	barycentric(p0, p1, p2, a, b)

	norm1(a)
	normInf(a)

	epsilonEquals(a, b, epsilon)
*/

// ----------------------------------------------------------------------------
// clamp(x, minValue, maxValue)

template <typename Type>
Type clamp(const Type& x, const Type& minValue, const Type& maxValue)
{
	return min(max(x, minValue), maxValue);
}

// vector2
template <typename Type>
Vector2<Type> clamp(const Vector2<Type>& x, const Type& minValue, const Type& maxValue)
{
	return min(max(x, minValue), maxValue);
}

// vector3
template <typename Type>
Vector3<Type> clamp(const Vector3<Type>& x, const Type& minValue, const Type& maxValue)
{
	return min(max(x, minValue), maxValue);
}

// vector4
template <typename Type>
Vector4<Type> clamp(const Vector4<Type>& x, const Type& minValue, const Type& maxValue)
{
	return min(max(x, minValue), maxValue);
}


// ----------------------------------------------------------------------------
// saturate(x)

template <typename Type>
inline Type saturate(const Type& x)
{
	return clamp(x, DIGI_ELEMENT(Type)(0), DIGI_ELEMENT(Type)(1));
}


// ----------------------------------------------------------------------------
// step(edge, x)

/// 0.0 if x < edge, else 1.0
template <typename Type>
Type step(const Type& edge, const Type& x)
{
	Type zeros = Type();
	Type ones;
	ones = DIGI_ELEMENT(Type)(1);
	return select(ones, zeros, x < edge);
}

// vector2
template <typename Type>
Vector2<Type> step(const Type& edge, const Vector2<Type>& x)
{
	Vector2<Type> zeros = splat2(Type(0));
	Vector2<Type> ones = splat2(Type(1));
	return select(ones, zeros, x < edge);
}

// vector3
template <typename Type>
Vector3<Type> step(const Type& edge, const Vector3<Type>& x)
{
	Vector3<Type> zeros = splat3(Type(0));
	Vector3<Type> ones = splat3(Type(1));
	return select(ones, zeros, x < edge);
}

// vector4
template <typename Type>
Vector4<Type> step(const Type& edge, const Vector4<Type>& x)
{
	Vector4<Type> zeros = splat4(Type(0));
	Vector4<Type> ones = splat4(Type(1));
	return select(ones, zeros, x < edge);
}


// ----------------------------------------------------------------------------
// linstep(start, end, x)

/// For values of x between start and end, returns a linearly interpolated value that
/// ranges from 0 at x = start to 1 at x = end.
template <typename Type>
Type linstep(const Type& start, const Type& end, const Type& x)
{
	Type cx = clamp(x, start, end);
	Type d = (cx - start) / (end - start);
	return d;
}


// ----------------------------------------------------------------------------
// smoothstep(start, end, x)

/// For values of x between start and end, returns a smoothly varying value that
/// ranges from 0 at x = start to 1 at x = end.
template <typename Type>
Type smoothstep(const Type& start, const Type& end, const Type& x)
{
	Type cx = clamp(x, start, end);
	Type d = (cx - start) / (end - start);
	return (Type(-2) * d + Type(3)) * d * d;
}


// ----------------------------------------------------------------------------
// lerp(p0, p1, a)

/// linearly interpolates from p0 to p1
/// lerp(scalar, scalar, scalar)
/// lerp(vector, vector, scalar)
/// lerp(vector, vector, vector)
template <typename Type1, typename Type2>
Type1 lerp(const Type1& p0, const Type1& p1, const Type2& a)
{
	return p0 + a * (p1 - p0);
}


// ----------------------------------------------------------------------------
// hermite(p0, t0, p1, t1, a)

/// the hermite spline Q(a) has these properties:
/// Q(a) is a cubic function. 
/// Q(a) interpolates between p0 and p1 as a ranges from 0 to 1. 
/// Q(0) = p0
/// Q'(0) = t0
/// Q(1) = p1
/// Q'(1) = t1
template <typename Type>
Type hermite(const Type& p0, const Type& t0, const Type& t1, const Type& p1, DIGI_BASE(Type) a)
{
	DIGI_BASE(Type) a2 = a * a;
	DIGI_BASE(Type) a3 = a2 * a;

	return
		+ p0 * (2.0f * a3 - 3.0f * a2 + 1.0f)
		+ t0 * (a3 - 2.0f * a2 + a)
		+ p1 * (-2.0f * a3 + 3.0f * a2)
		+ t1 * (a3 - a2);
}


// ----------------------------------------------------------------------------
// hermiteTangent(p0, t0, p1, t1, a)

/// returns the tangent of the hermite function
template <typename Type>
Type hermiteTangent(const Type& p0, const Type& t0, const Type& t1, const Type& p1, DIGI_BASE(Type) a)
{
	DIGI_BASE(Type) a2 = a * a;

	return
		+ p0 * ((a2 - a) * 6.0f)
		+ t0 * (3.0f * a2 - 4.0f * a + 1.0f)
		+ p1 * ((-a2 + a) * 6.0f)
		+ t1 * (3.0f * a2 - 2.0f * a);
}


// ----------------------------------------------------------------------------
// bezier(p0, p1, p2, p3, a)

/// the bezier spline Q(a) has these properties:
/// Q(a) is a cubic function. 
/// Q(a) interpolates between p1 and p4 as a ranges from 0 to 1. 
///
/// it can be converted to a hermite spline using
/// p0 = p0
/// p1 = p3
/// t0 = (p1 - p0) * 3
/// t1 = (p3 - p2) * 3
template <typename Type>
Type bezier(const Type& p0, const Type& p1, const Type& p2, const Type& p3, DIGI_BASE(Type) a)
{
	DIGI_BASE(Type) a2 = a * a;
	DIGI_BASE(Type) a3 = a2 * a;

	return
		+ p0 * (-a3 + 3.0f * (a2 - a) + 1.0f)
		+ p1 * ((a3 - 2.0f * a2 + a) * 3.0f)
		+ p2 * ((-a3 + a2) * 3.0f)
		+ p3 * (a3);
}


// ----------------------------------------------------------------------------
// catmullRom(p0, p1, p2, p3, a)

/// the catmull rom spline Q(a) has these properties:
/// Q(a) is a cubic function. 
/// Q(a) interpolates between p2 and p3 as a ranges from 0 to 1. 
/// Q'(a) is parallel to the line joining p1 to p3 when a is 0. 
/// Q'(a) is parallel to the line joining p2 to p4 when a is 1
///
/// it can be converted to a hermite spline using
/// p0 = p1
/// p1 = p2
/// t0 = (p2 - p0) / 2
/// t1 = (p3 - p1) / 2
template <typename Type>
Type catmullRom(const Type& p0, const Type& p1, const Type& p2, const Type& p3, DIGI_BASE(Type) a)
{
	DIGI_BASE(Type) a2 = a * a;
	DIGI_BASE(Type) a3 = a2 * a;

	return
		+ p0 * ((-a3 + 2.0f * a2 - a) * 0.5f)
		+ p1 * ((3.0f * a3 - 5.0f * a2 + 2.0f) * 0.5f)
		+ p2 * ((-3.0f * a3 + 4.0f * a2 + a) * 0.5f)
		+ p3 * ((a3 - a2) * 0.5f);
}


// ----------------------------------------------------------------------------
// barycentric(p0, p1, p2, a, b)

/// calculates a point from three points using barycentric coordinates
/// it calculates p0*(1 - a - b) + p1*a + p2*b
/// if (a >= 0 && b >= 0 && 1 - a - b >= 0), the point is inside the triangle p0,p1,p2
template <typename Type>
Type barycentric(const Type& p0, const Type& p1, const Type& p2, DIGI_BASE(Type) a, DIGI_BASE(Type) b)
{
	return
		+ p0 * (1.0f - a - b)
		+ p1 * (a)
		+ p2 * (b);
}


// ----------------------------------------------------------------------------
// norm1(a)

/// calculates the 1-norm.
/// for vectors it is the sum of the absolute values.
/// for matrices it is the maximum of the 1-norm for each column
template <typename Type>
inline DIGI_ELEMENT(DIGI_ELEMENT(DIGI_REAL(Type))) norm1(const Type& a)
{
	return max(sum(abs(a)));
}


// ----------------------------------------------------------------------------
// normInf(a)

/// calculates the infinity-norm.
/// for vectors it is the maximum of the absolute values.
/// for matrices it is the sum of the infinity-norm for each column
template <typename Type>
inline DIGI_ELEMENT(DIGI_ELEMENT(DIGI_REAL(Type))) normInf(const Type& a)
{
	return sum(max(abs(a)));
}


// ----------------------------------------------------------------------------
// epsilonEquals(a, b, epsilon)

template <typename Type>
bool epsilonEquals(const Type& a, const Type& b, DIGI_BASE(Type) epsilon)
{
	Type delta = a - b;
	//return sum(abs(dot(delta, delta))) <= epsilon * epsilon;
	return max(max(abs(delta))) <= max(max(abs(a))) * epsilon;
}


/// @}

} // namespace digi

#endif
