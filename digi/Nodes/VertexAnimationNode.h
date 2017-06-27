#ifndef digi_Nodes_VertexAnimationNode_h
#define digi_Nodes_VertexAnimationNode_h

#include <digi/Math/All.h>
#include <digi/CodeGenerator/Node.h>


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	animation curve for a scalar value

	inputs:
		time
	
	states:
		currentFrame
		buffer
		
	outputs:
		alpha: interpolation factor between two consecutive frames
*/
class VertexAnimationNode : public Node
{
	public:
		
		VertexAnimationNode();
		
		virtual ~VertexAnimationNode();

		virtual std::string getNodeType();

		virtual void generateStaticDataThis(NodeWriter& w);
		virtual void generateInitCodeThis(NodeWriter& w);
		virtual void writeUpdateCodeThis(NodeWriter& w);

		std::vector<ubyte1> positionData;
		std::vector<ubyte1> tangentData;

		int numPositions;
		int numTangents;

		int numPositionFrames;
		int numTangentFrames;
		
		float positionFPS;
		float tangentFPS;
		
	protected:
};


/*	
	vertex processing node for compressed vertex animation

	uniform:
		positionAlpha: interpolation factor
		tangentAlpha: interpolation factor
		
	inputs:
		positionIndex
		tangentBinormalIndex
		indexedPositions[]
		indexedBinormalsTangents[]

	outputs:
		position, tangent, binormal: output shape
*/
class VertexAnimationKernelNode : public Node
{
	public:

		VertexAnimationKernelNode() {}
		VertexAnimationKernelNode(const std::string& name, Pointer<Node> sceneNode);
		virtual ~VertexAnimationKernelNode();

		virtual std::string getNodeType();

		virtual Pointer<Node> getOuterScope();

		// copies positionAlpha and tangentAlpha from scene into uniform data for vertex processing
		virtual void generateInitCodeThis(NodeWriter& w, int flags);
			
		// calculates position, tangent and binormal inside the vertex loop
		virtual void writeUpdateCodeThis(NodeWriter& w);

	protected:

		// the corresponding scene node that contains the control parameters (alpha)
		Pointer<Node> sceneNode;
};


/// @}

} // namespace digi

#endif
