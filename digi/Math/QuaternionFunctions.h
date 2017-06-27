#ifndef digi_Math_QuaternionFunctions_h
#define digi_Math_QuaternionFunctions_h

#include "Quaternion.h"


namespace digi {

/// @addtogroup Math
/// @{

/**
	abs(a) absolute value
	arg(a) angle
	sqrt(a) square root of quaternion
	sqrtUnit(a) square root of unit quaternion
	exp(a) e^x for quaternion
	expUnit(a) e^x for unit quaternion (takes vector)
	log(a) ln for quaternion
	logUnit(a) ln for unit quaternion (yields vector)
	conj(a) conjugate: vector part gets negated (a + bi -> a - bi), inverse for unit quaternion
	dot(a, b) dot product: conj(a) * b
	inv(a) inverse
	normalize(a) returns a normalized quaternion
	
	slerp(a, b, s)
	slerpShortest(a, b, s)
	squad(a, b, c, d, s)

	some useful quaternion formulae:
	angular velocity: omega = log(q1 / q0) / (t1 - t0)
*/


// ----------------------------------------------------------------------------
// abs(a)

/// returns the absolute value of the quaternion (same as vector length)
template <typename Type>
Type abs(const Quaternion<Type>& a)
{
	return sqrt(a.v.x * a.v.x + a.v.y * a.v.y + a.v.z * a.v.z + a.v.w * a.v.w);
}


// ----------------------------------------------------------------------------
// arg(a)

/// returns the generalized arg for quaternion
template <typename Type> 
Vector3<Type> arg(const Quaternion<Type>& a)
{
	Type cosTheta = a.v.w;
	Type sinTheta = sqrt(a.v.x * a.v.x + a.v.y * a.v.y + a.v.z * a.v.z);
	Type theta = atan2(sinTheta, cosTheta);

	// if theta close to zero use s = 2 / cosTheta
	Type s = theta > 1e-6f ? 2.0f * theta / sinTheta : 2.0f / cosTheta;

	return vector3(a.v.x * s, a.v.y * s, a.v.z * s);
}


// ----------------------------------------------------------------------------
// sqrt(a)

/// square root of a quaternion, Q = sqrt(Q) * sqrt(Q)
template <typename Type>
Quaternion<Type> sqrt(const Quaternion<Type>& a) 
{ 
  // we calculate the halfway interpolation from a to b with b = (0 0 0 length(a))
  // the length of the resulting quaternion should be sqrt(length(a))
  // this can be done calculating ((a + b)/2) * sqrt(length(a)) / length((a + b)/2)
  // length((a + b)/2) / sqrt(length(a) is sqrt(0.5 * (a.w + length(a)))
  Type len = sqrt(0.5f * (a.v.w + length(a.v)));
  Type s = 0.5f / len;
  return quaternion(
    a.v.x * s, // (a.x * 0.5 + 0 * 0.5) / len = a.x * s
    a.v.y * s,
    a.v.z * s,
    len); // (a.w * 0.5 + length(a) * 0.5) / len = sqrt(a.w * 0.5 + length(a) * 0.5) = len
}

/// optimized square root only valid for a unit quaternion 
/// this is equivalent to a "half-angle" operation
template <typename Type>
Quaternion<Type> sqrtUnit(const Quaternion<Type>& a) 
{ 
  // we calculate the halfway interpolation from a to b with b = (0 0 0 1)
  // this can be done calculating ((a + b)/2) / length((a + b)/2)
  // length((a + b)/2) is sqrt(0.5 * (a.w + 1))
  Type len = sqrt(0.5f * (a.v.w + 1.0f));
  Type s = 0.5f / len;
  return quaternion(
    a.v.x * s, // (a.x * 0.5 + 0 * 0.5) / len = a.x * s
    a.v.y * s,
    a.v.z * s,
    len); // (a.w * 0.5 + 1 * 0.5) / len = sqrt(a.w * 0.5 + 1 * 0.5) = len
}


// ----------------------------------------------------------------------------
// exp(a)

/// quaternion exponential
/// The input to this function has the "format" a = (logscale, phi * i, phi * j, phi * k) where 
/// logscale is the logarithm of the uniform scale, phi is the rotation angle and i,j,k is
/// the rotation axis.
template <typename Type>
Quaternion<Type> exp(const Quaternion<Type>& a) 
{ 
	Type r, s;
	Type phi2 = a.v.x * a.v.x + a.v.y * a.v.y + a.v.z * a.v.z; // phi^2
	  
	Type m = exp(a.v.w);

	if (phi2 > 1e-6f)
	{
		// normal operation
		Type phi = sqrt(phi2);
		r = m * cos(phi);
		s = m * sin(phi) / phi;
	}
	else
	{
		// avoid division by phi == 0,
		// use low oder taylor series approximation
		// cos(phi) = 1 - (1/2) * phi^2
		// sin(phi) / phi == 1 - (1/6) * phi^2
		r = m * (1.0f - 0.5f * phi2);
		s = m * (1.0f - 0.166667f * phi2);
	}
	return quaternion(
		s * a.v.x,
		s * a.v.y,
		s * a.v.z,
		r);
}

/// optimized version of the quaternion exponential which ignores a.w and results
/// always in a unit quaternion
template <typename Type>
Quaternion<Type> expUnit(const Vector3<Type>& a) 
{ 
	Type r, s;
	Type phi2 = a.x * a.x + a.y * a.y + a.z * a.z; // phi^2, square of rotation angle
	  
	if (phi2 > 1e-6f)
	{
		// normal operation
		Type phi = sqrt(phi2);
		r = cos(phi);
		s = sin(phi) / phi;
	}
	else
	{
		// avoid division by phi == 0,
		// use low oder taylor series approximation
		// cos(phi) = 1 - (1/2) * phi^2
		// sin(phi) / phi == 1 - (1/6) * phi^2
		r = 1.0f - 0.5f * phi2;
		s = 1.0f - 0.166667f * phi2;
	}
	return quaternion(
		s * a.x,
		s * a.y,
		s * a.z,
		r);
}


// ----------------------------------------------------------------------------
// log(a)

/// quaternion logarithm, a = exp(log(a))
/// The output of this function has the "format" a = (logscale/2, phi/2 * i, phi/2 * j, phi/2 * k) 
/// where logscale is the logarithm of the uniform scale, phi is the rotation angle and i,j,k 
/// is the rotation axis.
template <typename Type>
Quaternion<Type> log(const Quaternion<Type>& a) 
{ 
	Type v2 = a.v.x * a.v.x + a.v.y * a.v.y + a.v.z * a.v.z;
	Type w2 = a.v.w * a.v.w;
	Type s;
	  
	if (v2 > 1e-6f)
	{
		// normal operation
		Type v = sqrt(v2);
		s = atan2(v, a.v.w) / v;
	}
	else
	{
		// avoid division by v == 0
		// use low order taylor approximation of 
		// atan2(v, 1) / v == 1 - (1/3) * v^2
		s = 1.0f - 0.333333f * v2;
	}

	return quaternion(
		s * a.v.x,
		s * a.v.y,
		s * a.v.z,
		0.5f * log(w2 + v2));
}

/// optimized quaternion logarithm only valid for unit quaternions
/// the result is a pure quaternion
template <typename Type>
Vector3<Type> logUnit(const Quaternion<Type>& a) 
{ 
	Type v2 = a.v.x * a.v.x + a.v.y * a.v.y + a.v.z * a.v.z;
	Type m2 = a.v.w * a.v.w;
	Type s;
	  
	if (v2 > 1e-6f)
	{
		// normal operation
		Type v = sqrt(v2);
		s = atan2(v, a.v.w) / v;
	}
	else
	{
		// avoid division by v == 0
		// use low order taylor approximation of 
		// atan2(v, 1) / v == 1 - (1/3) * v^2
		s = 1.0f - 0.333333f * v2;
	}

	return vector3(
		s * a.v.x,
		s * a.v.y,
		s * a.v.z);
}


// ----------------------------------------------------------------------------
// conj(a)

/// conjugate
template <typename Type>
const Quaternion<Type> conj(const Quaternion<Type>& a) 
{
	return quaternion(-a.v.x, -a.v.y, -a.v.z, a.v.w);
}


// ----------------------------------------------------------------------------
// dot(a, b)

template <typename Type>
const Quaternion<Type> dot(const Quaternion<Type>& a, const Quaternion<Type>& b)
{
	return conj(a) * b;
}


// ----------------------------------------------------------------------------
// inv(a)

/// quaternion inverse inv(a) = 1 / a = conj(a) / (a * conj(a))
template <typename Type>
Quaternion<Type> inv(const Quaternion<Type>& a)
{ 
	Type q = 1.0f / (a.v.x * a.v.x + a.v.y * a.v.y + a.v.z * a.v.z + a.v.w * a.v.w);
	return quaternion(-a.v.x * q, -a.v.y * q, -a.v.z * q, a.v.w * q);
}


// ----------------------------------------------------------------------------
// normalize(a)

/// returns a normalized quaternion that lies on the unit sphere
template <typename Type>
Quaternion<Type> normalize(const Quaternion<Type>& a)
{
	Type q = 1.0f / abs(a);
	return quaternion(a.v.x * q, a.v.y * q, a.v.z * q, a.v.w * q);
}


// ----------------------------------------------------------------------------
// slerp(a, b, s)

/// interpolates from a to b using "spherical linear interpolation" (slerp)
template <typename Type> 
Quaternion<Type> slerp(const Quaternion<Type>& a, const Quaternion<Type>& b, Type s)
{
	// difference rotation from a to b. the rotation angle is theta
	Quaternion<Type> d = conj(a) * b;

	Type cosTheta = d.v.w;
	Type sinTheta = sqrt(d.v.x*d.v.x + d.v.y*d.v.y + d.v.z*d.v.z);

	Type u;
	Type v;
	if (sinTheta <= 1e-6f)//Limits<Type>::EPSILON) 
	{
		// rotation angle theta too small
		u = 1.0f - s;
		v = s;
	} 
	else 
	{
		Type theta = atan2(sinTheta, cosTheta);
		u = sin((1.0f - s) * theta) / sinTheta;
		v = sin(s * theta) / sinTheta;
	}
	return quaternion(
		a.v.x*u + b.v.x*v,
		a.v.y*u + b.v.y*v,
		a.v.z*u + b.v.z*v,
		a.v.w*u + b.v.w*v);
}


// ----------------------------------------------------------------------------
// slerpShortest(a, b, s)

template <typename Type> 
Quaternion<Type> slerpShortest(const Quaternion<Type>& a, const Quaternion<Type>& b, Type s)
{
	// make sure we interpolate along the acute angle and not the obtuse one
	return slerp(a, dot(a.v, b.v) >= 0.0f ? b : -b, s);
}


// ----------------------------------------------------------------------------
// squad(a, b, c, d, s)

/// does spherical cubic interpolation using a and d as start/end and b and c as control points.
/// the spline shape is equivalent to a bezier spline
template <typename Type> 
Quaternion<Type> squad(const Quaternion<Type>& a, const Quaternion<Type>& b,
	const Quaternion<Type>& c, const Quaternion<Type>& d, Type s)
{
	return slerp(slerp(a, d, s), slerp(b, c, s), Type(3) * s * (Type(1) - s));
}

/// @}

} // namespace digi

#endif
