struct Transform
{
	bool visible;
	float4x4 matrix;
};

struct BoundingBox
{
	float3 center;
	float3 size;
};

// Projection for camera or orthographic view
struct Projection
{
	// note: this struct must be synced with digi/Engine/RenderTypes.h and care must be taken of aligment of vectors

	// fit mode: 1: fill, 2: horizontal, 3: vertical, 4: overscan
	// type: positive: perspective, negative: orthographic
	float mode;

	// focal length for perspective, orthographic width for orthographic projection
	float scale;

	float2 filmSize;
	float2 filmOffset;
	float nearClipPlane;
	float farClipPlane;
};
