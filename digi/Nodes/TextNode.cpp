#include "TextNode.h"


namespace digi {


// TextNode

TextNode::TextNode()
{
	this->addInput("scale", "float2");
	this->addInput("space", "float3");
	this->addInput("column", "float4");
	this->addInput("align", "int2");
	this->addInput("text", "string");

	this->addState("symbols", "TextSymbols");

	this->addOutput("center", "float3");
	this->addOutput("size", "float3");
}

TextNode::~TextNode()
{
}

std::string TextNode::getNodeType()
{
	return "TextNode";
}

void TextNode::writeUpdateCodeThis(NodeWriter& w)
{
	// the macro has to be implemented by the backend
	w << "macro" << createVariableName(this->name) << "("
		"$.scale, $.space, $.column, $.align, $.text, "
		"$.symbols, "
		"$.center, $.size);\n";
}

} // namespace digi
