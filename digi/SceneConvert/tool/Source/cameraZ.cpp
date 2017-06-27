// target point
float tx;
float ty;
float tz;

// rotate
float rx;
float ry;
float rz;

// distance
float di;

// resulting view matrix
float4x4 r;

extern "C" void main()
{
	float3x3 rot = matrix3x3Rotate(quaternionEulerYXZ(vector3(rx, rz, ry)) * quaternion(0.70710678118f, 0.0f, 0.0f, 0.70710678118f));
	float3 pos = vector3(tx, ty, tz) + rot * vector3(0.0f, 0.0f, di);
	r = matrix4x4TranslateMatrix3x3(-pos * rot, transpose(rot));	
}
