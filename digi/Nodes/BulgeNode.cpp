#include <digi/Utility/VectorUtility.h>

#include "BulgeNode.h"


namespace digi {



// BulgeNode

BulgeNode::BulgeNode(int inputDimension, int outputDimension, const Path& parameters)
	: TextureBaseNode(inputDimension, outputDimension)
{
	std::string inputType = VectorInfo(MatrixInfo::FLOAT, inputDimension).toString();
	this->addInput("width", inputType, parameters + ".width");
}

BulgeNode::BulgeNode(int inputDimension, int outputDimension)
	: TextureBaseNode(inputDimension, outputDimension)
{
	std::string inputType = VectorInfo(MatrixInfo::FLOAT, inputDimension).toString();
	this->addInput("width", inputType);
}

BulgeNode::~BulgeNode()
{
}

std::string BulgeNode::getNodeType()
{
	return "BulgeNode";
}

int BulgeNode::getAddressFlags()
{
	// bulge pattern is automatically clamped
	return AUTO_CLAMP;
}

void BulgeNode::generateTexture(NodeWriter& w)
{
	// per component we caluclate a quadratic function centered around zero
	// w = (1 - width) * 0.5
	// f(x) = (x - w)(x + w) * -1/w^2
	// f(0) = -w^2 * -1/w^2 = 1
	w << "$@input $w = (1.0f - $.width) * 0.5f;\n";
	w << "$@input $a = -1.0f / ($w * $w);\n";
	
	// x is in the range [-0.5, 0.5)
	w << "$@input $x = $coord - 0.5f;\n";

	// calc f(x), clamp at zero
	w << "$@input $f = max(($x - $w) * ($x + $w) * $a, 0.0f);\n";

	// multiply all dimensions (assigns to all output components if output is a vector)
	w << "$.output = prod($f);\n";
}

} // namespace digi
