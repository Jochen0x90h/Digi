#ifndef digi_Math_Vector2_h
#define digi_Math_Vector2_h
				  
#include "MathBase.h"


namespace digi {

/// @addtogroup Math
/// @{

template <typename Type> 
struct Vector3;

template <typename Type> 
struct Vector4;

template <typename Type>
struct Vector2Ref;


/**
	2D template vector
*/
template <typename Type> 
struct Vector2
{
	Type x;
	Type y;


	// assignment
	// ----------

	Vector2<Type>& operator =(const Type& b)
	{
		this->x = b;
		this->y = b;
		return *this;
	}

	Vector2<Type>& operator +=(const Vector2<Type>& b)
	{
		this->x += b.x;
		this->y += b.y;
		return *this;
	}

	Vector2<Type>& operator +=(const Type& b)
	{
		this->x += b;
		this->y += b;
		return *this;
	}

	Vector2<Type>& operator -=(const Vector2<Type>& b) 
	{
		this->x -= b.x;
		this->y -= b.y;
		return *this;
	}

	Vector2<Type>& operator -=(const Type& b)
	{
		this->x -= b;
		this->y -= b;
		return *this;
	}

	Vector2<Type>& operator *=(const Vector2<Type>& b) 
	{
		this->x *= b.x;
		this->y *= b.y;
		return *this;
	}

	Vector2<Type>& operator *=(const Type& b)
	{
		this->x *= b;
		this->y *= b;
		return *this;
	}

	Vector2<Type>& operator /=(const Vector2<Type>& b)
	{
		this->x /= b.x;
		this->y /= b.y;
		return *this;
	}

	Vector2<Type>& operator /=(const Type& b)
	{
		this->x /= b;
		this->y /= b;
		return *this;
	}


	// data access
	// -----------

	/// returns the ith element of the vector
	Type& operator [](size_t index) {return (&this->x)[index];}

	/// returns the ith element of the vector
	const Type& operator [](size_t index) const {return (&this->x)[index];}


	// swizzle
	// -------
	#include "Vector2.inc.h"
};

template <typename Type> 
struct Vector2Ref : public Vector2<Type>
{
	Type& xRef;
	Type& yRef;
	
	Vector2Ref(Type& x, Type& y)
		: xRef(x), yRef(y) {this->x = x; this->y = y;}


	// assignment
	// ----------

	// gcc needs overridden default assignment because of reference
	Vector2Ref<Type>& operator =(const Vector2Ref<Type>& b)
	{
		this->xRef = this->x = b.x;
		this->yRef = this->y = b.y;
		return *this;
	}

	Vector2Ref<Type>& operator =(const Vector2<Type>& b)
	{
		this->xRef = this->x = b.x;
		this->yRef = this->y = b.y;
		return *this;
	}

	Vector2Ref<Type>& operator =(const Type& b) 
	{
		this->xRef = this->x = b;
		this->yRef = this->y = b;
		return *this;
	}

	Vector2Ref<Type>& operator +=(const Vector2<Type>& b) 
	{
		this->xRef = this->x += b.x;
		this->yRef = this->y += b.y;
		return *this;
	}

	Vector2Ref<Type>& operator +=(const Type& b)
	{
		this->xRef = this->x += b;
		this->yRef = this->y += b;
		return *this;
	}

	Vector2Ref<Type>& operator -=(const Vector2<Type>& b) 
	{
		this->xRef = this->x -= b.x;
		this->yRef = this->y -= b.y;
		return *this;
	}

	Vector2Ref<Type>& operator -=(const Type& b)
	{
		this->xRef = this->x -= b;
		this->yRef = this->y -= b;
		return *this;
	}

	Vector2Ref<Type>& operator *=(const Vector2<Type>& b) 
	{
		this->xRef = this->x *= b.x;
		this->yRef = this->y *= b.y;
		return *this;
	}

	Vector2Ref<Type>& operator *=(const Type& b) 
	{
		this->xRef = this->x *= b;
		this->yRef = this->y *= b;
		return *this;
	}

	Vector2Ref<Type>& operator /=(const Vector2<Type>& b) 
	{
		this->xRef = this->x /= b.x;
		this->yRef = this->y /= b.y;
		return *this;
	}

	Vector2Ref<Type>& operator /=(const Type& b) 
	{
		this->xRef = this->x /= b;
		this->yRef = this->y /= b;
		return *this;
	}


