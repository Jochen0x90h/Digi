#ifndef digi_Nodes_CheckerNode_h
#define digi_Nodes_CheckerNode_h

#include <digi/Math/All.h>

#include "TextureBaseNode.h"


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Checker texture

	Inputs:
		value1 (float to float4)
		value2 (float to float4)
		contrast (float)
		
		input (float to float4)
	
	Outputs:
		outut (float to float4)
*/
class CheckerNode : public TextureBaseNode
{
public:

	CheckerNode(int inputDimension, int outputDimension, const Path& parameters);
	CheckerNode(int inputDimension, int outputDimension);
	
	virtual ~CheckerNode();

	virtual std::string getNodeType();

	virtual int getAddressFlags();
	virtual void generateTexture(NodeWriter& w);
};

/// @}

} // namespace digi

#endif
