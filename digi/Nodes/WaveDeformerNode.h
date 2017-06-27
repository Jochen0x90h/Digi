#ifndef digi_Nodes_WaveDeformerNode_h
#define digi_Nodes_WaveDeformerNode_h

#include "DeformerNode.h"


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Wave deformer node.
	
	inputs:
		objectMatrix (float4x4)
		handleMatrix (float4x4)
		amplitude (float)
		wavelength (float)
		offset (float)
		minRadius (float)
		maxRadius (float)
		
	outputs:
		frequency
		outOffset
		projection
		projectionOffset
		direction
*/
class WaveDeformerNode : public Node
{
public:

	WaveDeformerNode(const Path& worldMatrix);
	virtual ~WaveDeformerNode();

	virtual std::string getNodeType();
	
	virtual void writeUpdateCodeThis(NodeWriter& w);
};


/*	
	Wave deformer bounding box node. Calculates conservative bounding box.
	
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
class WaveDeformerBoundingBoxNode : public BoundingBoxNode
{
public:

	WaveDeformerBoundingBoxNode(const Path& inputPath, const Path& parameters);
	virtual ~WaveDeformerBoundingBoxNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);
};


/*	
	Wave deformer verex node. Does the vertex processing.
	
	inputs:
		projection
		projectionOffset
		direction
		frequency
		offset
		minRadius
		maxRadius
		
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
class WaveDeformerVertexNode : public VertexNode
{
public:

	WaveDeformerVertexNode(const Path& inputPath, const Path& parameters);
	virtual ~WaveDeformerVertexNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);
};

/// @}

} // namesapce digi

#endif
