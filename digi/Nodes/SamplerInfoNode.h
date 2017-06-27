#ifndef digi_Nodes_EnvNode_h
#define digi_Nodes_EnvNode_h

#include <digi/CodeGenerator/Node.h>


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	sampler info node

	Outputs:
		facingRatio (float, cosine between view vector and surface normal)
		backFacing (float, 0 for front facing, 1 for back facing)
*/
class SamplerInfoNode : public Node
{
public:
		
	SamplerInfoNode();
	
	virtual ~SamplerInfoNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);
};

/// @}

} // namespace digi

#endif
