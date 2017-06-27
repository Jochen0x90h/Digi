#include <digi/Utility/Ascii.h>
#include <digi/Utility/foreach.h>
#include <digi/Utility/lexicalCast.h>
#include <digi/Utility/MapUtility.h>
#include <digi/Utility/StringUtility.h>
#include <digi/Utility/VectorUtility.h>
#include <digi/System/Log.h>

#include "TypeInfo.h"
#include "TreeNode.h"
#include "CodeWriterFunctions.h"
#include "NodeVisitor.h"
#include "NodeWriter.h"
#include "Attribute.h"

namespace digi {

namespace {

	std::string getValue(const std::string& str, size_t s, size_t e)
	{
		// trim
		while (s < e && (uint8_t)str[s] <= 32)
			++s;
		while (e > s && (uint8_t)str[e - 1] <= 32)
			--e;
			
		return substring(str, s, e);
	}

} // anonymous namespace


// Connection

std::string Connection::getTargetPath() const
{
	std::string result;
	if (this->attribute != null)
		result = makeTargetPath(this->attribute->getPath(1));
	result += path;
	return result;
}

void Connection::writeVariable(NodeWriter& w) const
{
	if (this->attribute != null)
	{
		w << w.scopes[this->attribute->getScope()];	
		w << makeTargetPath(this->attribute->getPath(1));
	}
	w << this->path;	
}


// Attribute
/*
inline bool operator ==(const Attribute::OutConnection& a, const Attribute::OutConnection& b)
{
	return a.path == b.path;
}
*/
Attribute::~Attribute()
{
}

// attribute

static std::string selectMember(const std::string& type, const std::string& member)
{
	if (member[0] == '[')
	{
		// type is e.g. "float[5]" and member is e.g. "[1]";
		// remove the first "[.]" in type
		int pos1 = find(type, '[');
		if (pos1 >= 0)
		{
			int pos2 = find(type, ']');
			if (pos2 >= 0)
				return type.substr(0, pos1) + type.substr(pos2 + 1);
		} 
	}
	else
	{
		StringRef m(member);
		if (m[0] == '.')
			m = StringRef(m.data() + 1, m.length() - 1);
		
		// type is e.g. "float3"
		MatrixInfo typeInfo(type);
		
		if (typeInfo.type != MatrixInfo::INVALID)
		{
			// is a vector or matrix type
			
			// get length of component (e.g. "xyz" has length 3)
			int mLen = int(m.length());
			
			if (typeInfo.numColumns > 1)
			{
				// matrix
			
				// only one vector component may be accessed (e.g. ".x" but not ".xy")
				if (mLen > 1)
					return std::string();

				// check if member component is valid
				int index = find("xyzw", m[0]);
				if (index < 0 || index >= typeInfo.numColumns)
					return std::string();
			
				return MatrixInfo(typeInfo.type, typeInfo.numRows, mLen).toString();
			}
			else if (typeInfo.numRows > 1)
			{
				// vector
			
				// check if member components are valid
				for (int i = 0; i < mLen; ++i)
				{
					int index = find("xyzw", m[i]);
					if (index < 0 || index >= typeInfo.numRows)
						return std::string();
				}

				return VectorInfo(typeInfo.type, mLen).toString();
			}
			else
			{
				// scalar
				
				// has no members
				return std::string();
			}
		}
		/*else if (startsWith(type, "Sampler"))
		{
			// sampler type
			if (m == "texture")
				return "Texture";
			if (m == "stateIndex")
				return "int";
		}*/
	}
	return std::string();
}

static std::string getAttrType(std::string type, const std::string& path)
{
	// type is e.g. "float3", path is e.g. ".x"
	size_t s = 0;
	size_t len = path.length();
	
	// get component type
	while (s < len)
	{
		// get member (e.g. ".x")
		size_t e = findOneOf(path, ".[", s + 1, len);
		std::string member = path.substr(s, e - s);

		// get type of member (e.g. ".x" of "float3" is "float")
		type = selectMember(type, member);
		s = e;
	}
	return type;
}

InternalPath Attribute::findAttribute(const std::string& path, FindMode findMode)
{
	// if not partial find mode then check if type is valid
	if (findMode == FIND_TYPED && getAttrType(this->type, path).empty())
		return InternalPath();
		
	return InternalPath(this, path);
}

std::string Attribute::getType(const std::string& path)
{
	std::string type = getAttrType(this->type, path);

	if (type.empty())
		dError("attribute '" << this->getPath() << "' has no component named '" << path << "'");
	
	return type;
}

void Attribute::setType(const std::string& path, const std::string& type)
{
	if (path.empty())
		this->type = type;
	else
		dError("attribute '" << this->getPath() << "': can't set type of component '" << path << "'");
}

int Attribute::getFlags(const std::string& path)
{
	return this->flags;
}

void Attribute::setFlags(const std::string& path, int flags)
{
	this->flags = flags;
}

int Attribute::getScope(const std::string& path)
{
	return this->scope;
}

void Attribute::setScope(const std::string& path, int scope)
{
	this->scope = scope;
}

static std::string selectComponent(const std::string& value, const std::string& member)
{
	// value is e.g. "vector3(1, 2, 3)"
	
	if (startsWith(value, "vector"))
	{
		// vector
			
		int index = -1;
		if (member == ".x")
			index = 0;
		else if (member == ".y")
			index = 1;
		else if (member == ".z")
			index = 2;
		else if (member == ".w")
			index = 3;
		
		if (index < 0)
			return std::string();
			
		int s = find(value, '(', 6, -1);
		if (s == -1)
			return std::string();
		++s;
		int e;
		int len = value.length();
		while (true)
		{
			e = findOneOf(value, ",)", s, -1);
			if (e == -1)
				return std::string();
			if (index == 0)
				break;
			s = e + 1;
			while (s < len && value[s] == ' ')
				++s;
			--index;
		}

		return value.substr(s, e - s); 
	}
	return std::string();
}

void Attribute::setInitializer(const std::string& path, const std::string& value)
{
	this->initializers[path] = value;
}

std::string Attribute::getInitializer(const std::string& path)
{	
	// path is e.g. ".x"
	// initializers are e.g. ".x" = "1.0f" or "" = "vector3(1.0f, 2.0f, 3.0f)"
	Initializers::iterator end = this->initializers.end();
	size_t len = path.length();
	
	// find the initializer
	size_t e = len;
	while (true)
	{
		std::string head = path.substr(0, e);
		Initializers::iterator it = this->initializers.find(head);
		if (it != end)
		{
			// get initializer (e.g. "vector3(1.0f, 2.0f, 3.0f)")
			std::string initializer = it->second;
			
			// get component of initializer
			while (e < len)
			{
				size_t s = e;
								
				// get member (e.g. ".x" or "[0]")
				e = findOneOf(path, ".[", s + 1, len);
				std::string member = path.substr(s, e - s);
				
				// get component (e.g. "1.0f")
				initializer = selectComponent(initializer, member);			
			}
			return initializer;
		
		}
		if (e == 0)
			break;
		size_t s = findOneOfRev(path, ".[", e - 1);
		e = s;
	}
	return std::string();
}

void Attribute::getAttributes(const std::string& prefix, int flags, std::vector<std::string>& attributes)
{
	if (this->flags & flags)
		attributes += prefix;
}

// connectons	

void Attribute::connect(const std::string& path, const Path& srcPath)
{
	if (srcPath.isNull())
	{
		dError("Attribute::connect - source path is null");
		return;
	}

	// get lowest possible node
	InternalPath srcAttributePath = srcPath.node->findAttribute(srcPath.path, FIND_TYPED);
	Attribute* srcAttribute = dynamic_cast<Attribute*>(srcAttributePath.node);
	if (srcAttribute == NULL)
	{
		dError("Attribute::connect - source is invalid");
		return;
	}
	
	// get connection. silently overrides existing connection
	Connection& connection = this->connections[path];
		
	// remove old output connection
	if (connection.attribute != null)
		connection.attribute->outConnections.erase(&connection.path);
	
	// set input connection
	connection.attribute = srcAttribute;
	connection.path = srcAttributePath.path;
	
	// set output connection
	srcAttribute->outConnections[&connection.path] = this;
}

bool Attribute::isConnected(const std::string& path, bool component)
{
	Connections::iterator it = this->connections.lower_bound(path);
	if (it == this->connections.end())
		return false;
	
	// check if attribute itself or component are connected
	return it->first == path || (component && startsWith(it->first, path + "."));
}

bool Attribute::isDataSource(const std::string& path)
{
	// if path is empty then this attribute has an output connection if the array is not empty
	if (path.empty())
		return !this->outConnections.empty();
	
	std::string pp = path + '.';		
	foreach (OutConnectionPair& p, this->outConnections)
	{
		const std::string& connectionPath = *p.first;
		
		// path is e.g. ".x" then it's a data source if connection path is ".x" or ".x.y" (starts with ".x.")
		if (connectionPath == path || startsWith(connectionPath, pp))
			return true;
	}
	return false;
}

bool Attribute::isConnectedTo(Pointer<LeafNode> scope)
{
	foreach (Attribute::ConnectionPair& connection, this->connections)
	{
		if (connection.second.attribute->getRoot() == scope)
			return true;
	}
	return false;
}

bool Attribute::isConnectedTo(int scope)
{
	foreach (Attribute::ConnectionPair& connection, this->connections)
	{
		if (connection.second.attribute->getScope() == scope)
			return true;
	}
	return false;
}


// optimize

/*
static int getIndex(const std::string& member, int pos = 0)
{
	// member is e.g. ".xyz.x"
	int len = getLength(member);
	if (pos >= len)
		return -1;
	if (member[pos] == '.')
	{
		++pos;
		
		int index;
		int pos2 = find(member, '.', pos, -1);
		if (pos2 == -1)
		{
			if (len - pos != 1)
				return -1;
			index = 0;
		}
		else
		{
			index = getIndex(member, pos2);
		}
		
		char ch = member[pos + index];
		switch (ch)
		{
		case 'x':
			return 0;
		case 'y':
			return 1;
		case 'z':
			return 2;
		case 'w':
			return 3;
		default:
			return -1;
		}
	}
	return -1;
}
*/

namespace
{
	struct InitializerInfo
	{
		std::string initializer;
	
