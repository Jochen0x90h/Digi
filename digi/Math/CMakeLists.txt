# public header files (visible to users of this library)
set(HEADERS
	All.h
	Decompose.h
	half.h
	Intersection.h
	MathBase.h
	Matrix2.h
	Matrix3.h
	Matrix4.h
	MatrixFunctions.h
	MatrixTypes.h
	MatrixTypes.inc.h
	Noise.h
	Operators.h
	Quaternion.h
	QuaternionFunctions.h
	quaternionEulerXYZ.h
	quaternionEulerXZY.h
	quaternionEulerYXZ.h
	quaternionEulerYZX.h
	quaternionEulerZXY.h
	quaternionEulerZYX.h
	Random.h
	RotationFunctions.h
	SwizzleDefine.inc.h
	SwizzleUndef.inc.h
	TransformFunctions.h
	TypesDefine.inc.h
	TypesUndef.inc.h
	Utility.h
	Vector2.h
	Vector2.inc.h
	Vector2Ref.inc.h
	Vector3.h
	Vector3.inc.h
	Vector3Ref.inc.h
	Vector4.h
	Vector4.inc.h
	Vector4Ref.inc.h
	VectorFunctions.h
	VectorTypes.h
	VectorTypes.inc.h
	GTestHelpers.h
)

# source files
set(FILES
	All.cpp
	half.cpp
	Noise.cpp
	Random.cpp
)

# add definitions
add_definitions(${Math_DEFINITIONS})

CREATE_LIBRARY(digi.Math HEADERS ${HEADERS} FILES ${FILES} VERSIONIZE EXPORT Digi)


# add test
add_subdirectory(test)
