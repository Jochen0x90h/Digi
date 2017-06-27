#ifndef digi_Scene_ParticleSystem_h
#define digi_Scene_ParticleSystem_h

#include "Shape.h"


namespace digi {

/// @addtogroup Scene
/// @{

class ParticleSystem : public Shape
{
public:

	// field of initial state
	struct Field
	{
		// name of state (i.e. field in particle struct)
		std::string name;
		
		// the data
		Pointer<Buffer> buffer;
				
		Field() {}
		Field(std::string name, Pointer<Buffer> buffer)
			: name(name), buffer(buffer) {}
	};


	ParticleSystem()
		: maxNumParticles() {}

	virtual ~ParticleSystem();

	virtual void save(ObjectWriter& w);		


	// name of particle system (must be unique, used for struct names and debugging)
	std::string name;

	// path of particle system in instance.uniforms struct
	std::string path;

	// list of attributes to copy from scene into uniforms struct for this particle system
	std::vector<std::string> emitterAssignments;
	std::vector<std::string> assignments;

	
	// uniform input structure for emitters (null if no emitters)
	Pointer<Type> emitterUniformType;
	
	// emitter code
	std::string emitterCode;

	// maximum number of particles for emitters
	uint maxNumParticles;


	// uniform input structure (contains values that are equal for all particles)
	Pointer<Type> uniformType;

	// structure for one particle (contains per-particle values)
	Pointer<Type> particleType;

	// initial state of particle system
	std::vector<Field> initialState;
	
	// particle creation code
	std::string createCode;

	// particle update code
	std::string updateCode;
};

/// @}

} // namespace digi

#endif
