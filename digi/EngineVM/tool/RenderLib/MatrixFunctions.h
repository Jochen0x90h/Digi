/**
	matrix functions are applied column-wise
	
	abs(a) absolute value
	sgn(a) sign
	sqrt(a) square root
	sqr(a) square
	floor(a)
	ceil(a)
	round(a)
	frac(a) fractional part
	sin(a)
	cos(a)
	exp(a)
	log(a) logarithm base e
	log10(a) logarithm base 10
	log2(a) logarithm base 2
	conj(a) complex conjugate
	min(a)
	max(a)
	sum(a)
	min(a, b)
	max(a, b)
	mod(a, b)
	dot(a, b)

	operator *(matrix, vector) matrix vector multiplication
	operator *(vector, matrix) vector matrix multiplication
	operator *(matrix, matrix) matrix matrix multiplication
*/

	
// ----------------------------------------------------------------------------
/// abs(a)

// matrix?x2
inline MATRIX2 abs(MATRIX2 a)
{
	return matrix2(
		abs(a.x),
		abs(a.y));
}

// matrix?x3
inline MATRIX3 abs(MATRIX3 a)
{
	return matrix3(
		abs(a.x),
		abs(a.y),
		abs(a.z));
}

// matrix?x4
inline MATRIX4 abs(MATRIX4 a)
{
	return matrix4(
		abs(a.x),
		abs(a.y),
		abs(a.z),
		abs(a.w));
}


// ----------------------------------------------------------------------------
/// sgn(a)


// ----------------------------------------------------------------------------
/// sqrt(a)


// ----------------------------------------------------------------------------
/// sqr(a)


// ----------------------------------------------------------------------------
/// floor(a)


// ----------------------------------------------------------------------------
/// ceil(a)


// ----------------------------------------------------------------------------
/// round(a)


// ----------------------------------------------------------------------------
/// frac(a)


// ----------------------------------------------------------------------------
// sin(a)


// ----------------------------------------------------------------------------
// cos(a)


// ----------------------------------------------------------------------------
// exp(a)


// ----------------------------------------------------------------------------
// log(a)


// ----------------------------------------------------------------------------
// log10(a)


// ----------------------------------------------------------------------------
// log2(a)


// ----------------------------------------------------------------------------
/// conj(a)


// ----------------------------------------------------------------------------
/// min(a)

// matrix?x2
inline VECTOR2 min(MATRIX2 a)
{
	return vector2(
		min(a.x),
		min(a.y));
}

// matrix?x3
inline VECTOR3 min(MATRIX3 a)
{
	return vector3(
		min(a.x),
		min(a.y),
		min(a.z));
}

// matrix?x4
inline VECTOR4 min(MATRIX4 a)
{
	return vector4(
		min(a.x),
		min(a.y),
		min(a.z),
		min(a.w));
}


// ----------------------------------------------------------------------------
/// max(a)

// matrix?x2
inline VECTOR2 max(MATRIX2 a)
{
	return vector2(
		max(a.x),
		max(a.y));
}

// matrix?x3
inline VECTOR3 max(MATRIX3 a)
{
	return vector3(
		max(a.x),
		max(a.y),
		max(a.z));
}

// matrix?x4
inline VECTOR4 max(MATRIX4 a)
{
	return vector4(
		max(a.x),
		max(a.y),
		max(a.z),
		max(a.w));
}


// ----------------------------------------------------------------------------
/// sum(a)

// matrix?x2
inline VECTOR2 sum(MATRIX2 a)
{
	return vector2(
		sum(a.x),
		sum(a.y));
}

// matrix?x3
inline VECTOR3 sum(MATRIX3 a)
{
	return vector3(
		sum(a.x),
		sum(a.y),
		sum(a.z));
}

// matrix?x4
inline VECTOR4 sum(MATRIX4 a)
{
	return vector4(
		sum(a.x),
		sum(a.y),
		sum(a.z),
		sum(a.w));
}


// ----------------------------------------------------------------------------
// min(a, b)



// ----------------------------------------------------------------------------
// max(a, b)



// ----------------------------------------------------------------------------
// mod(a, b)



// ----------------------------------------------------------------------------
// dot(a, b)

// matrix?x2
inline VECTOR2 dot(MATRIX2 a, MATRIX2 b)
{
	return vector2(
		dot(a.x, b.x),
		dot(a.y, b.y));
}

// matrix?x3
inline VECTOR3 dot(MATRIX3 a, MATRIX3 b)
{
	return vector3(
		dot(a.x, b.x),
		dot(a.y, b.y),
		dot(a.z, b.z));
}

// matrix?x4
inline VECTOR4 dot(MATRIX4 a, MATRIX4 b)
{
	return vector4(
		dot(a.x, b.x),
		dot(a.y, b.y),
		dot(a.z, b.z),
		dot(a.w, b.w));
}


// ----------------------------------------------------------------------------
// matrix vector multiplication

// vector? = matrix?x2 * vector2
inline VECTOR operator *(MATRIX2 a, VECTOR2 b)
{
	return a.x * b.x + a.y * b.y;
}

// vector? = matrix?x3 * vector3
inline VECTOR operator *(MATRIX3 a, VECTOR3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

// vector? = matrix?x4 * vector4
inline VECTOR operator *(MATRIX4 a, VECTOR4 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}


// ----------------------------------------------------------------------------
// vector matrix multiplication

// vector2 = vector? * matrix?x2
inline VECTOR2 operator *(VECTOR a, MATRIX2 b)
{
	return vector2(
		mac(a, b.x),
		mac(a, b.y));
}

// vector3 = vector? * matrix?x3
inline VECTOR3 operator *(VECTOR a, MATRIX3 b)
{
	return vector3(
		mac(a, b.x),
		mac(a, b.y),
		mac(a, b.z));
}

// vector4 = vector? * matrix?x4
inline VECTOR4 operator *(VECTOR a, MATRIX4 b)
{
	return vector4(
		mac(a, b.x),
		mac(a, b.y),
		mac(a, b.z),
		mac(a, b.w));
}


// ----------------------------------------------------------------------------
// matrix matrix multiplication

// matrix?x2 = matrix?x2 * matrix2x2
inline MATRIX2 operator *(MATRIX2 a, MATRIX2X2 b)
{
	return matrix2(
		a.x * b.x.x + a.y * b.x.y,
		a.x * b.y.x + a.y * b.y.y);
}

// matrix?x3 = matrix?x3 * matrix3x3
inline MATRIX3 operator *(MATRIX3 a, MATRIX3X3 b)
{
	return matrix3(
		a.x * b.x.x + a.y * b.x.y + a.z * b.x.z,
		a.x * b.y.x + a.y * b.y.y + a.z * b.y.z,
		a.x * b.z.x + a.y * b.z.y + a.z * b.z.z);
}

// matrix?x4 = matrix?x4 * matrix4x4
inline MATRIX4 operator *(MATRIX4 a, MATRIX4X4 b)
{
	return matrix4(
		a.x * b.x.x + a.y * b.x.y + a.z * b.x.z + a.w * b.x.w,
		a.x * b.y.x + a.y * b.y.y + a.z * b.y.z + a.w * b.y.w,
		a.x * b.z.x + a.y * b.z.y + a.z * b.z.z + a.w * b.z.w,
		a.x * b.w.x + a.y * b.w.y + a.z * b.w.z + a.w * b.w.w);
}
