#ifndef digi_Nodes_BendDeformerNode_h
#define digi_Nodes_BendDeformerNode_h

#include "DeformerNode.h"


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Bend deformer node.
	
	inputs:
		objectMatrix (float4x4)
		handleMatrix (float4x4)
		curvature (float)
		lowBound (float)
		highBound (float)
		
	outputs:
		rp (rotate pivot, float)
		lb (low bound, float)
		hb (high bound, float)
		object2handle (float4x4)
		handle2object (float4x4)
*/
class BendDeformerNode : public Node
{
public:

	BendDeformerNode(const Path& worldMatrix);
	virtual ~BendDeformerNode();

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
class BendDeformerBoundingBoxNode : public BoundingBoxNode
{
public:

	BendDeformerBoundingBoxNode(const Path& inputPath, const Path& parameters);
	virtual ~BendDeformerBoundingBoxNode();

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
class BendDeformerVertexNode : public VertexNode
{
public:

	BendDeformerVertexNode(const Path& inputPath, const Path& parameters);
	virtual ~BendDeformerVertexNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);
};

/// @}

} // namesapce digi

#endif
