#ifndef digi_Math_Vector4_h
#define digi_Math_Vector4_h

#include "Vector3.h"


namespace digi {

/// @addtogroup Math
/// @{

template <typename Type>
struct Vector4Ref;


/**
	4D template vector
*/
template <typename Type> 
struct Vector4
{
	Type x;
	Type y;
	Type z;
	Type w;


	// assignment
	// ------------------------

	Vector4<Type>& operator =(const Type& b)
	{
		this->x = b;
		this->y = b;
		this->z = b;
		this->w = b;
		return *this;
	}

	Vector4<Type>& operator +=(const Vector4<Type>& b)
	{
		this->x += b.x;
		this->y += b.y;
		this->z += b.z;
		this->w += b.w;
		return *this;
	}

	Vector4<Type>& operator +=(const Type& b)
	{
		this->x += b;
		this->y += b;
		this->z += b;
		this->w += b;
		return *this;
	}

	Vector4<Type>& operator -=(const Vector4<Type>& b)
	{
		this->x -= b.x;
		this->y -= b.y;
		this->z -= b.z;
		this->w -= b.w;
		return *this;
	}

	Vector4<Type>& operator -=(const Type& b)
	{
		this->x -= b;
		this->y -= b;
		this->z -= b;
		this->w -= b;
		return *this;
	}

	Vector4<Type>& operator *=(const Vector4<Type>& b)
	{
		this->x *= b.x;
		this->y *= b.y;
		this->z *= b.z;
		this->w *= b.w;
		return *this;
	}

	Vector4<Type>& operator *=(const Type& b)
	{
		this->x *= b;
		this->y *= b;
		this->z *= b;
		this->w *= b;
		return *this;
	}

	Vector4<Type>& operator /=(const Vector4<Type>& b)
	{
		this->x /= b.x;
		this->y /= b.y;
		this->z /= b.z;
		this->w /= b.w;
		return *this;
	}

	Vector4<Type>& operator /=(const Type& b)
	{
		this->x /= b;
		this->y /= b;
		this->z /= b;
		this->w /= b;
		return *this;
	}


	// increment/decrement
	// -------------------

	Vector4<Type>& operator ++()
	{
		++this->x;
		++this->y;
		++this->z;
		++this->w;
		return *this;
	}

	Vector4<Type>& operator --()
	{
		--this->x;
		--this->y;
		--this->z;
		--this->w;
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
	#include "Vector4.inc.h"
};


template <typename Type> 
struct Vector4Ref : public Vector4<Type>
{
	Type& xRef;
	Type& yRef;
	Type& zRef;
	Type& wRef;
	
	Vector4Ref(Type& x, Type& y, Type& z, Type& w)
		: xRef(x), yRef(y), zRef(z), wRef(w) {this->x = x; this->y = y; this->z = z; this->w = w;}


	// assignment
	// ------------------------

	// gcc needs overridden default assignment because of reference
	Vector4Ref<Type>& operator =(const Vector4Ref<Type>& b)
	{
		this->xRef = this->x = b.x;
		this->yRef = this->y = b.y;
		this->zRef = this->z = b.z;
		this->wRef = this->w = b.w;
		return *this;
	}

	Vector4Ref<Type>& operator =(const Vector4<Type>& b)
	{
		this->xRef = this->x = b.x;
		this->yRef = this->y = b.y;
		this->zRef = this->z = b.z;
		this->wRef = this->w = b.w;
		return *this;
	}

	Vector4Ref<Type>& operator =(const Type& b) 
	{
		this->xRef = this->x = b;
		this->yRef = this->y = b;
		this->zRef = this->z = b;
		this->wRef = this->w = b;
		return *this;
	}

	Vector4Ref<Type>& operator +=(const Vector4<Type>& b)
	{
		this->xRef = this->x += b.x;
		this->yRef = this->y += b.y;
		this->zRef = this->z += b.z;
		this->wRef = this->w += b.w;
		return *this;
	}

	Vector4Ref<Type>& operator +=(const Type& b) 
	{
		this->xRef = this->x += b;
		this->yRef = this->y += b;
		this->zRef = this->z += b;
		this->wRef = this->w += b;
		return *this;
	}

