#ifndef digi_CodeGenerator_TreeNode_h
#define digi_CodeGenerator_TreeNode_h

#include <map>

#include <digi/Utility/foreach.h>

#include "LeafNode.h"
#include "Path.h"
#include "Attribute.h"

namespace digi {

/// @addtogroup CodeGenerator
/// @{

struct PathElementList;

class TreeNode : public LeafNode
{
	friend struct PathElementList;

public:
		
	typedef std::map<std::string, Pointer<LeafNode> > Children;
	typedef std::pair<const std::string, Pointer<LeafNode> > ChildPair;

	TreeNode() {}
	
	TreeNode(StringRef name)
		: LeafNode(name) {}
			
	virtual ~TreeNode();

// tree

	// get root of tree node. is the node itself if there is no parent
	virtual Pointer<TreeNode> getRoot();
			
	// add a node as child. returns false if failed because the name exists or no childs can be added
	virtual bool addNode(Pointer<LeafNode> node);

	// get an unique name for the given name
	std::string getUniqueName(const std::string& name);

	// add a node as child. automatically generates a new name if the name exists
	void addNode(Pointer<LeafNode> node, const std::string proposedName);

	// get chilren. iterate with foreach (const ChildPair& p, node->getChildren())
	const Children& getChildren() {return this->children;}
	
	/*
		get a list of path elements starting at path for use with boost::foreach

		e.g.
		foreach (Path path, node->getPathElementList(".foo"))
		{
			...
		}
	*/
	PathElementList getPathElementList(const std::string& path);

	// removes a node so that it can be released or inserted into another tree
	void removeNode(Pointer<LeafNode> node);

// attribute

	/// add an attribute to the node
	Pointer<Attribute> addAttribute(const std::string& attributeName, const std::string& type,
		int flags = 0, int scope = 1);

	/// add an attribute which is connected to an attribute of another node
	Pointer<Attribute> addAttribute(const std::string& attributeName, const std::string& type, const Path& connection,
		int flags = 0, int scope = 1);

	/// add an attribute with an initializer
	void addAttributeWithInitializer(const std::string& attributeName, const std::string& type,
		const std::string& initializer, int flags = 0, int scope = 1);

	template <typename Type>
	void addAttributeWithInitializer(const std::string& attributeName, Type initializer, int flags = 0, int scope = 1)	
	{
		this->addAttributeWithInitializer(attributeName, TypeName<Type>::get(), Code() << initializer, flags, scope);
	}
	
	/// add an input to the node
	Pointer<Attribute> addInput(const std::string& attributeName, const std::string& type, int scope = 1)
	{
		return this->addAttribute(attributeName, type, Attribute::INPUT, scope);
	}

	/// add an input which references the given data source
	Pointer<Attribute> addInput(const std::string& attributeName, const std::string& type, const Path& source)
	{
		return this->addAttribute(attributeName, type, source, Attribute::INPUT);
	}

	/// add an output to the node
	Pointer<Attribute> addOutput(const std::string& attributeName, const std::string& type, int scope = 1)
	{
		return this->addAttribute(attributeName, type, Attribute::OUTPUT, scope);
	}

	/// add a state variable to the node
	Pointer<Attribute> addState(const std::string& attributeName, const std::string& type)
	{
		return this->addAttribute(attributeName, type, Attribute::STATE, 0);
	}

	template <typename Type>
	void addStateWithInitializer(const std::string& attributeName, Type initializer)
	{
		this->addAttributeWithInitializer(attributeName, TypeName<Type>::get(), Code() << initializer, Attribute::STATE, 0);
	}

	/// add a constant attribute
	void addConstant(const std::string& attributeName, const std::string& type, const std::string& value)
	{
		this->addAttributeWithInitializer(attributeName, type, value, Attribute::CONSTANT);	
	}
	
	template <typename Type>
	void addConstant(const std::string& attributeName, Type value)
	{
		this->addAttributeWithInitializer(attributeName, TypeName<Type>::get(), Code() << value, Attribute::CONSTANT);
	}

	virtual InternalPath findAttribute(const std::string& path, FindMode findMode = FIND_TYPED);
	virtual std::string getType(const std::string& path);
	virtual void setType(const std::string& path, const std::string& type);
	virtual int getFlags(const std::string& path);
	virtual void setFlags(const std::string& path, int flags);
	virtual int getScope(const std::string& path);
	virtual void setScope(const std::string& path, int scope);
	virtual void setInitializer(const std::string& path, const std::string& value);
	using LeafNode::setInitializer;
	virtual std::string getInitializer(const std::string& path);
	virtual void getAttributes(const std::string& prefix, int flags, std::vector<std::string>& attributes);

// connectons
			
	virtual void connect(const std::string& path, const Path& srcPath);
	virtual bool isConnected(const std::string& path, bool component = true);
	virtual bool isDataSource(const std::string& path);

// optimize
	
	virtual void optimize(OptimizePhase phase);

	template <typename Type>
	bool getInitializerValue(const std::string& path, Type& value)
	{
		Attribute* attribute = dynamic_cast<Attribute*>(this->findAttribute(path, FIND_TYPED).node);
		if (attribute == NULL)
			return false;
		return attribute->getInitializerValue(value);
	}

// code generation

	virtual StructType* getTargetType(int scope);
	virtual StructType* getConnectionTargetType(Pointer<LeafNode> scope, bool targetNameMangling = true);
	virtual Connection resolveConnection(const std::string& path);
	virtual void generateBuffers(std::vector<NamedBuffer>& buffers);
	

	// propagate scopes to output connections, from input attributes to nodes and from nodes to output attributes
	virtual void propagateScopes();
	
