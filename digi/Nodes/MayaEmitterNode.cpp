#include <digi/Utility/StringUtility.h>

#include "MayaEmitterNode.h"


namespace digi {


MayaEmitterNode::MayaEmitterNode(bool scaleRateBySpeed)
	: scaleRateBySpeed(scaleRateBySpeed)
{
	this->addInput("rate", "float");
	this->addInput("speed", "float");
	this->addInput("speedRandom", "float");

	// tells the particle system how many particles to emit in a frame
	//this->addOutput("numNewParticles", "int");
	
	// states
	//this->addState("fraction", "float");
}

MayaEmitterNode::~MayaEmitterNode()
{
}

std::string MayaEmitterNode::getNodeType()
{
	return "MayaEmitterNode";
}

void MayaEmitterNode::writeUpdateCodeThis(NodeWriter& w)
{
	// get rate
	w << "float $rate = $.rate";
	if (this->scaleRateBySpeed)
		w << " * ($.speed + $.speedRandom * 0.5f)";
	w << ";\n";

	foreach (Path path, this->getPathElementList("instance"))
	{
		w.push(path);

		// calc number of new particles
		w << "float $npf = $.fraction + $.timeStep * $rate;\n";
		w << "float $np = floor($npf);\n";
		w << "$.numNewParticles = convert_int($np);\n";
		w << "$.fraction = $npf - $np;\n";

		w.pop();
	}
}

void MayaEmitterNode::addInstance(StringRef instanceString, const Path& particleSystem)
{
	this->addInput(instanceString + ".timeStep", "int", particleSystem + ".timeStep");
	this->addOutput(instanceString + ".numNewParticles", "int");
	this->addState(instanceString + ".fraction", "float");
}

} // namespace digi
