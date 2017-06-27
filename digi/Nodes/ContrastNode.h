#ifndef digi_Nodes_ContrastNode_h
#define digi_Nodes_ContrastNode_h

#include <digi/CodeGenerator/Node.h>


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Contrast node.
	
	inputs:
		input (float - float4)
		contrast (float - float4)
		bias (float - float4)
		
	outputs:
		result (float - float4)
*/	
class ContrastNode : public Node
{
public:

	ContrastNode(int numDimensions);

	virtual ~ContrastNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);
			
protected:
	
	int numDimensions;
};

/// @}

} // namespace digi

#endif
