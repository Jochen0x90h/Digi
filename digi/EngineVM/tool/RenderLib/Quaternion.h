/**
	quaternion

	these quaternion functions are provided:
	
	abs(a) absolute value
	arg(a) angle
	sqrt(a) square root of quaternion
	sqrtUnit(a) square root of unit quaternion
	exp(a) e^x for quaternion
	expUnit(a) e^x for unit quaternion (takes vector)
	log(a) ln for quaternion
	logUnit(a) ln for unit quaternion (yields vector)
	conj(a) conjugate: vector part gets negated (a + bi -> a - bi), inverse for unit quaternion
	inv(a) inverse
	normalize(a) returns a normalized quaternion
	
	
	some useful quaternion formulae:
	angular velocity: omega = log(q1 / q0) / (t1 - t0)
 */
 
struct QUATERNION;
inline QUATERNION inv(QUATERNION a);
inline QUATERNION operator *(QUATERNION a, QUATERNION b);
 
struct QUATERNION
{
	VECTOR4 v;


	// assignment
	// ------------------------

	QUATERNION& operator +=(QUATERNION b)
	{
		this->v += b.v;
		return *this;
	}

	QUATERNION& operator -=(QUATERNION b)
	{ 
		this->v -= b.v;
		return *this; 
	}

	QUATERNION& operator *=(QUATERNION b)
	{
		*this = *this * b;
		return *this;
	}

	QUATERNION& operator *=(SCALAR b)
	{ 
		this->v *= b; 
		return *this; 
	}

	QUATERNION& operator /=(QUATERNION b)
	{
		*this = *this * inv(b);
		return *this;
	}

	QUATERNION& operator /=(SCALAR b)
	{ 
		this->v /= b;
		return *this;
	}
};


// ----------------------------------------------------------------------------
// creators

inline QUATERNION quaternion(SCALAR x, SCALAR y, SCALAR z, SCALAR w)
{
	QUATERNION q = {{x, y, z, w}};
	return q;
}

inline QUATERNION quaternion(VECTOR3 v, SCALAR w)
{
	QUATERNION q = {{v.x, v.y, v.z, w}};
	return q;
}

inline QUATERNION quaternion(VECTOR4 v)
{
	QUATERNION q = {v};
	return q;
}


// ----------------------------------------------------------------------------
/// unary plus

inline QUATERNION operator +(QUATERNION a)
{
	return a;
}


// ----------------------------------------------------------------------------
/// unary minus

inline QUATERNION operator -(QUATERNION a) 
{
	return quaternion(-a.v);
}


// ----------------------------------------------------------------------------
/// binary plus

inline QUATERNION operator +(QUATERNION a, QUATERNION b) 
{
	return quaternion(a.v + b.v);
}


// ----------------------------------------------------------------------------
/// binary minus

inline QUATERNION operator -(QUATERNION a, QUATERNION b) 
{
	return quaternion(a.v - b.v);
}


// ----------------------------------------------------------------------------
/// multiply

inline QUATERNION operator *(QUATERNION a, SCALAR b) 
{
	return quaternion(a.v * b);
}

inline QUATERNION operator *(SCALAR a, QUATERNION b) 
{
	return quaternion(a * b.v);
}


// ----------------------------------------------------------------------------
/// quaternion multiply

inline QUATERNION operator *(QUATERNION a, QUATERNION b)
{
	return quaternion(
		a.v.w * b.v.x + b.v.w * a.v.x + a.v.y * b.v.z - a.v.z * b.v.y,
		a.v.w * b.v.y + b.v.w * a.v.y + a.v.z * b.v.x - a.v.x * b.v.z,
		a.v.w * b.v.z + b.v.w * a.v.z + a.v.x * b.v.y - a.v.y * b.v.x,
		a.v.w * b.v.w - a.v.x * b.v.x - a.v.y * b.v.y - a.v.z * b.v.z);
}


// ----------------------------------------------------------------------------
/// divide

inline QUATERNION operator /(QUATERNION a, SCALAR b) 
{
	return quaternion(a.v / b);
}


// ----------------------------------------------------------------------------
/// quaternion divide

inline QUATERNION operator /(QUATERNION a, QUATERNION b) 
{
	return a * inv(b);
}


// ----------------------------------------------------------------------------
/// abs(a)

/// returns the absolute value of the quaternion (same as vector length)
inline SCALAR abs(QUATERNION a)
{
	return sqrt(a.v.x * a.v.x + a.v.y * a.v.y + a.v.z * a.v.z + a.v.w * a.v.w);
}


// ----------------------------------------------------------------------------
/// arg(a)

/// returns the generalized arg for quaternion
inline VECTOR3 arg(QUATERNION a)
{
	SCALAR cosTheta = a.v.w;
	SCALAR sinTheta = sqrt(a.v.x * a.v.x + a.v.y * a.v.y + a.v.z * a.v.z);
	SCALAR theta = atan2(sinTheta, cosTheta);

	// if theta close to zero use s = 2 / cosTheta
	SCALAR s = theta > 1e-6f ? 2.0f * theta / sinTheta : 2.0f / cosTheta;

	return vector3(a.v.x * s, a.v.y * s, a.v.z * s);
}


