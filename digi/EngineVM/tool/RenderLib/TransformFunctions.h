/**
	this file contains a collection of transform related functions 

	matrix4x4Translate(translate3) -> m4x4
	matrix4x4TranslateRotate(translate3, quaternion) -> m4x4
	matrix4x4TranslateScale(translate3, scale3) -> m4x4
	matrix4x4TranslateRotateScale(translate3, quaternion, scale3) -> m4x4
	matrix4x4TranslateRotateScale(translate3, quaternion, scale) -> m4x4
	matrix4x4TranslateRotateStretch(translate3, quaternion, stretch3x2) -> m4x4
	matrix4x4TranslateShear(translate3, shear3) -> m4x4
	matrix4x4TranslateShearScale(translate3, shear3, scale3) -> m4x4
	matrix4x4TranslateMatrix3x3(translate3, m3x3) -> m4x4
	
	matrix3x3Rotate(quaternion) -> m3x3
	matrix3x3Scale(scale) -> m3x3
	matrix3x3Scale(scale3) -> m3x3

	matrix3x3Translate(position2) -> m3x3
	matrix3x3TranslateRotate(position2, angle) -> m3x3
	matrix3x3TranslateScale(position2, scale2) -> m3x3
	matrix3x3TranslateRotateScale(position2, angle, scale2) -> m3x3
	matrix3x3TranslateShearScale(position2, shear, scale2) -> m3x3
	matrix3x3TranslateMatrix2x2(position2, m2x2) -> m3x3

	getMatrix3x3(m4x4) -> m3x3

	getMatrix2x3(m3x3) -> m2x3

	transformPosition(m3x4, position3) -> position3
	transformPosition(m4x4, position3) -> position3
	transformDirection(m3x4, direction3) -> direction3
	transformDirection(m4x4, direction3) -> direction3

	getRotation(m3x3) -> quaternion
*/

// ----------------------------------------------------------------------------
/// matrix3x3Translate(position2) -> m3x3

inline MATRIX3X3 matrix3x3Translate(VECTOR2 translate)
{
	return matrix3x3(
		ONE,         ZERO,        ZERO,
		ZERO,        ONE,         ZERO,
		translate.x, translate.y, ONE);
}


// ----------------------------------------------------------------------------
/// matrix3x3TranslateRotate(position2, angle) -> m3x3

inline MATRIX3X3 matrix3x3TranslateRotate(VECTOR2 translate,
	SCALAR rotate)
{
	SCALAR c = cos(rotate);
	SCALAR s = sin(rotate);
	return matrix3x3(
		c,           s,           ZERO,
		-s,          c,           ZERO,
		translate.x, translate.y, ONE);
}


// ----------------------------------------------------------------------------
/// matrix3x3TranslateScale(position2, scale2) -> m3x3

inline MATRIX3X3 matrix3x3TranslateScale(VECTOR2 translate,
	VECTOR2 scale)
{
	return matrix3x3(
		scale.x,     ZERO,        ZERO,
		ZERO,        scale.y,     ZERO,
		translate.x, translate.y, ONE);
}


// ----------------------------------------------------------------------------
/// matrix3x3TranslateRotateScale(position2, angle, scale2) -> m3x3

inline MATRIX3X3 matrix3x3TranslateRotateScale(VECTOR2 translate,
	SCALAR rotate, VECTOR2 scale)
{
	SCALAR c = cos(rotate);
	SCALAR s = sin(rotate);
	return matrix3x3(
		scale.x * c,  scale.x * s, ZERO,
		scale.y * -s, scale.y * c, ZERO,
		translate.x,  translate.y, ONE);
}

// ----------------------------------------------------------------------------
// matrix4x4Translate(translate3) -> m4x4

inline MATRIX4X4 matrix4x4Translate(VECTOR3 translate)
{
	return matrix4x4(
		ONE,         ZERO,        ZERO,        ZERO,
		ZERO,        ONE,         ZERO,        ZERO,
		ZERO,        ZERO,        ONE,         ZERO,
		translate.x, translate.y, translate.z, ONE);
}


