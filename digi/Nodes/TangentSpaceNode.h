#ifndef digi_Nodes_TangentSpaceNode_h
#define digi_Nodes_TangentSpaceNode_h

#include <digi/CodeGenerator/Node.h>


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	node for calculating tangent spaces
	
	the outputs are calculated as follows:
		normal = cross(tangent, bitangent);
	
		output0.tangent = transform0.x * trangent + transform0.y * bitangent;
		output0.bitangent = transform0.z * trangent + transform0.w * bitangent;
	
	inputs:
		tangent
		bitangent
	
		transform0
	outputs:
		normal
	
		output0.tangent
		output0.bitangent
		...
*/
class TangentSpaceNode : public Node
{
	public:
		
		TangentSpaceNode(int numTangentSpaces);
		
		virtual ~TangentSpaceNode();

		virtual std::string getNodeType();

	protected:

		virtual void writeUpdateCodeThis(NodeWriter& w);
};


/// @}

} // namespace digi

#endif
