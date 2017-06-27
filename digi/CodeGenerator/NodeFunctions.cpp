#include <digi/Utility/foreach.h>

#include "NodeFunctions.h"


namespace digi {
/*
void createTypes(TypeAllocator& a, Pointer<StructType> uniformType, Pointer<StructType> stateType,
	Pointer<StructType> localType, const std::vector<Pointer<Node> >& nodes)
{
	// types for all scene nodes
	foreach (const Pointer<Node>& node, nodes)
	{
		// allocate target type names 
		node->allocateTargetTypes(a);

		// add attributes to structures
		uniformType->addMember(node->getName(), node->createUniformStructure());
		stateType->addMember(node->getName(), node->createStateStructure());
		localType->addMember(node->getName(), node->createLocalStructure());
	}		
}
*/

} // namespace digi
