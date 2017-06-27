/**
	intersectTriangle(orig, dir, vert0, vert1, vert2, tuv)
*/



/// test intersection between triangle and ray [Möller Trumbore 2005]
///
/// \param orig    origin of ray
/// \param dir     direction of ray
/// \param vert0-2 the triangle
/// \param tuv     result t (ray parameter) and u,v (triangle parameters)
/// \return        true: success, false: failure
inline bool intersectTriangle(float3 orig, float3 dir,
	float3 vert0, float3 vert1, float3 vert2,
	float3& tuv)
{
	float3 edge1, edge2, tvec, pvec, qvec;
	float det, inv_det;
	const float EPSILON = 0.000001f;
	
	// find vectors for two edges sharing vert0
	edge1 = vert1 - vert0;
	edge2 = vert2 - vert0;

	// begin calculating determinant - also used to calculate U parameter
	pvec = cross(dir, edge2);

	// if determinant is near zero, ray lies in plane of triangle
	det = dot(edge1, pvec);
	if (det > -EPSILON && det < EPSILON)
		return 0;
	inv_det = 1.0f / det;

	// calculate distance from vert0 to ray origin
	tvec = orig - vert0;

	// calculate U parameter and test bounds
	tuv.y = dot(tvec, pvec) * inv_det;
	if (tuv.y < 0.0f || tuv.y > 1.0f)
		return false;

	// prepare to test V parameter
	qvec = cross(tvec, edge1);

	// calculate V parameter and test bounds
	tuv.z = dot(dir, qvec) * inv_det;
	if (tuv.z < 0.0f || tuv.y + tuv.z > 1.0f)
		return false;

	// calculate t, ray intersects triangle
	tuv.x = dot(edge2, qvec) * inv_det;

	return true;
}
