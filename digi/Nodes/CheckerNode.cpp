#include <digi/Utility/VectorUtility.h>

#include "CheckerNode.h"


namespace digi {



// CheckerNode

CheckerNode::CheckerNode(int inputDimension, int outputDimension, const Path& parameters)
	: TextureBaseNode(inputDimension, outputDimension)
{
	std::string outputType = VectorInfo(MatrixInfo::FLOAT, outputDimension).toString();
	this->addInput("value1", outputType, parameters + ".value1");
	this->addInput("value2", outputType, parameters + ".value2");
	this->addInput("contrast", "float", parameters + ".contrast");
}

CheckerNode::CheckerNode(int inputDimension, int outputDimension)
	: TextureBaseNode(inputDimension, outputDimension)
{
	std::string outputType = VectorInfo(MatrixInfo::FLOAT, outputDimension).toString();
	this->addInput("value1", outputType);
	this->addInput("value2", outputType);
	this->addInput("contrast", "float");
}

CheckerNode::~CheckerNode()
{
}

std::string CheckerNode::getNodeType()
{
	return "CheckerNode";
}

int CheckerNode::getAddressFlags()
{
	// checker pattern is automatically clamped
	return AUTO_CLAMP;
}

void CheckerNode::generateTexture(NodeWriter& w)
{
	w << "$@output $valueDiff = ($.value2 - $.value1) * ((1.0f - $.contrast) * 0.5f);\n";
	w << "$@output $value1 = $.value1 + $valueDiff;\n";
	w << "$@output $value2 = $.value2 - $valueDiff;\n";
	
	// calculate the checker pattern in the sign bit
	w << "float $checker = prod($coord - 0.5f);\n";

	// calc output
	w << "$.output = 0.0f < $checker ? $value1 : $value2;\n";
}

} // namespace digi