	Vector4Ref<Type>& operator -=(const Vector4<Type>& b)
	{
		this->xRef = this->x -= b.x;
		this->yRef = this->y -= b.y;
		this->zRef = this->z -= b.z;
		this->wRef = this->w -= b.w;
		return *this;
	}

	Vector4Ref<Type>& operator -=(const Type& b) 
	{
		this->xRef = this->x -= b;
		this->yRef = this->y -= b;
		this->zRef = this->z -= b;
		this->wRef = this->w -= b;
		return *this;
	}

	Vector4Ref<Type>& operator *=(const Vector4<Type>& b)
	{
		this->xRef = this->x *= b.x;
		this->yRef = this->y *= b.y;
		this->zRef = this->z *= b.z;
		this->wRef = this->w *= b.w;
		return *this;
	}

	Vector4Ref<Type>& operator *=(const Type& b) 
	{
		this->xRef = this->x *= b;
		this->yRef = this->y *= b;
		this->zRef = this->z *= b;
		this->wRef = this->w *= b;
		return *this;
	}

	Vector4Ref<Type>& operator /=(const Vector4<Type>& b)
	{
		this->xRef = this->x /= b.x;
		this->yRef = this->y /= b.y;
		this->zRef = this->z /= b.z;
		this->wRef = this->w /= b.w;
		return *this;
	}

	Vector4Ref<Type>& operator /=(const Type& b)
	{
		this->xRef = this->x /= b;
		this->yRef = this->y /= b;
		this->zRef = this->z /= b;
		this->wRef = this->w /= b;
		return *this;
	}


	// increment/decrement
	// -------------------

	Vector4Ref<Type>& operator ++()
	{
		this->xRef = ++this->x;
		this->yRef = ++this->y;
		this->zRef = ++this->z;
		this->wRef = ++this->w;
		return *this;
	}

	Vector4Ref<Type>& operator --()
	{
		this->xRef = --this->x;
		this->yRef = --this->y;
		this->zRef = --this->z;
		this->wRef = --this->w;
		return *this;
	}


