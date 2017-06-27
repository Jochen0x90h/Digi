#ifndef digi_Nodes_ExtrudeNode_h
#define digi_Nodes_ExtrudeNode_h

#include <digi/CodeGenerator/Node.h>


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Extrude node. Contains the scene attributes of an extrude operator.
*/
class ExtrudeNode : public Node
{
	public:

		ExtrudeNode() {}
		ExtrudeNode(const std::string& name);
		virtual ~ExtrudeNode();

		virtual std::string getNodeType();
			
};

/*	
	Extrude vertex node. Does the vertex processing.
*/
class ExtrudeKernelNode : public Node
{
	public:

		ExtrudeKernelNode() {}
		ExtrudeKernelNode(const std::string& name, Pointer<ExtrudeNode> extrudeNode);
		virtual ~ExtrudeKernelNode();

		virtual std::string getNodeType();

		// copies blend weights into temp variables
		virtual void generateInitCodeThis(NodeWriter& w);
			
		// calculates the position inside the vertex loop
		virtual void writeUpdateCodeThis(NodeWriter& w);

	protected:

		// the corresponding blend shape node that contains the scene parameters
		Pointer<ExtrudeNode> extrudeNode;
};


/// @}

} // namesapce digi

#endif
