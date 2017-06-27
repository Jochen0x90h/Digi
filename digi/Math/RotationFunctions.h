#ifndef digi_Math_RotationFunctions_h
#define digi_Math_RotationFunctions_h



namespace digi {

/// @addtogroup Math
/// @{


/*
	this file contains a collection of functions
	that create and extract rotation quaternions

	quaternionAxisAngle(axis3, angle) -> quat
	quaternionEulerX(angle) -> quat
	quaternionEulerY(angle) -> quat
	quaternionEulerZ(angle) -> quat
	quaternionEulerXYZ(angles3) -> quat
	quaternionEulerYZX(angles3) -> quat
	quaternionEulerZXY(angles3) -> quat
	quaternionEulerXZY(angles3) -> quat
	quaternionEulerYXZ(angles3) -> quat
	quaternionEulerZYX(angles3) -> quat

	quaternionRQ(vector3) -> quat

	quaternionSaddleYZ(swing2) -> quat
	quaternionSaddleZX(swing2) -> quat
	quaternionSaddleXY(swing2) -> quat
	quaternionSaddle(s3, t3, swing2) -> quat
	

	getAngle(quat) -> angle
	getAngle(axis3, quat) -> angle
	getEulerX(quat) -> angle
	getEulerY(quat) -> angle
	getEulerZ(quat) -> angle
	getEulerXYZ(quat) -> vector3
	getEulerYZX(quat) -> vector3
	getEulerZXY(quat) -> vector3
	getEulerXZY(quat) -> vector3
	getEulerYXZ(quat) -> vector3
	getEulerZYX(quat) -> vector3
	
	getRQ(quat) -> vector3

	getTwistX(quat) -> angle
	getTwistY(quat) -> angle
	getTwistZ(quat) -> angle
	
	getSaddleYZ(quat) -> swing2
	getSaddleZX(quat) -> swing2
	getSaddleXY(quat) -> swing2
*/


//-----------------------------------------------------------------------------
/// quaternionAxisAngle(axis3, angle) -> quat - calculates rotation quaternion from axis and angle

template <typename Type>
Quaternion<Type> quaternionAxisAngle(const Vector3<Type>& axis, Type angle)
{
	Type a05 = Type(0.5f) * angle;
	return quaternion(axis * sin(a05), cos(a05));
}


//-----------------------------------------------------------------------------
/// quaternionEuler?(angle) -> quat - rotation about fixed axis

template <typename Type>
Quaternion<Type> quaternionEulerX(Type angle)
{
	Type a05 = Type(0.5f) * angle;
	return quaternion(sin(a05), Type(0), Type(0), cos(a05));
}

template <typename Type>
Quaternion<Type> quaternionEulerY(Type angle)
{
	Type a05 = Type(0.5f) * angle;
	return quaternion(Type(0), sin(a05), Type(0), cos(a05));
}

template <typename Type>
Quaternion<Type> quaternionEulerZ(Type angle)
{
	Type a05 = Type(0.5f) * angle;
	return quaternion(Type(0), Type(0), sin(a05), cos(a05));
}


//-----------------------------------------------------------------------------
/// quaternionEuler???(angles3) -> quat - euler rotation about the tree axes

template <typename Type>
Quaternion<Type> quaternionEulerXYZ(const Vector3<Type>& angles)
{
	Vector3<Type> a05 = Type(0.5f) * angles;
	Type cx = cos(a05.x);
	Type sx = sin(a05.x);
	Type cy = cos(a05.y);
	Type sy = sin(a05.y);
	Type cz = cos(a05.z);
	Type sz = sin(a05.z);
	
	return quaternion(
		#include "quaternionEulerXYZ.h"
}

template <typename Type>
Quaternion<Type> quaternionEulerYZX(const Vector3<Type>& angles)
{
	Vector3<Type> a05 = Type(0.5f) * angles;
	Type cx = cos(a05.x);
	Type sx = sin(a05.x);
	Type cy = cos(a05.y);
	Type sy = sin(a05.y);
	Type cz = cos(a05.z);
	Type sz = sin(a05.z);
	
	return quaternion(
		#include "quaternionEulerYZX.h"
}

template <typename Type>
Quaternion<Type> quaternionEulerZXY(const Vector3<Type>& angles)
{
	Vector3<Type> a05 = Type(0.5f) * angles;
	Type cx = cos(a05.x);
	Type sx = sin(a05.x);
	Type cy = cos(a05.y);
	Type sy = sin(a05.y);
	Type cz = cos(a05.z);
	Type sz = sin(a05.z);
	
	return quaternion(
		#include "quaternionEulerZXY.h"
}

template <typename Type>
Quaternion<Type> quaternionEulerXZY(const Vector3<Type>& angles)
{
	Vector3<Type> a05 = Type(0.5f) * angles;
	Type cx = cos(a05.x);
	Type sx = sin(a05.x);
	Type cy = cos(a05.y);
	Type sy = sin(a05.y);
	Type cz = cos(a05.z);
	Type sz = sin(a05.z);
	
	return quaternion(
		#include "quaternionEulerXZY.h"
}

template <typename Type>
Quaternion<Type> quaternionEulerYXZ(const Vector3<Type>& angles)
{
	Vector3<Type> a05 = Type(0.5f) * angles;
	Type cx = cos(a05.x);
	Type sx = sin(a05.x);
	Type cy = cos(a05.y);
	Type sy = sin(a05.y);
	Type cz = cos(a05.z);
	Type sz = sin(a05.z);
	
	return quaternion(
		#include "quaternionEulerYXZ.h"
}

template <typename Type>
Quaternion<Type> quaternionEulerZYX(const Vector3<Type>& angles)
{
	Vector3<Type> a05 = Type(0.5f) * angles;
	Type cx = cos(a05.x);
	Type sx = sin(a05.x);
	Type cy = cos(a05.y);
	Type sy = sin(a05.y);
	Type cz = cos(a05.z);
	Type sz = sin(a05.z);
	
	return quaternion(
		#include "quaternionEulerZYX.h"
}


//-----------------------------------------------------------------------------
/// quaternionRQ(vector3) -> quat - calculates rotation quaternion from RQ-parameter v. length of v is angle, direction is axis.

template <typename Type>
Quaternion<Type> quaternionRQ(const Vector3<Type>& v)
{
	// calc q
	Type theta = length(v);
	Type cosTheta = cos(Type(0.5f) * theta);
	Type scale;
	if (theta > 1e-10f)
	{
		scale = sin(Type(0.5f) * theta) / theta;
	}
	else
	{
		// theta close to zero
		scale = Type(0.5f) * cosTheta;
	}
	return quaternion(v * scale, cosTheta);
}


//-----------------------------------------------------------------------------
/// quaternionSaddle??(vec2) -> quat - calculates rotation quaternion from swing u. length of u is angle, direction is axis in the s-t-plane

template <typename Type>
Quaternion<Type> quaternionSaddleYZ(const Matrix2<Type>& u)
{
	// calc v
	Vector3<Type> v = {Type(0), u.x, u.y};

	return quaternionRQ(v);
}

template <typename Type>
Quaternion<Type> quaternionSaddleZX(const Matrix2<Type>& u)
{
	// calc v
	Vector3<Type> v = {u.y, Type(0), u.x};

	return quaternionRQ(v);
}

template <typename Type>
Quaternion<Type> quaternionSaddleXY(const Matrix2<Type>& u)
{
	// calc v
	Vector3<Type> v = {u.x, u.y, Type(0)};

	return quaternionRQ(v);
}

template <typename Type>
Quaternion<Type> quaternionSaddle(const Vector3<Type>& s, const Vector3<Type>& t, const Matrix2<Type>& u)
{
	// calc v
	Vector3<Type> v = u.x * s + u.y * t;

	return quaternionRQ(v);
}


//-----------------------------------------------------------------------------
// getAngle(quat) -> angle - gets the rotation angle from a rotation

template <typename Type>
Type getAngle(const Quaternion<Type>& rotation)
{
	Type cosTheta = rotation.v.w;
	Type sinTheta = sqrt(rotation.v.x * rotation.v.x + rotation.v.y * rotation.v.y + rotation.v.z * rotation.v.z);
	return Type(2) * atan2(sinTheta, cosTheta);
}


//-----------------------------------------------------------------------------
// getAngle(axis, quat) -> angle - gets the rotation angle from a rotation about the given axis

template <typename Type>
Type getAngle(const Vector3<Type>& axis, const Quaternion<Type>& rotation)
{
	return Type(2) * atan2(dot(axis, getXYZ(rotation.v)), rotation.v.w);
}


//-----------------------------------------------------------------------------
// getEuler?(quat) -> angle - gets the rotation angle from a rotation about a fixed axis

template <typename Type>
Type getEulerX(const Quaternion<Type>& rotation)
{
	return Type(2) * atan2(rotation.v.x, rotation.v.w);
}

template <typename Type>
Type getEulerY(const Quaternion<Type>& rotation)
{
	return Type(2) * atan2(rotation.v.y, rotation.v.w);
}

template <typename Type>
Type getEulerZ(const Quaternion<Type>& rotation)
{
	return Type(2) * atan2(rotation.v.z, rotation.v.w);
}


//-----------------------------------------------------------------------------
// getEuler???(quat) -> vector3 - gets the euler angles from a rotation

// helper function
template <typename Type>
Type getEulerFromVectors(Type ax, Type ay, Type bx, Type by)
{
	if (ax * bx + ay * by > 0)
	{
		ax += bx;
		ay += by;
	}
	else
	{
		ax -= bx;
		ay -= by;
	}
	return Type(2) * atan2(ax, ay);
}

template <typename Type>
Vector3<Type> getEulerXYZ(const Quaternion<Type>& q)
{
	// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	Type xx = q.v.x * q.v.x;
	Type yy = q.v.y * q.v.y;
	Type zz = q.v.z * q.v.z;
	Type ww = q.v.w * q.v.w;
	
	Type xy = q.v.x * q.v.y;

	Type wz = q.v.w * q.v.z;
	
	/*
		we assume q = a * b * c = rotateZ * rotateY * rotateX
		at first we calculte rotateZ and remove it from q
		p = rotateZ^-1 * q = rotateY * rotateX
	*/
	Type eulerZ = atan2(Type(2) * (wz + xy), ww - zz + xx - yy);

	// calc rotateZ^-1
	Type rz = -sin(eulerZ * Type(0.5f));
	Type rw = cos(eulerZ * Type(0.5f));
	
	// calc p
	Type px = rw * q.v.x - rz * q.v.y;
	Type py = rw * q.v.y + rz * q.v.x;
	Type pz = rw * q.v.z + q.v.w * rz;
	Type pw = rw * q.v.w - rz * q.v.z;	
	
	/*
		now we split p into rotateX and rotateY
		
		rotateY * rotateX = (px py pz pw) = (0 by 0 bw) * (cx 0 0 cw) = b * c
		
		px =  bw * cx
		py =  by * cw
		pz = -by * cx
		pw =  bw * cw
	*/
	Type eulerX = getEulerFromVectors(px, pw, -pz, py);
	Type eulerY = getEulerFromVectors(py, pw, -pz, px);
	
	return vector3(eulerX, eulerY, eulerZ);
}

template <typename Type>
Vector3<Type> getEulerYZX(const Quaternion<Type>& q)
{
	// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	Type xx = q.v.x * q.v.x;
	Type yy = q.v.y * q.v.y;
	Type zz = q.v.z * q.v.z;
	Type ww = q.v.w * q.v.w;
	
	Type yz = q.v.y * q.v.z;

	Type wx = q.v.w * q.v.x;
	
	/*
		we assume q = a * b * c = rotateX * rotateZ * rotateY
		at first we calculte rotateX and remove it from q
		p = rotateX^-1 * q = rotateZ * rotateY
	*/
	Type eulerX = atan2(Type(2) * (wx + yz), ww - xx + yy - zz);

	// calc rotateX^-1
	Type rx = -sin(eulerX * Type(0.5f));
	Type rw = cos(eulerX * Type(0.5f));
	
	// calc p
	Type px = rw * q.v.x + q.v.w * rx;
	Type py = rw * q.v.y - rx * q.v.z;
	Type pz = rw * q.v.z + rx * q.v.y;
	Type pw = rw * q.v.w - rx * q.v.x;	
	
	/*
		now we split p into rotateX and rotateY
		
		rotateZ * rotateY = (px py pz pw) = (0 0 bz bw) * (0 cy 0 cw) = b * c
		
		px = -bz * cy
		py =  bw * cy
		pz =  bz * cw
		pw =  bw * cw
	*/
	Type eulerZ = getEulerFromVectors(pz, pw, -px, py);
	Type eulerY = getEulerFromVectors(py, pw, -px, pz);
	
	return vector3(eulerX, eulerY, eulerZ);
}

template <typename Type>
Vector3<Type> getEulerZXY(const Quaternion<Type>& q)
{
	// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	Type xx = q.v.x * q.v.x;
	Type yy = q.v.y * q.v.y;
	Type zz = q.v.z * q.v.z;
	Type ww = q.v.w * q.v.w;
	
	Type zx = q.v.x * q.v.z;

	Type wy = q.v.w * q.v.y;

	/*
		we assume q = a * b * c = rotateY * rotateX * rotateZ
		at first we calculte rotateY and remove it from q
		p = rotateY^-1 * q = rotateX * rotateZ
	*/
	Type eulerY = atan2(Type(2) * (wy + zx), ww - yy + zz - xx);

	// calc rotateY^-1
	Type ry = -sin(eulerY * Type(0.5f));
	Type rw = cos(eulerY * Type(0.5f));
	
	// calc p
	Type px = rw * q.v.x + ry * q.v.z;
	Type py = rw * q.v.y + q.v.w * ry;
	Type pz = rw * q.v.z - ry * q.v.x;
	Type pw = rw * q.v.w - ry * q.v.y;	
	
	/*
		now we split p into rotateX and rotateY
		
		rotateX * rotateZ = (px py pz pw) = (bx 0 0 bw) * (0 0 cz cw) = b * c
				
		px =  bx * cw
		py = -bx * cz
		pz =  bw * cz
		pw =  bw * cw			
	*/
	Type eulerX = getEulerFromVectors(px, pw, -py, pz);
	Type eulerZ = getEulerFromVectors(pz, pw, -py, px);
	
	return vector3(eulerX, eulerY, eulerZ);
}

template <typename Type>
Vector3<Type> getEulerXZY(const Quaternion<Type>& q)
{
	// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	Type xx = q.v.x * q.v.x;
	Type yy = q.v.y * q.v.y;
	Type zz = q.v.z * q.v.z;
	Type ww = q.v.w * q.v.w;
	
	Type zx = q.v.x * q.v.z;

	Type wy = q.v.w * q.v.y;

	/*
		we assume q = a * b * c = rotateY * rotateZ * rotateX
		at first we calculte rotateY and remove it from q
		p = rotateY^-1 * q = rotateZ * rotateX
	*/
	Type eulerY = atan2(Type(2) * (wy - zx), ww - yy - zz + xx);

	// calc rotateY^-1
	Type ry = -sin(eulerY * Type(0.5f));
	Type rw = cos(eulerY * Type(0.5f));
	
	// calc p
	Type px = rw * q.v.x + ry * q.v.z;
	Type py = rw * q.v.y + q.v.w * ry;
	Type pz = rw * q.v.z - ry * q.v.x;
	Type pw = rw * q.v.w - ry * q.v.y;	
	
	/*
		now we split p into rotateX and rotateY
		
		rotateZ * rotateX = (px py pz pw) = (0 0 bz bw) * (cx 0 0 cw) = b * c
				
		px = bw * cx
		py = bz * cx
		pz = bz * cw
		pw = bw * cw			
	*/
	Type eulerZ = getEulerFromVectors(pz, pw, py, px);
	Type eulerX = getEulerFromVectors(px, pw, py, pz);
	
	return vector3(eulerX, eulerY, eulerZ);
}

template <typename Type>
Vector3<Type> getEulerYXZ(const Quaternion<Type>& q)
{
	// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	Type xx = q.v.x * q.v.x;
	Type yy = q.v.y * q.v.y;
	Type zz = q.v.z * q.v.z;
	Type ww = q.v.w * q.v.w;
	
	Type xy = q.v.x * q.v.y;

	Type wz = q.v.w * q.v.z;
	
	/*
		we assume q = a * b * c = rotateZ * rotateX * rotateY
		at first we calculte rotateZ and remove it from q
		p = rotateZ^-1 * q = rotateX * rotateY
	*/
	Type eulerZ = atan2(Type(2) * (wz - xy), ww - zz - xx + yy);

	// calc rotateZ^-1
	Type rz = -sin(eulerZ * Type(0.5f));
	Type rw = cos(eulerZ * Type(0.5f));
	
	// calc p
	Type px = rw * q.v.x - rz * q.v.y;
	Type py = rw * q.v.y + rz * q.v.x;
	Type pz = rw * q.v.z + q.v.w * rz;
	Type pw = rw * q.v.w - rz * q.v.z;	
	
	/*
		now we split p into rotateX and rotateY
		
		rotateX * rotateY = (px py pz pw) = (bx 0 0 bw) * (0 cy 0 cw) = b * c
		
		px = bx * cw
		py = bw * cy
		pz = bx * cy
		pw = bw * cw		
	*/
	Type eulerY = getEulerFromVectors(py, pw, pz, px);
	Type eulerX = getEulerFromVectors(px, pw, pz, py);
	
	return vector3(eulerX, eulerY, eulerZ);
}

template <typename Type>
Vector3<Type> getEulerZYX(const Quaternion<Type>& q)
{
	// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	Type xx = q.v.x * q.v.x;
	Type yy = q.v.y * q.v.y;
	Type zz = q.v.z * q.v.z;
	Type ww = q.v.w * q.v.w;
	
	Type yz = q.v.y * q.v.z;

	Type wx = q.v.w * q.v.x;
	
	/*
		we assume q = a * b * c = rotateX * rotateY * rotateZ
		at first we calculte rotateX and remove it from q
		p = rotateX^-1 * q = rotateY * rotateZ
	*/
	Type eulerX = atan2(Type(2) * (wx - yz), ww - xx - yy + zz);

	// calc rotateX^-1
	Type rx = -sin(eulerX * Type(0.5f));
	Type rw = cos(eulerX * Type(0.5f));
	
	// calc p
	Type px = rw * q.v.x + q.v.w * rx;
	Type py = rw * q.v.y - rx * q.v.z;
	Type pz = rw * q.v.z + rx * q.v.y;
	Type pw = rw * q.v.w - rx * q.v.x;	
	
	/*
		now we split p into rotateX and rotateY
		
		rotateY * rotateZ = (px py pz pw) = (0 0 bz bw) * (0 cy 0 cw) = b * c
		
		px = by * cz
		py = by * cw
		pz = bw * cz
		pw = bw * cw
	*/
	Type eulerY = getEulerFromVectors(py, pw, px, pz);
	Type eulerZ = getEulerFromVectors(pz, pw, px, py);
	
	return vector3(eulerX, eulerY, eulerZ);
}


//-----------------------------------------------------------------------------
// getRQ(quat) -> vector3 - calculates RQ = "reparameterized quaternion" from rotation q. length of vector is angle, direction is axis.

template <typename Type>
Vector3<Type> getRQ(const Quaternion<Type>& rotation)
{
	return arg(rotation);
}


//-----------------------------------------------------------------------------
// getTwist?(quat) -> angle - extracts the twist about an axis from rotation q

template <typename Type>
Type getTwistX(const Quaternion<Type>& rotation)
{
/*
	our rotation is made up of two components: r = a * b.
	rotation a is a saddleYZ rotation, rotation b is a twistX rotation.
	we want to split r into a and b and return b.
	
	// a.x is 0, length of a is 1
	a.w^2 + a.y^2 + a.z^2 = 1 (I)
	
	// b.y and b.z are 0, length of b is 1
	b.w^2 + b.x^2 = 1 (II)
	
	// r = a * b while removing all zero terms
	r.x = a.w * b.x (III)
	r.y = a.y * b.w + a.z * b.x (IV)
	r.z = a.z * b.w - a.y * b.x (V)
	r.w = a.w * b.w (VI)

	// eliminate a.w by resolving (VI) to a.w and inserting into (III)
	r.x = (r.w/b.w) * b.x (VII)

	// insert (VII) into (II)
	b.w^2 + (b.w * r.x/r.w)^2 = 1
	b.w^2 * (1 + (r.x/r.w)^2) = 1
	b.w^2 = 1 / (1 + (r.x/r.w)^2)
	b.w = sqrt(r.w^2 / (r.w^2 + r.x^2))
	
	// resolve (VII) to b.x
	b.x = r.x * b.w / r.w

	// heuristic simplification (moving r.w^2 out of the square root)
	b.w = r.w / sqrt(r.w^2 + r.x^2) (VIII)
	b.x = r.x / sqrt(r.w^2 + r.x^2) (IX)

	// insert (IX) into (III) or (VIII) into (VI)
	a.w = sqrt(r.w^2 + r.x^2)
*/

	Type aw = sqrt(sqr(rotation.v.w) + sqr(rotation.v.x));
	Type bx = Type(0);
	Type bw = Type(1);
	if (aw > 1e-6f)
	{
		bx = rotation.v.x / aw;
		bw = rotation.v.w / aw;
	}
	return Type(2) * atan2(bx, bw);
}

template <typename Type>
Type getTwistY(const Quaternion<Type>& rotation)
{
	Type aw = sqrt(sqr(rotation.v.w) + sqr(rotation.v.y));
	Type by = Type(0);
	Type bw = Type(1);
	if (aw > 1e-6f)
	{
		by = rotation.v.y / aw;
		bw = rotation.v.w / aw;
	}
	return Type(2) * atan2(by, bw);
}

template <typename Type>
Type getTwistZ(const Quaternion<Type>& rotation)
{
	Type aw = sqrt(sqr(rotation.v.w) + sqr(rotation.v.z));
	Type bz = Type(0);
	Type bw = Type(1);
	if (aw > 1e-6f)
	{
		bz = rotation.v.z / aw;
		bw = rotation.v.w / aw;
	}
	return Type(2) * atan2(bz, bw);
}


//-----------------------------------------------------------------------------
// getSaddle??(quat) -> swing2 - extracts swing u from rotation q. length of u is angle, direction is axis in the s-t-plane

template <typename Type>
Vector2<Type> getSaddleYZ(const Quaternion<Type>& rotation)
{
/*
	our rotation is made up of two components: r = a * b.
	rotation a is a saddleYZ rotation, rotation b is a twistX rotation.
	we want to split r into a and b and return b.
	
	// a.x is 0, length of a is 1
	a.w^2 + a.y^2 + a.z^2 = 1 (I)
	
	// b.y and b.z are 0, length of b is 1
	b.w^2 + b.x^2 = 1 (II)
	
	// r = a * b while removing all zero terms
	r.x = a.w * b.x (III)
	r.y = a.y * b.w + a.z * b.x (IV)
	r.z = a.z * b.w - a.y * b.x (V)
	r.w = a.w * b.w (VI)

	// eliminate a.w by resolving (VI) to a.w and inserting into (III)
	r.x = (r.w/b.w) * b.x (VII)

	// insert (VII) into (II)
	b.w^2 + (b.w * r.x/r.w)^2 = 1
	b.w^2 * (1 + (r.x/r.w)^2) = 1
	b.w^2 = 1 / (1 + (r.x/r.w)^2)
	b.w = sqrt(r.w^2 / (r.w^2 + r.x^2))
	
	// resolve (VII) to b.x
	b.x = r.x * b.w / r.w

	// heuristic simplification (moving r.w^2 out of the square root)
	b.w = r.w / sqrt(r.w^2 + r.x^2) (VIII)
	b.x = r.x / sqrt(r.w^2 + r.x^2) (IX)

	// insert (IX) into (III) or (VIII) into (VI)
	a.w = sqrt(r.w^2 + r.x^2)
	
	a.z = (r.z + a.y * b.x) / b.w
	r.y = a.y * b.w + (r.z + a.y * b.x) / b.w * b.x = a.y * (b.w + b.x * b.x / b.w) + r.z * b.x / b.w
	a.y = (r.y - r.z * b.x / b.w) / (b.w + b.x * b.x / b.w)
	a.y = (r.y * b.w - r.z * b.x) / (b.w^2 + b.x^2)
	
	a.y = (r.y - a.z * b.x) / b.w
	r.z = a.z * b.w - (r.y - a.z * b.x) / b.w * b.x = a.z * (b.w + b.x * b.x / b.w) - r.y * b.x / b.w
	a.z = (r.z + r.y * b.x / b.w) / (b.w + b.x * b.x / b.w)
	a.z = (r.z * b.w + r.y * b.x) / (b.w^2 + b.x^2)
*/
	Type aw = sqrt(sqr(rotation.v.w) + sqr(rotation.v.x));
	Type bx = 0;
	Type bw = 1;
	if (aw > 1e-6f)
	{
		bx = rotation.v.x / aw;
		bw = rotation.v.w / aw;
	}

	Type s = sqr(bw) + sqr(bx);
	Type ay = (rotation.v.y * bw - rotation.v.z * bx) / s;
	Type az = (rotation.v.z * bw + rotation.v.y * bx) / s;
	
	Vector3<Type> v = getRQ(quaternion(Type(0), ay, az, aw));

	return vector2(v.y, v.z);
}

template <typename Type>
Vector2<Type> getSaddleZX(const Quaternion<Type>& rotation)
{
	Type aw = sqrt(sqr(rotation.v.w) + sqr(rotation.v.y));
	Type by = 0;
	Type bw = 1;
	if (aw > 1e-6f)
	{
		by = rotation.v.y / aw;
		bw = rotation.v.w / aw;
	}

	Type s = sqr(bw) + sqr(by);
	Type az = (rotation.v.z * bw - rotation.v.x * by) / s;
	Type ax = (rotation.v.x * bw + rotation.v.z * by) / s;
	
	Vector3<Type> v = getRQ(quaternion(ax, Type(0), az, aw));

	return vector2(v.z, v.x);
}

template <typename Type>
Vector2<Type> getSaddleXY(const Quaternion<Type>& rotation)
{
	Type aw = sqrt(sqr(rotation.v.w) + sqr(rotation.v.z));
	Type bz = 0;
	Type bw = 1;
	if (aw > 1e-6f)
	{
		bz = rotation.v.z / aw;
		bw = rotation.v.w / aw;
	}

	Type s = sqr(bw) + sqr(bz);
	Type ax = (rotation.v.x * bw - rotation.v.y * bz) / s;
	Type ay = (rotation.v.y * bw + rotation.v.x * bz) / s;
	
	Vector3<Type> v = getRQ(quaternion(ax, ay, Type(0), aw));

	return vector2(v.x, v.y);
}


/// @}

} // namespace digi

#endif