	// swizzle
	// -------
	#include "Vector4Ref.inc.h"
};


// ----------------------------------------------------------------------------
// creators

template <typename Type>
Vector4<Type> splat4(const Type& splat)
{
	Vector4<Type> a = {splat, splat, splat, splat};
	return a;
}

template <typename Type>  
Vector4<Type> vector4(const Type& x, const Type& y, const Type& z, const Type& w)
{
	Vector4<Type> a = {x, y, z, w};
	return a;
}

template <typename Type>
Vector4<Type> vector4(const Vector3<Type>& xyz, const Type& w)
{
	Vector4<Type> a = {xyz.x, xyz.y, xyz.z, w};
	return a;
}

template <typename Type>
Vector4<Type> vector4(const Type& x, const Vector3<Type>& yzw)
{
	Vector4<Type> a = {x, yzw.x, yzw.y, yzw.z};
	return a;
}

template <typename Type>
Vector4<Type> vector4(const Vector2<Type>& xy, const Vector2<Type>& zw)
{
	Vector4<Type> a = {xy.x, xy.y, zw.x, zw.y};
	return a;
}

template <typename Type>
Vector4<Type> vector4(const Vector2<Type>& xy, const Type& z, const Type& w)
{
	Vector4<Type> a = {xy.x, xy.y, z, w};
	return a;
}

template <typename Type>  
Vector4<Type> vector4(const Type* b)
{
	Vector4<Type> a = {b[0], b[1], b[2], b[3]};
	return a;
}


// ----------------------------------------------------------------------------
/// unary plus

template <typename Type> 
Vector4<Type> operator +(const Vector4<Type>& a)
{
	return a;
}


// ----------------------------------------------------------------------------
/// unary minus

template <typename Type> 
Vector4<Type> operator -(const Vector4<Type>& a)
{
	return vector4(-a.x, -a.y, -a.z, -a.w);
}


// ----------------------------------------------------------------------------
/// binary plus

template <typename Type>
Vector4<Type> operator +(const Vector4<Type>& a, const Vector4<Type>& b)
{
	return vector4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

template <typename Type>
Vector4<Type> operator +(const Vector4<Type>& a, const Type& b)
{
	return vector4(a.x + b, a.y + b, a.z + b, a.w + b);
}

template <typename Type>
Vector4<Type> operator +(const Type& a, const Vector4<Type>& b)
{
	return vector4(a + b.x, a + b.y, a + b.z, a + b.w);
}


// ----------------------------------------------------------------------------
/// binary minus

template <typename Type>
Vector4<Type> operator -(const Vector4<Type>& a, const Vector4<Type>& b)
{
	return vector4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

template <typename Type>
Vector4<Type> operator -(const Vector4<Type>& a, const Type& b)
{
	return vector4(a.x - b, a.y - b, a.z - b, a.w - b);
}

template <typename Type>
Vector4<Type> operator -(const Type& a, const Vector4<Type>& b)
{
	return vector4(a - b.x, a - b.y, a - b.z, a - b.w);
}


// ----------------------------------------------------------------------------
/// multiply

template <typename Type>
Vector4<Type> operator *(const Vector4<Type>& a, const Vector4<Type>& b)
{
	return vector4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

template <typename Type>
Vector4<Type> operator *(const Vector4<Type>& a, const Type& b)
{
	return vector4(a.x * b, a.y * b, a.z * b, a.w * b);
}

template <typename Type>
Vector4<Type> operator *(const Type& a, const Vector4<Type>& b)
{
	return vector4(a * b.x, a * b.y, a * b.z, a * b.w);
}


// ----------------------------------------------------------------------------
/// divide

template <typename Type>
Vector4<Type> operator /(const Vector4<Type>& a, const Vector4<Type>& b)
{
	return vector4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}

template <typename Type>
Vector4<Type> operator /(const Vector4<Type>& a, const Type& b)
{
	return vector4(a.x / b, a.y / b, a.z / b, a.w / b);
}

template <typename Type>
Vector4<Type> operator /(const Type& a, const Vector4<Type>& b)
{
	return vector4(a / b.x, a / b.y, a / b.z, a / b.w);
}


// ----------------------------------------------------------------------------
/// equals

template <typename Type>
Vector4<bool> operator ==(const Vector4<Type>& a, const Vector4<Type>& b)
{
	return vector4(a.x == b.x, a.y == b.y, a.z == b.z, a.w == b.w);
}

template <typename Type>
Vector4<bool> operator ==(const Vector4<Type>& a, const Type& b)
{
	return vector4(a.x == b, a.y == b, a.z == b, a.w == b);
}

template <typename Type>
Vector4<bool> operator ==(const Type& a, const Vector4<Type>& b)
{
	return vector4(a == b.x, a == b.y, a == b.z, a == b.w);
}


// ----------------------------------------------------------------------------
/// not equals

template <typename Type>
Vector4<bool> operator !=(const Vector4<Type>& a, const Vector4<Type>& b)
{
	return vector4(a.x != b.x, a.y != b.y, a.z != b.z, a.w != b.w);
}

template <typename Type>
Vector4<bool> operator !=(const Vector4<Type>& a, const Type& b)
{
	return vector4(a.x != b, a.y != b, a.z != b, a.w != b);
}

template <typename Type>
Vector4<bool> operator !=(const Type& a, const Vector4<Type>& b)
{
	return vector4(a != b.x, a != b.y, a != b.z, a != b.w);
}


// ----------------------------------------------------------------------------
/// less

template <typename Type>
Vector4<bool> operator <(const Vector4<Type>& a, const Vector4<Type>& b)
{
	return vector4(a.x < b.x, a.y < b.y, a.z < b.z, a.w < b.w);
}

template <typename Type>
Vector4<bool> operator <(const Vector4<Type>& a, const Type& b)
{
	return vector4(a.x < b, a.y < b, a.z < b, a.w < b);
}

template <typename Type>
Vector4<bool> operator <(const Type& a, const Vector4<Type>& b)
{
	return vector4(a < b.x, a < b.y, a < b.z, a < b.w);
}


// ----------------------------------------------------------------------------
/// greater

template <typename Type>
Vector4<bool> operator >(const Vector4<Type>& a, const Vector4<Type>& b)
{
	return vector4(a.x > b.x, a.y > b.y, a.z > b.z, a.w > b.w);
}

template <typename Type>
Vector4<bool> operator >(const Vector4<Type>& a, const Type& b)
{
	return vector4(a.x > b, a.y > b, a.z > b, a.w > b);
}

template <typename Type>
Vector4<bool> operator >(const Type& a, const Vector4<Type>& b)
{
	return vector4(a > b.x, a > b.y, a > b.z, a > b.w);
}


// ----------------------------------------------------------------------------
/// shift left

template <typename Type>
Vector4<Type> operator <<(const Vector4<Type>& a, const Vector4<Type>& b)
{
	return vector4(a.x << b.x, a.y << b.y, a.z << b.z, a.w << b.w);
}

template <typename Type>
Vector4<Type> operator <<(const Vector4<Type>& a, const Type& b)
{
	return vector4(a.x << b, a.y << b, a.z << b, a.w << b);
}

template <typename Type>
Vector4<Type> operator <<(const Type& a, const Vector4<Type>& b)
{
	return vector4(a << b.x, a << b.y, a << b.z, a << b.w);
}


// ----------------------------------------------------------------------------
/// shift right

template <typename Type>
Vector4<Type> operator >>(const Vector4<Type>& a, const Vector4<Type>& b)
{
	return vector4(a.x >> b.x, a.y >> b.y, a.z >> b.z, a.w >> b.w);
}

template <typename Type>
Vector4<Type> operator >>(const Vector4<Type>& a, const Type& b)
{
	return vector4(a.x >> b, a.y >> b, a.z >> b, a.w >> b);
}

template <typename Type>
Vector4<Type> operator >>(const Type& a, const Vector4<Type>& b)
{
	return vector4(a >> b.x, a >> b.y, a >> b.z, a >> b.w);
}


// ----------------------------------------------------------------------------
/// and

template <typename Type>
Vector4<Type> operator &(const Vector4<Type>& a, const Vector4<Type>& b)
{
	return vector4(a.x & b.x, a.y & b.y, a.z & b.z, a.w & b.w);
}

template <typename Type>
Vector4<Type> operator &(const Vector4<Type>& a, const Type& b)
{
	return vector4(a.x & b, a.y & b, a.z & b, a.w & b);
}

template <typename Type>
Vector4<Type> operator &(const Type& a, const Vector4<Type>& b)
{
	return vector4(a & b.x, a & b.y, a & b.z, a & b.w);
}


// ----------------------------------------------------------------------------
/// or

template <typename Type>
Vector4<Type> operator |(const Vector4<Type>& a, const Vector4<Type>& b)
{
	return vector4(a.x | b.x, a.y | b.y, a.z | b.z, a.w | b.w);
}

template <typename Type>
Vector4<Type> operator |(const Vector4<Type>& a, const Type& b)
{
	return vector4(a.x | b, a.y | b, a.z | b, a.w | b);
}

template <typename Type>
Vector4<Type> operator |(const Type& a, const Vector4<Type>& b)
{
	return vector4(a | b.x, a | b.y, a | b.z, a | b.w);
}


// ----------------------------------------------------------------------------
/// xor

template <typename Type>
Vector4<Type> operator ^(const Vector4<Type>& a, const Vector4<Type>& b)
{
	return vector4(a.x ^ b.x, a.y ^ b.y, a.z ^ b.z, a.w ^ b.w);
}

template <typename Type>
Vector4<Type> operator ^(const Vector4<Type>& a, const Type& b)
{
	return vector4(a.x ^ b, a.y ^ b, a.z ^ b, a.w ^ b);
}

template <typename Type>
Vector4<Type> operator ^(const Type& a, const Vector4<Type>& b)
{
	return vector4(a ^ b.x, a ^ b.y, a ^ b.z, a ^ b.w);
}


// ----------------------------------------------------------------------------
// traits

template <typename T>
struct MathTraits<Vector4<T> >
{
	enum
	{
		COMPOUND = 1
	};
	
	typedef Vector4<T> Type;
	typedef typename MathTraits<T>::BaseType BaseType;
	typedef T ElementType;
	typedef Vector4<typename MathTraits<T>::RealType> RealType;
};


/// @}

} // namespace digi

#endif