// ----------------------------------------------------------------------------
/// sqrt(a)

/// square root of a quaternion, Q = sqrt(Q) * sqrt(Q)
inline QUATERNION sqrt(QUATERNION a) 
{ 
	// we calculate the halfway interpolation from a to b with b = (0 0 0 length(a))
	// the length of the resulting quaternion should be sqrt(length(a))
	// this can be done calculating ((a + b)/2) * sqrt(length(a)) / length((a + b)/2)
	// length((a + b)/2) / sqrt(length(a) is sqrt(0.5 * (a.w + length(a)))
	SCALAR len = sqrt(0.5f * (a.v.w + length(a.v)));
	SCALAR s = 0.5f / len;
	return quaternion(
		a.v.x * s, // (a.x * 0.5 + 0 * 0.5) / len = a.x * s
		a.v.y * s,
		a.v.z * s,
		len); // (a.w * 0.5 + length(a) * 0.5) / len = sqrt(a.w * 0.5 + length(a) * 0.5) = len
}

/// optimized square root only valid for a unit quaternion 
/// this is equivalent to a "half-angle" operation
inline QUATERNION sqrtUnit(QUATERNION a) 
{ 
	// we calculate the halfway interpolation from a to b with b = (0 0 0 1)
	// this can be done calculating ((a + b)/2) / length((a + b)/2)
	// length((a + b)/2) is sqrt(0.5 * (a.w + 1))
	SCALAR len = sqrt(0.5f * (a.v.w + 1.0f));
	SCALAR s = 0.5f / len;
	return quaternion(
		a.v.x * s, // (a.x * 0.5 + 0 * 0.5) / len = a.x * s
		a.v.y * s,
		a.v.z * s,
		len); // (a.w * 0.5 + 1 * 0.5) / len = sqrt(a.w * 0.5 + 1 * 0.5) = len
}


// ----------------------------------------------------------------------------
/// exp(a)

/// quaternion exponential
/// The input to this function has the "format" a = (logscale, phi * i, phi * j, phi * k) where 
/// logscale is the logarithm of the uniform scale, phi is the rotation angle and i,j,k is
/// the rotation axis.
inline QUATERNION exp(QUATERNION a) 
{ 
	SCALAR r, s;
	SCALAR phi2 = a.v.x * a.v.x + a.v.y * a.v.y + a.v.z * a.v.z; // phi^2
	  
	SCALAR m = exp(a.v.w);

	if (phi2 > 1e-6f)
	{
		// normal operation
		SCALAR phi = sqrt(phi2);
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
inline QUATERNION expUnit(VECTOR3 a) 
{ 
	SCALAR r, s;
	SCALAR phi2 = a.x * a.x + a.y * a.y + a.z * a.z; // phi^2
	  
	if (phi2 > 1e-6f)
	{
		// normal operation
		SCALAR phi = sqrt(phi2);
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
/// log(a)

/// quaternion logarithm, a = exp(log(a))
/// The output of this function has the "format" a = (logscale/2, phi/2 * i, phi/2 * j, phi/2 * k) 
/// where logscale is the logarithm of the uniform scale, phi is the rotation angle and i,j,k 
/// is the rotation axis.
inline QUATERNION log(QUATERNION a) 
{ 
	SCALAR v2 = a.v.x * a.v.x + a.v.y * a.v.y + a.v.z * a.v.z;
	SCALAR w2 = a.v.w * a.v.w;
	SCALAR s;
	  
	if (v2 > 1e-6f)
	{
		// normal operation
		SCALAR v = sqrt(v2);
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
inline VECTOR3 logUnit(QUATERNION a) 
{ 
	SCALAR v2 = a.v.x * a.v.x + a.v.y * a.v.y + a.v.z * a.v.z;
	SCALAR m2 = a.v.w * a.v.w;
	SCALAR s;
	  
	if (v2 > 1e-6f)
	{
		// normal operation
		SCALAR v = sqrt(v2);
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
/// conj(a)

/// conjugate
inline QUATERNION conj(QUATERNION a) 
{
	return quaternion(-a.v.x, -a.v.y, -a.v.z, a.v.w);
}


// ----------------------------------------------------------------------------
/// inv(a)

/// quaternion inverse inv(a) = 1 / a = conj(a) / (a * conj(a))
inline QUATERNION inv(QUATERNION a)
{ 
	SCALAR q = 1.0f / (a.v.x * a.v.x + a.v.y * a.v.y + a.v.z * a.v.z + a.v.w * a.v.w);
	return quaternion(-a.v.x * q, -a.v.y * q, -a.v.z * q, a.v.w * q);
}


// ----------------------------------------------------------------------------
/// normalize(a)

/// returns a normalized quaternion that lies on the unit sphere
inline QUATERNION normalize(QUATERNION a)
{
	SCALAR q = 1.0f / abs(a);
	return quaternion(a.v.x * q, a.v.y * q, a.v.z * q, a.v.w * q);
}
