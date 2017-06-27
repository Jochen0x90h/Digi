#include <digi/Utility/Ascii.h>
#include <digi/Utility/StringUtility.h>
#include <digi/Utility/VectorUtility.h>
#include <digi/Utility/MapUtility.h>
#include <digi/Utility/foreach.h>
#include <digi/System/Log.h>

#include "Node.h"


namespace digi {

// TreeNode

TreeNode::~TreeNode()
{
	foreach (ChildPair& child, this->children)
	{
		child.second->clearConnectionsThis();
	}
}

// tree

Pointer<TreeNode> TreeNode::getRoot()
{
	TreeNode* node = this;
	while (node->parent != NULL)
	{
		node = node->parent;
	}
	return node;
}

bool TreeNode::addNode(Pointer<LeafNode> node)
{
	if (node->parent != NULL)
	{
		dError("TreeNode::addNode - node is already part of another tree");
		return false;
	}
	if (node == this)
	{
		dError("TreeNode::addNode - can't add node to itself");
		return false;
	}

	// check if name is empty or conflicts with an existing name
	if (node->name.empty() || this->getMatchIndex(node->name) != -1)
		return false;

	node->parent = this;

	this->children[makePath(node->name)] = node;
	return true;
}

void TreeNode::removeNode(Pointer<LeafNode> node)
{
	for (Children::iterator it = this->children.begin(); it != this->children.end(); ++it)
	{
		if (it->second == node)
		{
			this->children.erase(it);
			node->parent = NULL;
			return;
		}	
	}
}

std::string TreeNode::getUniqueName(const std::string& name)
{
	/*
		assume a child "foo.bar" exists
		
		rename "foo" if name is "foo" 
		rename "bar" if name is "foo.bar"
		rename "bar" if name is "foo.bar.x"
	*/

	int index = this->getMatchIndex(name);
	if (index == -1)
		return name;
		
	// a child with conficting name exists
	
	int i = 0;
	do
	{
		std::string newName = name.substr(0, index) + toString(i) + name.substr(index);
		
		if (this->getMatchIndex(newName) == -1)
			return newName;
		++i;
	} while (true);
}

void TreeNode::addNode(Pointer<LeafNode> node, const std::string proposedName)
{
	if (node->parent != NULL)
	{
		dError("TreeNode::addNode - node is already part of another tree");
		return;
	}
	if (node == this)
	{
		dError("TreeNode::addNode - can't add node to itself");
		return;
	}

	// get unique name
	std::string name = this->getUniqueName(proposedName);
	
	// go to lowest node in tree
	InternalPath path = this->findAttribute(name, FIND_PARTIAL);
	
	// cast to TreeNode. do not use Pointer since addAttribute may be called in constructor
	TreeNode* pathNode = dynamic_cast<TreeNode*>(path.node);
	
	// check if tree node
	if (pathNode == NULL)
	{
		dError("TreeNode::addNode - path '" << path.getPath() << "' is part of a leaf node");
		return;	
	}
	
	// set node name	
	node->name = name;
	
	// link to node
	node->parent = pathNode;
	pathNode->children[path.path] = node;
}

// attribute

Pointer<Attribute> TreeNode::addAttribute(const std::string& attributeName, const std::string& type,
	int flags, int scope)
{
	// go to lowest node in tree
	InternalPath path = this->findAttribute(attributeName, FIND_PARTIAL);
	
	// cast to TreeNode. do not use Pointer since addAttribute may be called in constructor
	TreeNode* pathNode = dynamic_cast<TreeNode*>(path.node);
	
	// check if tree node
	if (pathNode == NULL)
	{
		dError("TreeNode::addAttribute - attribute '" << path.getPath() << "' already exists");
		return null;	
	}
	
	// check if attribute exists
	if (path.path.empty() || pathNode->getMatchIndex(path.path) != -1)
	{
		dError("TreeNode::addAttribute - attribute '" << path.getPath() << "' already exists");
		return null;	
	}

	// create attribute
	Pointer<Attribute> attribute = new Attribute(path.path.substr(1), type, flags, scope);
	attribute->parent = pathNode;
	pathNode->children[path.path] = attribute;
	return attribute;
}

Pointer<Attribute> TreeNode::addAttribute(const std::string& attributeName, const std::string& type, const Path& srcPath,
	int flags, int scope)
{
	Pointer<Attribute> attribute = this->addAttribute(attributeName, type, flags, scope);
	if (attribute == null)
		return null;
		
	if (srcPath.isNull())
	{
		dError("TreeNode::addAttribute - connection to '" << attribute->getPath() << "' is null");
		return attribute;
	}

	InternalPath srcAttributePath = srcPath.node->findAttribute(srcPath.path, FIND_TYPED);
	Attribute* srcAttribute = dynamic_cast<Attribute*>(srcAttributePath.node);
	if (srcAttribute == NULL)
	{
		dError("TreeNode::addAttribute - connection to '" << attribute->getPath() << "' is invalid");
		return attribute;
	}

	// add connection (note: since the attribute is newly created we don't need to check for old connection as in Attribute::connect)
	Connection& connection = attribute->connections[std::string()];
	connection.attribute = srcAttribute;
	connection.path = srcAttributePath.path;
	srcAttribute->outConnections[&connection.path] = attribute.getPointer();

	return attribute;
}

void TreeNode::addAttributeWithInitializer(const std::string& attributeName, const std::string& type,
	const std::string& initializer, int flags, int scope)
{
	Pointer<Attribute> attribute = this->addAttribute(attributeName, type, flags, scope);
	if (attribute == null)
		return;

	// add initializer
	attribute->initializers[std::string()] = initializer;
}

InternalPath TreeNode::findAttribute(const std::string& path, FindMode findMode)
{
	// empty path is the node itself
	if (path.empty())
	{
		// return path to this node only if untyped attribute is requested
		if (findMode != FIND_TYPED)
			return InternalPath(this);
	}
	else
	{
		// if path does not start with a path element member access is default
		// e.g. "foo" -> ".foo"
		std::string p = makePath(path);
		
		// assume a child ".foo.bar" exists
		Children::iterator it = this->children.lower_bound(p);
		
		if (it != this->children.end())
		{
			// path matches child name (path = ".foo.bar")
			if (it->first == p)
				return it->second->findAttribute(std::string(), findMode);
				
			// path is part of child name (path = ".foo")
			if (findMode != FIND_TYPED && startsWithPath(it->first, p))
				return InternalPath(this, p);
		}
				
		if (it != this->children.begin())
		{
			--it;
			
			// path starts with child name (path = ".foo.bar.x")
			if (startsWithPath(p, it->first))
				return it->second->findAttribute(p.substr(it->first.length()), findMode);
		}
		
		// if partial find mode then return path even if no child exists
		if (findMode == FIND_PARTIAL)
			return InternalPath(this, p);
	}
	return InternalPath();
}

std::string TreeNode::getType(const std::string& path)
{
	InternalPath attribute = this->findAttribute(path, FIND_TYPED);
	if (attribute.isNull())
	{
		dError("TreeNode::getType - attribute '" << path << "' not found in '" << this->getPath() << "'");
		return std::string();
	}
	return attribute.node->getType(attribute.path);
}

void TreeNode::setType(const std::string& path, const std::string& type)
{
	InternalPath attribute = this->findAttribute(path, FIND_TYPED);
	if (attribute.isNull())
	{
		dError("TreeNode::setType - attribute '" << path << "' not found in '" << this->getPath() << "'");
		return;
	}
	attribute.node->setType(attribute.path, type);
}

int TreeNode::getFlags(const std::string& path)
{
	InternalPath attribute = this->findAttribute(path, FIND_TYPED);
	if (attribute.isNull())
	{
		dError("TreeNode::getFlags - attribute '" << path << "' not found in '" << this->getPath() << "'");
		return 0;
	}
	return attribute.node->getFlags(attribute.path);	
}

void TreeNode::setFlags(const std::string& path, int flags)
{
	InternalPath attribute = this->findAttribute(path, FIND_TYPED);
	if (attribute.isNull())
	{
		dError("TreeNode::setFlags - attribute '" << path << "' not found in '" << this->getPath() << "'");
		return;
	}
	attribute.node->setFlags(attribute.path, flags);	
}

int TreeNode::getScope(const std::string& path)
{
	InternalPath attribute = this->findAttribute(path, FIND_TYPED);
	if (attribute.isNull())
	{
		dError("TreeNode::getScope - attribute '" << path << "' not found in '" << this->getPath() << "'");
		return 0;
	}
	return attribute.node->getScope(attribute.path);	
}

void TreeNode::setScope(const std::string& path, int scope)
{
	InternalPath attribute = this->findAttribute(path, FIND_TYPED);
	if (attribute.isNull())
	{
		dError("TreeNode::setScope - attribute '" << path << "' not found in '" << this->getPath() << "'");
		return;
	}
	attribute.node->setScope(attribute.path, scope);	
}

void TreeNode::setInitializer(const std::string& path, const std::string& value)
{
	InternalPath attribute = this->findAttribute(path, FIND_TYPED);
	if (attribute.isNull())
	{
		dError("TreeNode::setInitializer - attribute '" << path << "' not found in '" << this->getPath() << "'");
		return;
	}
	attribute.node->setInitializer(attribute.path, value);
}
		
std::string TreeNode::getInitializer(const std::string& path)
{
	InternalPath attribute = this->findAttribute(path, FIND_TYPED);
	if (attribute.isNull())
	{
		dError("TreeNode::getInitializer - attribute '" << path << "' not found in '" << this->getPath() << "'");
		return std::string();
	}
	return attribute.node->getInitializer(attribute.path);
}

void TreeNode::getAttributes(const std::string& prefix, int flags, std::vector<std::string>& attributes)
{
	foreach (ChildPair& child, this->children)
	{
		child.second->getAttributes(prefix + child.first, flags, attributes);
	}	
}


// connectons

void TreeNode::connect(const std::string& path, const Path& srcPath)
{
	InternalPath attribute = this->findAttribute(path, FIND_TYPED);
	if (attribute.isNull())
	{
		dError("TreeNode::connect - attribute '" << path << "' not found in '" << this->getPath() << "'");
		return;
	}
	attribute.node->connect(attribute.path, srcPath);
}

bool TreeNode::isConnected(const std::string& path, bool component)
{
	InternalPath attribute = this->findAttribute(path, FIND_UNTYPED);
	if (attribute.isNull())
	{
		dError("TreeNode::isConnected - attribute '" << path << "' not found in '" << this->getPath() << "'");
		return false;
	}
	if (attribute.node == this)
		return false;
	return attribute.node->isConnected(attribute.path, component);	
}

bool TreeNode::isDataSource(const std::string& path)
{
	InternalPath attribute = this->findAttribute(path, FIND_UNTYPED);
	if (attribute.isNull())
	{
		dError("TreeNode::isDataSource - attribute '" << path << "' not found in '" << this->getPath() << "'");
		return false;
	}
	if (attribute.node == this)
		return false;
	return attribute.node->isDataSource(attribute.path);	
}


// optimize

void TreeNode::optimize(OptimizePhase phase)
{
	// pass to children
	foreach (ChildPair& child, this->children)
	{
		child.second->optimize(phase);
	}
}

// code generation

StructType* TreeNode::getTargetType(int scope)
{
	// create struct type
	StructType* structType = new StructType();
	
	foreach (ChildPair& child, this->children)
	{
		Type* type = child.second->getTargetType(scope);
		if (type != NULL)
			structType->addMember(makeTargetPath(child.first), type);
	}
	
	return structType;
}

StructType* TreeNode::getConnectionTargetType(Pointer<LeafNode> scope, bool targetNameMangling)
{
	// create node type
	StructType* nodeType = new StructType();
	
	foreach (ChildPair& child, this->children)
	{
		Type* type = child.second->getConnectionTargetType(scope);
		if (type != NULL)
		{
			if (targetNameMangling)
				nodeType->addMember(makeTargetPath(child.first), type);
			else
				nodeType->addMember(child.first, type);
		}
	}
	
	return nodeType;
}

Connection TreeNode::resolveConnection(const std::string& path)
{
	InternalPath attribute = this->findAttribute(path, FIND_UNTYPED);
	if (attribute.isNull() || attribute.node == this) // if attribute.node is this, infinite recursion would occur
	{
		dError("TreeNode::resolveConnection - attribute '" << path << "' not found in '" << this->getPath() << "'");
		return Connection();
	}
	
	return attribute.node->resolveConnection(attribute.path);	
}

void TreeNode::generateBuffers(std::vector<NamedBuffer>& buffers)
{
	// pass to children
	foreach (ChildPair& child, this->children)
	{
		child.second->generateBuffers(buffers);
	}
}


void TreeNode::propagateScopes()
{
	foreach (ChildPair& child, this->children)
	{
		child.second->propagateScopes();
	}
}

void TreeNode::propagateScopesAttributes(int scope)
{
	// pass to children
	foreach (ChildPair& child, this->children)
	{
		child.second->propagateScopesAttributes(scope);
	}
}

void TreeNode::propagateScopesNode(int scope)
{
	// pass to parent
	if (this->parent != NULL)
		this->parent->propagateScopesNode(scope);
}

int TreeNode::getScope()
{
	// pass to parent
	if (this->parent != NULL)
		return this->parent->getScope();
	return 0;
}

void TreeNode::addScopeRecursive(int scopeAdd)
{
	// pass to children
	foreach (ChildPair& child, this->children)
	{
		child.second->addScopeRecursive(scopeAdd);
	}
}


void TreeNode::visit(NodeVisitor& v, const std::string& path, int minScope, int maxScope)
{
	if (path.empty())
	{
		foreach (ChildPair& child, this->children)
		{
			child.second->visit(v, std::string(), minScope, maxScope);
		}
	}
	else
	{
		InternalPath attribute = this->findAttribute(path, FIND_UNTYPED);
		if (attribute.node != this)
			attribute.node->visit(v, attribute.path, minScope, maxScope);
	}
}

void TreeNode::visitNode(NodeVisitor& v, int minScope, int maxScope)
{
	// pass to parent
	if (this->parent != NULL)
		this->parent->visitNode(v, minScope, maxScope);
}

void TreeNode::visitAttributes(NodeVisitor& v, int minScope, int maxScope)
{
	// pass to children
	foreach (ChildPair& child, this->children)
	{
		child.second->visitAttributes(v, minScope, maxScope);
	}	
}


void TreeNode::writeInitCode(NodeWriter& w, int scope)
{
	WriteInitVisitor v(w);
	this->visit(v, std::string(), scope, scope);
}

void TreeNode::writeOutputCode(NodeWriter& w, Pointer<LeafNode> scope, const std::string& prefix, bool targetNameMangling)
{
	foreach (ChildPair& child, this->children)
	{
		child.second->writeOutputCode(w, scope, prefix, targetNameMangling);
	}
}

void TreeNode::getOutputAssignments(const std::map<int, std::string>& scopes, std::vector<std::string>& assignments,
	Pointer<LeafNode> sourceRoot, int scope)
{
	foreach (ChildPair& child, this->children)
	{
		child.second->getOutputAssignments(scopes, assignments, sourceRoot, scope);
	}
}

void TreeNode::writeInputCode(NodeWriter& w, Pointer<LeafNode> scope, const std::string& prefix)
{
	foreach (ChildPair& child, this->children)
	{
		child.second->writeInputCode(w, scope, prefix);
	}
}
/*
void TreeNode::writeInputReference(Pointer<LeafNode> scope, const std::string& prefix)
{
	foreach (ChildPair& child, this->children)
	{
		child.second->writeInputReference(scope, prefix);
	}
}
*/

namespace
{
	class OutputVisitor : public NodeVisitor
	{
	public:
	
