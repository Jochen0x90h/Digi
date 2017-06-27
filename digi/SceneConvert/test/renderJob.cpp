float4x4 viewProjectionMatrix;
Transform transform;
BoundingBox boundingBox;
struct
{
	float4x4 matrix;
	float distance;
} renderJob;

extern "C" void main()
{
	float4x4& matrix = transform.matrix;
	renderJob.matrix = matrix;
}
