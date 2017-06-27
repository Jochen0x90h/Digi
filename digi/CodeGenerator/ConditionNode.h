#ifndef digi_CodeGenerator_ConditionNode_h
#define digi_CodeGenerator_ConditionNode_h

#include "Node.h"


namespace digi {

/// @addtogroup CodeGenerator
/// @{

using namespace digi;


/**
	calculates output = a op b ? input1 : input2
*/
class ConditionNode : public Node
{
public:

	enum Operator
	{
		OP_EQUAL,
		OP_NOT_EQUAL,
		OP_LESS,
		OP_LESS_EQUAL,
		OP_GREATER,
		OP_GREATER_EQUAL,
	};

	ConditionNode() {}

	// the condition type must be scalar, e.g. "int" or "float"
	ConditionNode(Operator op, const std::string& conditionType, const std::string& dataType);

	virtual ~ConditionNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);
	
protected:

	Operator op;
};


/// @}

} // namespace digi

#endif

