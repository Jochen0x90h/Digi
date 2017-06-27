#ifndef digi_Math_Quaternion_h
#define digi_Math_Quaternion_h

#include "Vector4.h"


namespace digi {

/// @addtogroup Math
/// @{

/**
	template quaternion
 */
template <typename Type> 
struct Quaternion
{
	Vector4<Type> v;


	// assignment
	// ------------------------

	Quaternion<Type>& operator +=(const Quaternion<Type>& b)
	{
		this->v += b.v;
		return *this;
	}

	Quaternion<Type>& operator -=(const Quaternion<Type>& b)
	{ 
		this->v -= b.v;
		return *this; 
	}

	Quaternion<Type>& operator *=(const Quaternion<Type>& b)
	{
		*this = *this * b;
		return *this;
	}

	Quaternion<Type>& operator *=(Type b)
	{ 
		this->v *= b; 
		return *this; 
	}

	Quaternion<Type>& operator /=(const Quaternion<Type>& b)
	{
		*this = *this * inv(b);
		return *this;
	}

	Quaternion<Type>& operator /=(Type b)
	{ 
		this->v /= b;
		return *this;
	}
};


// ----------------------------------------------------------------------------
// creators

template <typename Type>
Quaternion<Type> quaternion(Type x, Type y, Type z, Type w)
{
	Quaternion<Type> q = {{x, y, z, w}};
	return q;
}

template <typename Type>
Quaternion<Type> quaternion(const Vector3<Type>& v, Type w)
{
	Quaternion<Type> q = {{v.x, v.y, v.z, w}};
	return q;
}

template <typename Type>
Quaternion<Type> quaternion(const Vector4<Type>& v)
{
	Quaternion<Type> q = {v};
	return q;
}


// ----------------------------------------------------------------------------
/// unary plus

template <typename Type>  
const Quaternion<Type>& operator +(const Quaternion<Type>& a)
{
	return a;
}


// ----------------------------------------------------------------------------
/// unary minus

template <typename Type>  
Quaternion<Type> operator -(const Quaternion<Type>& a) 
{
	return quaternion(-a.v);
}


// ----------------------------------------------------------------------------
/// binary plus

template <typename Type> 
Quaternion<Type> operator +(const Quaternion<Type>& a, const Quaternion<Type>& b) 
{
	return quaternion(a.v + b.v);
}


// ----------------------------------------------------------------------------
/// binary minus

template <typename Type> 
Quaternion<Type> operator -(const Quaternion<Type>& a, const Quaternion<Type>& b) 
{
	return quaternion(a.v - b.v);
}


// ----------------------------------------------------------------------------
/// multiply

template <typename Type> 
Quaternion<Type> operator *(const Quaternion<Type>& a, Type b) 
{
	return quaternion(a.v * b);
}

template <typename Type> 
Quaternion<Type> operator *(Type a, const Quaternion<Type>& b) 
{
	return quaternion(a * b.v);
}


// ----------------------------------------------------------------------------
/// quaternion multiply

template <typename Type> 
Quaternion<Type> operator *(const Quaternion<Type>& a, const Quaternion<Type>& b)
{
	return quaternion(
		a.v.w * b.v.x + b.v.w * a.v.x + a.v.y * b.v.z - a.v.z * b.v.y,
		a.v.w * b.v.y + b.v.w * a.v.y + a.v.z * b.v.x - a.v.x * b.v.z,
		a.v.w * b.v.z + b.v.w * a.v.z + a.v.x * b.v.y - a.v.y * b.v.x,
		a.v.w * b.v.w - a.v.x * b.v.x - a.v.y * b.v.y - a.v.z * b.v.z);
}


// ----------------------------------------------------------------------------
/// divide

template <typename Type> 
Quaternion<Type> operator /(const Quaternion<Type>& a, Type b) 
{
	return Quaternion<Type>(a.v / b);
}


// ----------------------------------------------------------------------------
/// quaternion divide

template <typename Type> 
Quaternion<Type> operator /(const Quaternion<Type>& a, const Quaternion<Type>& b) 
{
	return a * inv(b);
}


// ----------------------------------------------------------------------------
// traits

template <typename T>
struct MathTraits<Quaternion<T> >
{
	enum
	{
		COMPOUND = 1
	};
	
	typedef Quaternion<T> Type;
	typedef typename MathTraits<T>::BaseType BaseType;
	typedef Quaternion<T> ElementType;
	typedef T RealType;
};


// ----------------------------------------------------------------------------
// typedefs 

typedef digi::Quaternion<float> floatQuaternion;
typedef digi::Quaternion<double> doubleQuaternion;


// ----------------------------------------------------------------------------
// identity

static inline floatQuaternion floatQuaternionIdentity()
{
	return quaternion(0.0f, 0.0f, 0.0f, 1.0f);
}

/// @}

} // namespace digi

#endif