		OutputVisitor(int minScope, int maxScope)
			: NodeVisitor(false), minScope(minScope), maxScope(maxScope)
		{
		}
	
		virtual ~OutputVisitor()
		{
		}
		
		virtual void visitConnection(const Connection& src, const Path& dstPath)
		{
			// note: resolved.path is not used
			Connection resolved = src.resolveConnection();
			if (resolved.attribute != null)
			{
				// check if scope of source attribute is in scope range we look for
				int srcScope = resolved.attribute->getScope();
				if (srcScope >= this->minScope && srcScope <= this->maxScope)
				{
					// multiple connections can originate from the same source, so check if attribute is new
					if (this->attributes.insert(resolved.attribute).second)
					{
						this->process(resolved);
					}
				}
			}
		}
		
		virtual void visitNodeAttribute(Pointer<Attribute> attribute)
		{
			Connection resolved = attribute->resolveConnection(std::string());
			if (resolved.attribute != null)
			{
				// check if scope of source attribute is in scope range we look for
				int srcScope = resolved.attribute->getScope();
				if (srcScope >= this->minScope && srcScope <= this->maxScope)
				{
					// multiple connections can originate from the same source, so check if attribute is new
					if (this->attributes.insert(resolved.attribute).second)
					{
						this->process(resolved);
					}
				}
			}
		}
		
