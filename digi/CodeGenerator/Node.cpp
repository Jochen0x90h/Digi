#include <digi/Utility/Ascii.h>
#include <digi/Utility/StringUtility.h>
#include <digi/Utility/VectorUtility.h>
#include <digi/Utility/foreach.h>
#include <digi/System/Log.h>

#include "NodeVisitor.h"
#include "NodeWriter.h"
#include "Node.h"


namespace digi {

// Node

Node::~Node()
{	
}

std::string Node::getNodeType()
{
	return "Node";
}

// code generation

void Node::propagateScopes()
{
		// propagate to attributes of node (for nodes with no inputs)
		foreach (ChildPair& child, this->children)
		{
			child.second->propagateScopesAttributes(this->scope);
		}
		
		this->TreeNode::propagateScopes();
}

void Node::propagateScopesAttributes(int scope)
{
	// stop here because the attributes of this node are not the attributes of the parent node
}

void Node::propagateScopesNode(int scope)
{
	// continue propagation if scope of attribute is greater than scope of node
	if (scope > this->scope)
	{
		this->scope = scope;
		
		// propagate to attributes of node
		foreach (ChildPair& child, this->children)
		{
			child.second->propagateScopesAttributes(scope);
		}
	}
}

int Node::getScope()
{
	return this->scope;
}

void Node::addScopeRecursive(int scopeAdd)
{
	this->scope += scopeAdd;
	TreeNode::addScopeRecursive(scopeAdd);
}


void Node::visit(NodeVisitor& v, const std::string& path, int minScope, int maxScope)
{
	TreeNode::visit(v, path, minScope, maxScope);
	
	// this forces visit of nodes that have no output attributes
	if (path.empty())
		this->visitNode(v, minScope, maxScope);
}

void Node::visitNode(NodeVisitor& v, int minScope, int maxScope)
{
	// check if node scope is between min and max and if node has not been visited yet
	if (this->scope >= minScope && this->scope <= maxScope && v.nodes.insert(this).second)
	{
		// visit all attributes of node (input and output)
		foreach (ChildPair& child, this->children)
		{
			child.second->visitAttributes(v, minScope, maxScope);
		}
	
		// visit node
		v.visitNode(this);
	}
}

void Node::visitAttributes(NodeVisitor& v, int fromScope, int toScope)
{
	// this is called when this node is a child of another node
	// therefore do nothing since the attributes of this node
	// are not attributes of the parent node.
}


void Node::writeUpdateCodeThis(NodeWriter& w)
{
}

} // namespace digi
