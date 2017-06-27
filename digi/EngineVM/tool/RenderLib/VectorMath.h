#define PASTER(x,y) x##y
#define CONCAT(x,y) PASTER(x,y)


// ----------------------------------------------------------------------------
// basic types

typedef signed char byte;
typedef unsigned char ubyte;
typedef unsigned short ushort;
typedef unsigned int uint;
//typedef unsigned long ulong;


// ----------------------------------------------------------------------------
// bool vector

typedef bool __attribute__((ext_vector_type(2))) bool2;
typedef bool __attribute__((ext_vector_type(3))) bool3;
typedef bool __attribute__((ext_vector_type(4))) bool4;


// ----------------------------------------------------------------------------
// byte vector

#define IS_INTEGER
#define SCALAR byte
#define SIZE 1
#define VECTOR2 byte2
#define VECTOR3 byte3
#define VECTOR4 byte4

	#include "Vector.h"

#undef VECTOR2
#undef VECTOR3
#undef VECTOR4
#undef SIZE
#undef SCALAR
#undef IS_INTEGER


// ----------------------------------------------------------------------------
// ubyte vector

#define IS_INTEGER
#define SCALAR ubyte
#define SIZE 1
#define VECTOR2 ubyte2
#define VECTOR3 ubyte3
#define VECTOR4 ubyte4

	#include "Vector.h"

#undef VECTOR2
#undef VECTOR3
#undef VECTOR4
#undef SIZE
#undef SCALAR
#undef IS_INTEGER


// ----------------------------------------------------------------------------
// short vector

#define IS_INTEGER
#define SCALAR short
#define SIZE 2
#define VECTOR2 short2
#define VECTOR3 short3
#define VECTOR4 short4

	#include "Vector.h"

#undef VECTOR2
#undef VECTOR3
#undef VECTOR4
#undef SIZE
#undef SCALAR
#undef IS_INTEGER


// ----------------------------------------------------------------------------
// ushort vector

#define IS_INTEGER
#define SCALAR ushort
#define SIZE 2
#define VECTOR2 ushort2
#define VECTOR3 ushort3
#define VECTOR4 ushort4

	#include "Vector.h"

#undef VECTOR2
#undef VECTOR3
#undef VECTOR4
#undef SIZE
#undef SCALAR
#undef IS_INTEGER


// ----------------------------------------------------------------------------
// int vector

#define IS_INTEGER
#define SCALAR int
#define SIZE 4
#define VECTOR2 int2
#define VECTOR3 int3
#define VECTOR4 int4

	#include "Vector.h"
	#include "IntFunctions.h"

#undef VECTOR2
#undef VECTOR3
#undef VECTOR4
#undef SIZE
#undef SCALAR
#undef IS_INTEGER


// ----------------------------------------------------------------------------
// uint vector

#define IS_INTEGER
#define IS_UNSIGNED
#define SCALAR uint
#define SIZE 4
#define VECTOR2 uint2
#define VECTOR3 uint3
#define VECTOR4 uint4

	#include "Vector.h"
	//#include "IntFunctions.h"

#undef VECTOR2
#undef VECTOR3
#undef VECTOR4
#undef SIZE
#undef SCALAR
#undef IS_UNSIGNED
#undef IS_INTEGER


// ----------------------------------------------------------------------------
// float vector, quaternion and matrix

#define ZERO 0.0f
#define ONE 1.0f
#define SCALAR float
#define SIZE 4
#define VECTOR2 float2
#define VECTOR3 float3
#define VECTOR4 float4

	#include "Vector.h"
	#include "VectorFunctions.h"

#define QUATERNION floatQuaternion

	#include "Quaternion.h"
	#include "RotationFunctions.h"

	#define VECTOR float2
	#define MATRIX2 float2x2
	#define MATRIX3 float2x3
	#define MATRIX4 float2x4

		#include "Matrix2.h"
		#include "Matrix3.h"
		#include "Matrix4.h"

	#undef VECTOR
	#undef MATRIX2
	#undef MATRIX3
	#undef MATRIX4

	#define VECTOR float3
	#define MATRIX2 float3x2
	#define MATRIX3 float3x3
	#define MATRIX4 float3x4

		#include "Matrix2.h"
		#include "Matrix3.h"
		#include "Matrix4.h"

	#undef VECTOR
	#undef MATRIX2
	#undef MATRIX3
	#undef MATRIX4

	#define VECTOR float4
	#define MATRIX2 float4x2
	#define MATRIX3 float4x3
	#define MATRIX4 float4x4

		#include "Matrix2.h"
		#include "Matrix3.h"
		#include "Matrix4.h"

	#undef VECTOR
	#undef MATRIX2
	#undef MATRIX3
	#undef MATRIX4

#define MATRIX2X2 float2x2
#define MATRIX2X3 float2x3
#define MATRIX2X4 float2x4
#define MATRIX3X2 float3x2
#define MATRIX3X3 float3x3
#define MATRIX3X4 float3x4
#define MATRIX4X2 float4x2
#define MATRIX4X3 float4x3
#define MATRIX4X4 float4x4

	#define VECTOR float2
	#define MATRIX2 float2x2
	#define MATRIX3 float2x3
	#define MATRIX4 float2x4

		#include "MatrixFunctions.h"

	#undef VECTOR
	#undef MATRIX2
	#undef MATRIX3
	#undef MATRIX4

	#define VECTOR float3
	#define MATRIX2 float3x2
	#define MATRIX3 float3x3
	#define MATRIX4 float3x4

		#include "MatrixFunctions.h"

	#undef VECTOR
	#undef MATRIX2
	#undef MATRIX3
	#undef MATRIX4

	#define VECTOR float4
	#define MATRIX2 float4x2
	#define MATRIX3 float4x3
	#define MATRIX4 float4x4

		#include "MatrixFunctions.h"

	#undef VECTOR
	#undef MATRIX2
	#undef MATRIX3
	#undef MATRIX4

	#define VECTOR2 float2
	#define VECTOR3 float3
	#define VECTOR4 float4

	#include "MatrixScalarFunctions.h"
	#include "TransformFunctions.h"
	#include "Decompose.h"

	#undef VECTOR2
	#undef VECTOR3
	#undef VECTOR4

#undef MATRIX2X2
#undef MATRIX2X3
#undef MATRIX2X4
#undef MATRIX3X2
#undef MATRIX3X3
#undef MATRIX3X4
#undef MATRIX4X2
#undef MATRIX4X3
#undef MATRIX4X4

#undef VECTOR2
#undef VECTOR3
#undef VECTOR4
#undef QUATERNION

	#define VALUE float
		#include "Utility.h"
	#undef VALUE

	#define VALUE float2
		#include "Utility.h"
	#undef VALUE

	#define VALUE float3
		#include "Utility.h"
	#undef VALUE

	#define VALUE float4
		#include "Utility.h"
	#undef VALUE

#undef SIZE
#undef SCALAR
#undef ONE
#undef ZERO

#undef PASTER
#undef CONCAT

#include "VectorTypes.inc.h"
//#include "MatrixTypes.inc.h"
