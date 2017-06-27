#ifndef digi_CodeGenerator_MultiplyAddNode_h
#define digi_CodeGenerator_MultiplyAddNode_h

#include <string>

#include "Node.h"


namespace digi {

/// @addtogroup CodeGenerator
/// @{

/**
	binary operation on two inputs.
*/
class MultiplyAddNode : public Node
{
	public:

		MultiplyAddNode(const std::string& type);

		template <typename Type>
		MultiplyAddNode(Type value2, Type value3)
		{
			this->addInput("input1", TypeName<Type>::get());
			this->addConstant("input2", value2);
			this->addConstant("input3", value3);
			this->addOutput("output", TypeName<Type>::get());
		}

		virtual ~MultiplyAddNode();

		virtual std::string getNodeType();

		virtual void writeUpdateCodeThis(NodeWriter& w);
};


/// @}

} // namespace digi

#endif
