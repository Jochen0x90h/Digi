#ifndef digi_Math_Vector3_h
#define digi_Math_Vector3_h

#include "Vector2.h"


namespace digi {

/// @addtogroup Math
/// @{

template <typename Type>
struct Vector3Ref;


/**
	3D template vector
*/
template <typename Type> 
struct Vector3
{
	Type x;
	Type y;
	Type z;


	// assignment
	// ------------------------

	Vector3<Type>& operator =(const Type& b)
	{
		this->x = b;
		this->y = b;
		this->z = b;
		return *this;
	}

	Vector3<Type>& operator +=(const Vector3<Type>& b) 
	{
		this->x += b.x;
		this->y += b.y;
		this->z += b.z;
		return *this;
	}

	Vector3<Type>& operator +=(const Type& b) 
	{
		this->x += b;
		this->y += b;
		this->z += b;
		return *this;
	}

	Vector3<Type>& operator -=(const Vector3<Type>& b)
	{
		this->x -= b.x;
		this->y -= b.y;
		this->z -= b.z;
		return *this;
	}

	Vector3<Type>& operator -=(const Type& b)
	{
		this->x -= b;
		this->y -= b;
		this->z -= b;
		return *this;
	}

	Vector3<Type>& operator *=(const Vector3<Type>& b)
	{
		this->x *= b.x;
		this->y *= b.y;
		this->z *= b.z;
		return *this;
	}

	Vector3<Type>& operator *=(const Type& b)
	{
		this->x *= b;
		this->y *= b;
		this->z *= b;
		return *this;
	}

	Vector3<Type>& operator /=(const Vector3<Type>& b)
	{
		this->x /= b.x;
		this->y /= b.y;
		this->z /= b.z;
		return *this;
	}

	Vector3<Type>& operator /=(const Type& b)
	{
		this->x /= b;
		this->y /= b;
		this->z /= b;
		return *this;
	}


	// data access
	// ------------------------

	/// returns the ith element of the vector
	Type& operator [](size_t index) {return (&this->x)[index];}

	/// returns the ith element of the vector
	const Type& operator [](size_t index) const {return (&this->x)[index];}


	// swizzle
	// -------
	#include "Vector3.inc.h"
};

template <typename Type> 
struct Vector3Ref : public Vector3<Type>
{
	Type& xRef;
	Type& yRef;
	Type& zRef;
	
	Vector3Ref(Type& x, Type& y, Type& z)
		: xRef(x), yRef(y), zRef(z) {this->x = x; this->y = y; this->z = z;}
	

	// assignment
	// ------------------------

	// gcc needs overridden default assignment because of reference
	Vector3Ref<Type>& operator =(const Vector3Ref<Type>& b)
	{
		this->xRef = this->x = b.x;
		this->yRef = this->y = b.y;
		this->zRef = this->z = b.z;
		return *this;
	}

	Vector3Ref<Type>& operator =(const Vector3<Type>& b)
	{
		this->xRef = this->x = b.x;
		this->yRef = this->y = b.y;
		this->zRef = this->z = b.z;
		return *this;
	}

	Vector3Ref<Type>& operator =(const Type& b) 
	{
		this->xRef = this->x = b;
		this->yRef = this->y = b;
		this->zRef = this->z = b;
		return *this;
	}

	Vector3Ref<Type>& operator +=(const Vector3<Type>& b)
	{
		this->xRef = this->x += b.x;
		this->yRef = this->y += b.y;
		this->zRef = this->z += b.z;
		return *this;
	}

	Vector3Ref<Type>& operator +=(const Type& b)
	{
		this->xRef = this->x += b;
		this->yRef = this->y += b;
		this->zRef = this->z += b;
		return *this;
	}

	Vector3Ref<Type>& operator -=(const Vector3<Type>& b)
	{
		this->xRef = this->x -= b.x;
		this->yRef = this->y -= b.y;
		this->zRef = this->z -= b.z;
		return *this;
	}

	Vector3Ref<Type>& operator -=(const Type& b)
	{
		this->xRef = this->x -= b;
		this->yRef = this->y -= b;
		this->zRef = this->z -= b;
		return *this;
	}

	Vector3Ref<Type>& operator *=(const Vector3<Type>& b)
	{
		this->xRef = this->x *= b.x;
		this->yRef = this->y *= b.y;
		this->zRef = this->z *= b.z;
		return *this;
	}

	Vector3Ref<Type>& operator *=(const Type& b)
	{
		this->xRef = this->x *= b;
		this->yRef = this->y *= b;
		this->zRef = this->z *= b;
		return *this;
	}

	Vector3Ref<Type>& operator /=(const Vector3<Type>& b)
	{
		this->xRef = this->x /= b.x;
		this->yRef = this->y /= b.y;
		this->zRef = this->z /= b.z;
		return *this;
	}

