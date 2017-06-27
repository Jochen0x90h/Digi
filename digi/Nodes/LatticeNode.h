#ifndef digi_Nodes_LatticeNode_h
#define digi_Nodes_LatticeNode_h

#include "DeformerNode.h"


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	free form deformation node.

	Inputs:
		baseMatrix (float4x4)
		deformedMatrix (float4x4)
	Outputs:
		inverseBaseMatrix (float4x4)
*/
class FFDNode : public Node
{
public:

	FFDNode(const Path& worldMatrix);
	virtual ~FFDNode();

	virtual std::string getNodeType();
	
	virtual void writeUpdateCodeThis(NodeWriter& w);
};


/*	
	Lattice deformer bounding box node. Calculates a conservative bounding box.

	Inputs:
		inverseBaseMatrix (float4x4)
		deformedMatrix (float4x4)

	Outputs:
		output
			center
			size
*/
class LatticeBoundingBoxNode : public BoundingBoxNode
{
public:

	LatticeBoundingBoxNode(const Path& inputPath, Pointer<Node> ffdNode, Pointer<Node> latticeNode,
		int3 resolution);
	virtual ~LatticeBoundingBoxNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);


	int3 resolution;
};


/*	
	Blend shape verex node. Does the vertex processing.

	Inputs:
		inverseBaseMatrix (float4x4)
		deformedMatrix (float4x4)

	Outputs:
		output
			position
			tangent
			bitangent
*/
class LatticeVertexNode : public VertexNode
{
public:

	LatticeVertexNode(const Path& inputPath, Pointer<Node> ffdNode, Pointer<Node> latticeNode,
		int3 resolution);
	virtual ~LatticeVertexNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);


	int3 resolution;
};

/// @}

} // namesapce digi

#endif
