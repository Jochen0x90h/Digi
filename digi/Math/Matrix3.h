#ifndef digi_Math_Matrix3_h
#define digi_Math_Matrix3_h

#include "MathBase.h"


namespace digi {

/// @addtogroup Math
/// @{

/**
	?x3 column-major template matrix
	column-major means that the matrix is stored in memory column by column (like in fortran)
*/
template <typename Type>
struct Matrix3
{
	Type x;
	Type y;
	Type z;


	// assignment
	// ------------------------

	Matrix3<Type>& operator +=(const Matrix3<Type> other) 
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		return *this;
	}
	  
	Matrix3<Type>& operator -=(const Matrix3<Type> other) 
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
		return *this;
	}

	Matrix3<Type>& operator *=(const Type& s)
	{ 
		this->x *= s;
		this->y *= s;
		this->z *= s;
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

// matrix2x3
template <typename Type>
DIGI_MATRIX2X3(Type) matrix2x3(
	Type m11, Type m21,
	Type m12, Type m22,
	Type m13, Type m23)
{
	DIGI_MATRIX2X3(Type) a =
	{
		{m11, m21},
		{m12, m22},
		{m13, m23}
	};
	return a;
}

// matrix3x3
template <typename Type>
DIGI_MATRIX3X3(Type) matrix3x3(
	Type m11, Type m21, Type m31,
	Type m12, Type m22, Type m32,
	Type m13, Type m23, Type m33)
{
	DIGI_MATRIX3X3(Type) a =
	{
		{m11, m21, m31},
		{m12, m22, m32},
		{m13, m23, m33}
	};
	return a;
}

// matrix4x3
template <typename Type>
DIGI_MATRIX4X3(Type) matrix4x3(
	Type m11, Type m21, Type m31, Type m41,
	Type m12, Type m22, Type m32, Type m42,
	Type m13, Type m23, Type m33, Type m43)
{
	DIGI_MATRIX4X3(Type) a =
	{
		{m11, m21, m31, m41},
		{m12, m22, m32, m42},
		{m13, m23, m33, m43}
	};
	return a;
}


// ----------------------------------------------------------------------------
// create from column vectors

// matrix2x3
template <typename Type>
Matrix3<Vector2<Type> > matrix3(const Vector2<Type>& c1, const Vector2<Type>& c2, const Vector2<Type>& c3)
{
	Matrix3<Vector2<Type> > a = {c1, c2, c3};
	return a;
}

// matrix3x3
template <typename Type>
Matrix3<Vector3<Type> > matrix3(const Vector3<Type>& c1, const Vector3<Type>& c2, const Vector3<Type>& c3)
{
	Matrix3<Vector3<Type> > a = {c1, c2, c3};
	return a;
}

// matrix4x3
template <typename Type>
Matrix3<Vector4<Type> > matrix3(const Vector4<Type>& c1, const Vector4<Type>& c2, const Vector4<Type>& c3)
{
	Matrix3<Vector4<Type> > a = {c1, c2, c3};
	return a;
}


// ----------------------------------------------------------------------------
// operators

/// unary plus
template <typename Type>  
const Matrix3<Type>& operator +(const Matrix3<Type>& a)
{
	return a;
}

/// unary minus, component-wise
template <typename Type>  
Matrix3<Type> operator -(const Matrix3<Type>& a)
{
	return matrix3(
		-a.x,
		-a.y,
		-a.z);
}

/// binary plus, component-wise
template <typename Type>
Matrix3<Type> operator +(const Matrix3<Type>& a, const Matrix3<Type>& b)
{
	return matrix3(
		a.x + b.x,
		a.y + b.y,
		a.z + b.z);
}

/// binary minus, component-wise
template <typename Type>
Matrix3<Type> operator -(const Matrix3<Type>& a, const Matrix3<Type>& b)
{ 
	return matrix3(
		a.x - b.x,
		a.y - b.y,
		a.z - b.z);
}

/// multiply by scalar
template <typename Type>
Matrix3<Type> operator *(const Matrix3<Type>& a, const DIGI_ELEMENT(Type)& b)
{ 
	return matrix3(
		a.x * b,
		a.y * b,
		a.z * b);  
}

/// multiply by scalar
template <typename Type>
Matrix3<Type> operator *(const DIGI_ELEMENT(Type)& a, const Matrix3<Type>& b)
{ 
	return matrix3(
		a * b.x,
		a * b.y,
		a * b.z);  
}

/// divide by scalar
template <typename Type>
Matrix3<Type> operator /(const Matrix3<Type>& a, const DIGI_ELEMENT(Type)& b)
{ 
	return matrix3(
		a.x / b,
		a.y / b,
		a.z / b);  
}

/*
/// equality comparison
template <typename Type>
bool operator ==(const Matrix3<Type>& a, const Matrix3<Type>& b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

/// inequality comparison
template <typename Type>
bool operator !=(const Matrix3<Type>& a, const Matrix3<Type>& b)
{
	return a.x != b.x || a.y != b.y || a.z != b.z;
}
*/


// ----------------------------------------------------------------------------
// traits

template <typename T>
struct MathTraits<Matrix3<T> >
{
	enum
	{
		COMPOUND = 1
	};

	typedef Matrix3<T> Type;
	typedef typename MathTraits<T>::BaseType BaseType;
	typedef T ElementType;
	typedef Matrix3<typename MathTraits<T>::RealType> RealType;
};


// ----------------------------------------------------------------------------
// typedefs

typedef Matrix3<float2> float2x3;
typedef Matrix3<float3> float3x3;
typedef Matrix3<float4> float4x3;

typedef Matrix3<double2> double2x3;
typedef Matrix3<double3> double3x3;
typedef Matrix3<double4> double4x3;


// ----------------------------------------------------------------------------
// typed creators 

inline float3x3 make_float3x3(
	float m11, float m21, float m31,
	float m12, float m22, float m32,
	float m13, float m23, float m33)
{
	float3x3 a =
	{
		{m11, m21, m31},
		{m12, m22, m32},
		{m13, m23, m33}
	};
	return a;
}

/// @}

} // namespace digi

#endif
