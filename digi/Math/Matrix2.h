#ifndef digi_Math_Matrix2_h
#define digi_Math_Matrix2_h

#include "MathBase.h"


namespace digi {

/// @addtogroup Math
/// @{

/**
	?x2 column-major template matrix
	column-major means that the matrix is stored in memory column by column (like in fortran)
*/
template <typename Type>
struct Matrix2
{
	Type x;
	Type y;


	// assignment
	// ------------------------

	Matrix2<Type>& operator +=(const Matrix2<Type>& other) 
	{
		this->x += other.x;
		this->y += other.y;
		return *this;
	}
	  
	Matrix2<Type>& operator -=(const Matrix2<Type>& other) 
	{
		this->x -= other.x;
		this->y -= other.y;
		return *this;
	}

	Matrix2<Type>& operator *=(const Type& s)
	{ 
		this->x *= s;
		this->y *= s;
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

// matrix2x2
template <typename Type>
DIGI_MATRIX2X2(Type) matrix2x2(
	Type m11, Type m21,
	Type m12, Type m22)
{
	DIGI_MATRIX2X2(Type) a =
	{
		{m11, m21},
		{m12, m22}
	};
	return a;
}

// matrix3x2
template <typename Type>
DIGI_MATRIX3X2(Type) matrix3x2(
	Type m11, Type m21, Type m31,
	Type m12, Type m22, Type m32)
{
	DIGI_MATRIX3X2(Type) a =
	{
		{m11, m21, m31},
		{m12, m22, m32}
	};
	return a;
}

// matrix4x2
template <typename Type>
DIGI_MATRIX4X2(Type) matrix4x2(
	Type m11, Type m21, Type m31, Type m41,
	Type m12, Type m22, Type m32, Type m42)
{
	DIGI_MATRIX4X2(Type) a =
	{
		{m11, m21, m31, m41},
		{m12, m22, m32, m42}
	};
	return a;
}


// ----------------------------------------------------------------------------
// create from column vectors

// matrix2x2
template <typename Type>
Matrix2<Vector2<Type> > matrix2(const Vector2<Type>& c1, const Vector2<Type>& c2)
{
	Matrix2<Vector2<Type> > a = {c1, c2};
	return a;
}

// matrix3x2
template <typename Type>
Matrix2<Vector3<Type> > matrix2(const Vector3<Type>& c1, const Vector3<Type>& c2)
{
	Matrix2<Vector3<Type> > a = {c1, c2};
	return a;
}

// matrix4x2
template <typename Type>
Matrix2<Vector4<Type> > matrix2(const Vector4<Type>& c1, const Vector4<Type>& c2)
{
	Matrix2<Vector4<Type> > a = {c1, c2};
	return a;
}


// ----------------------------------------------------------------------------
// operators

/// unary plus
template <typename Type>  
const Matrix2<Type>& operator +(const Matrix2<Type>& a)
{
	return a;
}

/// unary minus, component-wise
template <typename Type>  
Matrix2<Type> operator -(const Matrix2<Type>& a)
{
	return matrix2(
		-a.x,
		-a.y);
}

/// binary plus, component-wise
template <typename Type>
Matrix2<Type> operator +(const Matrix2<Type>& a, const Matrix2<Type>& b)
{
	return matrix2(
		a.x + b.x,
		a.y + b.y);
}

/// binary minus, component-wise
template <typename Type>
Matrix2<Type> operator -(const Matrix2<Type>& a, const Matrix2<Type>& b)
{ 
	return matrix2(
		a.x - b.x,
		a.y - b.y);
}

/// multiply by scalar
template <typename Type>
Matrix2<Type> operator *(const Matrix2<Type>& a, const DIGI_ELEMENT(Type)& b)
{ 
	return matrix2(
		a.x * b,
		a.y * b);  
}

/// multiply by scalar
template <typename Type>
Matrix2<Type> operator *(const DIGI_ELEMENT(Type)& a, const Matrix2<Type>& b)
{ 
	return matrix2(
		a * b.x,
		a * b.y);  
}

/// divide by scalar
template <typename Type>
Matrix2<Type> operator /(const Matrix2<Type>& a, const DIGI_ELEMENT(Type)& b)
{ 
	return matrix2(
		a.x / b,
		a.y / b);  
}

/*
/// equality comparison
template <typename Type>
bool operator ==(const Matrix2<Type>& a, const Matrix2<Type>& b)
{
	return a.x == b.x && a.y == b.y;
}

/// inequality comparison
template <typename Type>
bool operator !=(const Matrix2<Type>& a, const Matrix2<Type>& b)
{
	return a.x != b.x || a.y != b.y;
}
*/


// ----------------------------------------------------------------------------
// traits

template <typename T>
struct MathTraits<Matrix2<T> >
{
	enum
	{
		COMPOUND = 1
	};
	
	typedef Matrix2<T> Type;
	typedef typename MathTraits<T>::BaseType BaseType;
	typedef T ElementType;
	typedef Matrix2<typename MathTraits<T>::RealType> RealType;
};


// ----------------------------------------------------------------------------
// typedefs

typedef Matrix2<float2> float2x2;
typedef Matrix2<float3> float3x2;
typedef Matrix2<float4> float4x2;

typedef Matrix2<double2> double2x2;
typedef Matrix2<double3> double3x2;
typedef Matrix2<double4> double4x2;


// ----------------------------------------------------------------------------
// typed creators 

inline float2x2 make_float2x2(
	float m11, float m21,
	float m12, float m22)
{
	float2x2 a =
	{
		{m11, m21},
		{m12, m22},
	};
	return a;
}

/// @}

} // namespace digi

#endif
