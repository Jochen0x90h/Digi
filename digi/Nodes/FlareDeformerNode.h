#ifndef digi_Nodes_FlareDeformerNode_h
#define digi_Nodes_FlareDeformerNode_h

#include "DeformerNode.h"


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Flare deformer node.
	
	inputs:
		objectMatrix (float4x4)
		handleMatrix (float4x4)
		lowBound (float)
		highBound (float)
		startFlareX (float)
		startFlareZ (float)
		endFlareX (float)
		endFlareZ (float)
		curve (float)
		
	outputs
		object2handle (float4x4)
		handle2object (float4x4)
*/
class FlareDeformerNode : public Node
{
public:

	FlareDeformerNode(const Path& worldMatrix);
	virtual ~FlareDeformerNode();

	virtual std::string getNodeType();
	
	virtual void writeUpdateCodeThis(NodeWriter& w);
};


/*	
	Flare deformer bounding box node. Calculates conservative bounding box.
	
	inputs:
 
		input
			center
			size
		
	outputs:	
		output
			center
			size
*/
class FlareDeformerBoundingBoxNode : public BoundingBoxNode
{
public:

	FlareDeformerBoundingBoxNode(const Path& inputPath, const Path& parameters);
	virtual ~FlareDeformerBoundingBoxNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);
};


/*	
	Flare deformer verex node. Does the vertex processing.
	
	inputs:
		lowBound (float)
		highBound (float)
		startFlareX (float)
		startFlareZ (float)
		endFlareX (float)
		endFlareZ (float)
 		object2handle (float4x4)
		handle2object (float4x4)
		curve (float)

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
class FlareDeformerVertexNode : public VertexNode
{
public:

	FlareDeformerVertexNode(const Path& inputPath, const Path& parameters);
	virtual ~FlareDeformerVertexNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);
};

/// @}

} // namesapce digi

#endif
