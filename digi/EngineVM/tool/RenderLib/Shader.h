typedef int __attribute__((address_space(2)))* Texture2D;
typedef int __attribute__((address_space(3)))* Texture3D;
typedef int __attribute__((address_space(4)))* TextureCube;

float4 sample(Texture2D t, float2 p);
float4 sample(Texture3D t, float3 p);
float4 sample(TextureCube t, float3 p);

float4 sample(Texture2D t, float2 p, float lod);
float4 sample(Texture3D t, float3 p, float lod);
float4 sample(TextureCube t, float3 p, float lod);

float4 sample(Texture2D t, float2 p, float2 dpdx, float2 dpdy);
float4 sample(Texture3D t, float3 p, float3 dpdx, float3 dpdy);
float4 sample(TextureCube t, float3 p, float3 dpdx, float3 dpdy);

float dFdx(float a);
float dFdy(float a);

float2 dFdx(float2 a);
float2 dFdy(float2 a);

float3 dFdx(float3 a);
float3 dFdy(float3 a);

float4 dFdx(float4 a);
float4 dFdy(float4 a);
