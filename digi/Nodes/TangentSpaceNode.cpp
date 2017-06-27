#include <digi/Utility/VectorUtility.h>

#include "TangentSpaceNode.h"


namespace digi {


// TangentSpaceNode

TangentSpaceNode::TangentSpaceNode(int numTangentSpaces)
{
	this->addInput("tangent", "float3");
	this->addInput("bitangent", "float3");

	this->addOutput("normal", "float3");
	for (int i = 0; i < numTangentSpaces; ++i)
	{
		std::string is = toString(i);
		
		this->addInput("transform" + is, "float4");
		
		this->addOutput("output" + is + ".tangent", "float3");
		this->addOutput("output" + is + ".bitangent", "float3");
	}
}

TangentSpaceNode::~TangentSpaceNode()
{
}

std::string TangentSpaceNode::getNodeType()
{
	return "TangentSpaceNode";
}

void TangentSpaceNode::writeUpdateCodeThis(NodeWriter& w)
{	
	w << "$.normal = cross($.tangent, $.bitangent);\n";

	int i = 0;
	while (true)
	{
		// check for end of lights
		if (!this->hasAttribute("output" + toString(i) + ".tangent"))
			break;
		
		w << arg("$.output%0.tangent = $.tangent * $.transform%0.x + $.bitangent * $.transform%0.y;\n", i);
		w << arg("$.output%0.bitangent = $.tangent * $.transform%0.z + $.bitangent * $.transform%0.w;\n", i);
		
		++i;
	}
}


} // namespace digi
