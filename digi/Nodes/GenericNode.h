#ifndef digi_Nodes_GenericNode_h
#define digi_Nodes_GenericNode_h

#include <digi/Math/All.h>
#include <digi/CodeGenerator/Node.h>


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	generic node with settable type name and no functionality
*/
class GenericNode : public Node
{
	public:
		
		GenericNode() {}

		GenericNode(const std::string& name);
		GenericNode(const std::string& name, const std::string& type);
		
		virtual ~GenericNode();

		virtual std::string getNodeType();
			
		void setType(const std::string& type) {this->type = type;}
			
	protected:
		
		std::string type;
};

/*
class TargetTypeNode : public GenericNode
{
	public:
		
		TargetTypeNode() {}

		TargetTypeNode(const std::string& name, const std::string& type, int flags = 0);
		
		virtual ~TargetTypeNode();

		virtual void allocateTargetTypes(TypeAllocator& a);
		
};
*/

/// @}

} // namespace digi

#endif