	// propagate scope to output attributes of a node
	virtual void propagateScopesAttributes(int scope);

	// propagate scope to parent node
	virtual void propagateScopesNode(int scope);
	
	// get scope of this node (gets it from parent node)
	virtual int getScope();

	// recursively adds to the scope of this node and all its children (used to "lift" a subgraph onto a new scope range)
	virtual void addScopeRecursive(int scopeAdd);


	virtual void visit(NodeVisitor& v, const std::string& path, int minScope, int maxScope);
	virtual void visitNode(NodeVisitor& v, int minScope, int maxScope);
	virtual void visitAttributes(NodeVisitor& v, int minScope, int maxScope);
	
	void writeInitCode(NodeWriter& w, int scope);
	virtual void writeOutputCode(NodeWriter& w, Pointer<LeafNode> scope, const std::string& prefix, bool targetNameMangling = true);
	virtual void getOutputAssignments(const std::map<int, std::string>& scopes, std::vector<std::string>& assignments,
		Pointer<LeafNode> sourceRoot, int scope);
	virtual void writeInputCode(NodeWriter& w, Pointer<LeafNode> scope, const std::string& prefix);
	//virtual void writeInputReference(Pointer<LeafNode> scope, const std::string& prefix);

	// get type that transfers data from nodes between minScope and maxScope (e.g. scene) to outScope (e.g. shader)
	Pointer<StructType> getOutputTargetType(int minScope, int maxScope, int outScope);
	
	// write code that transfers data from nodes between minScope and maxScope (e.g. scene) to outScope (e.g. shader)
	void writeOutputCode(NodeWriter& w, int minScope, int maxScope, int outScope, const std::string& prefix);
	
	// get list of output assignments from nodes between minScope and maxScope (e.g. scene) to outScope (e.g. shader)
	void getOutputAssignments(const std::map<int, std::string>& scopes, std::vector<std::string>& assignments,
		int minScope, int maxScope, int outScope);

	//virtual void writeLoadStateCode(NodeWriter& w);
	//virtual void writeStoreStateCode(NodeWriter& w);
	
	// write update code. calls writeLoadStateCode(), writeUpdateCodeInternal() and writeStoreStateCode()
	void writeUpdateCode(NodeWriter& w, int minScope, int maxScope);
	void writeUpdateCode(NodeWriter& w, int scope) {this->writeUpdateCode(w, scope, scope);}
			
protected:

	int getMatchIndex(const std::string& path);

		
	Children children;
};




struct PathElementList
{
	struct Iterator
	{
		Iterator() {}

		Iterator(Pointer<TreeNode> node, TreeNode::Children::iterator it, const std::string& path, const std::string& element)
			: node(node), it(it), path(path), element(element) {}
		
		Path operator *()
		{
			return Path(this->node, this->path + this->element);
		}

		Iterator& operator ++()
		{
			do
			{
				++this->it;
				if (this->it == this->node->children.end())
				{
					this->element = std::string();
					break;
				}
				if (!startsWithPath(this->it->first, this->path + this->element))
				{
					if (!startsWithPath(this->it->first, this->path))
						this->element = std::string();
					else
						this->element = getPathElement(this->it->first, this->path.length());
					break;
				}
			} while (true);
			return *this;
		}

		bool operator ==(const Iterator& other) const
		{
			return this->element == other.element;
		}
		
		bool operator !=(const Iterator& other) const
		{
			return this->element != other.element;
		}

		Pointer<TreeNode> node;
		TreeNode::Children::iterator it;
		std::string path;
		std::string element;
	};

	PathElementList() {}
	PathElementList(Pointer<TreeNode> node, const std::string& path)
		: node(node), path(path) {}
		
	Iterator begin() const
	{
		if (this->node == null)
			return Iterator();
			
		TreeNode::Children::iterator it = this->node->children.lower_bound(this->path);
		if (it == this->node->children.end())
			return Iterator();
			
		std::string element = getPathElement(it->first, this->path.length());
		
		return Iterator(this->node, it, this->path, element);
	}
	Iterator end() const
	{
		return Iterator();
	}

	Pointer<TreeNode> node;
	std::string path;
};


inline PathElementList TreeNode::getPathElementList(const std::string& path)
{
	Path attribute = this->findAttribute(path, FIND_UNTYPED);
	
	Pointer<TreeNode> node = dynamicCast<TreeNode>(attribute.node);
	
	if (node == null)
		return PathElementList();
	
	return PathElementList(node, attribute.path);	
}



// boost.range implementation for node paths

static inline PathElementList::Iterator range_begin(const PathElementList& list)
{
	return list.begin();
}

static inline PathElementList::Iterator range_end(const PathElementList& list)
{
	return list.end();
}

/// @}

} // namespace digi


namespace boost
{
	template < >
	struct range_mutable_iterator<digi::PathElementList>
	{
		typedef digi::PathElementList::Iterator type;
	};

	template < >
	struct range_const_iterator<digi::PathElementList>
	{
		typedef digi::PathElementList::Iterator type;
	};

	// iterator traits

	template < >
	struct iterator_traversal<digi::PathElementList::Iterator>
	{
		typedef forward_traversal_tag type;
	};

	template < >
	struct iterator_difference<digi::PathElementList::Iterator>
	{
		typedef int type;
	};


	template < >
	struct iterator_value<digi::PathElementList::Iterator>
	{
		typedef digi::Path type;
	};

	template < >
	struct iterator_reference<digi::PathElementList::Iterator>
	{
		typedef digi::Path type;
	};

	template < >
	struct iterator_pointer<digi::PathElementList::Iterator>
	{
		typedef digi::Path* type;
	};
}

#endif 
