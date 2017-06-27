struct
{
	float3 position;
} vertex;
struct
{
	float3 position;
} result;

extern "C" void main()
{
	result.position = vertex.position;
}