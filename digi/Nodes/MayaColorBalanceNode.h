#ifndef digi_Nodes_TextureWrapperNode_h
#define digi_Nodes_TextureWrapperNode_h

#include "TextureBaseNode.h"


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Maya color balance for texture and vertex color nodes
	
	Inputs:
		defaultColor (float3)
		gain (float4, color and alpha)
		offset (float4, color and alpha)

		input (florat3 or float4, color and alpha)
		enable (float, 1.0f: input, 0.0f: defaultColor)

	Outputs:
		output.color (float4, color and alpha)
		output.transparency (float3)
*/
class MayaColorBalanceNode : public Node
{
public:

	MayaColorBalanceNode(const Path& parameters, const Path& input, const Path& enable);
	MayaColorBalanceNode(const Path& input, const Path& enable);
	
	virtual ~MayaColorBalanceNode();

	std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);

	// alpha gets replaced by luminance of color component
	bool alphaIsLuminance;

	// invert color/alpha
	bool invert;
};

/// @}

} // namespace digi

#endif
