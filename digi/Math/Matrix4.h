#ifndef digi_Math_Matrix4_h
#define digi_Math_Matrix4_h

#include "MathBase.h"


namespace digi {

/// @addtogroup Math
/// @{

/**
	?x4 column-major template matrix
	column-major means that the matrix is stored in memory column by column (like in fortran)
*/
template <typename Type>
struct Matrix4
{
	Type x;
	Type y;
	Type z;
	Type w;


	// assignment
	// ------------------------

	Matrix4<Type>& operator +=(const Matrix4<Type> other) 
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		this->w += other.w;
		return *this;
	}
	  
	Matrix4<Type>& operator -=(const Matrix4<Type> other) 
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
		this->w -= other.w;
		return *this;
	}

	Matrix4<Type>& operator *=(const Type& s)
	{ 
		this->x *= s;
		this->y *= s;
		this->z *= s;
		this->w *= s;
		return *this;
	}

	  
	// data access
	// ------------------------

	/// returns the column with the given index
	Type& operator [](size_t index) {return (&this->x)[index];}

	/// returns the column with the given index
	const Type& operator [](size_t index) const {return (&this->x)[index];}
};


// ----------------------------------------------------------------------------
// create from components

// matrix2x4
template <typename Type>
DIGI_MATRIX2X4(Type) matrix2x4(
	Type m11, Type m21,
	Type m12, Type m22,
	Type m13, Type m23,
	Type m14, Type m24)
{
	DIGI_MATRIX2X4(Type) a =
	{
		{m11, m21},
		{m12, m22},
		{m13, m23},
		{m14, m24}
	};
	return a;
}

// matrix3x4
template <typename Type>
DIGI_MATRIX3X4(Type) matrix3x4(
	Type m11, Type m21, Type m31,
	Type m12, Type m22, Type m32,
	Type m13, Type m23, Type m33,
	Type m14, Type m24, Type m34)
{
	DIGI_MATRIX3X4(Type) a =
	{
		{m11, m21, m31},
		{m12, m22, m32},
		{m13, m23, m33},
		{m14, m24, m34}
	};
	return a;
}

// matrix4x4
template <typename Type>
DIGI_MATRIX4X4(Type) matrix4x4(
	Type m11, Type m21, Type m31, Type m41,
	Type m12, Type m22, Type m32, Type m42,
	Type m13, Type m23, Type m33, Type m43,
	Type m14, Type m24, Type m34, Type m44)
{
	DIGI_MATRIX4X4(Type) a =
	{
		{m11, m21, m31, m41},
		{m12, m22, m32, m42},
		{m13, m23, m33, m43},
		{m14, m24, m34, m44}
	};
	return a;
}


// ----------------------------------------------------------------------------
// create from column vectors

// matrix2x4
template <typename Type>
Matrix4<Vector2<Type> > matrix4(const Vector2<Type>& c1, const Vector2<Type>& c2, const Vector2<Type>& c3, const Vector2<Type>& c4)
{
	Matrix4<Vector2<Type> > a = {c1, c2, c3, c4};
	return a;
}

// matrix3x4
template <typename Type>
Matrix4<Vector3<Type> > matrix4(const Vector3<Type>& c1, const Vector3<Type>& c2, const Vector3<Type>& c3, const Vector3<Type>& c4)
{
	Matrix4<Vector3<Type> > a = {c1, c2, c3, c4};
	return a;
}

// matrix4x4
template <typename Type>
Matrix4<Vector4<Type> > matrix4(const Vector4<Type>& c1, const Vector4<Type>& c2, const Vector4<Type>& c3, const Vector4<Type>& c4)
{
	Matrix4<Vector4<Type> > a = {c1, c2, c3, c4};
	return a;
}


// ----------------------------------------------------------------------------
// operators

/// unary plus
template <typename Type>  
const Matrix4<Type>& operator +(const Matrix4<Type>& a)
{
	return a;
}

/// unary minus, component-wise
template <typename Type>  
Matrix4<Type> operator -(const Matrix4<Type>& a)
{
	return matrix4(
		-a.x,
		-a.y,
		-a.z,
		-a.w);
}

/// binary plus, component-wise
template <typename Type>
Matrix4<Type> operator +(const Matrix4<Type>& a, const Matrix4<Type>& b)
{
	return matrix4(
		a.x + b.x,
		a.y + b.y,
		a.z + b.z,
		a.w + b.w);
}

/// binary minus, component-wise
template <typename Type>
Matrix4<Type> operator -(const Matrix4<Type>& a, const Matrix4<Type>& b)
{ 
	return matrix4(
		a.x - b.x,
		a.y - b.y,
		a.z - b.z,
		a.w - b.w);
}

/// multiply by scalar
template <typename Type>
Matrix4<Type> operator *(const Matrix4<Type>& a, const DIGI_ELEMENT(Type)& b)
{ 
	return matrix4(
		a.x * b,
		a.y * b,
		a.z * b,
		a.w * b);  
}

/// multiply by scalar
template <typename Type>
inline Matrix4<Type> operator *(const DIGI_ELEMENT(Type)& a, const Matrix4<Type>& b)
{ 
	return matrix4(
		a * b.x,
		a * b.y,
		a * b.z,
		a * b.w);  
}

/// divide by scalar
template <typename Type>
Matrix4<Type> operator /(const Matrix4<Type>& a, const DIGI_ELEMENT(Type)& b)
{ 
	return matrix4(
		a.x / b,
		a.y / b,
		a.z / b,
		a.w / b);  
}

/*
/// equality comparison
template <typename Type>
bool operator ==(const Matrix4<Type>& a, const Matrix4<Type>& b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

/// inequality comparison
template <typename Type>
bool operator !=(const Matrix4<Type>& a, const Matrix4<Type>& b)
{
	return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w;
}
*/


// ----------------------------------------------------------------------------
// traits

template <typename T>
struct MathTraits<Matrix4<T> >
{
	enum
	{
		COMPOUND = 1
	};

	typedef Matrix4<T> Type;
	typedef typename MathTraits<T>::BaseType BaseType;
	typedef T ElementType;
	typedef Matrix4<typename MathTraits<T>::RealType> RealType;
};


// ----------------------------------------------------------------------------
// typedefs
	
typedef Matrix4<float2> float2x4;
typedef Matrix4<float3> float3x4;
typedef Matrix4<float4> float4x4;

typedef Matrix4<double2> double2x4;
typedef Matrix4<double3> double3x4;
typedef Matrix4<double4> double4x4;


// ----------------------------------------------------------------------------
// typed creators 

inline float4x4 make_float4x4(
	float m11, float m21, float m31, float m41,
	float m12, float m22, float m32, float m42,
	float m13, float m23, float m33, float m43,
	float m14, float m24, float m34, float m44)
{
	float4x4 a =
	{
		{m11, m21, m31, m41},
		{m12, m22, m32, m42},
		{m13, m23, m33, m43},
		{m14, m24, m34, m44}
	};
	return a;
}

/// @}

} // namespace digi

#endif
