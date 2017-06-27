float4x4 viewMatrix;
float4x4 projectionMatrix;
float4x4 viewProjectionMatrix;
struct
{
	Texture2D sampler;
} material;
struct
{
	float scale;
	float3 offset;
} deformer;
float4x4 matrix;
int objectId;
struct
{
	float3 position;
	float2 texCoord;
} vertex;
float4 pPosition;
float4 output;
float4 outputs[16u];
bool sort;
float4 vPosition;
float frontFacing;
bool flip;
extern "C" void discard();
extern "C" void main()
{
	struct
	{
		float3 position;
		float2 texCoord;
	} input;
	{
		struct
		{
			float3 position;
		} output;
		{
			struct
			{
				float3 position;
			} input;
			input.position = vertex.position;
			output.position = input.position * deformer.scale + deformer.offset;
		}
		input.position = output.position;
		input.texCoord = vertex.texCoord * 1.4f + -0.2f;
	}
	float4x4 worldViewMatrix = viewMatrix * matrix;
	float3 position = transformPosition(worldViewMatrix, input.position);
	float4 position4 = vector4(position, 1.0f);
	pPosition = projectionMatrix * position4;
	float3 normal;
	float3 tangent;
	float3 bitangent;
	normal = 0.0f;
	tangent = 0.0f;
	bitangent = 0.0f;
	float3 viewVector = -normalize(position);
	float3 color = sample(material.sampler, input.texCoord).xyz;
	float3 alpha = 1.0f;
	sort = alpha.x < 0.99f;
	if (max(color) < 0.01f & alpha.x < 0.01f) discard();
	output = vector4(color, alpha.x);
}