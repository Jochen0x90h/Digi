#include "BinOpNode.h"


namespace digi {


// BinOpNode

BinOpNode::BinOpNode(Operator op, const std::string& type)
	: op(op)
{
	this->addInput("input1", type);
	this->addInput("input2", type);
	this->addOutput("output", type);
}

BinOpNode::~BinOpNode()
{
}

std::string BinOpNode::getNodeType()
{
  return "BinOpNode";
}

void BinOpNode::writeUpdateCodeThis(NodeWriter& w)
{
	w << "$.output = ";
	switch (this->op)
	{
	case OP_IN1:
		w << "$.input1";
		break;
	case OP_IN2:
		w << "$.input2";
		break;
	case OP_ADD:
		w << "$.input1 + $.input2";
		break;
	case OP_SUB:
		w << "$.input1 - $.input2";
		break;
	case OP_MUL:
		w << "$.input1 * $.input2";
		break;
	case OP_DIV:
		w << "$.input1 / $.input2";
		break;
	case OP_POW:
		w << "pow($.input1, $.input2)";
		break;
	case OP_MIN:
		w << "min($.input1, $.input2)";
		break;
	case OP_MAX:
		w << "max($.input1, $.input2)";
		break;
	}
	w << ";\n";
}


} // namespace digi