// ----------------------------------------------------------------------------
// matrix4x4TranslateRotate(translate3, quaternion) -> m4x4

inline MATRIX4X4 matrix4x4TranslateRotate(VECTOR3 translate, QUATERNION rotate)
{
	SCALAR qxx = rotate.v.x * rotate.v.x;
	SCALAR qyy = rotate.v.y * rotate.v.y;
	SCALAR qzz = rotate.v.z * rotate.v.z;
	SCALAR qww = rotate.v.w * rotate.v.w;

	SCALAR qxy = rotate.v.x * rotate.v.y;
	SCALAR qyz = rotate.v.y * rotate.v.z;
	SCALAR qzx = rotate.v.x * rotate.v.z;

	SCALAR qwx = rotate.v.w * rotate.v.x;
	SCALAR qwy = rotate.v.w * rotate.v.y;
	SCALAR qwz = rotate.v.w * rotate.v.z;

	return matrix4x4(
		qww + qxx - qyy - qzz, 2.0f * (qxy + qwz),    2.0f * (qzx - qwy),    ZERO,
		2.0f * (qxy - qwz),    qww - qxx + qyy - qzz, 2.0f * (qyz + qwx),    ZERO,
		2.0f * (qzx + qwy),    2.0f * (qyz - qwx),    qww - qxx - qyy + qzz, ZERO,
		translate.x,           translate.y,           translate.z,           ONE);
}


// ----------------------------------------------------------------------------
// matrix4x4TranslateScale(translate3, scale3) -> m4x4

inline MATRIX4X4 matrix4x4TranslateScale(VECTOR3 translate, VECTOR3 scale)
{
	return matrix4x4(
		scale.x,     ZERO,        ZERO,        ZERO,
		ZERO,        scale.y,     ZERO,        ZERO,
		ZERO,        ZERO,        scale.z,     ZERO,
		translate.x, translate.y, translate.z, ONE);
}


// ----------------------------------------------------------------------------
// matrix4x4TranslateRotateScale(translate3, quaternion, scale3) -> m4x4

inline MATRIX4X4 matrix4x4TranslateRotateScale(VECTOR3 translate,
	QUATERNION rotate, VECTOR3 scale)
{
	SCALAR qxx = rotate.v.x * rotate.v.x;
	SCALAR qyy = rotate.v.y * rotate.v.y;
	SCALAR qzz = rotate.v.z * rotate.v.z;
	SCALAR qww = rotate.v.w * rotate.v.w;

	SCALAR qxy = rotate.v.x * rotate.v.y;
	SCALAR qyz = rotate.v.y * rotate.v.z;
	SCALAR qzx = rotate.v.x * rotate.v.z;

	SCALAR qwx = rotate.v.w * rotate.v.x;
	SCALAR qwy = rotate.v.w * rotate.v.y;
	SCALAR qwz = rotate.v.w * rotate.v.z;

	return matrix4x4(
		(qww + qxx - qyy - qzz) * scale.x, 2.0f * (qxy + qwz) * scale.x,      2.0f * (qzx - qwy) * scale.x,      ZERO,
		2.0f * (qxy - qwz) * scale.y,      (qww - qxx + qyy - qzz) * scale.y, 2.0f * (qyz + qwx) * scale.y,      ZERO,
		2.0f * (qzx + qwy) * scale.z,      2.0f * (qyz - qwx) * scale.z,      (qww - qxx - qyy + qzz) * scale.z, ZERO,
		translate.x,                       translate.y,                       translate.z,                       ONE);
}


// ----------------------------------------------------------------------------
// matrix4x4TranslateRotateScale(translate3, quaternion, scale) -> m4x4

inline MATRIX4X4 matrix4x4TranslateRotateScale(VECTOR3 translate,
	QUATERNION rotate, SCALAR scale)
{
	return matrix4x4TranslateRotateScale(translate, rotate, vector3(scale, scale, scale));
}


