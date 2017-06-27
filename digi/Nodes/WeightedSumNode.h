#ifndef digi_Nodes_WeightedSumNode_h
#define digi_Nodes_WeightedSumNode_h

#include <digi/CodeGenerator/Node.h>


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	calculates weighed sum
	output = input.0.value * input.0.weight + input.1.value * input.1.weight + ...
	
	inputs:
		input
			0
				value
				weight (1.0 if missing)
			1
				value
				weight
			...
	
	outputs:
		type output
*/
class WeightedSumNode : public Node
{
public:

	WeightedSumNode(const std::string& type);

	virtual ~WeightedSumNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);


	std::string addInputValue(int inputIndex);
	std::string addInputWeight(int inputIndex);
};

/// @}

} // namespace digi

#endif
