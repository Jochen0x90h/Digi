struct
{
	float f1;
	float3 v1;
} state;
struct
{
	float b[100u];
} buffers;
struct
{
	float x;
} uniforms;
Transform transforms[1u];
BoundingBox boundingBoxes[1u];

extern "C" void initState()
{
	state.f1 = buffers.b[55];
	state.v1 = 0.0f;
}
extern "C" void update()
{
	state.f1 *= 3.0f;
	state.v1 *= 5.0f * state.f1;
	uniforms.x = state.f1;
	transforms[0].matrix.x.x = state.f1;
	boundingBoxes[0].center = 0.0f;
	boundingBoxes[0].size = 1.0f;
}