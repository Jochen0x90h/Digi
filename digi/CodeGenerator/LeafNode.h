#ifndef digi_CodeGenerator_LeafNode_h
#define digi_CodeGenerator_LeafNode_h

#include <string>

#include <digi/Utility/Object.h>
#include <digi/Utility/Pointer.h>
#include <digi/Image/Buffer.h>

#include "Type.h"
#include "CodeWriterFunctions.h"


namespace digi {

/// @addtogroup CodeGenerator
/// @{

class NodeVisitor;
class NodeWriter;
struct InternalPath;
struct Path;
struct Connection;
class TreeNode;


class LeafNode : public Object
{
	friend class TreeNode;

public:
		
	LeafNode()
		: parent(NULL) {}
	
	LeafNode(StringRef name)
		: parent(NULL), name(name) {}
			
	virtual ~LeafNode();
			
	// get name of node
	const std::string& getName() {return this->name;}
	
	virtual std::string getNodeType();

// tree
	
	// get parent node
	Pointer<TreeNode> getParent() {return this->parent;}
			
	// get root of tree node. null if there is no parent
	virtual Pointer<TreeNode> getRoot();
	
	// get depth of node in tree starting at root which has depth of zero
	int getDepth();
	
	// get full name starting at given depth
	std::string getPath(int startDepth = 0);

	// add a node as child. returns false if failed because the name exists or no childs can be added
	virtual bool addNode(Pointer<LeafNode> node);

// attribute

	enum FindMode
	{
		// find typed attribute, i.e. node of type Attribute or a component (e.g. "foo.x" if "foo" is float3)
		FIND_TYPED,
		
		// find untyped attribute, i.e. any LeafNode
		FIND_UNTYPED,
		
		// find attributes with partial path, e.g. if "node.foo.bar" is an attribute of some node then "node.foo" is also found
		FIND_PARTIAL
	};

	virtual InternalPath findAttribute(const std::string& path, FindMode findMode = FIND_TYPED) = 0;
	
	// check if an attribute exists. may be anonymous and therefore untyped
	bool hasAttribute(const std::string& path);
	
	// check if an attribute has a type and therefore can be connected
	bool hasTypedAttribute(const std::string& path);

	// get type of attribute
	virtual std::string getType(const std::string& path) = 0;

	// set type of attribute
	virtual void setType(const std::string& path, const std::string& type) = 0;

	// get flags of attribute
	virtual int getFlags(const std::string& path) = 0;

	// set flags of attribute
	virtual void setFlags(const std::string& path, int flags) = 0;

	// get scope of attribute
	virtual int getScope(const std::string& path) = 0;

	// set scope of attribute
	virtual void setScope(const std::string& path, int scope) = 0;

	// set initializer for attribute
	virtual void setInitializer(const std::string& path, const std::string& value) = 0;

	// set initializer for attribute
	template <typename Type>
	void setInitializer(const std::string& path, const Type& value)
	{
		this->setInitializer(path, std::string(Code() << value));
	}

	// get initializer for attribute
	virtual std::string getInitializer(const std::string& path) = 0;

	// get all attributs with given flags
	virtual void getAttributes(const std::string& prefix, int flags, std::vector<std::string>& attributes) = 0;
	
// connectons

	// connect an attribute with given source attribute
	virtual void connect(const std::string& path, const Path& srcPath) = 0;

	// check if attribute or component of attribute is connected
	virtual bool isConnected(const std::string& path, bool component = true) = 0;

	// check if attribute or component of attribute is a data source for other attributes
	virtual bool isDataSource(const std::string& path) = 0;

// optimize

	enum OptimizePhase
	{
		// vectorize initializers and connections of attributes
		VECTORIZE,
		
		// mark attributes as references or constants if possible, remove unused outputs
		OPTIMIZE_ATTRIBUTES
	};

	virtual void optimize(OptimizePhase phase) = 0;
			
// code generation

	// write target type for this node
	virtual Type* getTargetType(int scope) = 0;

	// get target type of connected nodes
	Pointer<StructType> getTargetTypeOfConnected(int scope);

	// write target type for connection to given scope
	virtual Type* getConnectionTargetType(Pointer<LeafNode> scope, bool targetNameMangling = true) = 0;

	// resolve connection, i.e. follow references
	virtual Connection resolveConnection(const std::string& path) = 0;

	// generate data buffers
	virtual void generateBuffers(std::vector<NamedBuffer>& buffers);
	
	
	// propagate scopes to output connections, from input attributes to nodes and from nodes to output attributes
	virtual void propagateScopes() = 0;

	// propagate scope to output attributes of a node
	virtual void propagateScopesAttributes(int scope) = 0;

	// get scope of this node
	virtual int getScope() = 0;

	// recursively adds to the scope of this node and all its children (used to "lift" a subgraph onto a new scope range)
	virtual void addScopeRecursive(int scopeAdd) = 0;


	// visit nodes and connections in execution order
	virtual void visit(NodeVisitor& v, const std::string& path, int minScope, int maxScope) = 0;
	
	// visit attributes of a node
	virtual void visitAttributes(NodeVisitor& v, int minScope, int maxScope) = 0;


	// get target path of a variable for access of an attibute in written code
	std::string getTargetPath(const std::string& path);
	
	// write variable for access of an attibute in written code
	void writeVariable(NodeWriter& w, const std::string& path);
	
	// write code that copies from this scope into given scope
	virtual void writeOutputCode(NodeWriter& w, Pointer<LeafNode> scope, const std::string& prefix, bool targetNameMangling = true) = 0;

	// get assignments that copy from this scope into given scope
	virtual void getOutputAssignments(const std::map<int, std::string>& scopes, std::vector<std::string>& assignments,
		Pointer<LeafNode> sourceRoot, int scope) = 0;

	// write code that copies from given scope into this scope
	virtual void writeInputCode(NodeWriter& w, Pointer<LeafNode> scope, const std::string& prefix) = 0;

	// make all attributes that connect (only) to given scope access given prefix instead of local
	//virtual void writeInputReference(Pointer<LeafNode> scope, const std::string& prefix) = 0;

	// write code that loads from state into local
	//virtual void writeLoadStateCode(NodeWriter& w) = 0;
	
	// write code that stores from local into state
	//virtual void writeStoreStateCode(NodeWriter& w) = 0;

protected:

	// clear connections. gets called when parent gets deleted to prevent circular references
	virtual void clearConnectionsThis();

private:
	
	TreeNode* parent;
	std::string name;
};


struct InternalPath
{
	// pointer to a node
	LeafNode* node;
	
	// attribute path starting at node (e.g. "input.x")
	std::string path;


	InternalPath()
		: node(NULL) {}
	
	explicit InternalPath(LeafNode* node)
		: node(node) {}
	
	InternalPath(LeafNode* node, const std::string& path)
		: node(node), path(path) {}

	bool isNull() const
	{
		return this->node == NULL;
	}
	
// tree

	// get full path (e.g. "scope.node.attribute.x")
	std::string getPath(int startDepth = 0) const
	{
		return this->node->getPath(startDepth) + this->path;
	}
};


inline bool LeafNode::hasAttribute(const std::string& path)
{
	return this->findAttribute(path, FIND_UNTYPED).node != NULL;
}

inline bool LeafNode::hasTypedAttribute(const std::string& path)
{
	return this->findAttribute(path, FIND_TYPED).node != NULL;
}

/// @}

} // namespace digi

#endif 
