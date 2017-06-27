#include <digi/Utility/StringUtility.h>
#include <digi/Utility/VectorUtility.h>
#include <digi/Utility/Ascii.h>
#include <digi/Utility/foreach.h>
#include <digi/System/Log.h>

#include "TypeInfo.h"
#include "Type.h"


namespace digi {


bool startsWithPath(const std::string& str, const std::string& path)
{
	size_t pathLength = path.length();
	if (str.compare(0, pathLength, path) == 0)
	{
		// str starts with path, now check if str ends or has a separator
		if (str.length() == pathLength)
			return true;
		
		if (str[pathLength] == '.' || str[pathLength] == '[')
			return true;
	}
	return false;
}

std::string getPathElement(const std::string& path, size_t start)
{
	size_t pathLength = path.length();
	size_t i = start + 1;
	while (i < pathLength)
	{
		if (path[i] == '.' || path[i] == '[')
			break;
		++i;
	}
	return path.substr(start, i - start);
}

// Type::Visitor
Type::Visitor::~Visitor()
{
}

class ArrayMemberVisitor : public Type::Visitor
{
	public:
		ArrayMemberVisitor(std::vector<std::string>& members) : members(members) {}
		virtual ~ArrayMemberVisitor() {}
		virtual void visit(const std::string& member, const std::string& type)
		{
			this->members += member;
		}
		
		std::vector<std::string>& members;
};

class ArrayPairMemberVisitor : public Type::Visitor
{
	public:
		ArrayPairMemberVisitor(std::vector<std::pair<std::string, std::string> >& members) : members(members) {}
		virtual ~ArrayPairMemberVisitor() {}
		virtual void visit(const std::string& member, const std::string& type)
		{
			this->members += std::make_pair(member, type);
		}
		
		std::vector<std::pair<std::string, std::string> >& members;
};

class SetMemberVisitor : public Type::Visitor
{
	public:
		SetMemberVisitor(std::set<std::string>& members) : members(members) {}
		virtual ~SetMemberVisitor() {}
		virtual void visit(const std::string& member, const std::string& type)
		{
			this->members.insert(member);
		}
		
		std::set<std::string>& members;
};

class MapMemberVisitor : public Type::Visitor
{
	public:
		MapMemberVisitor(std::map<std::string, std::string>& members) : members(members) {}
		virtual ~MapMemberVisitor() {}
		virtual void visit(const std::string& member, const std::string& type)
		{
			this->members[member] = type;
		}
		
