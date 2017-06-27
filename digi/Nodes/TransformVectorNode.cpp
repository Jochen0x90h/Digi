#include <digi/Utility/StringUtility.h>

#include "TransformVectorNode.h"


namespace digi {



// TransformVectorNode

TransformVectorNode::TransformVectorNode(float4 localVector)
	: localVector(localVector)
{
	this->addInput("matrix", "float4x4");
	this->addOutput("vector", "float4");
}

TransformVectorNode::~TransformVectorNode()
{
}

std::string TransformVectorNode::getNodeType()
{
	return "TransformVectorNode";
}

void TransformVectorNode::writeUpdateCodeThis(NodeWriter& w)
{
	float4 absVector = abs(this->localVector);
	
	// calc matrix * localVector, check for some special cases
	if (all(absVector == vector4(1.0f, 0.0f, 0.0f, 0.0f)))
	{
		if (this->localVector.x > 0)
			w.writeLine("$.vector = $.matrix.x;");
		else
			w.writeLine("$.vector = -$.matrix.x;");
	}
	else if (all(absVector == vector4(0.0f, 1.0f, 0.0f, 0.0f)))
	{
		if (this->localVector.y > 0)
			w.writeLine("$.vector = $.matrix.y;");
		else
			w.writeLine("$.vector = -$.matrix.y;");
	}
	else if (all(absVector == vector4(0.0f, 0.0f, 1.0f, 0.0f)))
	{
		if (this->localVector.z > 0)
			w.writeLine("$.vector = $.matrix.z;");
		else
			w.writeLine("$.vector = -$.matrix.z;");
	}
	else if (all(absVector == vector4(0.0f, 0.0f, 0.0f, 1.0f)))
	{
		if (this->localVector.w > 0)
			w.writeLine("$.vector = $.matrix.w;");
		else
			w.writeLine("$.vector = -$.matrix.w;");
	}
	else
	{
		// general case
		w << "$.vector = $.matrix * " << this->localVector << ";\n";
	}
}



} // namespace digi
