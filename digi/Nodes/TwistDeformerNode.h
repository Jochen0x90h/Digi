#ifndef digi_Nodes_TwistDeformerNode_h
#define digi_Nodes_TwistDeformerNode_h

#include "DeformerNode.h"


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Twist deformer node.
	
	inputs:
		objectMatrix (float4x4)
		handleMatrix (float4x4)
		startAngle (float)
		endAngle (float)
		lowBound (float)
		highBound (float)
		
	outputs
		object2handle (float4x4)
		handle2object (float4x4)
		
*/
class TwistDeformerNode : public Node
{
public:

	TwistDeformerNode(const Path& worldMatrix);
	virtual ~TwistDeformerNode();

	virtual std::string getNodeType();
	
	virtual void writeUpdateCodeThis(NodeWriter& w);
};


/*	
	Twist deformer bounding box node. Calculates conservative bounding box.
	
	inputs:
		input
			center
			size
		
	outputs:	
		output
			center
			size
*/
class TwistDeformerBoundingBoxNode : public BoundingBoxNode
{
public:

	TwistDeformerBoundingBoxNode(const Path& inputPath, const Path& parameters);
	virtual ~TwistDeformerBoundingBoxNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);
};


/*	
	Twist deformer verex node. Does the vertex processing.
	
	inputs:
		startAngle (float)
		endAngle (float)
		lowBound (float)
		highBound (float)
		object2handle (float4x4)
		handle2object (float4x4)
 
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
class TwistDeformerVertexNode : public VertexNode
{
public:

	TwistDeformerVertexNode(const Path& inputPath, const Path& parameters);
	virtual ~TwistDeformerVertexNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);
};

/// @}

} // namesapce digi

#endif
