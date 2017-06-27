struct
{
	int numNewParticles;
} emitter;
struct
{
	float timeStep;
} uniform;
struct
{
	float3 position;
	float size;
	int index;
} particle;
int index;
int id;
int seed;
bool alive;

extern "C" void create()
{
	{
	}
	{
		particle.position = vector3(-0.2f + 0.1f * float(id), 0.0f, 0.0f);
		particle.size = 0.1f;
		particle.index = 0;
	}
}
extern "C" void update()
{
	particle.position.y += uniform.timeStep * 0.05f;
}
