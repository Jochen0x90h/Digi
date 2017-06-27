#ifndef digi_Nodes_SquashDeformerNode_h
#define digi_Nodes_SquashDeformerNode_h

#include "DeformerNode.h"


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Squash deformer node.
	
	inputs:
		objectMatrix (float4x4)
		handleMatrix (float4x4)
		factor (float)
		expand (float)
		startSmoothness (float)
		endSmoothness (float)
		lowBound (float)
		highBound (float)
 
	outputs:
		f (float)
		object2handle (float4x4)
		handle2object (float4x4)
*/
class SquashDeformerNode : public Node
{
public:

	SquashDeformerNode(const Path& worldMatrix);
	virtual ~SquashDeformerNode();

	virtual std::string getNodeType();
	
	virtual void writeUpdateCodeThis(NodeWriter& w);
};


/*	
	Bend deformer bounding box node. Calculates conservative bounding box.
	
	inputs:
		rotatePivot
		rotateAxis
		
		input
			center
			size
		
	outputs:	
		output
			center
			size
*/
class SquashDeformerBoundingBoxNode : public BoundingBoxNode
{
public:

	SquashDeformerBoundingBoxNode(const Path& inputPath, const Path& parameters);
	virtual ~SquashDeformerBoundingBoxNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);
};


/*	
	Bend deformer verex node. Does the vertex processing.
	
	inputs:
		projection
		projectionOffset
		xyProjection
		rotatePivot
		rotateAxis
		angleScale
		lowBound
		highBound
		
		input
			position
			tangent
			bitangent
		
	outputs:	
		output
			position
			tangent
			bitangent		
*/
class SquashDeformerVertexNode : public VertexNode
{
public:

	SquashDeformerVertexNode(const Path& inputPath, const Path& parameters);
	virtual ~SquashDeformerVertexNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);
};

/// @}

} // namesapce digi

#endif
