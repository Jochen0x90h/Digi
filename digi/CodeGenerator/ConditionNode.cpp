#include "ConditionNode.h"


namespace digi {


// ConditionNode

ConditionNode::ConditionNode(Operator op, const std::string& conditionType, const std::string& dataType)
	: op(op)
{
	this->addInput("a", conditionType);
	this->addInput("b", conditionType);
	
	this->addOutput("input1", dataType);
	this->addOutput("input2", dataType);
	this->addOutput("output", dataType);
}

ConditionNode::~ConditionNode()
{
}

std::string ConditionNode::getNodeType()
{
  return "ConditionNode";
}

void ConditionNode::writeUpdateCodeThis(NodeWriter& w)
{
	const char* ops[] = {"==", "!=", "<", "<=", ">", ">="};
	
	w << "$.output = $.a " << ops[this->op] << " $.b ? $.input1 : $.input2;\n";
}


} // namespace digi
