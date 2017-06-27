#ifndef digi_Nodes_DeformerNode_h
#define digi_Nodes_DeformerNode_h

#include <digi/CodeGenerator/Node.h>


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Base class for bounding box calculation nodes

	Inputs:
		input
			center
			size

	Outputs:
		output
			center
			size
*/
class BoundingBoxNode : public Node
{
public:

	BoundingBoxNode(const Path& inputPath);
	virtual ~BoundingBoxNode();
};

/**
	Base class for vertex deformer nodes

	Inputs:
		input
			position
			tangent
			bitangent

	Outputs:
		output
			position
			tangent
			bitangent
*/
class VertexNode : public Node
{
public:

	VertexNode(const Path& inputPath);
	virtual ~VertexNode();
};

/// @}

} // namesapce digi

#endif
