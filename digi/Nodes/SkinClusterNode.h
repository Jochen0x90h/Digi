#ifndef digi_Nodes_SkinClusterNode_h
#define digi_Nodes_SkinClusterNode_h

#include <digi/Math/All.h>

#include "DeformerNode.h"


namespace digi {

/// @addtogroup Nodes
/// @{

/*
	Skin cluster node.
	
	Inputs:
		input0 (reference to float4x4): world transforms of bones
		input1
		...
		
	Output
		output (float3x4[]): output bone matrices for vertex processing
*/
class SkinClusterNode : public Node
{
public:

	SkinClusterNode(int numBones);
	virtual ~SkinClusterNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);

	// connect a bone to the bone world matrix
	void connectBone(int index, const float4x4& invBoneBindMatrix, const Path& boneWorldMatrix);

	// bind matrix of geometry (world matrix at bind time)
	float4x4 geometryBindMatrix;
	
	// bind matrix of bones (world matrix at bind time)
	std::vector<float4x4> invBoneBindMatrices;
};


/*
	Skin cluster bounding box node. Calculates conservative bounding box.
	
	Inputs:
		matrix (float3x4[], bone matrices)
		
		input
			center
			size
	
	Outputs:
		output
			center
			size
*/
class SkinClusterBoundingBoxNode : public BoundingBoxNode
{
public:

	// constructor. inputPath is data source for input.position, input.tangent and input.bitangent
	SkinClusterBoundingBoxNode(const Path& inputPath, Pointer<SkinClusterNode> skinClusterNode,
		const std::vector<int>& boneSet);

	virtual ~SkinClusterBoundingBoxNode();

	virtual std::string getNodeType();

	// calculates the vertex properties inside the vertex loop
	virtual void writeUpdateCodeThis(NodeWriter& w);

protected:

	// number of bone matrices in matrix[] array attribute
	int numBones;
};


/*
	Skin cluster vertex node. Does the vertex processing
	
	Inputs:
		matrix (float3x4[], bone matrices)
		
		input
			position
			tangent
			bitangent
		indices0, indices1...
		weights0, weights1...
	
	Outputs:
		output
			position
			tangent
			bitangent
*/
class SkinClusterVertexNode : public VertexNode
{
public:

	// constructor. inputPath is data source for input.position, input.tangent and input.bitangent
	SkinClusterVertexNode(const Path& inputPath, Pointer<SkinClusterNode> skinClusterNode,
		const std::vector<int>& boneSet);

	virtual ~SkinClusterVertexNode();

	virtual std::string getNodeType();

	// calculates the vertex properties inside the vertex loop
	virtual void writeUpdateCodeThis(NodeWriter& w);


	// number of weights per vertex
	int numWeights;
};

/// @}

} // namesapce digi

#endif
