#ifndef digi_Nodes_CurveNode_h
#define digi_Nodes_CurveNode_h

#include <digi/CodeGenerator/Node.h>


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Curve node. Contains a vertex node that evenly spaced points on the
	curve and a linear transform to map the range [0..1] onto the vertex index
	range.
*/
class CurveNode : public Node
{
	public:

		CurveNode() : numVertices(0) {}
		CurveNode(const std::string& name);
		virtual ~CurveNode();

		virtual std::string getNodeType();

		//virtual void generateStruct(NodeWriter& w);
		//virtual void generateStaticDataThis(NodeWriter& w);
		virtual void generateInitCodeThis(NodeWriter& w);
		//virtual void generateUpdateCodeThis(NodeWriter& w);
			
		// vertices of curve
		//Pointer<KernelNode> vertexNode;
		
		// number of vertices of curve
		int numVertices;
};



/*
	Sub curve node. Contains a vertex node that evenly spaced points on the
	curve and a linear transform to map the range [0..1] on the vertex index
	range.
*/
class SubCurveNode : public Node
{
	public:

		SubCurveNode(const Path& inCurve);
		virtual ~SubCurveNode();

		virtual std::string getNodeType();

		virtual void generateUpdateCodeThis(NodeWriter& w);
};


/// @}

} // namesapce digi

#endif
