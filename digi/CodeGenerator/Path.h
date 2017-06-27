#ifndef digi_CodeGenerator_Path_h
#define digi_CodeGenerator_Path_h

//#include "LeafNode.h"
#include "Attribute.h"


namespace digi {

/// @addtogroup CodeGenerator
/// @{

// replaces "." by "._" (e.g. "foo.bar" -> "foo._bar")
std::string makeTargetPath(const std::string& path);


struct Path
{
	// pointer to a node
	Pointer<LeafNode> node;
	
	// attribute path starting at node (e.g. "input.x")
	std::string path;


	Path() {}
	
	Path(Pointer<LeafNode> node)
		: node(node) {}
	
	Path(Pointer<LeafNode> node, const std::string& path)
		: node(node), path(path) {}

	Path(const InternalPath& path)
		: node(path.node), path(path.path) {}

	bool isNull() const
	{
		return this->node == null;
	}

	Path& operator =(const InternalPath& path)
	{
		this->node = path.node;
		this->path = path.path;
		return *this;
	}

// tree

	// get full path (e.g. "scope.node.attribute.x")
	std::string getPath(int startDepth = 0) const
	{
		return this->node->getPath(startDepth) + makePath(this->path);
	}

// attribute

	// get type of attribute
	std::string getType() const
	{
		return this->node->getType(this->path);
	}			

	int getFlags() const
	{
		return this->node->getFlags(this->path);
	}

	void setFlags(int flags)
	{
		this->node->setFlags(this->path, flags);
	}

	int getScope() const
	{
		return this->node->getScope(this->path);
	}

	void setScope(int scope)
	{
		this->node->setScope(this->path, scope);
	}

	bool hasAttribute() const
	{
		return this->node->findAttribute(this->path, LeafNode::FIND_UNTYPED).node != NULL;
	}

	bool hasAttribute(const std::string& suffix) const
	{
		return this->node->findAttribute(this->path + suffix, LeafNode::FIND_UNTYPED).node != NULL;
	}

	bool hasTypedAttribute() const
	{
		return this->node->findAttribute(this->path, LeafNode::FIND_TYPED).node != NULL;
	}

	void setInitializer(const std::string& value) const
	{
		this->node->setInitializer(this->path, value);
	}

	template <typename Type>
	void setInitializer(const Type& value) const
	{
		this->node->setInitializer(this->path, value);
	}

	std::string getInitializer() const
	{
		return this->node->getInitializer(this->path);
	}

// connectons
	
	void connect(const Path& srcPath) const
	{
		this->node->connect(this->path, srcPath);
	}

	bool isConnected(bool component = true) const
	{
		return this->node->isConnected(this->path, component);
	}

	bool isDataSource() const
	{
		return this->node->isDataSource(this->path);
	}

// code generation

	// get variable of attribute
	Connection resolveConnection() const
	{
		return this->node->resolveConnection(this->path);
	}

	// get target path of attribute
	std::string getTargetPath() const
	{
		return this->node->getTargetPath(this->path);
	}

	// write variable of attribute
	void writeVariable(NodeWriter& w) const
	{
		this->node->writeVariable(w, this->path);
	}

	void visit(NodeVisitor& v, int fromScope, int toScope)
	{
		this->node->visit(v, this->path, fromScope, toScope);
	}
	
	std::string toString()
	{
		return this->node->getPath() + makePath(this->path);
	}
};

inline Path operator +(const Path& attribute, StringRef path)
{
	return Path(attribute.node, attribute.path + path);
}

inline bool operator <(const Path& a, const Path& b)
{
	return a.node < b.node || (a.node == b.node && a.path < b.path);
}


/// @}

} // namespace digi

#endif 
