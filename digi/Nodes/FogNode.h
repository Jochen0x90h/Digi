#ifndef digi_Nodes_FogNode_h
#define digi_Nodes_FogNode_h

#include <digi/CodeGenerator/Node.h>


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Fog node for calculating simple fog

	Inputs:
		color (float3)
		saturationDistance (float)
		nearDistance (float, if USE_DISTANCE)
		farDistance (float, USE_DISTANCE)
		minHeight (float, if USE_HEIGHT)
		maxHeight (float, if USE_HEIGHT)
		
		lightColor (float3)
	
		surfacePosition (float3, view space)
		input (float3)
	
	Outputs:
		output (float3)
*/
class FogNode : public Node
{
public:
	
	enum Flags
	{
		USE_DISTANCE = 1,
		USE_HEIGHT = 2,
		COLOR_BASED_TRANSPARENCY = 4,
	};
	

	/*FogNode(const Path& parameters, const Path& lightColor, const Path& surfacePosition,
		int flags = 0);*/
	FogNode(const Path& lightColor, int flags = 0);
	
	virtual ~FogNode();

	virtual std::string getNodeType();
	
	//Type getType() {return this->type;}
	int getFlags() {return this->flags;}

	virtual void writeUpdateCodeThis(NodeWriter& w);
	
protected:

	//Type type;
	int flags;
};


/// @}

} // namespace digi

#endif
