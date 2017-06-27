#ifndef digi_Math_Operators_h
#define digi_Math_Operators_h

#include <iostream>

#include "Matrix4.h"


namespace digi {

/// @addtogroup Math
/// @{

// override behaviour of std::stream: output int8_t and uint8_t as number, not as char

static inline std::ostream& operator <<(std::ostream& w, int8_t v)
{
	w << int(v);
	return w;
}

static inline std::ostream& operator <<(std::ostream& w, uint8_t v)
{
	w << int(v);
	return w;
}


// vector output operators

template <typename Type>
inline std::ostream& operator <<(std::ostream& w, const Vector2<Type>& v)
{
	w << '[' << v.x << ',' << v.y << ']';
	return w;
}

template <typename Type>
inline std::ostream& operator <<(std::ostream& w, const Vector3<Type>& v)
{
	w << '[' << v.x << ',' << v.y << ',' << v.z << ']';
	return w;
}

template <typename Type>
inline std::ostream& operator <<(std::ostream& w, const Vector4<Type>& v)
{
	w << '[' << v.x << ',' << v.y << ',' << v.z << ',' << v.w << ']';
	return w;
}


// vector input operators

template <typename Type>
inline std::istream& operator >>(std::istream& r, Vector2<Type>& v)
{
	char ch1;
	char ch2;
	char ch3;

	r >> ch1 >> v.x >> ch2 >> v.y >> ch3;
	return r;
}

template <typename Type>
inline std::istream& operator >>(std::istream& r, Vector3<Type>& v)
{
	char ch1;
	char ch2;
	char ch3;
	char ch4;

	r >> ch1 >> v.x >> ch2 >> v.y >> ch3 >> v.z >> ch4;
	return r;
}

template <typename Type>
inline std::istream& operator >>(std::istream& r, Vector4<Type>& v)
{
	char ch1;
	char ch2;
	char ch3;
	char ch4;
	char ch5;

	r >> ch1 >> v.x >> ch2 >> v.y >> ch3 >> v.z >> ch4 >> v.w >> ch5;
	return r;
}


// quaternion output operators

template <typename Type>
inline std::ostream& operator <<(std::ostream& w, const Quaternion<Type>& v)
{
	w << v.v;
	return w;
}


// matrix output operators

template <typename Type>
inline std::ostream& operator <<(std::ostream& w, const Matrix2<Type>& v)
{
	w << '[' << v.x << ',' << v.y << ']';
	return w;
}

template <typename Type>
inline std::ostream& operator <<(std::ostream& w, const Matrix3<Type>& v)
{
	w << '[' << v.x << ',' << v.y << ',' << v.z << ']';
	return w;
}

template <typename Type>
inline std::ostream& operator <<(std::ostream& w, const Matrix4<Type>& v)
{
	w << '[' << v.x << ',' << v.y << ',' << v.z << ',' << v.w << ']';
	return w;
}

/// @}

} // namespace digi

#endif
