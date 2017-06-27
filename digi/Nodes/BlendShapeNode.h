#ifndef digi_Nodes_BlendShapeNode_h
#define digi_Nodes_BlendShapeNode_h

#include "DeformerNode.h"


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Blend shape node. Contains the scene attributes of a blend shape deformer.
	Use addInput() to add the input attributes for the blend weights.

	Inputs:
		targetA (float)
		targetB (float)
		...
*/
class BlendShapeNode : public Node
{
public:

	struct Weight
	{
		std::string name;
		bool relative;
		
		Weight()
			: relative() {}
			
		Weight(const std::string& name, bool relative)
			: name(name), relative(relative) {}
	};

	BlendShapeNode();
	virtual ~BlendShapeNode();

	virtual std::string getNodeType();
	
	virtual void writeUpdateCodeThis(NodeWriter& w);
	
	void addWeight(const std::string& name, bool relative);
	
	
	// list of weights
	std::vector<Weight> weights;
};


/*	
	Blend shape bounding box node. Calculates a conservative bounding box.

	Inputs:
		input
			center
			size
		target
			targetA
				weight
				center
				size
			targetB
				...

	Outputs:
		output
			center
			size
*/
class BlendShapeBoundingBoxNode : public BoundingBoxNode
{
public:

	BlendShapeBoundingBoxNode(const Path& inputPath, Pointer<BlendShapeNode> sceneNode);
	virtual ~BlendShapeBoundingBoxNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);
};


/*	
	Blend shape verex node. Does the vertex processing.

	Inputs:
		input
			position
			tangent
			bitangent
		target
			targetA
				weight
				position
				tangent
				bitangent
			targetB
				...

	Outputs:
		output
			position
			tangent
			bitangent
*/
class BlendShapeVertexNode : public VertexNode
{
public:

	BlendShapeVertexNode(const Path& inputPath, Pointer<BlendShapeNode> sceneNode);
	virtual ~BlendShapeVertexNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);
};

/// @}

} // namesapce digi

#endif
