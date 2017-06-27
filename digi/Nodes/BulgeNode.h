#ifndef digi_Nodes_BulgeNode_h
#define digi_Nodes_BulgeNode_h

#include "TextureBaseNode.h"


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Bulge texture

	Inputs:
		width (float to float4)
		
		input (float to float4)
	
	Outputs:
		outut (float to float4)
*/
class BulgeNode : public TextureBaseNode
{
public:

	BulgeNode(int inputDimension, int outputDimension, const Path& parameters);
	BulgeNode(int inputDimension, int outputDimension);
	
	virtual ~BulgeNode();

	virtual std::string getNodeType();

	virtual int getAddressFlags();
	virtual void generateTexture(NodeWriter& w);
};

/// @}

} // namespace digi

#endif