		std::map<std::string, std::string>& members;
};


// Type

Type::~Type()
{
}

Type* Type::create(const std::string& typeName)
{
	if (typeName.empty())
		return NULL;
		
	const char* tn = typeName.c_str();
	return Type::createInternal(tn);
}

Type* Type::createInternal(const char*& typeName)
{
	if (*typeName == '{')
	{
		// struct type (e.g. "{foo int,bar float}")

		StructType* structType = new StructType();
		while (*typeName != 0)
		{
			// skip '{' or ','
			++typeName;
			
			if (*typeName == '}')
				break;

			// get name
			const char* name = typeName;
			while (Ascii::isIdentifier(*typeName))
				++typeName;
			const char* nameEnd = typeName;

			// skip ' '
			if (typeName == name || *typeName != ' ')
				break;
			++typeName;
						
			// get type
			Type* type = Type::createInternal(typeName);
			if (type == NULL)
				break;

			structType->members += Variable(substring(name, 0, nameEnd - name), type);
		}
				
		return structType;
	}
	
	if (*typeName == '[')
	{
		// array type (e.g. "[5]int")
		++typeName;

		// get number of elements
		int numElements = 0;
		while (Ascii::isDecimal(*typeName))
		{
			numElements *= 10;
			numElements += *typeName - '0';
			++typeName;
		}
		
		// skip ']'
		if (*typeName != ']')
			return NULL;
		++typeName;
		
		Type* type = Type::createInternal(typeName);
		if (type == NULL)
			return NULL;
		
		return new ArrayType(type, numElements);
	}

	// get type name
	const char* name = typeName;
	while (Ascii::isIdentifier(*typeName) || *typeName == ':' || *typeName == '*') // allow e.g. IK2D::Joint or float* as type name
		++typeName;
	const char* nameEnd = typeName;

	if (typeName == name)
		return NULL;

	return new NamedType(substring(name, 0, nameEnd - name));
}

void Type::addMember(const std::string& path, Pointer<Type> type)
{
	if (type->isEmpty())
		return;
	
	if (path.empty())
	{
		dError("Type::addMember: empty path");
		return;
	}
	
	// example: path = ".foo.bar[3].x"
	std::string p = makePath(path);
	
	int pos = 0;
	int length = int(p.length());
	
	Type* current = this;
	while (true)
	{	
		int start = pos;
		
		do
			++pos;
		while (pos < length && p[pos] != '.' && p[pos] != '[');
		
		std::string element = p.substr(start, pos - start);
		if (pos == length)
		{
			// last element
			current->addMemberInternal(element, type);
			return;
		}

		// intermediate element
		Type* next = current->getMemberInternal(element);
		
		if (next == NULL)
		{
			if (p[pos] == '.')
			{
				next = new StructType();
				current->addMemberInternal(element, next);
			}
			else
			{
				next = new ArrayType();
				current->addMemberInternal(element, next);
			}
		}
		current = next;
	}
}

void Type::addMember(const std::string& name, const std::string& type)
{
	this->addMember(name, Type::create(type));
}

void Type::addMemberInternal(const std::string& name, Pointer<Type> type)
{
}

Type* Type::getMemberInternal(const std::string& name)
{
	return NULL;
}

void Type::getMembers(std::vector<std::string>& members)
{
	std::string prefix;
	ArrayMemberVisitor visitor(members);
	this->visitMembers(visitor, prefix);
}

void Type::getMembers(std::vector<std::pair<std::string, std::string> >& members)
{
	std::string prefix;
	ArrayPairMemberVisitor visitor(members);
	this->visitMembers(visitor, prefix);
}

void Type::getMembers(std::set<std::string>& members)
{
	std::string prefix;
	SetMemberVisitor visitor(members);
	this->visitMembers(visitor, prefix);
}

void Type::getMembers(std::map<std::string, std::string>& members)
{
	std::string prefix;
	MapMemberVisitor visitor(members);
	this->visitMembers(visitor, prefix);
}

void Type::writeType(CodeWriter& w, const std::string& name, AlignMode alignMode)
{
	if (alignMode == NO_ALIGN)
		w << "#pragma pack(1)\n";

	w << "typedef ";
	this->writeVariable(w, name, alignMode);

	if (alignMode == NO_ALIGN)
		w << "#pragma pack()\n";
}

void Type::writeVariable(CodeWriter& w, const std::string& name, AlignMode alignMode)
{
	if (this->isEmpty())
		return;

	this->writeVariableInternal(w, name, alignMode);
	w << ";\n";
}


// NamedType

NamedType::~NamedType()
{
}
/*
uint NamedType::getHash(uint hash)
{
	return getCRC32(this->name, hash);
}
*/
void NamedType::visitMembers(Visitor& visitor, std::string& prefix)
{
	visitor.visit(prefix, this->name);
}

bool NamedType::isEmpty()
{
	 return false;
}

void NamedType::writeVariableInternal(CodeWriter& w, const std::string& name, AlignMode alignMode)
{
	if (alignMode == NO_ALIGN)
	{
		MatrixInfo typeInfo(this->name);
		if (typeInfo.numRows == 3)
		{
			w << "packed_" << this->name << " " << name;
			return;
		}
	}
	w << this->name << " " << name;
}

std::string NamedType::toString()
{
	return this->name;
}


// ArrayType

ArrayType::~ArrayType()
{

}

void ArrayType::addMemberInternal(const std::string& name, Pointer<Type> type)
{
	if (this->parseName(name))
		this->elementType = type;
}
/*
uint ArrayType::getHash(uint hash)
{
	hash = this->elementType->getHash(hash);

	std::stringstream s;
	s << "[" << this->numElements << "]";
	return getCRC32(s.str(), hash);
}
*/
Type* ArrayType::getMemberInternal(const std::string& name)
{
	if (this->parseName(name))
		return this->elementType.getPointer();
	return NULL;
}

void ArrayType::visitMembers(Visitor& visitor, std::string& prefix)
{
	size_t len = prefix.length();
	prefix += '[';
	for (size_t i = 0; i < this->numElements; ++i)
	{
		prefix += digi::toString(i);
		prefix += ']';
		this->elementType->visitMembers(visitor, prefix);
		prefix.resize(len + 1);
	}
	prefix.resize(len);
}

bool ArrayType::isEmpty()
{
	if (this->numElements <= 0)
		return true;

	return this->elementType->isEmpty();
}

void ArrayType::writeVariableInternal(CodeWriter& w, const std::string& name, AlignMode alignMode)
{
	if (this->isEmpty())
		return;

	this->elementType->writeVariableInternal(w, name, alignMode);
	w << "[" << this->numElements << "]";
}

std::string ArrayType::toString()
{
	std::string s;
	s += '[';
	s += digi::toString(this->numElements);
	s += ']';
	s += this->elementType->toString();
	return s;
}

bool ArrayType::parseName(const std::string& name)
{
	size_t e = name.size() - 1;
	if (name.empty() || name[0] != '[' || name[e] != ']')
		return false;
		
	size_t index = 0;
	for (size_t i = 1; i < e; ++i)
	{
		index *= 10;
		index += name[i] - '0';
	}
	this->numElements = max(this->numElements, index + 1);
	return true;
}


// StructType

StructType::~StructType()
{
}

void StructType::addMemberInternal(const std::string& name, Pointer<Type> type)
{
	if (name.length() < 2)
		return;
	if (name[0] != '.')
		return;
		
	const char* n = name.c_str() + 1;

	// check if member exists
	foreach (const Variable& member, this->members)
	{
		if (member.name == n)
			return;
	}
	
	// add member
	this->members += Variable(n, type);
}
/*
uint StructType::getHash(uint hash)
{
	hash = getCRC32("{", 1, hash);
	foreach (const Variable& member, this->members)
	{
		hash = getCRC32(member.name, hash);
		hash = getCRC32(" ", 1, hash);
		hash = member.type->getHash(hash);
		hash = getCRC32(";", 1, hash);
	}
	hash = getCRC32("}", 1, hash);
	return hash;
}
*/
Type* StructType::getMemberInternal(const std::string& name)
{
	if (name.length() < 2)
		return NULL;
	if (name[0] != '.')
		return NULL;

	const char* n = name.c_str() + 1;

	foreach (const Variable& member, this->members)
	{
		if (member.name == n)
			return member.type.getPointer();
	}
	return NULL;
}

void StructType::visitMembers(Visitor& visitor, std::string& prefix)
{
	size_t len = prefix.length();
	size_t len2 = len;
	if (len > 0)
	{
		prefix += '.';
		++len2;
	}

	foreach (const Variable& member, this->members)
	{
		prefix += member.name;
		member.type->visitMembers(visitor, prefix);
		prefix.resize(len2);
	}
	prefix.resize(len);
}

void StructType::writeMembers(CodeWriter& w)
{
	// write member variable declarations for all members
	foreach (const Variable& member, this->members)
	{
		member.type->writeVariable(w, member.name);
	}
}

bool StructType::isEmpty()
{
	foreach (const Variable& member, this->members)
	{
		if (!member.type->isEmpty())
			return false;
	}
	return true;
}

void StructType::writeType(CodeWriter& w, const std::string& name, AlignMode alignMode)
{
	if (alignMode == NO_ALIGN)
		w << "#pragma pack(1)\n";
	
	w.beginStruct(name);

	// write member variable declarations for all members
	foreach (const Variable& member, this->members)
	{
		member.type->writeVariable(w, member.name, alignMode);
	}
		
	w.endStruct();

	if (alignMode == NO_ALIGN)
		w << "#pragma pack()\n";
}

void StructType::writeVariableInternal(CodeWriter& w, const std::string& name, AlignMode alignMode)
{
	// begin anonymous struct
	w.beginStruct();

	// write member variable declarations for all members
	foreach (const Variable& member, this->members)
	{
		member.type->writeVariable(w, member.name, alignMode);
	}
			
	w.decIndent();
	w << "} " << name;
}

std::string StructType::toString()
{
	std::string s = "{";
	if (this->members.empty())
	{
		s += '}';
	}
	else
	{
		foreach (const Variable& member, this->members)
		{
			s += member.name;
			s += ' ';
			s += member.type->toString();
			s += ',';
		}	
		s[s.length() - 1] = '}';
	}
	return s;
}


} // namespace digi