// ----------------------------------------------------------------------------
// matrix4x4TranslateRotateStretch(translate3, quaternion, stretch3x2) -> m4x4

// ----------------------------------------------------------------------------
// matrix4x4TranslateShear(translate3, shear3) -> m4x4

inline MATRIX4X4 matrix4x4TranslateShear(VECTOR3 translate,
	VECTOR3 shear)
{
	return matrix4x4(
		ONE,         ZERO,        ZERO,        ZERO,
		shear.x,     ONE,         ZERO,        ZERO,
		shear.y,     shear.z,     ONE,         ZERO,
		translate.x, translate.y, translate.z, ONE);
}


// ----------------------------------------------------------------------------
// matrix4x4TranslateShearScale(translate3, shear3, scale3) -> m4x4

inline MATRIX4X4 matrix4x4TranslateShearScale(VECTOR3 translate,
	VECTOR3 shear, VECTOR3 scale)
{
	return matrix4x4(
		scale.x,           ZERO,              ZERO,        ZERO,
		shear.x * scale.y, scale.y,           ZERO,        ZERO,
		shear.y * scale.z, shear.z * scale.z, scale.z,     ZERO,
		translate.x,       translate.y,       translate.z, ONE);
}


// ----------------------------------------------------------------------------
// matrix4x4TranslateMatrix3x3(translate3, m3x3) -> m4x4

inline MATRIX4X4 matrix4x4TranslateMatrix3x3(VECTOR3 translate, MATRIX3X3 m)
{
	return matrix4x4(
		m.x.x,       m.x.y,       m.x.z,       ZERO,
		m.y.x,       m.y.y,       m.y.z,       ZERO,
		m.z.x,       m.z.y,       m.z.z,       ZERO,
		translate.x, translate.y, translate.z, ONE);
}


// ----------------------------------------------------------------------------
// matrix3x3Rotate(quaternion) -> m3x3

/// constructs a Matrix3x3 from a rotation quaternion
inline MATRIX3X3 matrix3x3Rotate(QUATERNION rotate)
{
	SCALAR qxx = rotate.v.x * rotate.v.x;
	SCALAR qyy = rotate.v.y * rotate.v.y;
	SCALAR qzz = rotate.v.z * rotate.v.z;
	SCALAR qww = rotate.v.w * rotate.v.w;

	SCALAR qxy = rotate.v.x * rotate.v.y;
	SCALAR qyz = rotate.v.y * rotate.v.z;
	SCALAR qzx = rotate.v.x * rotate.v.z;

	SCALAR qwx = rotate.v.w * rotate.v.x;
	SCALAR qwy = rotate.v.w * rotate.v.y;
	SCALAR qwz = rotate.v.w * rotate.v.z;

	return matrix3x3(
		qww + qxx - qyy - qzz, 2.0f * (qxy + qwz),    2.0f * (qzx - qwy),
		2.0f * (qxy - qwz),    qww - qxx + qyy - qzz, 2.0f * (qyz + qwx),
		2.0f * (qzx + qwy),    2.0f * (qyz - qwx),    qww - qxx - qyy + qzz);
}


// ----------------------------------------------------------------------------
/// matrix3x3Scale(scale) -> m3x3

inline MATRIX3X3 matrix3x3Scale(SCALAR scale)
{
	return matrix3x3(
		scale,  ZERO,   ZERO,
		ZERO,   scale,  ZERO,
		ZERO,   ZERO,   scale);
}


// ----------------------------------------------------------------------------
/// matrix3x3Scale(scale3) -> m3x3

inline MATRIX3X3 matrix3x3Scale(VECTOR3 scale)
{
	return matrix3x3(
		scale.x, ZERO,    ZERO,
		ZERO,    scale.y, ZERO,
		ZERO,    ZERO,    scale.z);
}


// ----------------------------------------------------------------------------
/// matrix3x3TranslateShearScale(position2, shear, scale2) -> m3x3