		// dimension of initializer
		int dimension;
	
		// index of initialize element (0 = .x, 1 = .y etc.)
		int index;
	
		InitializerInfo()
			: dimension(), index() {}
		InitializerInfo(const std::string& initializer, int dimension, int index)
			: initializer(initializer), dimension(dimension), index(index) {}
	};

	typedef InitializerInfo InitializerInfos[4];

	void parseInitializers(const std::string& member, int pos, const std::string& value, InitializerInfos& initInfos, int dimension)
	{
		int len = member.length();
		if (pos < len)
		{
			// phase1: parse member (e.g. ".xy.x")
			if (member[pos] == '.')
			{
				++pos;
			
				int pos2 = find(member, '.', pos, len);
				InitializerInfos initInfos2;
				int dimension2 = pos2 - pos;
				parseInitializers(member, pos2, value, initInfos2, dimension2);
			
				for (int i = 0; i < dimension2; ++i)
				{
					char ch = member[pos + i];
					int index;
					switch (ch)
					{
					case 'x':
						index = 0;
						break;
					case 'y':
						index = 1;
						break;
					case 'z':
						index = 2;
						break;
					case 'w':
						index = 3;
						break;
					default:
						index = -1;
					}
				
					if (index != -1)
						initInfos[index] = initInfos2[i];
				}
			}
		
		}
		else
		{
			// phase 2: parse value
			if (startsWith(value, "vector"))
			{
				// vector (e.g. "vector3(1.0f, 2.0f, 3.0f)")
				int dimension = value[6] - '0';

				size_t s = 8;
				for (int i = 0; i < dimension; ++i)
				{
					size_t e = findOneOf(value, ",)", s);
					initInfos[i] = InitializerInfo(getValue(value, s, e), 1, 0);
					s = e + 1;
				}
			}
			else if (startsWith(value, "splat"))
			{
				// splat of scalar value (e.g. "splat3(1.0f)")
				int dimension = value[5] - '0';

				size_t s = 7;
				size_t e = find(value, ')', s);
				std::string initializer = getValue(value, s, e);
				for (int i = 0; i < dimension; ++i)
				{
					initInfos[i] = InitializerInfo(initializer, 1, 0);
				}
			}
			else
			{
				// scalar or constant (e.g. "1.0f" or "time"). constants may be vector-valued
				for (int i = 0; i < dimension; ++i)
				{
					// dimension and component index are stored for vector constants
					initInfos[i] = InitializerInfo(value, dimension, i);
				}
			}
		}
	}
} // anonymous namespace

void Attribute::optimize(OptimizePhase phase)
{
	// vectorize initializers
	switch (phase)
	{
	case VECTORIZE:
		{
			MatrixInfo typeInfo(this->type);
			
			// check if this is a vector type (e.g. "float3")
			if (typeInfo.numRows >= 2 && typeInfo.numColumns == 1)
			{
				// vectorize connections
				//!
				
				// vectorize initializers
				if (!this->initializers.empty())
				{
					// collect the max. 4 initializers
					InitializerInfos initInfos; 
					foreach (InitializerPair& initializerPair, this->initializers)
					{
						parseInitializers(initializerPair.first, 0, initializerPair.second, initInfos, typeInfo.numRows);
					}

					// fill empty initializers with zeros
					for (int i = 0; i < typeInfo.numRows; ++i)
					{
						InitializerInfo& initInfo = initInfos[i];
						if (initInfo.initializer.empty())
						{
							// generate zero value
							switch (typeInfo.type)
							{
							case MatrixInfo::BOOL:
								initInfo.initializer = Code() << false;
								break;
							case MatrixInfo::FLOAT:
								initInfo.initializer = Code() << 0.0f;
								break;
							case MatrixInfo::DOUBLE:
								initInfo.initializer = Code() << 0.0;
								break;
							default:
								initInfo.initializer = Code() << 0;
							}
							initInfo.dimension = 1;
							initInfo.index = 0;
						}
					}
					
					// check if we can use splat or all components belong to the same value
					std::string initializer = initInfos[0].initializer;
					bool initializerIsEqual = true;
					bool dimensionIsOne = true;
					bool indexIsEqual = true;
					for (int i = 0; i < typeInfo.numRows; ++i)
					{
						const InitializerInfo& initInfo = initInfos[i];
						initializerIsEqual &= initInfo.initializer == initializer;
						dimensionIsOne &= initInfo.dimension == 1;
						indexIsEqual &= initInfo.index == i;
					}
					if (initializerIsEqual && dimensionIsOne)
					{
						initializer = "splat" + toString(typeInfo.numRows) + "(" + initializer + ")";
					}
					else if (!(initializerIsEqual && indexIsEqual))
					{
						initializer = "vector" + toString(typeInfo.numRows) + "(";
						for (int i = 0; i < typeInfo.numRows; ++i)
						{
							if (i != 0)
								initializer += ", ";
							
							const InitializerInfo& initInfo = initInfos[i];
							const std::string& init = initInfo.initializer;

							initializer += init;
							//char ch = init[0];
							if (initInfo.dimension == 1)// || Ascii::isDecimal(ch) || ch == '+' || ch == '-' || init == "false" || init == "true")
							{
								// this is a scalar or literal (e.g. "1.0f" or "time")
							}
							else
							{
								// this is a vector
								initializer += '.';
								initializer += "xyzw"[initInfo.index];
							}
						}
						initializer += ")";
					}
					this->initializers.clear();
					this->initializers[std::string()] = initializer;				
				}
			}
		}
		break;
	case OPTIMIZE_ATTRIBUTES:
		
		this->getReferenceFlags();

		break;
	}	
}

int Attribute::getReferenceFlags()
{
	// check if the whole attribute is connected and not a member (e.g. "" and not ".x")
	if (this->connections.size() == 1 && this->connections.begin()->first.empty())
	{
		const Connection& connection = this->connections.begin()->second;
				
		// recursively create references and get flags of attribute at end of reference chain
		int referenceFlags = connection.attribute->getReferenceFlags();
		
		// don't make a reference if this is an interface attribute.
		// otherwise the attribute would not be accessible or updated in the interface any more
		if ((this->flags & INTERFACE) == 0)
		{
			// don't reference from outer scope (e.g. scene) into inner scope (e.g. shader)
			if (connection.attribute->scope <= this->scope)
			{	
				// set reference if connected attribte is constant or this attribute and connected attribte have the same root node
				if ((referenceFlags & CONSTANT) != 0 || this->getRoot() == connection.attribute->getRoot())
				{
					this->flags |= REFERENCE;
					return referenceFlags | REFERENCE;
				}
			}
		}
	}
	
	// set constant if attribute is not an output, reference or interface and not connected
	if ((this->flags & (OUTPUT | REFERENCE | INTERFACE)) == 0 && this->connections.empty())
	{
		// check if the whole attribute has an initializer and not a member (e.g. "" and not ".x")
		if (this->initializers.size() == 1 && contains(this->initializers, std::string()))
		{
			this->flags |= CONSTANT;
			return CONSTANT;
		}
	}

	return 0;
}

bool Attribute::getInitializerValue(bool& value)
{
	if ((this->flags & CONSTANT) == 0)
		return false;

	// collect the max. 4 initializers
	InitializerInfos initInfos; 
	foreach (InitializerPair& initializerPair, this->initializers)
	{
		parseInitializers(initializerPair.first, 0, initializerPair.second, initInfos, 3);
	}

	value = toBool(initInfos[0].initializer);
	return true;
}

bool Attribute::getInitializerValue(float3& value)
{
	if ((this->flags & CONSTANT) == 0)
		return false;

	// collect the max. 4 initializers
	InitializerInfos initInfos; 
	foreach (InitializerPair& initializerPair, this->initializers)
	{
		parseInitializers(initializerPair.first, 0, initializerPair.second, initInfos, 3);
	}

	value.x = toFloat(initInfos[0].initializer); // initializer is e.g. "1.0f"
	value.y = toFloat(initInfos[1].initializer);
	value.z = toFloat(initInfos[2].initializer);
	return true;
}


// code generation

Type* Attribute::getTargetType(int scope)
{
	if ((this->flags & (REFERENCE | CONSTANT)) == 0 && this->scope == scope)
		return Type::create(this->type);
	return NULL;
}

Type* Attribute::getConnectionTargetType(Pointer<LeafNode> scope, bool targetNameMangling)
{
	if ((this->flags & (REFERENCE | CONSTANT)) == 0)
	{
		// add to connection target type if this attribute is connected to the given scope
		if (this->isConnectedTo(scope))
			return Type::create(this->type);
	}
	return NULL;
}

Connection Attribute::resolveConnection(const std::string& path)
{
	if ((this->flags & REFERENCE) != 0)
	{
		// reference: replace by connected variable
		Connections::iterator it = this->connections.find(std::string());
		if (it == this->connections.end())
		{
			dError("Attribute::resolveConnection - reference attribute '" << this->getPath() << "' has no connection");
			return Connection();
		}

		// recursively resolve
		return it->second.resolveConnection(path);
	}
	else if ((this->flags & CONSTANT) != 0)
	{
		// constant: replace by initializer
		Initializers::iterator it = this->initializers.find(std::string());
		if (it == this->initializers.end())
		{
			dError("Attribute::resolveConnection - constant attribute '" << this->getPath() << "' has no initializer");
			return Connection();
		}
		return Connection(null, it->second + path);
	}

	return Connection(this, path);
}


void Attribute::propagateScopes()
{
	// iterate over output connections
	foreach (OutConnectionPair& p, this->outConnections)
	{
		Attribute* attribute = p.second;
		if (this->scope > attribute->scope)
		{
			attribute->scope = this->scope;
			attribute->propagateScopes();
		}
	}

	// propagate to node if this is an input of a node
	if ((this->flags & INPUT) != 0)
	{
		this->getParent()->propagateScopesNode(this->scope);
	}	
}

void Attribute::propagateScopesAttributes(int scope)
{
	// propagate scope if this is only an output, not a state
	if ((this->flags & INPUT_OUTPUT) == OUTPUT)
	{
		// continue propagation if scope of node is greater than scope of attribute
		if (scope > this->scope)
		{
			this->scope = scope;
			this->propagateScopes();
		}
	}
}

int Attribute::getScope()
{
	return this->scope;
}

void Attribute::addScopeRecursive(int scopeAdd)
{
	this->scope += scope;
}


void Attribute::visit(NodeVisitor& v, const std::string& path, int minScope, int maxScope)
{
	if ((this->flags & CONSTANT) != 0 || !(this->scope >= minScope && this->scope <= maxScope))
		return;
	
	// visit attribute if this attribute is new
	if (v.attributes.insert(this).second)
		v.visitAttribute(this);
	
	// follow connections and mark them with "ongoing" state
	// connections to compound overwrite connections to fields (e.g. ".x")
	reverse_foreach (ConnectionPair& connection, this->connections)
	{
		int& visited = v.connections[&connection.second];
		
		// only follow connections that were not followed before
		if (visited == 0)
		{
			// determine if we have to follow the connection and visit the connected node
			bool followConnection = false;
						
			// check if connection needs visit. attribute is e.g. "myAttribute.x"
			if (connection.first.length() >= path.length())
			{
				// length of connected attribute name is equal or longer, e.g. "myAttribute.x.y"
				if (startsWith(connection.first, path))
				{
					followConnection = true;
				}
			}
			else
			{
				// length of connected attribute is shorter, e.g. "myAttribute".
				// exclude e.g. "myAttr" by checking for '.' or '[' at the position in
				// the requested path where the node path ends
				if (startsWithPath(path, connection.first))
				{
					followConnection = true;
				}
			}
						
			if (followConnection)
			{
				// mark connecton as "ongoing"
				visited = 1;

				int connectedScope = connection.second.attribute->getScope();
				//Pointer<TreeNode> connectedRoot = connection.second.attribute->getRoot();

				// visit connected node if it is in same scope
				if (connectedScope >= minScope && connectedScope <= maxScope)// && connectedRoot == this->getRoot())
					connection.second.attribute->visit(v, connection.second.path, minScope, maxScope);
			}
		}
	}			

	// visit connections still in "ongoing" state and mark them as "finished"
	// connections to compound overwrite connections to fields (e.g. ".x")
	reverse_foreach (ConnectionPair& connection, this->connections)
	{
		int& visited = v.connections[&connection.second];

		// check if connection is "ongoing"
		if (visited == 1)
		{
			// determine if we have to follow the connection and update the connected node
			bool followConnection = false;
						
			// check if connection needs visit. attribute is e.g. "myAttribute.x"
			if (connection.first.length() >= path.length())
			{
				// length of connected attribute name is equal or longer, e.g. "myAttribute.x.y"
				if (startsWith(connection.first, path))
				{
					followConnection = true;
				}
			}
			else
			{
				// length of connected attribute is shorter, e.g. "myAttribute".
				// exclude e.g. "myAttr" by checking for '.' or '[' at the position in
				// the requested path where the node path ends
				if (startsWithPath(path, connection.first))
				{
					followConnection = true;
				}
			}
						
			if (followConnection)
			{
				// mark connecton as "finished"
				visited = 2;

				// visit connection
				if ((this->flags & REFERENCE) == 0)
				{
					//Pointer<TreeNode> connectedRoot = connection.second.attribute->getRoot();
					
					//if (connectedRoot == this->getRoot())
					{
						v.visitConnection(connection.second, Path(this, connection.first));
					}
				}			
			}
		}			
	}

	// visit the node if this is an output (i.e. follow implicit connection of node to output attribute)
	if ((this->flags & OUTPUT) != 0)
	{
		this->getParent()->visitNode(v, minScope, maxScope);
	}
}

void Attribute::visitAttributes(NodeVisitor& v, int minScope, int maxScope)
{
	// visit the attributes of a node, therefore check if this is an input or output
	if ((this->flags & INPUT_OUTPUT) != 0 && (!v.stop || (this->flags & STOP) == 0))
	{
		v.visitNodeAttribute(this);
		this->visit(v, std::string(), minScope, maxScope);
	}
}


void Attribute::writeInitCode(NodeWriter& w)
{			
	// initializers
	if ((this->flags & (REFERENCE | CONSTANT)) == 0)
	{
		for (Initializers::iterator it = this->initializers.begin(); it != this->initializers.end(); ++it)
		{
			this->writeVariable(w, it->first);
			w << " = " << it->second << ";\n";
		}
	}
}

void Attribute::writeOutputCode(NodeWriter& w, Pointer<LeafNode> scope, const std::string& prefix, bool targetNameMangling)
{
	if ((this->flags & (REFERENCE | CONSTANT)) == 0)
	{
		foreach (ConnectionPair& connection, this->connections)
		{
			if (connection.second.attribute->getRoot() == scope)
			{
				// copy from data source that is in other scope into this attribute which is in a struct given by prefix
				std::string path = this->getPath(1);
				if (targetNameMangling)
					path = makeTargetPath(path);
					
				w << prefix << path << connection.first << " = ";
				connection.second.resolveConnection().writeVariable(w);
				w << ";\n";	
			}
		}
	}	
}

void Attribute::getOutputAssignments(const std::map<int, std::string>& scopes, std::vector<std::string>& assignments,
	Pointer<LeafNode> sourceRoot, int scope)
{
	if ((this->flags & (REFERENCE | CONSTANT)) == 0)
	{
		foreach (ConnectionPair& connection, this->connections)
		{
			if (connection.second.attribute->getRoot() == sourceRoot)
			{
				// copy from data source that is in other scope into this attribute which is in a struct given by prefix
				std::string path = makeTargetPath(this->getPath(1));
				
				Connection resolved = connection.second.resolveConnection();
				assignments += path + connection.first + " = "
					+ getValue(scopes, resolved.attribute->getScope(), std::string()) + resolved.getTargetPath();
					
				// set scope to attribute that receives data from source graph
				this->scope = scope;
			}
		}
	}	
}

void Attribute::writeInputCode(NodeWriter& w, Pointer<LeafNode> scope, const std::string& prefix)
{
	if ((this->flags & (REFERENCE | CONSTANT)) == 0)
	{
		foreach (ConnectionPair& connection, this->connections)
		{
			if (connection.second.attribute->getRoot() == scope)
			{
				// copy from data source that is in other scope into this attribute
				this->writeVariable(w, connection.first);
				w << " = " << prefix << makeTargetPath(this->getPath(1)) << connection.first << ";\n";
			}
		}
	}	
}
/*
void Attribute::writeInputReference(Pointer<LeafNode> scope, const std::string& prefix)
{
	// assignments from other scope
	if ((this->flags & (REFERENCE | CONSTANT)) == 0)
	{
		if (this->connections.empty())
			return;
		
		// all connections must point to the given scope
		foreach (ConnectionPair& connection, this->connections)
		{
			if (connection.second.attribute->getRoot() != scope)
				return;
		}

		this->connections.clear();
		this->initializers[std::string()] = Code() << prefix << makeTargetPath(this->getPath(1));
		this->flags |= CONSTANT;
	}	
}
*/
/*
void Attribute::writeLoadStateCode(NodeWriter& w)
{
	if ((this->flags & (REFERENCE | CONSTANT)) == 0)
	{
		// load if this is a state
		// or an interface that is not an output and not fully connected
		if ((this->flags & STATE) != 0
			|| ((this->flags & INTERFACE) != 0 && (this->flags & OUTPUT) == 0 && !contains(this->connections, std::string())))
		{		 
			// copy from state to local
			std::string path = this->getPath(1);
			w << makeTargetPath("local" + path) << " = " << makeTargetPath("state" + path) << ";\n";
		}
	}
}

void Attribute::writeStoreStateCode(NodeWriter& w)
{
	if ((this->flags & (REFERENCE | CONSTANT)) == 0)
	{
		// store if this is a state
		// or an interface that is an output or has a connection
		if ((this->flags & STATE) != 0
			|| ((this->flags & INTERFACE) != 0 && ((this->flags & OUTPUT) != 0 || !this->connections.empty())))
		{
			// copy from local to state
			std::string path = this->getPath(1);
			w << makeTargetPath("state" + path) << " = " << makeTargetPath("local" + path) << ";\n";
		}
	}		
}
*/


// protected

void Attribute::clearConnectionsThis()
{
	// call inherited method
	//this->LeafNode::clearConnectionsThis();
	
	//! remove from outConnections of connected attributes
	
	this->connections.clear();
	//this->outConnections.clear();
}


} // namespace digi
