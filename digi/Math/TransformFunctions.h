#ifndef digi_Math_TransformFunctions_h
#define digi_Math_TransformFunctions_h

#include "Matrix4.h"


namespace digi {

/// @addtogroup Engine
/// @{

/*
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
	matrix4x4LookAt(position3, target3, up3) -> m4x4
	matrix4x4Orthographic(f2, o2, znear, zfar) -> m4x4
	matrix4x4Perspective(f2, o2, znear, zfar) -> m4x4
	matrix4x4Orthographic(left, right, bottom, top, znear, zfar) -> m4x4
	matrix4x4Perspective(left, right, bottom, top, znear, zfar) -> m4x4
	matrix4x4PerspectiveX(fovx, aspect, znear, zfar) -> m4x4
	matrix4x4PerspectiveY(fovy, aspect, znear, zfar) -> m4x4

	matrix3x3Rotate(quaternion) -> m3x3
	matrix3x3Scale(scale) -> m3x3
	matrix3x3Scale(scale3) -> m3x3
	
	matrix3x3Translate(position2) -> m3x3
	matrix3x3TranslateRotate(position2, angle) -> m3x3
	matrix3x3TranslateScale(position2, scale2) -> m3x3
	matrix3x3TranslateRotateScale(position2, angle, scale2) -> m3x3
	matrix3x3TranslateShearScale(position2, shear, scale2) -> m3x3
	matrix3x3TranslateMatrix2x2(position2, m2x2) -> m3x3
	
	matrix2x2Rotate(angle) -> m2x2
	matrix2x2RotateScale(angle, scale2) -> m2x2
	
	getPosition(m4x4) -> translate3
	getMatrix3x3(m4x4) -> m3x3
	getMatrix3x4(m4x4) -> m3x4

	getMatrix2x3(m3x3) -> m2x3
	
	transformPosition(m3x4, position3) -> position3
	transformPosition(m4x4, position3) -> position3
	transformDirection(m3x4, direction3) -> direction3
	transformDirection(m4x4, direction3) -> direction3
	
	getRotation(m3x3) -> quaternion
*/


// ----------------------------------------------------------------------------
// matrix4x4Translate(translate3) -> m4x4

template <typename Type>
DIGI_MATRIX4X4(Type) matrix4x4Translate(const Vector3<Type>& translate)
{
	return matrix4x4(
		Type(1),     Type(0),     Type(0),     Type(0),
		Type(0),     Type(1),     Type(0),     Type(0),
		Type(0),     Type(0),     Type(1),     Type(0),
		translate.x, translate.y, translate.z, Type(1));
}


// ----------------------------------------------------------------------------
// matrix4x4TranslateRotate(translate3, quaternion) -> m4x4

template <typename Type>
DIGI_MATRIX4X4(Type) matrix4x4TranslateRotate(const Vector3<Type>& translate, const Quaternion<Type>& rotate)
{
	Type qxx = rotate.v.x * rotate.v.x;
	Type qyy = rotate.v.y * rotate.v.y;
	Type qzz = rotate.v.z * rotate.v.z;
	Type qww = rotate.v.w * rotate.v.w;

	Type qxy = rotate.v.x * rotate.v.y;
	Type qyz = rotate.v.y * rotate.v.z;
	Type qzx = rotate.v.x * rotate.v.z;

	Type qwx = rotate.v.w * rotate.v.x;
	Type qwy = rotate.v.w * rotate.v.y;
	Type qwz = rotate.v.w * rotate.v.z;

	return matrix4x4(
		qww + qxx - qyy - qzz, 2.0f * (qxy + qwz),    2.0f * (qzx - qwy),    Type(0),
		2.0f * (qxy - qwz),    qww - qxx + qyy - qzz, 2.0f * (qyz + qwx),    Type(0),
		2.0f * (qzx + qwy),    2.0f * (qyz - qwx),    qww - qxx - qyy + qzz, Type(0),
		translate.x,           translate.y,           translate.z,           Type(1));
}


// ----------------------------------------------------------------------------
// matrix4x4TranslateScale(translate3, scale3) -> m4x4

template <typename Type>
DIGI_MATRIX4X4(Type) matrix4x4TranslateScale(const Vector3<Type>& translate, const Vector3<Type>& scale)
{
	return matrix4x4(
		scale.x,     Type(0),     Type(0),     Type(0),
		Type(0),     scale.y,     Type(0),     Type(0),
		Type(0),     Type(0),     scale.z,     Type(0),
		translate.x, translate.y, translate.z, Type(1));
}


// ----------------------------------------------------------------------------
// matrix4x4TranslateRotateScale(translate3, quaternion, scale3) -> m4x4

template <typename Type>
DIGI_MATRIX4X4(Type) matrix4x4TranslateRotateScale(const Vector3<Type>& translate,
	const Quaternion<Type>& rotate, const Vector3<Type>& scale)
{
	Type qxx = rotate.v.x * rotate.v.x;
	Type qyy = rotate.v.y * rotate.v.y;
	Type qzz = rotate.v.z * rotate.v.z;
	Type qww = rotate.v.w * rotate.v.w;

	Type qxy = rotate.v.x * rotate.v.y;
	Type qyz = rotate.v.y * rotate.v.z;
	Type qzx = rotate.v.x * rotate.v.z;

	Type qwx = rotate.v.w * rotate.v.x;
	Type qwy = rotate.v.w * rotate.v.y;
	Type qwz = rotate.v.w * rotate.v.z;

	return matrix4x4(
		(qww + qxx - qyy - qzz) * scale.x, 2.0f * (qxy + qwz) * scale.x,      2.0f * (qzx - qwy) * scale.x,      Type(0),
		2.0f * (qxy - qwz) * scale.y,      (qww - qxx + qyy - qzz) * scale.y, 2.0f * (qyz + qwx) * scale.y,      Type(0),
		2.0f * (qzx + qwy) * scale.z,      2.0f * (qyz - qwx) * scale.z,      (qww - qxx - qyy + qzz) * scale.z, Type(0),
		translate.x,                       translate.y,                       translate.z,                       Type(1));
}


// ----------------------------------------------------------------------------
// matrix4x4TranslateRotateScale(translate3, quaternion, scale) -> m4x4

template <typename Type>
DIGI_MATRIX4X4(Type) matrix4x4TranslateRotateScale(const Vector3<Type>& translate,
	const Quaternion<Type>& rotate, const Type& scale)
{
	return matrix4x4TranslateRotateScale(translate, rotate, splat3(scale));
}


// ----------------------------------------------------------------------------
// matrix4x4TranslateRotateStretch(translate3, quaternion, stretch3x2) -> m4x4

template <typename Type>
DIGI_MATRIX4X4(Type) matrix4x4TranslateRotateStretch(const Vector3<Type>& translate,
	const Quaternion<Type>& rotate, const DIGI_MATRIX3X2(Type)& s)
{
	Type qxx = rotate.v.x * rotate.v.x;
	Type qyy = rotate.v.y * rotate.v.y;
	Type qzz = rotate.v.z * rotate.v.z;
	Type qww = rotate.v.w * rotate.v.w;

	Type qxy = rotate.v.x * rotate.v.y;
	Type qyz = rotate.v.y * rotate.v.z;
	Type qzx = rotate.v.x * rotate.v.z;

	Type qwx = rotate.v.w * rotate.v.x;
	Type qwy = rotate.v.w * rotate.v.y;
	Type qwz = rotate.v.w * rotate.v.z;

	Type m11 = qww + qxx - qyy - qzz;
	Type m12 = 2.0f * (qxy - qwz);
	Type m13 = 2.0f * (qzx + qwy);
	
	Type m21 = 2.0f * (qxy + qwz);
	Type m22 = qww - qxx + qyy - qzz;
	Type m23 = 2.0f * (qyz - qwx);
	
	Type m31 = 2.0f * (qzx - qwy);
	Type m32 = 2.0f * (qyz + qwx);
	Type m33 = qww - qxx - qyy + qzz;

	/*
		the stretch-factor represents a symmetric 3x3 matrix:
		x.x y.x y.y
		y.x x.y y.z
		y.y y.z x.z
	*/
	return matrix4x4(
		m11*s.x.x + m12*s.y.x + m13*s.y.y, m21*s.x.x + m22*s.y.x + m23*s.y.y, m31*s.x.x + m32*s.y.x + m33*s.y.y, Type(0),
		m11*s.y.x + m12*s.x.y + m13*s.y.z, m21*s.y.x + m22*s.x.y + m23*s.y.z, m31*s.y.x + m32*s.x.y + m33*s.y.z, Type(0),
		m11*s.y.y + m12*s.y.z + m13*s.x.z, m21*s.y.y + m22*s.y.z + m23*s.x.z, m31*s.y.y + m32*s.y.z + m33*s.x.z, Type(0),
		translate.x,                       translate.y,                       translate.z,                       Type(1));
}


// ----------------------------------------------------------------------------
// matrix4x4TranslateShear(translate3, shear3) -> m4x4

template <typename Type>
DIGI_MATRIX4X4(Type) matrix4x4TranslateShear(const Vector3<Type>& translate,
	const Vector3<Type>& shear)
{
	return matrix4x4(
		Type(1),     Type(0),     Type(0),     Type(0),
		shear.x,     Type(1),     Type(0),     Type(0),
		shear.y,     shear.z,     Type(1),     Type(0),
		translate.x, translate.y, translate.z, Type(1));
}


// ----------------------------------------------------------------------------
// matrix4x4TranslateShearScale(translate3, shear3, scale3) -> m4x4

template <typename Type>
DIGI_MATRIX4X4(Type) matrix4x4TranslateShearScale(const Vector3<Type>& translate,
	const Vector3<Type>& shear, const Vector3<Type>& scale)
{
	return matrix4x4(
		scale.x,           Type(0),           Type(0),     Type(0),
		shear.x * scale.y, scale.y,           Type(0),     Type(0),
		shear.y * scale.z, shear.z * scale.z, scale.z,     Type(0),
		translate.x,       translate.y,       translate.z, Type(1));
}


// ----------------------------------------------------------------------------
// matrix4x4TranslateMatrix3x3(translate3, m3x3) -> m4x4

template <typename Type>
DIGI_MATRIX4X4(Type) matrix4x4TranslateMatrix3x3(const Vector3<Type>& translate, const DIGI_MATRIX3X3(Type)& m)
{
	return matrix4x4(
		m.x.x,       m.x.y,       m.x.z,       Type(0),
		m.y.x,       m.y.y,       m.y.z,       Type(0),
		m.z.x,       m.z.y,       m.z.z,       Type(0),
		translate.x, translate.y, translate.z, Type(1));
}


// ----------------------------------------------------------------------------
// matrix4x4LookAt(position3, target3, up3) -> m4x4

/// creates a view matrix that looks from a position at a target point with given up vector. equivalent to gluLookAt
template <typename Type>
DIGI_MATRIX4X4(Type) matrix4x4LookAt(const Vector3<Type>& position, const Vector3<Type>& target, const Vector3<Type>& up)
{
	Vector3<Type> z = normalize(position - target);
	Vector3<Type> x = normalize(cross(up, z));
	Vector3<Type> y = cross(z, x);

	return matrix4x4(
		x.x, y.x, z.x, Type(0),
		x.y, y.y, z.y, Type(0),
		x.z, y.z, z.z, Type(0),
		-dot(x, position), -dot(y, position), -dot(z, position), Type(1));
}


// ----------------------------------------------------------------------------
// matrix4x4Orthographic(f2, o2, znear, zfar) -> m4x4

/// creates an orthographic projection matrix. f is orthographic width per film size, o is film offset per film size.
/// near plane is at z = -znear, far plane is at z = -zfar
template <typename Type>
DIGI_MATRIX4X4(Type) matrix4x4Orthographic(const Vector2<Type>& f, const Vector2<Type>& o, Type znear, Type zfar)
{
	Type _n_f = 1.0f / (znear - zfar);
	return matrix4x4(
		f.x,     Type(0), Type(0),               Type(0),
		Type(0), f.y,     Type(0),               Type(0),
		Type(0), Type(0), 2.0f * _n_f,           Type(0),
		o.x,     o.y,     (zfar + znear) * _n_f, Type(1));
}


// ----------------------------------------------------------------------------
// matrix4x4Perspective(f2, o2, znear, zfar) -> m4x4

/// creates a perspective projection matrix. f is focal length per film size, o is film offset per film size.
/// near plane is at z = -znear, far plane is at z = -zfar
template <typename Type>
DIGI_MATRIX4X4(Type) matrix4x4Perspective(const Vector2<Type>& f, const Vector2<Type>& o, Type znear, Type zfar)
{
	Type _n_f = 1.0f / (znear - zfar);
	return matrix4x4(
		f.x,     Type(0), Type(0),                    Type(0),
		Type(0), f.y,     Type(0),                    Type(0),
		o.x,     o.y,     (zfar + znear) * _n_f,      -Type(1),
		Type(0), Type(0), 2.0f * zfar * znear * _n_f, Type(0));
}


// ----------------------------------------------------------------------------
// matrix4x4Orthographic(left, right, bottom, top, znear, zfar) -> m4x4

/// creates an orthographic projection matrix like glOrtho.
/// near plane is at z = -znear, far plane is at z = -zfar
template <typename Type>
DIGI_MATRIX4X4(Type) matrix4x4Orthographic(Type left, Type right, Type bottom, Type top, Type znear, Type zfar)
{
	Type _r_l = 1.0f / (right - left);
	Type _t_b = 1.0f / (top - bottom);
	Type _n_f = 1.0f / (znear - zfar);
	return matrix4x4(
		2.0f * _r_l,            Type(0),                Type(0),               Type(0),
		Type(0),                2.0f * _t_b,            Type(0),               Type(0),
		Type(0),                Type(0),                2.0f * _n_f,           Type(0),
		-(right + left) * _r_l, -(top + bottom) * _t_b, (zfar + znear) * _n_f, Type(1));
}


// ----------------------------------------------------------------------------
// matrix4x4Perspective(left, right, bottom, top, znear, zfar) -> m4x4

/// creates a perspective projection matrix like glFrustum.
/// left, right, bottom and top are on near plane which is at z = -znear. far plane is at z = -zfar
template <typename Type>
DIGI_MATRIX4X4(Type) matrix4x4Perspective(Type left, Type right, Type bottom, Type top, Type znear, Type zfar)
{
	Type _r_l = 1.0f / (right - left);
	Type _t_b = 1.0f / (top - bottom);
	Type _n_f = 1.0f / (znear - zfar);
	return matrix4x4(
		2.0f * znear * _r_l,    Type(0),              Type(0),                    Type(0),
		Type(0),                2.0f * znear * _t_b,  Type(0),                    Type(0),
		(right + left) * _r_l, (top + bottom) * _t_b, (zfar + znear) * _n_f,      -Type(1),
		Type(0),               Type(0),               2.0f * zfar * znear * _n_f, Type(0));
}


// ----------------------------------------------------------------------------
// matrix4x4PerspectiveX(fovx, aspect, znear, zfar) -> m4x4

/// creates a perspective projection matrix. fovx is field of view in degree along x-axis
/// near plane is at z = -znear, far plane is at z = -zfar
template <typename Type>
DIGI_MATRIX4X4(Type) matrix4x4PerspectiveX(Type fovx, Type aspect, Type znear, Type zfar)
{
	Type f = 1.0f / tan(fovx * 0.0087266f); // 0.5 * pi / 180
	Type _n_f = 1.0f / (znear - zfar);
	return matrix4x4(
		f,       Type(0),    Type(0),                    Type(0),
		Type(0), f * aspect, Type(0),                    Type(0),
		Type(0), Type(0),    (zfar + znear) * _n_f,      -Type(1),
		Type(0), Type(0),    2.0f * zfar * znear * _n_f, Type(0));
}


// ----------------------------------------------------------------------------
// matrix4x4PerspectiveY(fovy, aspect, znear, zfar) -> m4x4

/// creates a perspective projection matrix like gluPerspective. fovy is field of view in degree along y-axis
/// near plane is at z = -znear, far plane is at z = -zfar
template <typename Type>
DIGI_MATRIX4X4(Type) matrix4x4PerspectiveY(Type fovy, Type aspect, Type znear, Type zfar)
{
	Type f = 1.0f / tan(fovy * 0.0087266f); // 0.5 * pi / 180
	Type _n_f = 1.0f / (znear - zfar);
	return matrix4x4(
		f / aspect, Type(0), Type(0),                    Type(0),
		Type(0),    f,       Type(0),                    Type(0),
		Type(0),    Type(0), (zfar + znear) * _n_f,      -Type(1),
		Type(0),    Type(0), 2.0f * zfar * znear * _n_f, Type(0));
}


// ----------------------------------------------------------------------------
// matrix3x3Rotate(quaternion) -> m3x3

/// constructs a Matrix3x3 from a rotation quaternion
template <typename Type>
DIGI_MATRIX3X3(Type) matrix3x3Rotate(const Quaternion<Type>& rotate)
{
	Type qxx = rotate.v.x * rotate.v.x;
	Type qyy = rotate.v.y * rotate.v.y;
	Type qzz = rotate.v.z * rotate.v.z;
	Type qww = rotate.v.w * rotate.v.w;

	Type qxy = rotate.v.x * rotate.v.y;
	Type qyz = rotate.v.y * rotate.v.z;
	Type qzx = rotate.v.x * rotate.v.z;

	Type qwx = rotate.v.w * rotate.v.x;
	Type qwy = rotate.v.w * rotate.v.y;
	Type qwz = rotate.v.w * rotate.v.z;

	return matrix3x3(
		qww + qxx - qyy - qzz,  2.0f * (qxy + qwz),     2.0f * (qzx - qwy),
		 2.0f * (qxy - qwz),    qww - qxx + qyy - qzz,  2.0f * (qyz + qwx),
		 2.0f * (qzx + qwy),    2.0f * (qyz - qwx),     qww - qxx - qyy + qzz);
}


// ----------------------------------------------------------------------------
// matrix3x3Scale(scale) -> m3x3

/// constructs a Matrix3x3 from an uniform scale
template <typename Type>
DIGI_MATRIX3X3(Type) matrix3x3Scale(const Type& scale)
{
	return matrix3x3(
		scale,   Type(0), Type(0),
		Type(0), scale,   Type(0),
		Type(0), Type(0), scale);
}


// ----------------------------------------------------------------------------
// matrix3x3Scale(scale3) -> m3x3

/// constructs a Matrix3x3 from a nonuniform scale
template <typename Type>
DIGI_MATRIX3X3(Type) matrix3x3Scale(const Vector3<Type>& scale)
{
	return matrix3x3(
		scale.x, Type(0), Type(0),
		Type(0), scale.y, Type(0),
		Type(0), Type(0), scale.z);
}


// ----------------------------------------------------------------------------
/// matrix3x3Translate(position2) -> m3x3

template <typename Type>
DIGI_MATRIX3X3(Type) matrix3x3Translate(const Vector2<Type>& translate)
{
	return matrix3x3(
		Type(1), Type(0), Type(0),
		Type(0), Type(1), Type(0),
		translate.x, translate.y, Type(1));
}


// ----------------------------------------------------------------------------
/// matrix3x3TranslateRotate(position2, angle) -> m3x3

template <typename Type>
DIGI_MATRIX3X3(Type) matrix3x3TranslateRotate(const Vector2<Type>& translate,
	Type rotate)
{
	Type c = cos(rotate);
	Type s = sin(rotate);
	return matrix3x3(
		c,  s, Type(0),
		-s, c, Type(0),
		translate.x, translate.y, Type(1));
}


// ----------------------------------------------------------------------------
/// matrix3x3TranslateScale(position2, scale2) -> m3x3

template <typename Type>
DIGI_MATRIX3X3(Type) matrix3x3TranslateScale(const Vector2<Type>& translate,
	const Vector2<Type>& scale)
{
	return matrix3x3(
		scale.x, Type(0), Type(0),
		Type(0), scale.y, Type(0),
		translate.x, translate.y, Type(1));
}


// ----------------------------------------------------------------------------
/// matrix3x3TranslateRotateScale(position2, angle, scale2) -> m3x3

template <typename Type>
DIGI_MATRIX3X3(Type) matrix3x3TranslateRotateScale(const Vector2<Type>& translate,
	Type rotate, const Vector2<Type>& scale)
{
	Type c = cos(rotate);
	Type s = sin(rotate);
	return matrix3x3(
		scale.x * c,  scale.x * s, Type(0),
		scale.y * -s, scale.y * c, Type(0),
		translate.x,  translate.y, Type(1));
}


// ----------------------------------------------------------------------------
/// matrix3x3TranslateShearScale(position2, shear, scale2) -> m3x3

template <typename Type>
DIGI_MATRIX3X3(Type) matrix3x3TranslateShearScale(const Vector2<Type>& translate,
	Type shear, const Vector2<Type>& scale)
{
	return matrix3x3(
		scale.x,         Type(0),     Type(0),
		shear * scale.y, scale.y,     Type(0),
		translate.x,     translate.y, Type(1));
}


// ----------------------------------------------------------------------------
/// matrix3x3TranslateMatrix2x2(position2, m2x2) -> m3x3

template <typename Type>
DIGI_MATRIX3X3(Type) matrix3x3TranslateMatrix2x2(const Vector2<Type>& translate,
	const DIGI_MATRIX2X2(Type)& m)
{
	return matrix3x3(
		m.x.x, m.x.y, Type(0),
		m.y.x, m.y.y, Type(0),
		translate.x, translate.y, Type(1));
}


// ----------------------------------------------------------------------------
/// matrix2x2Rotate(angle) -> m2x2

template <typename Type>
DIGI_MATRIX2X2(Type) matrix2x2Rotate(Type rotate)
{
	Type c = cos(rotate);
	Type s = sin(rotate);
	return matrix2x2(
		c,  s,
		-s, c);
}


// ----------------------------------------------------------------------------
/// matrix2x2RotateScale(angle, scale2) -> m2x2

template <typename Type>
DIGI_MATRIX2X2(Type) matrix2x2RotateScale(Type rotate, const Vector2<Type>& scale)
{
	Type c = cos(rotate);
	Type s = sin(rotate);
	return matrix2x2(
		scale.x * c,  scale.x * s,
		scale.y * -s, scale.y * c);
}


template <typename Type>
Vector3<Type> getPosition(const DIGI_MATRIX4X4(Type)& m)
{
	return getXYZ(m.w);
}

template <typename Type>
DIGI_MATRIX3X3(Type) getMatrix3x3(const DIGI_MATRIX4X4(Type)& m)
{
	return matrix3(
		getXYZ(m.x),
		getXYZ(m.y),
		getXYZ(m.z));
}

template <typename Type>
DIGI_MATRIX3X4(Type) getMatrix3x4(const DIGI_MATRIX4X4(Type)& a)
{
	return matrix4(
		getXYZ(a.x),
		getXYZ(a.y),
		getXYZ(a.z),
		getXYZ(a.w));
}


// ----------------------------------------------------------------------------
/// getMatrix2x3(m3x3) -> m2x3

template <typename Type>
DIGI_MATRIX2X3(Type) getMatrix2x3(const DIGI_MATRIX3X3(Type)& a)
{
	return matrix3(
		getXY(a.x),
		getXY(a.y),
		getXY(a.z));
}


// ----------------------------------------------------------------------------
/// transformPosition(m3x4, position3) -> position3

template <typename Type>
Vector3<Type> transformPosition(const DIGI_MATRIX3X4(Type)& m, const Vector3<Type>& position)
{
	return m.x * position.x + m.y * position.y + m.z * position.z + m.w;
}


// ----------------------------------------------------------------------------
/// transformPosition(m4x4, position3) -> position3

template <typename Type>
Vector3<Type> transformPosition(const DIGI_MATRIX4X4(Type)& m, const Vector3<Type>& position)
{
	return getXYZ(m.x) * position.x + getXYZ(m.y) * position.y + getXYZ(m.z) * position.z + getXYZ(m.w);
}


// ----------------------------------------------------------------------------
/// transformDirection(m3x4, direction3) -> direction3

template <typename Type>
Vector3<Type> transformDirection(const DIGI_MATRIX3X4(Type)& m, const Vector3<Type>& position)
{
	return m.x * position.x + m.y * position.y + m.z * position.z;
}


// ----------------------------------------------------------------------------
/// transformDirection(m4x4, direction3) -> direction3

template <typename Type>
Vector3<Type> transformDirection(const DIGI_MATRIX4X4(Type)& m, const Vector3<Type>& position)
{
	return getXYZ(m.x) * position.x + getXYZ(m.y) * position.y + getXYZ(m.z) * position.z;
}


// ----------------------------------------------------------------------------
/// getRotation(m3x3) -> quaternion

template <typename Type>
Quaternion<Type> getRotation(const DIGI_MATRIX3X3(Type)& m)
{
	Type trace = m.x.x + m.y.y + m.z.z;
	if (trace > 0)
	{
		Type s = 0.5f / sqrt(trace + 1.0f);
		return quaternion(
			(m.y.z - m.z.y) * s,
			(m.z.x - m.x.z) * s,
			(m.x.y - m.y.x) * s,
			0.25f / s);
	}
	if (m.x.x > m.y.y && m.x.x > m.z.z)
	{
		Type s = 2.0f * sqrt(1.0f + m.x.x - m.y.y - m.z.z);
		return quaternion(
			0.25f * s,
			(m.y.x + m.x.y) / s,
			(m.z.x + m.x.z) / s,
			(m.y.z - m.z.y) / s);
	}
	if (m.y.y > m.z.z)
	{
		Type s = 2.0f * sqrt(1.0f + m.y.y - m.x.x - m.z.z);
		return quaternion(
			(m.y.x + m.x.y) / s,
			0.25f * s,
			(m.z.y + m.y.z) / s,
			(m.z.x - m.x.z) / s);
	}

	Type s = 2.0f * sqrt(1.0f + m.z.z - m.x.x - m.y.y);
	return quaternion(
		(m.z.x + m.x.z) / s,
		(m.z.y + m.y.z) / s,
		0.25f * s,
		(m.x.y - m.y.x) / s);
}

/// @}

} // namespace digi

#endif
