#include "ExtrudeNode.h"


namespace digi {


// ExtrudeNode

ExtrudeNode::ExtrudeNode(const std::string& name)
	: Node(name)
{
}

ExtrudeNode::~ExtrudeNode()
{
}

std::string ExtrudeNode::getNodeType()
{
	return "ExtrudeNode";
}


// ExtrudeKernelNode

ExtrudeKernelNode::ExtrudeKernelNode(const std::string& name, Pointer<ExtrudeNode> extrudeNode)
	: Node(name), extrudeNode(extrudeNode)
{
}

ExtrudeKernelNode::~ExtrudeKernelNode()
{
}

std::string ExtrudeKernelNode::getNodeType()
{
	return "ExtrudeKernelNode";
}

void ExtrudeKernelNode::generateInitCodeThis(NodeWriter& w)
{
}

void ExtrudeKernelNode::writeUpdateCodeThis(NodeWriter& w)
{
}


} // namespace digi
