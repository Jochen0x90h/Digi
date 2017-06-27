#ifndef digi_Nodes_GeometryDependent_h
#define digi_Nodes_GeometryDependent_h

#include <digi/CodeGenerator/TreeNode.h>
#include <digi/CodeGenerator/NodeWriter.h>
#include <digi/CodeGenerator/NodeVisitor.h>


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Interface for geometry dependent nodes such as light and env mapping.

*/
class GeometryDependent
{
public:

	virtual ~GeometryDependent();

	// geometry contains surface position, view vector (V) and normal (N)
	virtual void writeUpdateCodeThis(NodeWriter& w, const Path& geometry) = 0;
};


class GeometryVisitor : public WriteUpdateVisitor
{
public:

	GeometryVisitor(NodeWriter& w, int maxScope, const Path& geometry)
		: WriteUpdateVisitor(w, maxScope), geometry(geometry) {}
	virtual ~GeometryVisitor();
	
	virtual void visitNode(Pointer<Node> node);

protected:

	Path geometry;
};

/// @}

} // namespace digi

#endif