	// swizzle
	// -------
	#include "Vector2Ref.inc.h"
};


// ----------------------------------------------------------------------------
// creators

template <typename Type>
Vector2<Type> splat2(const Type& splat)
{
	Vector2<Type> a = {splat, splat};
	return a;
}

template <typename Type>
Vector2<Type> vector2(const Type& x, const Type& y)
{
	Vector2<Type> a = {x, y};
	return a;
}

template <typename Type>
Vector2<Type> vector2(const Type* b)
{
	Vector2<Type> a = {b[0], b[1]};
	return a;
}


// ----------------------------------------------------------------------------
/// unary plus

template <typename Type>
Vector2<Type> operator +(const Vector2<Type>& a)
{
	return a;
}


// ----------------------------------------------------------------------------
/// unary minus

template <typename Type>
Vector2<Type> operator -(const Vector2<Type>& a)
{
	return vector2(-a.x, -a.y);
}


// ----------------------------------------------------------------------------
/// binary plus

template <typename Type>
Vector2<Type> operator +(const Vector2<Type>& a, const Vector2<Type>& b)
{
	return vector2(a.x + b.x, a.y + b.y);
}

template <typename Type>
Vector2<Type> operator +(const Vector2<Type>& a, const Type& b)
{
	return vector2(a.x + b, a.y + b);
}

template <typename Type>
Vector2<Type> operator +(const Type& a, const Vector2<Type>& b)
{
	return vector2(a + b.x, a + b.y);
}


// ----------------------------------------------------------------------------
/// binary minus

template <typename Type>
Vector2<Type> operator -(const Vector2<Type>& a, const Vector2<Type>& b)
{
	return vector2(a.x - b.x, a.y - b.y);
}

template <typename Type>
Vector2<Type> operator -(const Vector2<Type>& a, const Type& b)
{
	return vector2(a.x - b, a.y - b);
}

template <typename Type>
Vector2<Type> operator -(const Type& a, const Vector2<Type>& b)
{
	return vector2(a - b.x, a - b.y);
}


// ----------------------------------------------------------------------------
/// multiply

template <typename Type>
Vector2<Type> operator *(const Vector2<Type>& a, const Vector2<Type>& b)
{
	return vector2(a.x * b.x, a.y * b.y);
}

template <typename Type>
Vector2<Type> operator *(const Vector2<Type>& a, const Type& b)
{
	return vector2(a.x * b, a.y * b);
}

template <typename Type>
Vector2<Type> operator *(const Type& a, const Vector2<Type>& b) 
{
	return vector2(a * b.x, a * b.y);
}


// ----------------------------------------------------------------------------
/// divide

template <typename Type>
Vector2<Type> operator /(const Vector2<Type>& a, const Vector2<Type>& b)
{
	return vector2(a.x / b.x, a.y / b.y);
}

template <typename Type>
Vector2<Type> operator /(const Vector2<Type>& a, const Type& b)
{
	return vector2(a.x / b, a.y / b);
}

template <typename Type>
Vector2<Type> operator /(const Type& a, const Vector2<Type>& b)
{
	return vector2(a / b.x, a / b.y);
}


// ----------------------------------------------------------------------------
/// equals

template <typename Type>
Vector2<bool> operator ==(const Vector2<Type>& a, const Vector2<Type>& b)
{
	return vector2(a.x == b.x, a.y == b.y);
}

template <typename Type>
Vector2<bool> operator ==(const Vector2<Type>& a, const Type& b)
{
	return vector2(a.x == b, a.y == b);
}

template <typename Type>
Vector2<bool> operator ==(const Type& a, const Vector2<Type>& b)
{
	return vector2(a == b.x, a == b.y);
}


// ----------------------------------------------------------------------------
/// not equals

template <typename Type>
Vector2<bool> operator !=(const Vector2<Type>& a, const Vector2<Type>& b)
{
	return vector2(a.x != b.x, a.y != b.y);
}

template <typename Type>
Vector2<bool> operator !=(const Vector2<Type>& a, const Type& b)
{
	return vector2(a.x != b, a.y != b);
}

template <typename Type>
Vector2<bool> operator !=(const Type& a, const Vector2<Type>& b)
{
	return vector2(a != b.x, a != b.y);
}


// ----------------------------------------------------------------------------
/// less

template <typename Type>
Vector2<bool> operator <(const Vector2<Type>& a, const Vector2<Type>& b)
{
	return vector2(a.x < b.x, a.y < b.y);
}

template <typename Type>
Vector2<bool> operator <(const Vector2<Type>& a, const Type& b)
{
	return vector2(a.x < b, a.y < b);
}

template <typename Type>
Vector2<bool> operator <(const Type& a, const Vector2<Type>& b)
{
	return vector2(a < b.x, a < b.y);
}


// ----------------------------------------------------------------------------
/// greater

template <typename Type>
Vector2<bool> operator >(const Vector2<Type>& a, const Vector2<Type>& b)
{
	return vector2(a.x > b.x, a.y > b.y);
}

template <typename Type>
Vector2<bool> operator >(const Vector2<Type>& a, const Type& b)
{
	return vector2(a.x > b, a.y > b);
}

template <typename Type>
Vector2<bool> operator >(const Type& a, const Vector2<Type>& b)
{
	return vector2(a > b.x, a > b.y);
}


// ----------------------------------------------------------------------------
/// shift left

template <typename Type>
Vector2<Type> operator <<(const Vector2<Type>& a, const Vector2<Type>& b)
{
	return vector2(a.x << b.x, a.y << b.y);
}

template <typename Type>
Vector2<Type> operator <<(const Vector2<Type>& a, const Type& b)
{
	return vector2(a.x << b, a.y << b);
}

template <typename Type>
Vector2<Type> operator <<(const Type& a, const Vector2<Type>& b)
{
	return vector2(a << b.x, a << b.y);
}


// ----------------------------------------------------------------------------
/// shift right

template <typename Type>
Vector2<Type> operator >>(const Vector2<Type>& a, const Vector2<Type>& b)
{
	return vector2(a.x >> b.x, a.y >> b.y);
}

template <typename Type>
Vector2<Type> operator >>(const Vector2<Type>& a, const Type& b)
{
	return vector2(a.x >> b, a.y >> b);
}

template <typename Type>
Vector2<Type> operator >>(const Type& a, const Vector2<Type>& b)
{
	return vector2(a >> b.x, a >> b.y);
}


// ----------------------------------------------------------------------------
/// and

template <typename Type>
Vector2<Type> operator &(const Vector2<Type>& a, const Vector2<Type>& b)
{
	return vector2(a.x & b.x, a.y & b.y);
}

template <typename Type>
Vector2<Type> operator &(const Vector2<Type>& a, const Type& b)
{
	return vector2(a.x & b, a.y & b);
}

template <typename Type>
Vector2<Type> operator &(const Type& a, const Vector2<Type>& b) 
{
	return vector2(a & b.x, a & b.y);
}


// ----------------------------------------------------------------------------
/// or

template <typename Type>
Vector2<Type> operator |(const Vector2<Type>& a, const Vector2<Type>& b)
{
	return vector2(a.x | b.x, a.y | b.y);
}

template <typename Type>
Vector2<Type> operator |(const Vector2<Type>& a, const Type& b)
{
	return vector2(a.x | b, a.y | b);
}

template <typename Type>
Vector2<Type> operator |(const Type& a, const Vector2<Type>& b) 
{
	return vector2(a | b.x, a | b.y);
}


// ----------------------------------------------------------------------------
/// xor

template <typename Type>
Vector2<Type> operator ^(const Vector2<Type>& a, const Vector2<Type>& b)
{
	return vector2(a.x ^ b.x, a.y ^ b.y);
}

template <typename Type>
Vector2<Type> operator ^(const Vector2<Type>& a, const Type& b)
{
	return vector2(a.x ^ b, a.y ^ b);
}

template <typename Type>
Vector2<Type> operator ^(const Type& a, const Vector2<Type>& b) 
{
	return vector2(a ^ b.x, a ^ b.y);
}


// ----------------------------------------------------------------------------
// traits

template <typename T>
struct MathTraits<Vector2<T> >
{
	enum
	{
		COMPOUND = 1
	};

	typedef Vector2<T> Type;
	typedef typename MathTraits<T>::BaseType BaseType;
	typedef T ElementType;
	typedef Vector2<typename MathTraits<T>::RealType> RealType;
};


/// @}

} // namespace digi

#endif
