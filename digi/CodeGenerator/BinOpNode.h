#ifndef digi_CodeGenerator_BinOpNode_h
#define digi_CodeGenerator_BinOpNode_h

#include <string>

#include "Node.h"
#include "CodeWriterFunctions.h"


namespace digi {

/// @addtogroup CodeGenerator
/// @{

/**
	binary operation on two inputs.
	
	Inputs:
		input1
		input2
		
	Outputs:
		output
*/
class BinOpNode : public Node
{
public:

	enum Operator
	{
		OP_IN1,
		OP_IN2,
		OP_ADD,
		OP_SUB,
		OP_MUL,
		OP_DIV,
		OP_POW,
		OP_MIN,
		OP_MAX,
	};

	BinOpNode(Operator op, const std::string& type);

	template <typename Type>
	BinOpNode(Type value2, Operator op)
		: op(op)
	{
		this->addInput("input1", TypeName<Type>::get());
		this->addConstant("input2", value2);
		this->addOutput("output", TypeName<Type>::get());
	}

	virtual ~BinOpNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);

protected:
	
	Operator op;
};


/// @}

} // namespace digi

#endif
