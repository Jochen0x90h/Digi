#include <digi/System/Log.h>

#include "NodeVisitor.h"
#include "TreeNode.h"


namespace digi {

// LeafNode

LeafNode::~LeafNode()
{
}

std::string LeafNode::getNodeType()
{
	return std::string();
}

// tree

Pointer<TreeNode> LeafNode::getRoot()
{
	TreeNode* node = this->parent;
	if (node != NULL)
	{
		while (node->parent != NULL)
		{
			node = node->parent;
		}
	}
	return node;
}

int LeafNode::getDepth()
{
	int depth = 0;
	LeafNode* node = this->parent;
	while (node != NULL)
	{
		++depth;
		node = node->parent;
	}
	
	return depth;
}

std::string LeafNode::getPath(int startDepth)
{
	// get depth of this node from root
	int depth = this->getDepth();
	
	std::string name;
	LeafNode* node = this;
	while (depth > startDepth)
	{
		name = makePath(node->name) + name;
		node = node->parent;
		--depth;
	}
	
	// don't prepend a '.' to name of root node
	if (startDepth == 0)
		return node->name + name;
	else
		return makePath(node->name) + name;
}

bool LeafNode::addNode(Pointer<LeafNode> node)
{
	return false;
}

// code generation

Pointer<StructType> LeafNode::getTargetTypeOfConnected(int scope)
{
	TargetTypeVisitor v;
	this->visit(v, std::string(), scope, scope);
	return v.type;
}

void LeafNode::generateBuffers(std::vector<NamedBuffer>& buffers)
{
}

std::string LeafNode::getTargetPath(const std::string& path)
{
	return this->resolveConnection(path).getTargetPath();	
}

void LeafNode::writeVariable(NodeWriter& w, const std::string& path)
{
	this->resolveConnection(path).writeVariable(w);
}


// protected

void LeafNode::clearConnectionsThis()
{
}

} // namespace digi
