#include "GenericNode.h"


namespace digi {


// GenericNode

GenericNode::GenericNode(const std::string& name)
	: Node(name)
{
}

GenericNode::GenericNode(const std::string& name, const std::string& type)
	: Node(name), type(type)
{
}

GenericNode::~GenericNode()
{
}

std::string GenericNode::getNodeType()
{
	return this->type;
}

/*
// TargetTypeNode

TargetTypeNode::TargetTypeNode(const std::string& name, const std::string& type, int flags)
	: GenericNode(name, type, flags)
{
}

TargetTypeNode::~TargetTypeNode()
{
}

void TargetTypeNode::allocateTargetTypes(TypeAllocator& a)
{
	this->targetType = this->type;
}
*/

} // namespace digi
