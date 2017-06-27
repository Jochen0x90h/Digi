#ifndef digi_Nodes_MayaEmitterNode_h
#define digi_Nodes_MayaEmitterNode_h

#include <digi/Math/All.h>
#include <digi/CodeGenerator/Node.h>

#include "MayaTransformNode.h"


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	emitter node, calculates how many particles to emit for each connected
	particle system. since the time of a particle system is an animatable attribute
	the timeStep may be different.
	
	Inputs:
		rate (particles/second)
		speed
		speedRandom
		instance
			0
				timeStep (from particle system)
						
	Outputs:
		instance
			0
				numNewParticles (int, particles to emit)
		
	State:
		instance
			0
				fraction
*/
class MayaEmitterNode : public Node
{
public:
	
	MayaEmitterNode(bool scaleRateBySpeed);

	virtual ~MayaEmitterNode();

	virtual std::string getNodeType();
	

	virtual void writeUpdateCodeThis(NodeWriter& w);

	
	void addInstance(StringRef instanceString, const Path& particleSystem);

	
	bool scaleRateBySpeed;
};

/// @}

} // namespace digi

#endif
