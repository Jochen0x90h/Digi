#include "SamplerInfoNode.h"


namespace digi {


// SamplerInfoNode

SamplerInfoNode::SamplerInfoNode()
	: Node(2)
{
	this->addOutput("facingRatio", "float");
	this->addOutput("backFacing", "float");
}

SamplerInfoNode::~SamplerInfoNode()
{
}

std::string SamplerInfoNode::getNodeType()
{
	return "SamplerInfoNode";
}

void SamplerInfoNode::writeUpdateCodeThis(NodeWriter& w)
{
	w << "$.facingRatio = max(dot(normal, viewVector), 0.0f);\n";
	//w << "$.backFacing = frontFacing ? 0.0f : 1.0f;\n";
	w << "$.backFacing = 0.5f - 0.5f * frontFacing;\n";
}

} // namespace digi
