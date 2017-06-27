#include "ContrastNode.h"


namespace digi {


// ContrastNode

ContrastNode::ContrastNode(int numDimensions)
	: numDimensions(numDimensions)
{
	std::string type = VectorInfo(VectorInfo::FLOAT, numDimensions).toString();
	this->addInput("input", type);
	this->addInput("contrast", type);
	this->addInput("bias", type);

	this->addOutput("output", type);
}

ContrastNode::~ContrastNode()
{
}

std::string ContrastNode::getNodeType()
{
  return "ContrastNode";
}

void ContrastNode::writeUpdateCodeThis(NodeWriter& w)
{
	// bias: remap the [0,1] interval on itself with the constraint
	// f(0) = 0
	// f(bias) = 0.5
	// f(1) = 1
	// maya: f(x) = x^alpha, with alpha = log(0.5)/log(bias)
	// this function is smooth and is the identity if bias==0.5
	 
	w << "$@input $I = pow($.input, -0.69315f / log($.bias));\n";


	// The contrast operation consists in redistributing the energy around the middle point. In other words: every 
	// intensity below 0.5 is pushed towards black, and every intensity above is pushed towards white. The trick is 
	// to do that in a "smooth" way.
	//
	// on interval [0,0.5] apply a gamma to intensity
	// on interval [0.5,1] apply an "inverted" gamma to intensity
	// when I < 0.5, newI = 0.5*(2*I)^contrast
	// when I >= 0.5, newI = 1-0.5*(2*(1-I))^contrast
	//
	// this function is C1 on [0,1], and if contrast==1, it is the identity.

	w << "$.output = select(0.5f * pow(2.0f * $I, $.contrast), 1.0f - 0.5f * pow(2.0f * (1.0f - $I), $.contrast), $I > 0.5f);\n";
}

} // namespace digi
