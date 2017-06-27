#include <digi/Utility/StringUtility.h>

#include "ParticleNode.h"


namespace digi {

// ParticleSystemNode

ParticleSystemNode::ParticleSystemNode()
{
	this->addInput("time", "float");
	this->addState("lastTime", "float");
	this->addOutput("timeStep", "float");
}

ParticleSystemNode::~ParticleSystemNode()
{
}

std::string ParticleSystemNode::getNodeType()
{
	return "ParticleSystemNode";
}

void ParticleSystemNode::writeUpdateCodeThis(NodeWriter& w)
{
	// call inherited method
	this->MayaDagNode::writeUpdateCodeThis(w);

	w << "$.timeStep = max($.time - $.lastTime, 0.0f);\n";
	w << "$.lastTime = $.time;\n";
}


// ParticleNode

ParticleNode::ParticleNode(LifespanMode lifespanMode, const Path& parameters)
	: lifespanMode(lifespanMode), numForces(0), numAccelerations(0)
{
	this->addInput("time", "float", parameters + ".time");
	this->addInput("timeStep", "float", parameters + ".timeStep");
	
	this->addInput("id", "float");
	this->addState("birthTime", "float");
	this->addOutput("age", "float");
	switch (lifespanMode)
	{
	case FOREVER:
		this->addConstant("lifespan", 1e20f);
		break;
	case CONSTANT:
		this->addInput("lifespan", "float", parameters + ".lifespan");
		break;
	case CONSTANT_RANDOM:
		this->addInput("lifespanConstant", "float", parameters + ".lifespanConstant");	
		this->addInput("lifespanRandom", "float", parameters + ".lifespanRandom");	
		this->addState("lifespan", "float");	
		break;
	case PER_PARTICLE:
		this->addStateWithInitializer("lifespan", 1e20f);
		break;
	}
	
	this->addState("position", "float3");
	this->addState("velocity", "float3");
	this->addOutput("acceleration", "float3");
	
}

ParticleNode::~ParticleNode()
{
}

std::string ParticleNode::getNodeType()
{
	return "ParticleNode";
}

void ParticleNode::writeUpdateCodeThis(NodeWriter& w)
{
	if (this->scope == 0)
	{
		// create
		
		// set birth time
		w << "$.birthTime = time;\n";
		
		// set age
		w << "$.age = 0.0f;\n";

		// set acceleration
		w << "$.acceleration = 0.0f;\n";

		// set particle id
		w << "$.id = convert_$@id(id);\n";
			
		// check if lifespan mode is constant + random
		if (this->lifespanMode == CONSTANT_RANDOM)
			w << "$.lifespan = $.lifespanConstant + random($.lifespanRandom, seed);\n";

		// creation expression code
		if (!this->creationExpression.empty())
			w << this->creationExpression << "\n";	
	}
	else
	{
		// update
		
		// calc age
		w << "$.age = time - $.birthTime;\n";
		
		// clear acceleration. before dynamics expression may set it.
		w << "$.acceleration = 0.0f;\n";
		
		// before dynamics expression
		if (!this->beforeDynamicsExpression.empty())
		{
			w.writeLine();
			w.writeComment("before dynamics");
			w << this->beforeDynamicsExpression << "\n";
		}
		
		// dynamics
		if (this->numForces > 0 || this->numAccelerations > 0)
		{
			w.writeLine();
			w.writeComment("dynamics");
			
			w << "$.acceleration += ";
					
			// sum of forces times inverse mass
			if (this->numForces > 0)
			{
				w << "(";
				for (int i = 0; i < this->numForces; ++i)
				{
					if (i != 0)
						w << " + ";
					w << "$.forces[" << i << "]";
				}
				w << ")";
				if (this->hasAttribute("mass"))
					w << " / $.mass";

				if (this->numAccelerations > 0)
					w << "\n\t+ ";
			}
			
			// add accelerations
			for (int i = 0; i < this->numAccelerations; ++i)
			{
				if (i != 0)
					w << " + ";
				w << "$.accelerations[" << i << "]";
			}
			w << ";\n";	
		}

		// update velocity
		w << "$.velocity += $.acceleration * $.timeStep;\n";

		// update position
		w << "$.position += $.velocity * $.timeStep;\n";

		
		// after dynamics expression
		if (!this->afterDynamicsExpression.empty())
		{
			w.writeLine();
			w.writeComment("after dynamics");
			w << this->afterDynamicsExpression << "\n";
		}

		// output to alive variable
		if (this->lifespanMode != FOREVER)
			w << "alive = $.age < $.lifespan;\n";
	}
}

void ParticleNode::addForce(const Path& force)
{
	this->addInput("forces[" + toString(this->numForces) + "]", "float3", force);
	++this->numForces;
}

void ParticleNode::addAcceleration(const Path& acceleration)
{
	this->addInput("accelerations[" + toString(this->numAccelerations) + "]", "float3", acceleration);
	++this->numAccelerations;
}


// EmitterNode

EmitterNode::EmitterNode(Type type, const Path& numNewParticles, const Path& index,
	const Path& instance, const Path& parameters, const Path& position, const Path& velocity)
	: type(type)
{
	// chained index to determine which emitter is active
	if (!index.isNull())
	{
		this->addInput("numNewParticles", "int", numNewParticles);
		this->addInput("index", "int", index);
		this->addOutput("indexOut", "int");
	}
	
	this->addInput("worldMatrix", "float4x4", instance + ".worldMatrix");
	if (type == DIRECTIONAL_EMITTER)
	{
		// directional
		this->addInput("direction", "float3", parameters + ".direction");
		this->addInput("spread", "float", parameters + ".spread");
	}
	this->addInput("speed", "float", parameters + ".speed");
	this->addInput("speedRandom", "float", parameters + ".speedRandom");
		
	this->addAttribute("position", "float3", position, Attribute::OUTPUT | Attribute::REFERENCE);
	this->addAttribute("velocity", "float3", velocity, Attribute::OUTPUT | Attribute::REFERENCE);
}

EmitterNode::~EmitterNode()
{
}

std::string EmitterNode::getNodeType()
{
	return "EmitterNode";
}

void EmitterNode::writeUpdateCodeThis(NodeWriter& w)
{
	bool hasIndex = this->hasAttribute("index");
	if (hasIndex)
	{
		w << "int $nnp = $.numNewParticles;\n";
		w << "$.indexOut = $.index - $nnp;\n";
		w << "if ($.index >= 0 & $.index < $nnp)\n";
		w.beginScope();
	}
	
	// speed
	w << "float $speed = $.speed + $.speedRandom * (random(1.0f, seed) - 0.5f);\n";

	// set initial position and velocity
	w << "$.position = $.worldMatrix.w.xyz;\n";
	w << "$.velocity = ";
	switch (this->type)
	{
	case DIRECTIONAL_EMITTER:
		// directional
		w << "normalize(normalize(transformDirection($.worldMatrix, $.direction)) + sphereSurfaceRandom($.spread, seed)) * $speed;\n";
		break;
	default:
		// omni
		w << "sphereSurfaceRandom($speed, seed);\n";
		break;
	}
	
	if (hasIndex)
		w.endScope();
}


// EmitParticleNode

EmitParticleNode::EmitParticleNode(const Path& parameters)
{
	this->addInput("time", "float", parameters + ".time");
	
	this->addInput("tempPosition", "float3");
	this->addInput("tempVelocity", "float3");

	this->addState("position", "float3");
	this->addState("velocity", "float3");
}

EmitParticleNode::~EmitParticleNode()
{
}

std::string EmitParticleNode::getNodeType()
{
	return "EmitParticleNode";
}

void EmitParticleNode::writeUpdateCodeThis(NodeWriter& w)
{
	if (this->hasAttribute("particleSystemMatrix"))
	{
		w << "float4x4 $matrix = inv($.particleSystemMatrix);\n";
		w << "$.position = transformPosition($matrix, $.tempPosition);\n";
		w << "$.velocity = transformDirection($matrix, $.tempVelocity);\n";
	}
	else
	{
		w << "$.position = $.tempPosition;\n";
		w << "$.velocity = $.tempVelocity;\n";
	}
}


// FieldNode

FieldNode::FieldNode(StringRef name, Type type, const Path& instance, const Path& parameters,
	const Path& position, const Path& velocity, bool useMaxDistance, bool useAttenuation)
	: Node(name), type(type), useMaxDistance(useMaxDistance), useAttenuation(useAttenuation)
{
	this->addInput("worldMatrix", "float4x4", instance + ".worldMatrix");

	if (useMaxDistance)
		this->addInput("maxDistance", "float", parameters + ".maxDistance");
	if (useAttenuation)
		this->addInput("attenuation", "float", parameters + ".attenuation");

	// all fields have magnitude
	this->addInput("magnitude", "float", parameters + ".magnitude");

	switch (this->type)
	{
	case AIR_FIELD:
		this->addInput("direction", "float3", parameters + ".direction");
		this->addInput("speed", "float", parameters + ".speed");
		break;
	case DRAG_FIELD:
		break;
	case DIRECTIONAL_DRAG_FIELD:
		this->addInput("direction", "float3", parameters + ".direction");
		break;
	case NEWTON_FIELD:
		this->addInput("minDistance", "float", parameters + ".minDistance");
		break;
	case RADIAL_FIELD:
		break;
	case TURBULENCE_FIELD:
		this->addInput("frequency", "float", parameters + ".frequency");
		//!
		break;
	case UNIFORM_FIELD:
		this->addInput("direction", "float3", parameters + ".direction");
		break;
	case VORTEX_FIELD:
		this->addInput("axis", "float3", parameters + ".axis");
		break;
	}

	// particle position and velocity	
	this->addInput("position", "float3", position);
	this->addInput("velocity", "float3", velocity);

	// resulting force or acceleration
	this->addOutput("output", "float3");
}

FieldNode::~FieldNode()
{
}

std::string FieldNode::getNodeType()
{
	switch (this->type)
	{
		case AIR_FIELD:
			return "AirFieldNode";
		case DRAG_FIELD:
			return "DragFieldNode";
		case DIRECTIONAL_DRAG_FIELD:
			return "DirectionalDragFieldNode";
		case NEWTON_FIELD:
			return "NewtonFieldNode";
		case RADIAL_FIELD:
			return "RadialFieldNode";
		case TURBULENCE_FIELD:
			return "TurbulenceFieldNode";
		case UNIFORM_FIELD:
			return "UniformFieldNode";
		case VORTEX_FIELD:
			return "VortexFieldNode";
	}
	
	return "FieldNode";
}

void FieldNode::writeUpdateCodeThis(NodeWriter& w)
{
	// generate force
	std::string force;
	bool usePosition = false;
	bool useDistance = this->useMaxDistance || this->useAttenuation;
	switch (this->type)
	{
		case AIR_FIELD:
			force = "$.speed * ($.magnitude * normalize($.direction) - $.velocity)";
			break;
		case DRAG_FIELD:
			force = "-$.magnitude * $.velocity";
			break;
		case DIRECTIONAL_DRAG_FIELD:
			useDistance = true;
			force = "-$.magnitude / lengthSquared($.direction) * $.direction * dot($.velocity, $.direction)";
			break;
		case NEWTON_FIELD:
			// exerts an attractive force in the direction of the force position
			force = "$.magnitude * 0.66666f * (1.0f - 1.0f / (1.0f + max($distance - $.minDistance, 0.0f))) * $direction / (length($direction) + 1e-10f)";
			break;
		case RADIAL_FIELD:
			useDistance = true;
			force = "$.magnitude * $distance";
			break;
		case TURBULENCE_FIELD:
			usePosition = true;
			force = "$.magnitude * noise3($.position * $.frequency)";
			break;
		case UNIFORM_FIELD:
			force = "$.magnitude * normalize($.direction)";
			break;
		case VORTEX_FIELD:
			force = "($.magnitude * cross($direction, normalize($.axis)) + sqr($.magnitude * 0.067f) * $direction) * $.scale"; // scale = fps/100
			break;
	}

	// generate position if needed
	if (usePosition || useDistance)
			w << "float3 $position = $.worldMatrix.w.xyz;\n";

	// generate distance if needed
	if (useDistance)
	{
		w << "float3 $direction = $position - $.position;\n"; // field position - particle position
		w << "float $distance = length($direction);\n";
	}

	// apply max distance and/or  attenuation
	if (this->useMaxDistance && this->useAttenuation)
		force += "\n\t* ($distance <= $.maxDistance ? pow($distance + 1.0f, -$.attenuation) : 0.0f)";
	else if (this->useAttenuation)
		force += "\n\t* pow($distance + 1.0f, -$.attenuation)";
	else if (this->useMaxDistance)
		force += "\n\t* ($distance <= $.maxDistance ? 1.0f : 0.0f)";

	// write to output
	w << "$.output = " << force << ";\n";
}

} // namespace digi