		virtual void process(const Connection& resolved)
		{
			if (resolved.attribute != null)
			{
				// check if
				int srcScope = resolved.attribute->getScope();
				if (srcScope >= this->minScope && srcScope <= this->maxScope)
				{
					// multiple connections can originate from the same source, so check if attribute is new
					if (this->attributes.insert(resolved.attribute).second)
					{
						this->process(resolved);
					}
				}
			}
		}
			
		int minScope;
		int maxScope;
		std::set<Pointer<Attribute> > attributes;
	};


	class GetOutputTargetType : public OutputVisitor
	{
	public:
	
		GetOutputTargetType(int minScope, int maxScope)
			: OutputVisitor(minScope, maxScope)
		{
			this->type = new StructType();
		}
	
		virtual ~GetOutputTargetType()
		{
		}
				
		virtual void process(const Connection& resolved)
		{
			this->type->addMember(makeTargetPath(resolved.attribute->getPath(1)), resolved.attribute->getType());
		}
			
		Pointer<StructType> type;
	};

	class WriteOutputCode : public OutputVisitor
	{
	public:
	
		WriteOutputCode(NodeWriter& w, int minScope, int maxScope, const std::string& prefix)
			: OutputVisitor(minScope, maxScope), w(w), prefix(prefix)
		{
		}
	
