Transform transform;
struct
{
	float3 position;
	float size;
	int index;
} particle;
int index;
float4x4 particleMatrix;

extern "C" void main()
{
	float4x4& matrix = transform.matrix;
	particleMatrix = matrix4x4PositionScale(particle.position, splat3(particle.size));
}
