#include "MultiplyAddNode.h"


namespace digi {


// MultiplyAddNode

MultiplyAddNode::MultiplyAddNode(const std::string& type)
{
	this->addInput("input1", type);
	this->addInput("input2", type);
	this->addInput("input3", type);
	this->addOutput("output", type);
}

MultiplyAddNode::~MultiplyAddNode()
{
}

std::string MultiplyAddNode::getNodeType()
{
  return "MultiplyAddNode";
}

void MultiplyAddNode::writeUpdateCodeThis(NodeWriter& w)
{
	w << "$.output = $.input1 * $.input2 + $.input3;\n";
}


} // namespace digi
