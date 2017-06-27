#ifndef digi_Nodes_SineDeformerNode_h
#define digi_Nodes_SineDeformerNode_h

#include "DeformerNode.h"


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Sine deformer node.
	
	inputs:
		objectMatrix (float4x4)
		handleMatrix (float4x4)
		amplitude (float)
		wavelength (float)
		offset (float)
		lowBound (float)
		highBound (float)
		
	outputs:
		projection
		projectionOffset
		direction
		frequency
*/
class SineDeformerNode : public Node
{
public:

	SineDeformerNode(const Path& worldMatrix);
	virtual ~SineDeformerNode();

	virtual std::string getNodeType();
	
	virtual void writeUpdateCodeThis(NodeWriter& w);
};


/*	
	Sine deformer bounding box node. Calculates conservative bounding box.
	
	inputs:
		direction
		
		input
			center
			size
		
	outputs:	
		output
			center
			size
*/
class SineDeformerBoundingBoxNode : public BoundingBoxNode
{
public:

	SineDeformerBoundingBoxNode(const Path& inputPath, const Path& parameters);
	virtual ~SineDeformerBoundingBoxNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);
};


/*	
	Sine deformer verex node. Does the vertex processing.
	
	inputs:
		projection
		projectionOffset
		direction
		frequency
		offset
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
class SineDeformerVertexNode : public VertexNode
{
public:

	SineDeformerVertexNode(const Path& inputPath, const Path& parameters);
	virtual ~SineDeformerVertexNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);
};

/// @}

} // namesapce digi

#endif
