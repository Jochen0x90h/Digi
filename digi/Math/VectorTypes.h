#ifndef digi_Math_VectorTypes_h
#define digi_Math_VectorTypes_h

#include <limits>


namespace digi {

/// @addtogroup Math
/// @{

// helper functions for saturate functions (convert_<type>_sat)

template <typename Type1, typename Type2>
Type1 min_rtz(Type2 a, Type1 b)
{
	return a < Type2(b) ? Type1(a) : b;
}

template <typename Type1, typename Type2>
Vector2<Type1> min_rtz(const Vector2<Type2>& a, Type1 b)
{
	return vector2(min_rtz(a.x, b), min_rtz(a.y, b));
}

template <typename Type1, typename Type2>
Vector3<Type1> min_rtz(const Vector3<Type2>& a, Type1 b)
{
	return vector3(min_rtz(a.x, b), min_rtz(a.y, b), min_rtz(a.z, b));
}

template <typename Type1, typename Type2>
Vector4<Type1> min_rtz(const Vector4<Type2>& a, Type1 b)
{
	return vector4(min_rtz(a.x, b), min_rtz(a.y, b), min_rtz(a.z, b), min_rtz(a.w, b));
}


template <typename Type1, typename Type2>
Type1 max_rtz(Type2 a, Type1 b)
{
	return a > Type2(b) ? Type1(a) : b;
}

template <typename Type1, typename Type2>
Vector2<Type1> max_rtz(const Vector2<Type2>& a, Type1 b)
{
	return vector2(min_rtz(a.x, b), min_rtz(a.y, b));
}

template <typename Type1, typename Type2>
Vector3<Type1> max_rtz(const Vector3<Type2>& a, Type1 b)
{
	return vector3(min_rtz(a.x, b), min_rtz(a.y, b), min_rtz(a.z, b));
}

template <typename Type1, typename Type2>
Vector4<Type1> max_rtz(const Vector4<Type2>& a, Type1 b)
{
	return vector4(min_rtz(a.x, b), min_rtz(a.y, b), min_rtz(a.z, b), min_rtz(a.w, b));
}


template <typename Type1, typename Type2>
Type1 clamp_rtz(Type2 a, Type1 b, Type1 c)
{
	return a > Type2(b) ? (a < Type2(c) ? Type1(a) : c ) : b;
}

template <typename Type1, typename Type2>
Vector2<Type1> clamp_rtz(const Vector2<Type2>& a, Type1 b, Type1 c)
{
	return vector2(clamp_rtz(a.x, b, c), clamp_rtz(a.y, b, c));
}

template <typename Type1, typename Type2>
Vector3<Type1> clamp_rtz(const Vector3<Type2>& a, Type1 b, Type1 c)
{
	return vector3(clamp_rtz(a.x, b, c), clamp_rtz(a.y, b, c), clamp_rtz(a.z, b, c));
}

template <typename Type1, typename Type2>
Vector4<Type1> clamp_rtz(const Vector4<Type2>& a, Type1 b, Type1 c)
{
	return vector4(clamp_rtz(a.x, b, c), clamp_rtz(a.y, b, c), clamp_rtz(a.z, b, c), clamp_rtz(a.w, b, c));
}

// helper functions for saturate round to even functions (convert_<type>_sat_rte)

template <typename Type1, typename Type2>
Type1 clamp_rte(Type2 a, Type1 b, Type1 c)
{
	return a > Type2(b) ? (a < Type2(c) ? rint<Type1>(a) : c ) : b;
}

template <typename Type1, typename Type2>
Vector2<Type1> clamp_rte(const Vector2<Type2>& a, Type1 b, Type1 c)
{
	return vector2(clamp_rte(a.x, b, c), clamp_rte(a.y, b, c));
}

template <typename Type1, typename Type2>
Vector3<Type1> clamp_rte(const Vector3<Type2>& a, Type1 b, Type1 c)
{
	return vector3(clamp_rte(a.x, b, c), clamp_rte(a.y, b, c), clamp_rte(a.z, b, c));
}

template <typename Type1, typename Type2>
Vector4<Type1> clamp_rte(const Vector4<Type2>& a, Type1 b, Type1 c)
{
	return vector4(clamp_rte(a.x, b, c), clamp_rte(a.y, b, c), clamp_rte(a.z, b, c), clamp_rte(a.w, b, c));
}

// include auto-generated vector functions
#include "VectorTypes.inc.h"

/// @}

} // namespace digi

#endif
