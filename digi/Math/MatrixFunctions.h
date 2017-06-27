#ifndef digi_Math_MatrixFunctions_h
#define digi_Math_MatrixFunctions_h


namespace digi {

/// @addtogroup Math
/// @{


/**
	matrix functions are applied column-wise
	(e.g. sum(a) of a 4x4 matrix returns a vector containing the sums of the columns)

	abs(a) absolute value
	sign(a) sign
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

	diag(vector) diagonal matrix with given diagonal elements
	diag(matrix) vector of diagonal elements
	trace(matrix)
	det(matrix)
	inv(matrix)
	transpose(matrix) transpose matrix, generates conjugate for complex numbers
	adjointTranspose(matrix)
	outer(vector, vector) outer product of two vectors resulting in a matrix

	float2x2Identity() 2x2 identity matrix
	float3x3Identity() 3x3 identity matrix
	float4x4Identity() 4x4 identity matrix
*/
	
// ----------------------------------------------------------------------------
/// abs(a)

// matrix?x2
template <typename Type>
Matrix2<DIGI_REAL(Type)> abs(const Matrix2<Type>& a)
{
	return matrix2(
		abs(a.x),
		abs(a.y));
}

// matrix?x3
template <typename Type>
Matrix3<DIGI_REAL(Type)> abs(const Matrix3<Type>& a)
{
	return matrix3(
		abs(a.x),
		abs(a.y),
		abs(a.z));
}

// matrix?x4
template <typename Type>
Matrix4<DIGI_REAL(Type)> abs(const Matrix4<Type>& a)
{
	return matrix4(
		abs(a.x),
		abs(a.y),
		abs(a.z),
		abs(a.w));
}


// ----------------------------------------------------------------------------
/// sign(a)


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
template <typename Type>
Vector2<DIGI_ELEMENT(Type)> min(const Matrix2<Type>& a)
{
	return vector2(
		min(a.x),
		min(a.y));
}

// matrix?x3
template <typename Type>
Vector3<DIGI_ELEMENT(Type)> min(const Matrix3<Type>& a)
{
	return vector3(
		min(a.x),
		min(a.y),
		min(a.z));
}

// matrix?x4
template <typename Type>
Vector4<DIGI_ELEMENT(Type)> min(const Matrix4<Type>& a)
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
template <typename Type>
Vector2<DIGI_ELEMENT(Type)> max(const Matrix2<Type>& a)
{
	return vector2(
		max(a.x),
		max(a.y));
}

// matrix?x3
template <typename Type>
Vector3<DIGI_ELEMENT(Type)> max(const Matrix3<Type>& a)
{
	return vector3(
		max(a.x),
		max(a.y),
		max(a.z));
}

// matrix?x4
template <typename Type>
Vector4<DIGI_ELEMENT(Type)> max(const Matrix4<Type>& a)
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
template <typename Type>
Vector2<DIGI_ELEMENT(Type)> sum(const Matrix2<Type>& a)
{
	return vector2(
		sum(a.x),
		sum(a.y));
}

// matrix?x3
template <typename Type>
Vector3<DIGI_ELEMENT(Type)> sum(const Matrix3<Type>& a)
{
	return vector3(
		sum(a.x),
		sum(a.y),
		sum(a.z));
}

// matrix?x4
template <typename Type>
Vector4<DIGI_ELEMENT(Type)> sum(const Matrix4<Type>& a)
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
template <typename Type>
Vector2<DIGI_ELEMENT(Type)> dot(const Matrix2<Type>& a, const Matrix2<Type>& b)
{
	return vector2(
		dot(a.x, b.x),
		dot(a.y, b.y));
}

// matrix?x3
template <typename Type>
Vector3<DIGI_ELEMENT(Type)> dot(const Matrix3<Type>& a, const Matrix3<Type>& b)
{
	return vector3(
		dot(a.x, b.x),
		dot(a.y, b.y),
		dot(a.z, b.z));
}

// matrix?x4
template <typename Type>
Vector4<DIGI_ELEMENT(Type)> dot(const Matrix4<Type>& a, const Matrix4<Type>& b)
{
	return vector4(
		dot(a.x, b.x),
		dot(a.y, b.y),
		dot(a.z, b.z),
		dot(a.w, b.w));
}


// ----------------------------------------------------------------------------
/// operator *(matrix, vector)

// vector? = matrix?x2 * vector2
template <typename Type>
Type operator *(const Matrix2<Type>& a, const Vector2<DIGI_ELEMENT(Type)>& b)
{
	return a.x * b.x + a.y * b.y;
}

// vector? = matrix?x3 * vector3
template <typename Type>
Type operator *(const Matrix3<Type>& a, const Vector3<DIGI_ELEMENT(Type)>& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

// vector? = matrix?x4 * vector4
template <typename Type>
Type operator *(const Matrix4<Type>& a, const Vector4<DIGI_ELEMENT(Type)>& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}


// ----------------------------------------------------------------------------
/// operator *(vector, matrix)

// vector2 = vector? * matrix?x2
template <typename Type>
Type operator *(const Type& a, const Matrix2<Type>& b)
{
	return vector2(
		mac(a, b.x),
		mac(a, b.y));
}

// vector3 = vector? * matrix?x3
template <typename Type>
Type operator *(const Type& a, const Matrix3<Type>& b)
{
	return vector3(
		mac(a, b.x),
		mac(a, b.y),
		mac(a, b.z));
}

// vector4 = vector? * matrix?x4
template <typename Type>
Type operator *(const Type& a, const Matrix4<Type>& b)
{
	return vector4(
		mac(a, b.x),
		mac(a, b.y),
		mac(a, b.z),
		mac(a, b.w));
}


// ----------------------------------------------------------------------------
/// operator *(matrix, matrix)

// matrix?x2 = matrix?x2 * matrix2x2
template <typename Type>
Matrix2<Type> operator *(const Matrix2<Type>& a, const DIGI_MATRIX2X2(DIGI_ELEMENT(Type))& b)
{
	return matrix2(
		a.x * b.x.x + a.y * b.x.y,
		a.x * b.y.x + a.y * b.y.y);
}

// matrix?x3 = matrix?x2 * matrix2x3
template <typename Type>
Matrix3<Type> operator *(const Matrix2<Type>& a, const DIGI_MATRIX2X3(DIGI_ELEMENT(Type))& b)
{
	return matrix3(
		a.x * b.x.x + a.y * b.x.y,
		a.x * b.y.x + a.y * b.y.y,
		a.x * b.z.x + a.y * b.z.y);
}

// matrix?x3 = matrix?x3 * matrix3x3
template <typename Type>
Matrix3<Type> operator *(const Matrix3<Type>& a, const DIGI_MATRIX3X3(DIGI_ELEMENT(Type))& b)
{
	return matrix3(
		a.x * b.x.x + a.y * b.x.y + a.z * b.x.z,
		a.x * b.y.x + a.y * b.y.y + a.z * b.y.z,
		a.x * b.z.x + a.y * b.z.y + a.z * b.z.z);
}

// matrix?x4 = matrix?x4 * matrix4x4
template <typename Type>
Matrix4<Type> operator *(const Matrix4<Type>& a, const DIGI_MATRIX4X4(DIGI_ELEMENT(Type))& b)
{
	return matrix4(
		a.x * b.x.x + a.y * b.x.y + a.z * b.x.z + a.w * b.x.w,
		a.x * b.y.x + a.y * b.y.y + a.z * b.y.z + a.w * b.y.w,
		a.x * b.z.x + a.y * b.z.y + a.z * b.z.z + a.w * b.z.w,
		a.x * b.w.x + a.y * b.w.y + a.z * b.w.z + a.w * b.w.w);
}


// ----------------------------------------------------------------------------
/// diag(vector)

// matrix2x2
template <typename Type>
DIGI_MATRIX2X2(Type) diag(const Vector2<Type>& a)
{
	return matrix2x2(
		a.x, Type(0),
		Type(0), a.y);
}

// matrix3x3
template <typename Type>
DIGI_MATRIX3X3(Type) diag(const Vector3<Type>& a)
{
	return matrix3x3(
		a.x, Type(0), Type(0),
		Type(0), a.y, Type(0),
		Type(0), Type(0), a.z);
}

// matrix4x4
template <typename Type>
DIGI_MATRIX4X4(Type) diag(const Vector4<Type>& a)
{
	return matrix4x4(
		a.x, Type(0), Type(0), Type(0),
		Type(0), a.y, Type(0), Type(0),
		Type(0), Type(0), a.z, Type(0),
		Type(0), Type(0), Type(0), a.w);
}


// ----------------------------------------------------------------------------
/// diag(matrix)

// matrix2x2
template <typename Type>
Vector2<Type> diag(const DIGI_MATRIX2X2(Type)& a)
{
	return vector2(a.x.x, a.y.y);
}

// matrix3x3
template <typename Type>
Vector3<Type> diag(const DIGI_MATRIX3X3(Type)& a)
{
	return vector3(a.x.x, a.y.y, a.z.z);
}

// matrix4x4
template <typename Type>
Vector4<Type> diag(const DIGI_MATRIX4X4(Type)& a)
{
	return vector4(a.x.x, a.y.y, a.z.z, a.w.w);
}


// ----------------------------------------------------------------------------
/// trace(matrix)

// matrix2x2
template <typename Type>
Type trace(const DIGI_MATRIX2X2(Type)& a)
{
	return a.x.x + a.y.y;
}

// matrix3x3
template <typename Type>
Type trace(const DIGI_MATRIX3X3(Type)& a)
{
	return a.x.x + a.y.y + a.z.z;
}

// matrix4x4
template <typename Type>
Type trace(const DIGI_MATRIX4X4(Type)& a)
{
	return a.x.x + a.y.y + a.z.z + a.w.w;
}


// ----------------------------------------------------------------------------
/// det(matrix)

// scalar
static inline float det(float a)
{
	return a;
}

static inline double det(double a)
{
	return a;
}

// matrix2x2
template <typename Type>
Type det(const DIGI_MATRIX2X2(Type)& a)
{
	return a.x.x * a.y.y - a.y.x * a.x.y;
}

// matrix3x3
template <typename Type>
Type det(const DIGI_MATRIX3X3(Type)& a)
{
	return mac(a.x, cross(a.y, a.z));
}

// matrix4x4
template <typename Type>
Type det(const DIGI_MATRIX4X4(Type)& a)
{
	// determinant -> signed volume of the parallelepiped spanned by column vectors
	return mac(a.x, cross3(a.y, a.z, a.w));
}


// ----------------------------------------------------------------------------
/// inv(matrix)

// scalar
static inline float inv(float a)
{
	return 1.0f / a;
}

static inline double inv(double a)
{
	return 1.0 / a;
}

// matrix2x2
template <typename Type>
DIGI_MATRIX2X2(Type) inv(const DIGI_MATRIX2X2(Type)& a)
{
	Vector2<Type> cx =  perp(a.y);
	Vector2<Type> cy = -perp(a.x);
	
	Type invdet = inv(mac(a.x, cx));

	return matrix2x2(
		invdet * cx.x, invdet * cy.x,
		invdet * cx.y, invdet * cy.y);
}

// matrix3x3
template <typename Type>
DIGI_MATRIX3X3(Type) inv(const DIGI_MATRIX3X3(Type)& a)
{
	Vector3<Type> cx = cross(a.y, a.z);
	Vector3<Type> cy = cross(a.z, a.x);
	Vector3<Type> cz = cross(a.x, a.y);
	
	Type invdet = inv(mac(a.x, cx));

	return matrix3x3(
		invdet * cx.x, invdet * cy.x, invdet * cz.x,
		invdet * cx.y, invdet * cy.y, invdet * cz.y,
		invdet * cx.z, invdet * cy.z, invdet * cz.z);
}
	
// matrix4x4
template <typename Type>
DIGI_MATRIX4X4(Type) inv(const DIGI_MATRIX4X4(Type)& a)
{
	Vector4<Type> cx =  cross3(a.y, a.z, a.w);
	Vector4<Type> cy = -cross3(a.z, a.w, a.x);
	Vector4<Type> cz =  cross3(a.w, a.x, a.y);
	Vector4<Type> cw = -cross3(a.x, a.y, a.z);

	Type invdet = inv(mac(a.x, cx));
	
	return matrix4x4(
		invdet * cx.x, invdet * cy.x, invdet * cz.x, invdet * cw.x,
		invdet * cx.y, invdet * cy.y, invdet * cz.y, invdet * cw.y,
		invdet * cx.z, invdet * cy.z, invdet * cz.z, invdet * cw.z,
		invdet * cx.w, invdet * cy.w, invdet * cz.w, invdet * cw.w);
}


// ----------------------------------------------------------------------------
/// transpose(matrix)

// matrix2x2
template <typename Type>
DIGI_MATRIX2X2(Type) transpose(const DIGI_MATRIX2X2(Type)& a)
{
	return matrix2x2(
		conj(a.x.x), conj(a.y.x),
		conj(a.x.y), conj(a.y.y));
}

// matrix3x2
template <typename Type>
DIGI_MATRIX2X3(Type) transpose(const DIGI_MATRIX3X2(Type)& a)
{
	return matrix2x3(
		conj(a.x.x), conj(a.y.x),
		conj(a.x.y), conj(a.y.y),
		conj(a.x.z), conj(a.y.z));
}

// matrix4x2
template <typename Type>
DIGI_MATRIX2X4(Type) transpose(const DIGI_MATRIX4X2(Type)& a)
{
	return matrix2x4(
		conj(a.x.x), conj(a.y.x),
		conj(a.x.y), conj(a.y.y),
		conj(a.x.z), conj(a.y.z),
		conj(a.x.w), conj(a.y.w));
}

// matrix2x3
template <typename Type>
DIGI_MATRIX3X2(Type) transpose(const DIGI_MATRIX2X3(Type)& a)
{
	return matrix3x2(
		conj(a.x.x), conj(a.y.x), conj(a.z.x),
		conj(a.x.y), conj(a.y.y), conj(a.z.y));
}

// matrix3x3
template <typename Type>
DIGI_MATRIX3X3(Type) transpose(const DIGI_MATRIX3X3(Type)& a)
{
	return matrix3x3(
		conj(a.x.x), conj(a.y.x), conj(a.z.x),
		conj(a.x.y), conj(a.y.y), conj(a.z.y),
		conj(a.x.z), conj(a.y.z), conj(a.z.z));
}

// matrix4x3
template <typename Type>
DIGI_MATRIX3X4(Type) transpose(const DIGI_MATRIX4X3(Type)& a)
{
	return matrix3x4(
		conj(a.x.x), conj(a.y.x), conj(a.z.x),
		conj(a.x.y), conj(a.y.y), conj(a.z.y),
		conj(a.x.z), conj(a.y.z), conj(a.z.z),
		conj(a.x.w), conj(a.y.w), conj(a.z.w));
}

// matrix2x4
template <typename Type>
DIGI_MATRIX4X2(Type) transpose(const DIGI_MATRIX2X4(Type)& a)
{
	return matrix4x2(
		conj(a.x.x), conj(a.y.x), conj(a.z.x), conj(a.w.x),
		conj(a.x.y), conj(a.y.y), conj(a.z.y), conj(a.w.y));
}

// matrix3x4
template <typename Type>
DIGI_MATRIX4X3(Type) transpose(const DIGI_MATRIX3X4(Type)& a)
{
	return matrix4x3(
		conj(a.x.x), conj(a.y.x), conj(a.z.x), conj(a.w.x),
		conj(a.x.y), conj(a.y.y), conj(a.z.y), conj(a.w.y),
		conj(a.x.z), conj(a.y.z), conj(a.z.z), conj(a.w.z));
}

// matrix4x4
template <typename Type>
DIGI_MATRIX4X4(Type) transpose(const DIGI_MATRIX4X4(Type)& a)
{
	return matrix4x4(
		conj(a.x.x), conj(a.y.x), conj(a.z.x), conj(a.w.x),
		conj(a.x.y), conj(a.y.y), conj(a.z.y), conj(a.w.y),
		conj(a.x.z), conj(a.y.z), conj(a.z.z), conj(a.w.z),
		conj(a.x.w), conj(a.y.w), conj(a.z.w), conj(a.w.w));
}


// ----------------------------------------------------------------------------
/// adjointTranspose(matrix)

// matrix3x3
template <typename Type>
DIGI_MATRIX3X3(Type) adjointTranspose(const DIGI_MATRIX3X3(Type)& a)
{
	return matrix3x3(
		(a.y.y * a.z.z - a.y.z * a.z.y),
		(a.y.z * a.z.x - a.y.x * a.z.z),
		(a.y.x * a.z.y - a.y.y * a.z.x),

		(a.z.y * a.x.z - a.z.z * a.x.y),
		(a.z.z * a.x.x - a.z.x * a.x.z),
		(a.z.x * a.x.y - a.z.y * a.x.x),

		(a.x.y * a.y.z - a.x.z * a.y.y),
		(a.x.z * a.y.x - a.x.x * a.y.z),
		(a.x.x * a.y.y - a.x.y * a.y.x));
}


// ----------------------------------------------------------------------------
// outer(vector, vector)

// matrix2x2
// returs the value of a * transpose(b) when viewed as 2x1 and 1x2 matrix
template <typename Type>
DIGI_MATRIX2X2(Type) outer(const Vector2<Type>& a, const Vector2<Type>& b)
{ 
	return matrix2x2(
		a.x * conj(b.x), a.y * conj(b.x),
		a.x * conj(b.y), a.y * conj(b.y));
}

// matrix2x3
// returs the value of a * transpose(b) when viewed as 2x1 and 1x3 matrix
template <typename Type>
DIGI_MATRIX2X3(Type) outer(const Vector2<Type>& a, const Vector3<Type>& b)
{ 
	return matrix2x3(
		a.x * conj(b.x), a.y * conj(b.x),
		a.x * conj(b.y), a.y * conj(b.y),
		a.x * conj(b.z), a.y * conj(b.z));
}

// matrix3x3
// returs the value of a * transpose(b) when viewed as 3x1 and 1x3 matrix
template <typename Type>
DIGI_MATRIX3X3(Type) outer(const Vector3<Type>& a, const Vector3<Type>& b)
{ 
	return matrix3x3(
		a.x * conj(b.x), a.y * conj(b.x), a.z * conj(b.x),
		a.x * conj(b.y), a.y * conj(b.y), a.z * conj(b.y),
		a.x * conj(b.z), a.y * conj(b.z), a.z * conj(b.z));
}

// matrix4x4
// returs the value of a * transpose(b) when viewed as 4x1 and 1x4 matrix
template <typename Type>
DIGI_MATRIX4X4(Type) outer(const Vector4<Type>& a, const Vector4<Type>& b)
{ 
	return matrix4x4(
		a.x * conj(b.x), a.y * conj(b.x), a.z * conj(b.x), a.w * conj(b.x),
		a.x * conj(b.y), a.y * conj(b.y), a.z * conj(b.y), a.w * conj(b.y),
		a.x * conj(b.z), a.y * conj(b.z), a.z * conj(b.z), a.w * conj(b.z),
		a.x * conj(b.w), a.y * conj(b.w), a.z * conj(b.w), a.w * conj(b.w));
}


// ----------------------------------------------------------------------------
// identity

static inline float2x2 float2x2Identity()
{
	return matrix2x2(
		1.0f, 0.0f,
		0.0f, 1.0f);
}

static inline float3x3 float3x3Identity()
{
	return matrix3x3(
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f);
}

static inline float4x4 float4x4Identity()
{
	return matrix4x4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

static inline double2x2 double2x2Identity()
{
	return matrix2x2(
		1.0, 0.0,
		0.0, 1.0);
}

static inline double3x3 double3x3Identity()
{
	return matrix3x3(
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0);
}

static inline double4x4 double4x4Identity()
{
	return matrix4x4(
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0);
}


/// @}

} // namespace digi

#endif
