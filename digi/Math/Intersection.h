#ifndef digi_Math_Intersection_h
#define digi_Math_Intersection_h

#include "Vector3.h"


namespace digi {

/// @addtogroup Math
/// @{

/**
	intersectRayTriangle(orig, dir, vert0, vert1, vert2, tuv) -> bool
	intersectBoxBox(m4x4) -> int
*/


// ----------------------------------------------------------------------------
// intersectRayTriangle(orig, dir, vert0, vert1, vert2, tuv) -> bool

/// test intersection between triangle and ray [Möller Trumbore 2005]
///
/// \param orig    origin of ray
/// \param dir     direction of ray
/// \param vert0-2 the triangle
/// \param tuv     result t (ray parameter) and u,v (triangle parameters)
/// \return        true: success, false: failure
template <typename Type>
bool intersectRayTriangle(Vector3<Type> orig, Vector3<Type> dir,
	Vector3<Type> vert0, Vector3<Type> vert1, Vector3<Type> vert2,
	Vector3<Type>& tuv)
{
	const Type EPSILON = 0.000001f;
	
	// find vectors for two edges sharing vert0
	Vector3<Type> edge1 = vert1 - vert0;
	Vector3<Type> edge2 = vert2 - vert0;

	// begin calculating determinant - also used to calculate U parameter
	Vector3<Type> pvec = cross(dir, edge2);

	// if determinant is near zero, ray lies in plane of triangle
	Type det = dot(edge1, pvec);
	if (det > -EPSILON && det < EPSILON)
		return 0;
	Type inv_det = Type(1) / det;

	// calculate distance from vert0 to ray origin
	Vector3<Type> tvec = orig - vert0;

	// calculate U parameter and test bounds
	tuv.y = dot(tvec, pvec) * inv_det;
	if (tuv.y < 0.0f || tuv.y > 1.0f)
		return false;

	// prepare to test V parameter
	Vector3<Type> qvec = cross(tvec, edge1);

	// calculate V parameter and test bounds
	tuv.z = dot(dir, qvec) * inv_det;
	if (tuv.z < 0.0f || tuv.y + tuv.z > 1.0f)
		return false;

	// calculate t, ray intersects triangle
	tuv.x = dot(edge2, qvec) * inv_det;

	return true;
}


// ----------------------------------------------------------------------------
// intersectBoxBox(m4x4) -> int

/// calc intersection of two boxes. the projectionMatrix transforms the second box into the space of the first box
template <typename Type>
int intersectBoxBox(const DIGI_MATRIX4X4(Type)& projectionMatrix)
{
	int allOutside = 0x3f;
	int anyOutside = 0;

	// iterate over all 8 corners of second box
	for (int i = 0; i < 8; ++i)
	{
		const Type one = 1;

		// transform to homogenous clip space of firt box
		Vector4<Type> p = projectionMatrix * vector4(
			(i & 1) == 0 ? -one : one,
			(i & 2) == 0 ? -one : one,
			(i & 4) == 0 ? -one : one,
			one);
			
		// clip conditions: -w <= x <= w, -w <= y <= w, -w <= z <= w
		int clip = 0;
			
		// left
		if (p.x < -p.w)
			clip |= 0x01;
			
		// right
		if (p.x > p.w)
			clip |= 0x02;
			
		// bottom
		if (p.y < -p.w)
			clip |= 0x04;
			
		// top
		if (p.y > p.w)
			clip |= 0x08;
			
		// near
		if (p.z < -p.w)
			clip |= 0x10;
			
		// far
		if (p.z > p.w)
			clip |= 0x20;
			
		allOutside &= clip;
		anyOutside |= clip;
	}
		
	// return -1 if second box does not intersect first box.
	// otherwise return anyOutside that tells us which side of first box is intersected.
	// note: some cases of non-intersection are not reported
	return allOutside != 0 ? -1 : anyOutside;
}

/// @}

} // namespace digi

#endif
