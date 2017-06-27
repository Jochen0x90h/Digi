#ifndef digi_Nodes_LightNode_h
#define digi_Nodes_LightNode_h

#include <digi/CodeGenerator/Node.h>


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Light node for vertex or pixel shaders

	Inputs:
		worldMatrix (float4x4)
		worldVisibility (bool)
		color (float3)
		intensity (float)		
		coneAngle (float, only spot)
		penumbraAngle (float, only spot)
		shade (float, only ambient, interpolates between ambient and diffuse)
	
	Outputs:
		output
			vector (float3, normalized)
			color (float3, color multiplied by intensity and falloff)
			shade (float, only ambient)
*/
class LightNode : public Node
{
public:

	enum Type
	{
		AMBIENT_LIGHT,
		DIRECTIONAL_LIGHT,
		POINT_LIGHT,
		SPOT_LIGHT
	};
	
	enum Decay
	{
		NONE,
		LINEAR,
		QUADRATIC,
		CUBIC
	};

	enum Flags
	{
		EMITS_DIFFUSE = 1,
		EMITS_SPECULAR = 2,
	};
		
	LightNode(Type type, Decay decay, int flags = EMITS_DIFFUSE | EMITS_SPECULAR);
	
	virtual ~LightNode();

	virtual std::string getNodeType();
	
	Type getType() {return this->type;}
	int getFlags() {return this->flags;}
	
	virtual void writeUpdateCodeThis(NodeWriter& w);

protected:

	Type type;
	Decay decay;
	int flags;
};

/// @}

} // namespace digi

#endif