inline MATRIX3X3 matrix3x3TranslateShearScale(VECTOR2 translate,
	SCALAR shear, VECTOR2 scale)
{
	return matrix3x3(
		scale.x,         ZERO,        ZERO,
		shear * scale.y, scale.y,     ZERO,
		translate.x,     translate.y, ONE);
}


// ----------------------------------------------------------------------------
/// matrix3x3TranslateMatrix2x2(position2, m2x2) -> m3x3

inline MATRIX3X3 matrix3x3TranslateMatrix2x2(VECTOR2 translate,
	MATRIX2X2 m)
{
	return matrix3x3(
		m.x.x,       m.x.y,       ZERO,
		m.y.x,       m.y.y,       ZERO,
		translate.x, translate.y, ONE);
}


// ----------------------------------------------------------------------------
/// getMatrix3x3(m4x4) -> m3x3

inline MATRIX3X3 getMatrix3x3(MATRIX4X4 a)
{
	return matrix3(
		a.x.xyz,
		a.y.xyz,
		a.z.xyz);
}

// ----------------------------------------------------------------------------
/// getMatrix2x3(m3x3) -> m2x3

inline MATRIX2X3 getMatrix2x3(MATRIX3X3 a)
{
	return matrix3(
		a.x.xy,
		a.y.xy,
		a.z.xy);
}


// ----------------------------------------------------------------------------
/// transformPosition(m3x4, position3) -> position3

inline VECTOR3 transformPosition(MATRIX3X4 m, VECTOR3 position)
{
	return m.x * position.x + m.y * position.y + m.z * position.z + m.w;
}


// ----------------------------------------------------------------------------
/// transformPosition(m4x4, position3) -> position3

inline VECTOR3 transformPosition(MATRIX4X4 m, VECTOR3 position)
{
	return m.x.xyz * position.x + m.y.xyz * position.y + m.z.xyz * position.z + m.w.xyz;
}


// ----------------------------------------------------------------------------
/// transformDirection(m3x4, direction3) -> direction3

inline VECTOR3 transformDirection(MATRIX3X4 m, VECTOR3 position)
{
	return m.x * position.x + m.y * position.y + m.z * position.z;
}


// ----------------------------------------------------------------------------
/// transformDirection(m4x4, direction3) -> direction3

inline VECTOR3 transformDirection(MATRIX4X4 m, VECTOR3 position)
{
	return m.x.xyz * position.x + m.y.xyz * position.y + m.z.xyz * position.z;
}


// ----------------------------------------------------------------------------
/// getRotation(m3x3) -> quaternion

inline QUATERNION getRotation(MATRIX3X3 m)
{
	SCALAR trace = m.x.x + m.y.y + m.z.z;
	if (trace > 0)
	{
		SCALAR s = 0.5f / sqrt(trace + 1.0f);
		return quaternion(
			(m.y.z - m.z.y) * s,
			(m.z.x - m.x.z) * s,
			(m.x.y - m.y.x) * s,
			0.25f / s);
	}
	if (m.x.x > m.y.y && m.x.x > m.z.z)
	{
		SCALAR s = 2.0f * sqrt(1.0f + m.x.x - m.y.y - m.z.z);
		return quaternion(
			0.25f * s,
			(m.y.x + m.x.y) / s,
			(m.z.x + m.x.z) / s,
			(m.y.z - m.z.y) / s);
	}
	if (m.y.y > m.z.z)
	{
		SCALAR s = 2.0f * sqrt(1.0f + m.y.y - m.x.x - m.z.z);
		return quaternion(
			(m.y.x + m.x.y) / s,
			0.25f * s,
			(m.z.y + m.y.z) / s,
			(m.z.x - m.x.z) / s);
	}

	SCALAR s = 2.0f * sqrt(1.0f + m.z.z - m.x.x - m.y.y);
	return quaternion(
		(m.z.x + m.x.z) / s,
		(m.z.y + m.y.z) / s,
		0.25f * s,
		(m.x.y - m.y.x) / s);
}
