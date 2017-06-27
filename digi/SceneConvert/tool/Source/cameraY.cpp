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
	//r = inv(matrix4x4TranslateRotate(vector3(tx, ty, tz), quaternionEulerZXY(vector3(rx, ry, rz))) * matrix4x4Translate(vector3(0.0f, 0.0f, di)));
	
	float3x3 rot = matrix3x3Rotate(quaternionEulerZXY(vector3(rx, ry, rz)));
	float3 pos = vector3(tx, ty, tz) + rot * vector3(0.0f, 0.0f, di);
	r = matrix4x4TranslateMatrix3x3(-pos * rot, transpose(rot));	
}