	Vector3Ref<Type>& operator /=(const Type& b)
	{
		this->xRef = this->x /= b;
		this->yRef = this->y /= b;
		this->zRef = this->z /= b;
		return *this;
	}


	// swizzle
	// -------
	#include "Vector3Ref.inc.h"
};


// ----------------------------------------------------------------------------
// creators

template <typename Type>
Vector3<Type> splat3(const Type& splat)
{
	Vector3<Type> a = {splat, splat, splat};
	return a;
}

template <typename Type>
Vector3<Type> vector3(const Type& x, const Type& y, const Type& z)
{
	Vector3<Type> a = {x, y, z};
	return a;
}

template <typename Type>
Vector3<Type> vector3(const Vector2<Type>& xy, const Type& z)
{
	Vector3<Type> a = {xy.x, xy.y, z};
	return a;
}

template <typename Type>
Vector3<Type> vector3(const Type& x, const Vector2<Type>& yz)
{
	Vector3<Type> a = {x, yz.x, yz.y};
	return a;
}

template <typename Type>
Vector3<Type> vector3(const Type* b)
{
	Vector3<Type> a = {b[0], b[1], b[2]};
	return a;
}


// ----------------------------------------------------------------------------
/// unary plus

template <typename Type> 
Vector3<Type> operator +(const Vector3<Type>& a)
{
	return a;
}


// ----------------------------------------------------------------------------
/// unary minus

template <typename Type> 
Vector3<Type> operator -(const Vector3<Type>& a)
{
	return vector3(-a.x, -a.y, -a.z);
}


// ----------------------------------------------------------------------------
/// binary plus

template <typename Type> 
Vector3<Type> operator +(const Vector3<Type>& a, const Vector3<Type>& b)
{
	return vector3(a.x + b.x, a.y + b.y, a.z + b.z);
}

template <typename Type>
Vector3<Type> operator +(const Vector3<Type>& a, const Type& b)
{
	return vector3(a.x + b, a.y + b, a.z + b);
}

template <typename Type>
Vector3<Type> operator +(const Type& a, const Vector3<Type>& b)
{
	return vector3(a + b.x, a + b.y, a + b.z);
}


// ----------------------------------------------------------------------------
/// binary minus

template <typename Type> 
Vector3<Type> operator -(const Vector3<Type>& a, const Vector3<Type>& b)
{
	return vector3(a.x - b.x, a.y - b.y, a.z - b.z);
}

template <typename Type>
Vector3<Type> operator -(const Vector3<Type>& a, const Type& b)
{
	return vector3(a.x - b, a.y - b, a.z - b);
}

template <typename Type>
Vector3<Type> operator -(const Type& a, const Vector3<Type>& b)
{
	return vector3(a - b.x, a - b.y, a - b.z);
}


// ----------------------------------------------------------------------------
/// multiply

template <typename Type> 
Vector3<Type> operator *(const Vector3<Type>& a, const Vector3<Type>& b)
{
	return vector3(a.x * b.x, a.y * b.y, a.z * b.z);
}

template <typename Type>
Vector3<Type> operator *(const Vector3<Type>& a, const Type& b)
{
	return vector3(a.x * b, a.y * b, a.z * b);
}

template <typename Type>
Vector3<Type> operator *(const Type& a, const Vector3<Type>& b)
{
	return vector3(a * b.x, a * b.y, a * b.z);
}


// ----------------------------------------------------------------------------
/// divide

template <typename Type> 
Vector3<Type> operator /(const Vector3<Type>& a, const Vector3<Type>& b)
{
	return vector3(a.x / b.x, a.y / b.y, a.z / b.z);
}

template <typename Type>
Vector3<Type> operator /(const Vector3<Type>& a, const Type& b)
{
	return vector3(a.x / b, a.y / b, a.z / b);
}

template <typename Type>
Vector3<Type> operator /(const Type& a, const Vector3<Type>& b)
{
	return vector3(a / b.x, a / b.y, a / b.z);
}


// ----------------------------------------------------------------------------
/// equals

template <typename Type>
Vector3<bool> operator ==(const Vector3<Type>& a, const Vector3<Type>& b)
{
	return vector3(a.x == b.x, a.y == b.y, a.z == b.z);
}

template <typename Type>
Vector3<bool> operator ==(const Vector3<Type>& a, const Type& b)
{
	return vector3(a.x == b, a.y == b, a.z == b);
}

template <typename Type>
Vector3<bool> operator ==(const Type& a, const Vector3<Type>& b)
{
	return vector3(a == b.x, a == b.y, a == b.z);
}


// ----------------------------------------------------------------------------
/// not equals

template <typename Type>
Vector3<bool> operator !=(const Vector3<Type>& a, const Vector3<Type>& b)
{
	return vector3(a.x != b.x, a.y != b.y, a.z != b.z);
}

template <typename Type>
Vector3<bool> operator !=(const Vector3<Type>& a, const Type& b)
{
	return vector3(a.x != b, a.y != b, a.z != b);
}

template <typename Type>
Vector3<bool> operator !=(const Type& a, const Vector3<Type>& b)
{
	return vector3(a != b.x, a != b.y, a != b.z);
}


// ----------------------------------------------------------------------------
/// less

template <typename Type>
Vector3<bool> operator <(const Vector3<Type>& a, const Vector3<Type>& b)
{
	return vector3(a.x < b.x, a.y < b.y, a.z < b.z);
}

template <typename Type>
Vector3<bool> operator <(const Vector3<Type>& a, const Type& b)
{
	return vector3(a.x < b, a.y < b, a.z < b);
}

template <typename Type>
Vector3<bool> operator <(const Type& a, const Vector3<Type>& b)
{
	return vector3(a < b.x, a < b.y, a < b.z);
}


// ----------------------------------------------------------------------------
/// greater

template <typename Type>
Vector3<bool> operator >(const Vector3<Type>& a, const Vector3<Type>& b)
{
	return vector3(a.x > b.x, a.y > b.y, a.z > b.z);
}

template <typename Type>
Vector3<bool> operator >(const Vector3<Type>& a, const Type& b)
{
	return vector3(a.x > b, a.y > b, a.z > b);
}

template <typename Type>
Vector3<bool> operator >(const Type& a, const Vector3<Type>& b)
{
	return vector3(a > b.x, a > b.y, a > b.z);
}


// ----------------------------------------------------------------------------
/// shift left

template <typename Type>
Vector3<Type> operator <<(const Vector3<Type>& a, const Vector3<Type>& b)
{
	return vector3(a.x << b.x, a.y << b.y, a.z << b.z);
}

template <typename Type>
Vector3<Type> operator <<(const Vector3<Type>& a, const Type& b)
{
	return vector3(a.x << b, a.y << b, a.z << b);
}

template <typename Type>
Vector3<Type> operator <<(const Type& a, const Vector3<Type>& b)
{
	return vector3(a << b.x, a << b.y, a << b.z);
}


// ----------------------------------------------------------------------------
/// shift right

template <typename Type>
Vector3<Type> operator >>(const Vector3<Type>& a, const Vector3<Type>& b)
{
	return vector3(a.x >> b.x, a.y >> b.y, a.z >> b.z);
}

template <typename Type>
Vector3<Type> operator >>(const Vector3<Type>& a, const Type& b)
{
	return vector3(a.x >> b, a.y >> b, a.z >> b);
}

template <typename Type>
Vector3<Type> operator >>(const Type& a, const Vector3<Type>& b)
{
	return vector3(a >> b.x, a >> b.y, a >> b.z);
}


// ----------------------------------------------------------------------------
/// and

template <typename Type> 
Vector3<Type> operator &(const Vector3<Type>& a, const Vector3<Type>& b)
{
	return vector3(a.x & b.x, a.y & b.y, a.z & b.z);
}

template <typename Type>
Vector3<Type> operator &(const Vector3<Type>& a, const Type& b)
{
	return vector3(a.x & b, a.y & b, a.z & b);
}

template <typename Type>
Vector3<Type> operator &(const Type& a, const Vector3<Type>& b)
{
	return vector3(a & b.x, a & b.y, a & b.z);
}


// ----------------------------------------------------------------------------
/// or

template <typename Type> 
Vector3<Type> operator |(const Vector3<Type>& a, const Vector3<Type>& b)
{
	return vector3(a.x | b.x, a.y | b.y, a.z | b.z);
}

template <typename Type>
Vector3<Type> operator |(const Vector3<Type>& a, const Type& b)
{
	return vector3(a.x | b, a.y | b, a.z | b);
}

template <typename Type>
Vector3<Type> operator |(const Type& a, const Vector3<Type>& b)
{
	return vector3(a | b.x, a | b.y, a | b.z);
}


// ----------------------------------------------------------------------------
/// xor

template <typename Type> 
Vector3<Type> operator ^(const Vector3<Type>& a, const Vector3<Type>& b)
{
	return vector3(a.x ^ b.x, a.y ^ b.y, a.z ^ b.z);
}

template <typename Type>
Vector3<Type> operator ^(const Vector3<Type>& a, const Type& b)
{
	return vector3(a.x ^ b, a.y ^ b, a.z ^ b);
}

template <typename Type>
Vector3<Type> operator ^(const Type& a, const Vector3<Type>& b)
{
	return vector3(a ^ b.x, a ^ b.y, a ^ b.z);
}


// ----------------------------------------------------------------------------
// traits

template <typename T>
struct MathTraits<Vector3<T> >
{
	enum
	{
		COMPOUND = 1
	};
	
	typedef Vector3<T> Type;
	typedef typename MathTraits<T>::BaseType BaseType;
	typedef T ElementType;
	typedef Vector3<typename MathTraits<T>::RealType> RealType;
};


/// @}

} // namespace digi

#endif
