#ifndef digi_Nodes_GeometryNode_h
#define digi_Nodes_GeometryNode_h

#include <digi/CodeGenerator/Node.h>


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	geometry node. uses input.position, input.tangent and input.bitangent or input.normal
	
	inputs:
		tangentSpaceNormal (float3, optional)
	
	outputs:
		surfacePosition (float3)
		viewVector (float3)
		tangent (float3)
		bitangent (float3, optional)
		normal (float3, optional)
*/
class GeometryNode : public Node
{
public:
	
	GeometryNode(const Path& tangentSpaceNormal, bool outputTangent);
	
	virtual ~GeometryNode();

	virtual std::string getNodeType();
		
	void writeUpdateCodeThis(NodeWriter& w);
	

	bool hasTangentSpace;
};

/// @}

} // namespace digi

#endif
