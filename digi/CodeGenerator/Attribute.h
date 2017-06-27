#ifndef digi_CodeGenerator_Attribute_h
#define digi_CodeGenerator_Attribute_h

#include <string>

#include "LeafNode.h"


namespace digi {

/// @addtogroup CodeGenerator
/// @{

class Attribute;

struct Connection
{
	Pointer<Attribute> attribute;

	std::string path;

	Connection();
	Connection(Pointer<Attribute> attribute, std::string path);
	~Connection();

	Connection resolveConnection() const;
	Connection resolveConnection(const std::string& path) const;
	std::string getTargetPath() const;
	void writeVariable(NodeWriter& w) const;
};


class Attribute : public LeafNode
{
	friend class TreeNode;
	
public:
		
	enum Flags
	{
		// input attribute. nodes only read from it.
		INPUT = 1,

		// output attribute. nodes write or modify it.
		OUTPUT = 2,
		
		// input and output
		INPUT_OUTPUT = INPUT | OUTPUT,
		
		// state variable
		STATE = INPUT | OUTPUT,

		// reference to attribute of other node. a data source must be set.
		REFERENCE = 8,
		
		// constant attribute. will be inline-replaced by its initializer
		CONSTANT = 16,
	
		// extra attribute
		EXTRA = 32,

		// interface to the outside
		INTERFACE = 64,
		
		// stop visit flag
		STOP = 128,
	};
		
			
	Attribute(const std::string& type, int flags = 0, int scope = 0)
		: type(type), flags(flags), scope(scope) {}

	Attribute(const std::string& name, const std::string& type, int flags, int scope)
		: LeafNode(name), type(type), flags(flags), scope(scope) {}

	virtual ~Attribute();

// attribute
	
	virtual InternalPath findAttribute(const std::string& path, FindMode findMode = FIND_TYPED);
	virtual std::string getType(const std::string& path);
	virtual void setType(const std::string& path, const std::string& type);
	const std::string& getType() {return this->type;}
	virtual int getFlags(const std::string& path);
	virtual void setFlags(const std::string& path, int flags);
	int getFlags() {return this->flags;}
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

	// check if attribute is connected an attribute in given scope
	bool isConnectedTo(Pointer<LeafNode> scope);	

	// check if attribute is connected an attribute in given scope
	bool isConnectedTo(int scope);	

// optimize

	virtual void optimize(OptimizePhase phase);
	int getReferenceFlags();

	// get initializer
	bool getInitializerValue(bool& value);
	bool getInitializerValue(float3& value);

// code generation

	virtual Type* getTargetType(int scope);
	virtual Type* getConnectionTargetType(Pointer<LeafNode> scope, bool targetNameMangling = true);
	virtual Connection resolveConnection(const std::string& path);


	// propagate scopes to output connections and to the node if this is an input
	virtual void propagateScopes();
	
	// continue propagation if this attribute is an output
	virtual void propagateScopesAttributes(int scope);
	
	// get scope of this attribute
	virtual int getScope();

	// adds to the scope of this attribute (used to "lift" a subgraph onto a new scope range)
	virtual void addScopeRecursive(int scopeAdd);


	virtual void visit(NodeVisitor& v, const std::string& path, int minScope, int maxScope);	
	virtual void visitAttributes(NodeVisitor& v, int minScope, int maxScope);

	void writeInitCode(NodeWriter& w);
	virtual void writeOutputCode(NodeWriter& w, Pointer<LeafNode> scope, const std::string& prefix, bool targetNameMangling = true);
	virtual void getOutputAssignments(const std::map<int, std::string>& scopes, std::vector<std::string>& assignments,
		Pointer<LeafNode> sourceRoot, int scope);
	virtual void writeInputCode(NodeWriter& w, Pointer<LeafNode> scope, const std::string& prefix);
	//virtual void writeInputReference(Pointer<LeafNode> scope, const std::string& prefix);
	//virtual void writeLoadStateCode(NodeWriter& w);
	//virtual void writeStoreStateCode(NodeWriter& w);

protected:

	// clear parent and all connections to remove circular references when finished with a node graph
	virtual void clearConnectionsThis();

	typedef std::map<std::string, Connection> Connections;
	typedef std::pair<const std::string, Connection> ConnectionPair;

	typedef std::map<std::string, std::string> Initializers;
	typedef std::pair<const std::string, std::string> InitializerPair;
				
	// type of attribute
	std::string type;
	
	// flags of attribute (e.g. INPUT)
	int flags;
	
	// scope of attribute
	int scope;
	
	// input connections to other attributes. components may get connected (e.g. ".x")
	Connections connections;
	
	// initializers of attribute. components may get connected
	Initializers initializers;
	
	// output connections. key is pointer to path to connected component (e.g. ".x")
	std::map<std::string*, Attribute*> outConnections;
	typedef std::pair<std::string* const, Attribute*> OutConnectionPair;
};



inline Connection::Connection()
{
}

inline Connection::~Connection()
{
}

inline Connection::Connection(Pointer<Attribute> attribute, std::string path)
	: attribute(attribute), path(path)
{
}

inline Connection Connection::resolveConnection() const
{
	return this->attribute->resolveConnection(this->path);
}

inline Connection Connection::resolveConnection(const std::string& path) const
{
	return this->attribute->resolveConnection(this->path + path);
}

/// @}

} // namespace digi

#endif 
