#ifndef digi_Nodes_ParticleNode_h
#define digi_Nodes_ParticleNode_h

#include "MayaDagNode.h"


namespace digi {

/// @addtogroup Nodes
/// @{

class ParticleSystemNode : public MayaDagNode
{
public:

	ParticleSystemNode();
	
	virtual ~ParticleSystemNode();
	
	virtual std::string getNodeType();
	
	virtual void writeUpdateCodeThis(NodeWriter& w);	
};

/**
	This node holds all attributes of a particle.
	
	Inputs:
		time
		timeStep
		
	Attributes (can be input, output or state):
		id
		birthTime
		age
		lifespan
		position
		velocity		
		acceleration
		mass (optional)
*/
class ParticleNode : public Node
{
public:
	
	enum LifespanMode
	{
		FOREVER,
		CONSTANT,
		CONSTANT_RANDOM,
		PER_PARTICLE
	};
		
	ParticleNode(LifespanMode lifespanMode, const Path& parameters);
	virtual ~ParticleNode();

	virtual std::string getNodeType();

	//void writeInitCode(NodeWriter& w, int scope);
	virtual void writeUpdateCodeThis(NodeWriter& w);
	
	void addForce(const Path& force);
	void addAcceleration(const Path& acceleration);

	LifespanMode getLifespanMode() {return this->lifespanMode;}

	std::string creationExpression;
	std::string beforeDynamicsExpression;
	std::string afterDynamicsExpression;

	Pointer<Node> outerNode;

protected:

	LifespanMode lifespanMode;
	int numForces;
	int numAccelerations;
};


/**
	this node represents an emitter inside the particle system and calculates initial
	position and velocity when a particle is emitted.

	Inputs:
		numNewParticles (int)
		index (int)
		worldMatrix (float4x4)
		direction (float3, only directional)
		spread (float3, only directional)
		speed (float)
		speedRandom(float)
		
	Outputs:
		indexOut (int)

*/
class EmitterNode : public Node
{
public:

	enum Type
	{
		DIRECTIONAL_EMITTER,
		OMNI_EMITTER,
	};

	EmitterNode(Type type, const Path& numNewParticles, const Path& index,
		const Path& instance, const Path& parameters, const Path& position, const Path& velocity);
	virtual ~EmitterNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);

protected:

	Type type;
};

/**
	this node stands for the ParticleNode inside the emitter node graph.
	it gets position and velocity from emitter and writes them into particle state

	Inputs:
		tempPosition (float3)
		tempVelocity (float3)
		particleSystemMatrix (float4x4, optional)
		
	State:
		position (in particle state)
		velocity (in particle state)
*/
class EmitParticleNode : public Node
{
public:

	EmitParticleNode(const Path& parameters);
	
	virtual ~EmitParticleNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);
};


/**
	Force field for particle systems. The force output also may be used
	as acceleration by connecting it to the acceleration input of the particle node.
	This way the mass gets ignored.

	inputs:
		position (reference)
		velocity (reference)

	outputs:
		force


	all fields:
		uniform.position = field.position
		uniform.maxDistance = field.maxDistance
		uniform.attenuation = field.attenuation
		
		distance = length(position - uniform.position)
		attenuation = distance <= uniform.maxDistance ? pow(distance + 1, -uniform.attenuation) : 0
	
	air field:
		uniform.magnitudeDirection = field.magnitude * normalize(field.direction)
		uniform.speed = field.speed
		
		force = uniform.speed * (uniform.magnitudeDirection - velocity) * attenuation

	drag field:
		uniform.magnitude = field.magnitude

		force = -uniform.magnitude * velocity * attenuation
		
	directional drag field:
		uniform.direction = normalize(field.direction)
		uniform.magnitudeDirection = field.magnitude * normalize(field.direction)

		force = -uniform.magnitudeDirection * dot(velocity, uniform.direction) * attenuation

	newton field:
		uniform.magnitude = field.magnitude
		uniform.minDistance = field.minDistance
		
		force = -uniform.magnitude * mass / max(distance, uniform.minDistance)^2 * attenuation

	radial field:
		uniform.magnitude = field.magnitude

		strength = 1 - distance / uniform.maxDistance * (1 - field.radialType)
		force = uniform.magnitude * (position - uniform.position) / distance * strength * attenuation

	turbulence field:
		uniform.magnitude = field.magnitude
		uniform.frequency = field.frequency
		uniform.phase = vector3(field.phaseX, field.phaseY, field.phaseZ)
		uniform.noiseLevel = field.noiseLevel // number of octaves
		uniform.noiseRatio = field.noiseRatio // attenuation per octave
		
		force = uniform.magnitude * noise3(position * uniform.frequency + uniform.phase)
	
	uniform field:
		uniform.magnitudeDirection = field.magnitude * normalize(field.direction)

		force = uniform.magnitudeDirection * attenuation
		
	vortex field
		uniform.axis = field.axis
		
		force = cross(position - uniform.position, field.axis) / distance
*/
class FieldNode : public Node
{
public:
	enum Type
	{
		AIR_FIELD,
		DRAG_FIELD,
		DIRECTIONAL_DRAG_FIELD,
		NEWTON_FIELD,
		RADIAL_FIELD,
		TURBULENCE_FIELD,
		UNIFORM_FIELD,
		VORTEX_FIELD
	};
	
	// constructor. parameters contains the scene parameters for the field
	FieldNode(StringRef name, Type type, const Path& instance, const Path& parameters, const Path& position,
		const Path& velocity, bool useMaxDistance, bool useAttenuation = true);

	virtual ~FieldNode();
	
	virtual std::string getNodeType();
	
	virtual void writeUpdateCodeThis(NodeWriter& w);

protected:
	
	Type type;
	bool useMaxDistance;
	bool useAttenuation;
};

/// @}

} // namesapce digi

#endif
