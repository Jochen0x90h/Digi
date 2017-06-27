#ifndef digi_CodeGenerator_NodeVisitor_h
#define digi_CodeGenerator_NodeVisitor_h

#include "NodeWriter.h"
#include "Attribute.h"


namespace digi {

/// @addtogroup CodeGenerator
/// @{

class Node;

class NodeVisitor
{
public:
	NodeVisitor(bool stop) : stop(stop) {}
	virtual ~NodeVisitor();
	
	// visit a connection if the scope of the destination attribute matches
	virtual void visitConnection(const Connection& src, const Path& dstPath);
	
	// visit an attribute if the scope of the attribute matches
	virtual void visitAttribute(Pointer<Attribute> attribute);
	
	// visit a node if the scope of the node matches
	virtual void visitNode(Pointer<Node> node);

	// visit an attribute of a node if the scope of the node matches
	virtual void visitNodeAttribute(Pointer<Attribute> attribute);

	
	// stop at attributes with STOP flag
	bool stop;
	
	// map of visited connections (with state 0 = not visited, 1 = ongoing, 2 = finished)
	std::map<Connection*, int> connections;

	// set of visited attributes
	std::set<Attribute*> attributes;

	// set of visited nodes
	std::set<Node*> nodes;
};


// visitor for generating target structure
class TargetTypeVisitor : public NodeVisitor
{
public:

	TargetTypeVisitor()
		: NodeVisitor(true)
	{
		type = new StructType();
	}

	virtual ~TargetTypeVisitor();
	
	virtual void visitAttribute(Pointer<Attribute> attribute);

	
	Pointer<StructType> type;
};


// visitor for writing init code of nodes
class WriteInitVisitor : public NodeVisitor
{
public:

	WriteInitVisitor(NodeWriter& w)
		: NodeVisitor(true), w(w) {}
	virtual ~WriteInitVisitor();
	
	virtual void visitAttribute(Pointer<Attribute> attribute);

protected:

	NodeWriter& w;
};

// visitor for writing update code of nodes
class WriteUpdateVisitor : public NodeVisitor
{
public:

	WriteUpdateVisitor(NodeWriter& w, int maxScope)
		: NodeVisitor(true), w(w), maxScope(maxScope) {}
	virtual ~WriteUpdateVisitor();
	
	virtual void visitNode(Pointer<Node> node);
	virtual void visitConnection(const Connection& src, const Path& dstPath);

protected:

	NodeWriter& w;
	int maxScope;
};

/// @}

} // namespace digi

#endif
