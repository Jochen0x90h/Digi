#include "WeightedSumNode.h"


namespace digi {


// WeightedSumNode

WeightedSumNode::WeightedSumNode(const std::string& type)
{
	this->addOutput("output", type);
}

WeightedSumNode::~WeightedSumNode()
{
}

std::string WeightedSumNode::getNodeType()
{
  return "WeightedSumNode";
}

void WeightedSumNode::writeUpdateCodeThis(NodeWriter& w)
{
	w << "$.output =\n\t";
	
	bool first = true;
	foreach (Path path, this->getPathElementList("input"))
	{
		if (!first)
		{
			w << "\n\t+ ";
			w.pop();
		}
		first = false;

		w.push(path);

		w << "$.value";
		if (path.hasAttribute(".weight"))
			w << " * $.weight";
	}
	
	if (first)
	{
		// no inputs
		w << "0.0f;\n";	
	}
	else
	{
		w << ";\n";
		w.pop();
	}
}


std::string WeightedSumNode::addInputValue(int inputIndex)
{
	std::string inputValue = arg("input.%0.value", inputIndex);
	this->addInput(inputValue, this->getType("output"));
	return inputValue;
}

std::string WeightedSumNode::addInputWeight(int inputIndex)
{
	std::string inputWeight = arg("input.%0.weight", inputIndex);
	this->addInput(inputWeight, "float");
	return inputWeight;
}


} // namespace digi
