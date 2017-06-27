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

inline QUATERNION quaternionAxisAngle(VECTOR3 axis, SCALAR angle)
{
	SCALAR a05 = 0.5f * angle;
	return quaternion(axis * sin(a05), cos(a05));
}


//-----------------------------------------------------------------------------
/// quaternionEuler?(angle) -> quat - rotation about fixed axis

inline QUATERNION quaternionEulerX(SCALAR angle)
{
	SCALAR a05 = 0.5f * angle;
	return quaternion(sin(a05), ZERO, ZERO, cos(a05));
}

inline QUATERNION quaternionEulerY(SCALAR angle)
{
	SCALAR a05 = 0.5f * angle;
	return quaternion(ZERO, sin(a05), ZERO, cos(a05));
}

inline QUATERNION quaternionEulerZ(SCALAR angle)
{
	SCALAR a05 = 0.5f * angle;
	return quaternion(ZERO, ZERO, sin(a05), cos(a05));
}


//-----------------------------------------------------------------------------
/// quaternionEuler???(angles3) -> quat - euler rotation about the tree axes

inline QUATERNION quaternionEulerXYZ(VECTOR3 angles)
{
	VECTOR3 a05 = 0.5f * angles;
	SCALAR cx = cos(a05.x);
	SCALAR sx = sin(a05.x);
	SCALAR cy = cos(a05.y);
	SCALAR sy = sin(a05.y);
	SCALAR cz = cos(a05.z);
	SCALAR sz = sin(a05.z);
	
	return quaternion(
		#include "quaternionEulerXYZ.h"
}

inline QUATERNION quaternionEulerYZX(VECTOR3 angles)
{
	VECTOR3 a05 = 0.5f * angles;
	SCALAR cx = cos(a05.x);
	SCALAR sx = sin(a05.x);
	SCALAR cy = cos(a05.y);
	SCALAR sy = sin(a05.y);
	SCALAR cz = cos(a05.z);
	SCALAR sz = sin(a05.z);
	
	return quaternion(
		#include "quaternionEulerYZX.h"
}

inline QUATERNION quaternionEulerZXY(VECTOR3 angles)
{
	VECTOR3 a05 = 0.5f * angles;
	SCALAR cx = cos(a05.x);
	SCALAR sx = sin(a05.x);
	SCALAR cy = cos(a05.y);
	SCALAR sy = sin(a05.y);
	SCALAR cz = cos(a05.z);
	SCALAR sz = sin(a05.z);
	
	return quaternion(
		#include "quaternionEulerZXY.h"
}

inline QUATERNION quaternionEulerXZY(VECTOR3 angles)
{
	VECTOR3 a05 = 0.5f * angles;
	SCALAR cx = cos(a05.x);
	SCALAR sx = sin(a05.x);
	SCALAR cy = cos(a05.y);
	SCALAR sy = sin(a05.y);
	SCALAR cz = cos(a05.z);
	SCALAR sz = sin(a05.z);
	
	return quaternion(
		#include "quaternionEulerXZY.h"
}

inline QUATERNION quaternionEulerYXZ(VECTOR3 angles)
{
	VECTOR3 a05 = 0.5f * angles;
	SCALAR cx = cos(a05.x);
	SCALAR sx = sin(a05.x);
	SCALAR cy = cos(a05.y);
	SCALAR sy = sin(a05.y);
	SCALAR cz = cos(a05.z);
	SCALAR sz = sin(a05.z);
	
	return quaternion(
		#include "quaternionEulerYXZ.h"
}

inline QUATERNION quaternionEulerZYX(VECTOR3 angles)
{
	VECTOR3 a05 = 0.5f * angles;
	SCALAR cx = cos(a05.x);
	SCALAR sx = sin(a05.x);
	SCALAR cy = cos(a05.y);
	SCALAR sy = sin(a05.y);
	SCALAR cz = cos(a05.z);
	SCALAR sz = sin(a05.z);
	
	return quaternion(
		#include "quaternionEulerZYX.h"
}


//-----------------------------------------------------------------------------
/// quaternionRQ(vector3) -> quat - calculates rotation quaternion from RQ-parameter v. length of v is angle, direction is axis.

inline QUATERNION quaternionRQ(VECTOR3 v)
{
	// calc q
	SCALAR theta = length(v);
	SCALAR cosTheta = cos(0.5f * theta);
	SCALAR scale;
	if (theta > 1e-10f)
	{
		scale = sin(0.5f * theta) / theta;
	}
	else
	{
		// theta close to zero
		scale = 0.5f * cosTheta;
	}
	return quaternion(v * scale, cosTheta);
}


//-----------------------------------------------------------------------------
/// quaternionSaddle??(vec2) -> quat - calculates rotation quaternion from swing u. length of u is angle, direction is axis in the s-t-plane

inline QUATERNION quaternionSaddleYZ(VECTOR2 u)
{
	// calc v
	VECTOR3 v = {0, u.x, u.y};

	return quaternionRQ(v);
}

inline QUATERNION quaternionSaddleZX(VECTOR2 u)
{
	// calc v
	VECTOR3 v = {u.y, 0, u.x};

	return quaternionRQ(v);
}

inline QUATERNION quaternionSaddleXY(VECTOR2 u)
{
	// calc v
	VECTOR3 v = {u.x, u.y, 0};

	return quaternionRQ(v);
}

inline QUATERNION quaternionSaddle(VECTOR3 s, VECTOR3 t, VECTOR2 u)
{
	// calc v
	VECTOR3 v = u.x * s + u.y * t;

	return quaternionRQ(v);
}


//-----------------------------------------------------------------------------
// getAngle(quat) -> angle - gets the rotation angle from a rotation

inline SCALAR getAngle(QUATERNION rotation)
{
	SCALAR cosTheta = rotation.v.w;
	SCALAR sinTheta = sqrt(rotation.v.x * rotation.v.x + rotation.v.y * rotation.v.y + rotation.v.z * rotation.v.z);
	return 2.0f * atan2(sinTheta, cosTheta);
}


//-----------------------------------------------------------------------------
// getAngle(axis, quat) -> angle - gets the rotation angle from a rotation about the given axis

inline SCALAR getAngle(VECTOR3 axis, QUATERNION rotation)
{
	return 2.0f * atan2(dot(axis, rotation.v.xyz), rotation.v.w);
}


//-----------------------------------------------------------------------------
// getEuler?(quat) -> angle - gets the rotation angle from a rotation about a fixed axis

inline SCALAR getEulerX(QUATERNION rotation)
{
	return 2.0f * atan2(rotation.v.x, rotation.v.w);
}

inline SCALAR getEulerY(QUATERNION rotation)
{
	return 2.0f * atan2(rotation.v.y, rotation.v.w);
}

inline SCALAR getEulerZ(QUATERNION rotation)
{
	return 2.0f * atan2(rotation.v.z, rotation.v.w);
}


//-----------------------------------------------------------------------------
// getEuler???(quat) -> vector3 - gets the euler angles from a rotation

// helper function
inline SCALAR getEulerFromVectors(SCALAR ax, SCALAR ay, SCALAR bx, SCALAR by)
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
	return 2.0f * atan2(ax, ay);
}

inline VECTOR3 getEulerXYZ(QUATERNION q)
{
	// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	SCALAR xx = q.v.x * q.v.x;
	SCALAR yy = q.v.y * q.v.y;
	SCALAR zz = q.v.z * q.v.z;
	SCALAR ww = q.v.w * q.v.w;
	
	SCALAR xy = q.v.x * q.v.y;

	SCALAR wz = q.v.w * q.v.z;
	
	/*
		we assume q = a * b * c = rotateZ * rotateY * rotateX
		at first we calculte rotateZ and remove it from q
		p = rotateZ^-1 * q = rotateY * rotateX
	*/
	SCALAR eulerZ = atan2(2.0f * (wz + xy), ww - zz + xx - yy);

	// calc rotateZ^-1
	SCALAR rz = -sin(eulerZ * 0.5f);
	SCALAR rw = cos(eulerZ * 0.5f);
	
	// calc p
	SCALAR px = rw * q.v.x - rz * q.v.y;
	SCALAR py = rw * q.v.y + rz * q.v.x;
	SCALAR pz = rw * q.v.z + q.v.w * rz;
	SCALAR pw = rw * q.v.w - rz * q.v.z;	
	
	/*
		now we split p into rotateX and rotateY
		
		rotateY * rotateX = (px py pz pw) = (0 by 0 bw) * (cx 0 0 cw) = b * c
		
		px =  bw * cx
		py =  by * cw
		pz = -by * cx
		pw =  bw * cw
	*/
	SCALAR eulerX = getEulerFromVectors(px, pw, -pz, py);
	SCALAR eulerY = getEulerFromVectors(py, pw, -pz, px);
	
	return vector3(eulerX, eulerY, eulerZ);
}

inline VECTOR3 getEulerYZX(QUATERNION q)
{
	// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	SCALAR xx = q.v.x * q.v.x;
	SCALAR yy = q.v.y * q.v.y;
	SCALAR zz = q.v.z * q.v.z;
	SCALAR ww = q.v.w * q.v.w;
	
	SCALAR yz = q.v.y * q.v.z;

	SCALAR wx = q.v.w * q.v.x;
	
	/*
		we assume q = a * b * c = rotateX * rotateZ * rotateY
		at first we calculte rotateX and remove it from q
		p = rotateX^-1 * q = rotateZ * rotateY
	*/
	SCALAR eulerX = atan2(2.0f * (wx + yz), ww - xx + yy - zz);

	// calc rotateX^-1
	SCALAR rx = -sin(eulerX * 0.5f);
	SCALAR rw = cos(eulerX * 0.5f);
	
	// calc p
	SCALAR px = rw * q.v.x + q.v.w * rx;
	SCALAR py = rw * q.v.y - rx * q.v.z;
	SCALAR pz = rw * q.v.z + rx * q.v.y;
	SCALAR pw = rw * q.v.w - rx * q.v.x;	
	
	/*
		now we split p into rotateX and rotateY
		
		rotateZ * rotateY = (px py pz pw) = (0 0 bz bw) * (0 cy 0 cw) = b * c
		
		px = -bz * cy
		py =  bw * cy
		pz =  bz * cw
		pw =  bw * cw
	*/
	SCALAR eulerZ = getEulerFromVectors(pz, pw, -px, py);
	SCALAR eulerY = getEulerFromVectors(py, pw, -px, pz);
	
	return vector3(eulerX, eulerY, eulerZ);
}

inline VECTOR3 getEulerZXY(QUATERNION q)
{
	// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	SCALAR xx = q.v.x * q.v.x;
	SCALAR yy = q.v.y * q.v.y;
	SCALAR zz = q.v.z * q.v.z;
	SCALAR ww = q.v.w * q.v.w;
	
	SCALAR zx = q.v.x * q.v.z;

	SCALAR wy = q.v.w * q.v.y;

	/*
		we assume q = a * b * c = rotateY * rotateX * rotateZ
		at first we calculte rotateY and remove it from q
		p = rotateY^-1 * q = rotateX * rotateZ
	*/
	SCALAR eulerY = atan2(2.0f * (wy + zx), ww - yy + zz - xx);

	// calc rotateY^-1
	SCALAR ry = -sin(eulerY * 0.5f);
	SCALAR rw = cos(eulerY * 0.5f);
	
	// calc p
	SCALAR px = rw * q.v.x + ry * q.v.z;
	SCALAR py = rw * q.v.y + q.v.w * ry;
	SCALAR pz = rw * q.v.z - ry * q.v.x;
	SCALAR pw = rw * q.v.w - ry * q.v.y;	
	
	/*
		now we split p into rotateX and rotateY
		
		rotateX * rotateZ = (px py pz pw) = (bx 0 0 bw) * (0 0 cz cw) = b * c
				
		px =  bx * cw
		py = -bx * cz
		pz =  bw * cz
		pw =  bw * cw			
	*/
	SCALAR eulerX = getEulerFromVectors(px, pw, -py, pz);
	SCALAR eulerZ = getEulerFromVectors(pz, pw, -py, px);
	
	return vector3(eulerX, eulerY, eulerZ);
}

inline VECTOR3 getEulerXZY(QUATERNION q)
{
	// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	SCALAR xx = q.v.x * q.v.x;
	SCALAR yy = q.v.y * q.v.y;
	SCALAR zz = q.v.z * q.v.z;
	SCALAR ww = q.v.w * q.v.w;
	
	SCALAR zx = q.v.x * q.v.z;

	SCALAR wy = q.v.w * q.v.y;

	/*
		we assume q = a * b * c = rotateY * rotateZ * rotateX
		at first we calculte rotateY and remove it from q
		p = rotateY^-1 * q = rotateZ * rotateX
	*/
	SCALAR eulerY = atan2(2.0f * (wy - zx), ww - yy - zz + xx);

	// calc rotateY^-1
	SCALAR ry = -sin(eulerY * 0.5f);
	SCALAR rw = cos(eulerY * 0.5f);
	
	// calc p
	SCALAR px = rw * q.v.x + ry * q.v.z;
	SCALAR py = rw * q.v.y + q.v.w * ry;
	SCALAR pz = rw * q.v.z - ry * q.v.x;
	SCALAR pw = rw * q.v.w - ry * q.v.y;	
	
	/*
		now we split p into rotateX and rotateY
		
		rotateZ * rotateX = (px py pz pw) = (0 0 bz bw) * (cx 0 0 cw) = b * c
				
		px = bw * cx
		py = bz * cx
		pz = bz * cw
		pw = bw * cw			
	*/
	SCALAR eulerZ = getEulerFromVectors(pz, pw, py, px);
	SCALAR eulerX = getEulerFromVectors(px, pw, py, pz);
	
	return vector3(eulerX, eulerY, eulerZ);
}

inline VECTOR3 getEulerYXZ(QUATERNION q)
{
	// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	SCALAR xx = q.v.x * q.v.x;
	SCALAR yy = q.v.y * q.v.y;
	SCALAR zz = q.v.z * q.v.z;
	SCALAR ww = q.v.w * q.v.w;
	
	SCALAR xy = q.v.x * q.v.y;

	SCALAR wz = q.v.w * q.v.z;
	
	/*
		we assume q = a * b * c = rotateZ * rotateX * rotateY
		at first we calculte rotateZ and remove it from q
		p = rotateZ^-1 * q = rotateX * rotateY
	*/
	SCALAR eulerZ = atan2(2.0f * (wz - xy), ww - zz - xx + yy);

	// calc rotateZ^-1
	SCALAR rz = -sin(eulerZ * 0.5f);
	SCALAR rw = cos(eulerZ * 0.5f);
	
	// calc p
	SCALAR px = rw * q.v.x - rz * q.v.y;
	SCALAR py = rw * q.v.y + rz * q.v.x;
	SCALAR pz = rw * q.v.z + q.v.w * rz;
	SCALAR pw = rw * q.v.w - rz * q.v.z;	
	
	/*
		now we split p into rotateX and rotateY
		
		rotateX * rotateY = (px py pz pw) = (bx 0 0 bw) * (0 cy 0 cw) = b * c
		
		px = bx * cw
		py = bw * cy
		pz = bx * cy
		pw = bw * cw		
	*/
	SCALAR eulerY = getEulerFromVectors(py, pw, pz, px);
	SCALAR eulerX = getEulerFromVectors(px, pw, pz, py);
	
	return vector3(eulerX, eulerY, eulerZ);
}

inline VECTOR3 getEulerZYX(QUATERNION q)
{
	// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
	SCALAR xx = q.v.x * q.v.x;
	SCALAR yy = q.v.y * q.v.y;
	SCALAR zz = q.v.z * q.v.z;
	SCALAR ww = q.v.w * q.v.w;
	
	SCALAR yz = q.v.y * q.v.z;

	SCALAR wx = q.v.w * q.v.x;
	
	/*
		we assume q = a * b * c = rotateX * rotateY * rotateZ
		at first we calculte rotateX and remove it from q
		p = rotateX^-1 * q = rotateY * rotateZ
	*/
	SCALAR eulerX = atan2(2.0f * (wx - yz), ww - xx - yy + zz);

	// calc rotateX^-1
	SCALAR rx = -sin(eulerX * 0.5f);
	SCALAR rw = cos(eulerX * 0.5f);
	
	// calc p
	SCALAR px = rw * q.v.x + q.v.w * rx;
	SCALAR py = rw * q.v.y - rx * q.v.z;
	SCALAR pz = rw * q.v.z + rx * q.v.y;
	SCALAR pw = rw * q.v.w - rx * q.v.x;	
	
	/*
		now we split p into rotateX and rotateY
		
		rotateY * rotateZ = (px py pz pw) = (0 0 bz bw) * (0 cy 0 cw) = b * c
		
		px = by * cz
		py = by * cw
		pz = bw * cz
		pw = bw * cw
	*/
	SCALAR eulerY = getEulerFromVectors(py, pw, px, pz);
	SCALAR eulerZ = getEulerFromVectors(pz, pw, px, py);
	
	return vector3(eulerX, eulerY, eulerZ);
}


//-----------------------------------------------------------------------------
// getRQ(quat) -> vector3 - calculates RQ = "reparameterized quaternion" from rotation q. length of vector is angle, direction is axis.

inline VECTOR3 getRQ(QUATERNION rotation)
{
	return arg(rotation);
}


//-----------------------------------------------------------------------------
// getTwist?(quat) -> angle - extracts the twist about an axis from rotation q

inline SCALAR getTwistX(QUATERNION rotation)
{
	SCALAR aw = sqrt(sqr(rotation.v.w) + sqr(rotation.v.x));
	SCALAR bx = 0;
	SCALAR bw = 1;
	if (aw > 1e-6f)
	{
		bx = rotation.v.x / aw;
		bw = rotation.v.w / aw;
	}
	return 2.0f * atan2(bx, bw);
}

inline SCALAR getTwistY(QUATERNION rotation)
{
	SCALAR aw = sqrt(sqr(rotation.v.w) + sqr(rotation.v.y));
	SCALAR by = 0;
	SCALAR bw = 1;
	if (aw > 1e-6f)
	{
		by = rotation.v.y / aw;
		bw = rotation.v.w / aw;
	}
	return 2.0f * atan2(by, bw);
}

inline SCALAR getTwistZ(QUATERNION rotation)
{
	SCALAR aw = sqrt(sqr(rotation.v.w) + sqr(rotation.v.z));
	SCALAR bz = 0;
	SCALAR bw = 1;
	if (aw > 1e-6f)
	{
		bz = rotation.v.z / aw;
		bw = rotation.v.w / aw;
	}
	return 2.0f * atan2(bz, bw);
}


//-----------------------------------------------------------------------------
// getSaddle??(quat) -> swing2 - extracts swing u from rotation q. length of u is angle, direction is axis in the s-t-plane

inline VECTOR2 getSaddleYZ(QUATERNION rotation)
{
	SCALAR aw = sqrt(sqr(rotation.v.w) + sqr(rotation.v.x));
	SCALAR bx = 0;
	SCALAR bw = 1;
	if (aw > 1e-6f)
	{
		bx = rotation.v.x / aw;
		bw = rotation.v.w / aw;
	}

	SCALAR s = sqr(bw) + sqr(bx);
	SCALAR ay = (rotation.v.y * bw - rotation.v.z * bx) / s;
	SCALAR az = (rotation.v.z * bw + rotation.v.y * bx) / s;
	
	VECTOR3 v = getRQ(quaternion(0, ay, az, aw));

	return vector2(v.y, v.z);
}

inline VECTOR2 getSaddleZX(QUATERNION rotation)
{
	SCALAR aw = sqrt(sqr(rotation.v.w) + sqr(rotation.v.y));
	SCALAR by = 0;
	SCALAR bw = 1;
	if (aw > 1e-6f)
	{
		by = rotation.v.y / aw;
		bw = rotation.v.w / aw;
	}

	SCALAR s = sqr(bw) + sqr(by);
	SCALAR az = (rotation.v.z * bw - rotation.v.x * by) / s;
	SCALAR ax = (rotation.v.x * bw + rotation.v.z * by) / s;
	
	VECTOR3 v = getRQ(quaternion(ax, ZERO, az, aw));

	return vector2(v.z, v.x);
}

inline VECTOR2 getSaddleXY(QUATERNION rotation)
{
	SCALAR aw = sqrt(sqr(rotation.v.w) + sqr(rotation.v.z));
	SCALAR bz = 0;
	SCALAR bw = 1;
	if (aw > 1e-6f)
	{
		bz = rotation.v.z / aw;
		bw = rotation.v.w / aw;
	}

	SCALAR s = sqr(bw) + sqr(bz);
	SCALAR ax = (rotation.v.x * bw - rotation.v.y * bz) / s;
	SCALAR ay = (rotation.v.y * bw + rotation.v.x * bz) / s;
	
	VECTOR3 v = getRQ(quaternion(ax, ay, ZERO, aw));

	return vector2(v.x, v.y);
}