		virtual ~WriteOutputCode()
		{
		}
		
		virtual void process(const Connection& resolved)
		{
			std::string targetPath = makeTargetPath(resolved.attribute->getPath(1));
			this->w << this->prefix << targetPath << " = "
				<< this->w.scopes[resolved.attribute->getScope()] << targetPath << ";\n";
		}
		
		NodeWriter& w;
		const std::string& prefix;
	};

	class GetOutputAssignments : public OutputVisitor
	{
	public:
	
		GetOutputAssignments(const std::map<int, std::string>& scopes, std::vector<std::string>& assignments,
			int minScope, int maxScope)
			: OutputVisitor(minScope, maxScope), scopes(scopes), assignments(assignments)
		{
		}
	
		virtual ~GetOutputAssignments()
		{
		}
				
		virtual void process(const Connection& resolved)
		{
			std::string targetPath = makeTargetPath(resolved.attribute->getPath(1));
			this->assignments += targetPath + " = "
				+ getValue(this->scopes, resolved.attribute->getScope(), std::string()) + targetPath;
		}
		
		const std::map<int, std::string>& scopes;
		std::vector<std::string>& assignments;
	};
}

Pointer<StructType> TreeNode::getOutputTargetType(int minScope, int maxScope, int outScope)
{
	GetOutputTargetType v(minScope, maxScope);
	this->visit(v, std::string(), outScope, outScope);
	return v.type;
}

void TreeNode::writeOutputCode(NodeWriter& w, int minScope, int maxScope, int outScope, const std::string& prefix)
{
	WriteOutputCode v(w, minScope, maxScope, prefix);
	this->visit(v, std::string(), outScope, outScope);
}

void TreeNode::getOutputAssignments(const std::map<int, std::string>& scopes, std::vector<std::string>& assignments,
	int minScope, int maxScope, int outScope)
{
	GetOutputAssignments v(scopes, assignments, minScope, maxScope);
	this->visit(v, std::string(), outScope, outScope);
}

/*
void TreeNode::writeLoadStateCode(NodeWriter& w)
{
	foreach (ChildPair& child, this->children)
	{
		child.second->writeLoadStateCode(w);
	}
}

void TreeNode::writeStoreStateCode(NodeWriter& w)
{
	foreach (ChildPair& child, this->children)
	{
		child.second->writeStoreStateCode(w);
	}
}
*/

void TreeNode::writeUpdateCode(NodeWriter& w, int minScope, int maxScope)
{
	// optimization: load state into local, then do update on local, then write back
	//w.writeLine();
	//w.writeComment("load state");
	//this->writeLoadStateCode(w);
	//w.localOverride = true;
	WriteUpdateVisitor v(w, maxScope);
	this->visit(v, std::string(), minScope, maxScope);
	//this->writeUpdateCodeInternal(w, std::string(), scope);
	//w.localOverride = false;
	//this->writeStoreStateCode(w);
}


// protected

int TreeNode::getMatchIndex(const std::string& path)
{
	// if path does not start with a path element member access is default
	// e.g. "foo" -> ".foo"
	std::string p;
	int o = 0;
	if (!isPathElementStart(path[0]))
	{
		o = 1;
		p += '.';
	}
	p += path;

	// assume a child "foo.bar" exists
	Children::iterator it = this->children.lower_bound(p);
	
	if (it != this->children.end())
	{
		// path matches child name (path = ".foo.bar")
		// path is part of child name (path = ".foo")
		if (startsWithPath(it->first, p))
			return path.length();
	}
			
	if (it != this->children.begin())
	{
		--it;
		
		// path starts with child name (path = ".foo.bar.x")
		if (startsWithPath(p, it->first))
			return it->first.length() - o;
	}

	return -1;
}

} // namespace digi
