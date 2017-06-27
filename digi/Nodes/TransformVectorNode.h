#ifndef digi_Nodes_TransformVectorNode_h
#define digi_Nodes_TransformVectorNode_h

#include <digi/Math/All.h>
#include <digi/CodeGenerator/Node.h>


namespace digi {

/// @addtogroup Nodes
/// @{

/*
	multiplies a constant vector with the input matrix
	
	inputs:
		matrix (float4x4)
	outputs:
		vector (float4)
*/
class TransformVectorNode : public Node
{
public:

	TransformVectorNode(float4 localVector);
	virtual ~TransformVectorNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);
	
	// the local vector to transform by the matrix
	float4 localVector;
};

/// @}

} // namespace digi

#endif
