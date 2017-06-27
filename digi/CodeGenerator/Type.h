#ifndef digi_CodeGenerator_Type_h
#define digi_CodeGenerator_Type_h

#include <string>
#include <set>
#include <map>

#include <digi/Utility/Object.h>

#include "CodeWriter.h"


namespace digi {

/// @addtogroup CodeGenerator
/// @{


// check if a character is start of path element ('.' or '[')
static inline bool isPathElementStart(char ch)
{
	return ch == '.' || ch == '[';
}

// ensure that a path begins with '.' or '['
static inline std::string makePath(StringRef path)
{
	if (path.empty() || isPathElementStart(path[0]))
		return path;
	return '.' + path;
}

/*
	check if a string starts with a path
	startsWithPath("foo.bar.x", "foo.bar.xy") == false
	startsWithPath("foo.bar.x", "foo.bar.x") == true
	startsWithPath("foo.bar.x", "foo.bar") == true
	startsWithPath("foo.bar.x", "foo.b") == false
*/
bool startsWithPath(const std::string& str, const std::string& path);

/*
	get first path element
	getPathElement("foo.bar") == "foo";
	getPathElement("foo.bar", 3) == ".bar";
*/
std::string getPathElement(const std::string& str, size_t start = 0);


/// base class for describing named types, array types and struct types
class Type : public Object
{
public:

	class Visitor
	{
		public:
			virtual ~Visitor();
			virtual void visit(const std::string& member, const std::string& type) = 0;
	};

	// align mode of type
	enum AlignMode
	{
		// use #pragma pack(1) to pack the type
		NO_ALIGN,

		// align components (e.g. float2, float3, float4 are is 4-aligned)
		COMPONENT_ALIGN,

		// align vectors (e.g. float2 is 8-aligned, float3 and float4 are 16-aligned)
		VECTOR_ALIGN,

		// extend to size 4 (directx 8 style)
		EXTEND_TO_4,
	};

	Type() {}
	virtual ~Type();

	// create a type by name. e.g. "int[3]" creates an ArrayType with int as element and length 3 
	static Type* create(const std::string& typeName);
	static Type* createInternal(const char*& typeName);
	
	// add a member. member selection and array access in name automatically write sub-types
	void addMember(const std::string& path, Pointer<Type> type);
	void addMember(const std::string& path, const std::string& type);
	
	
	virtual void addMemberInternal(const std::string& name, Pointer<Type> type);
	//virtual uint getHash(uint hash) = 0;
	
	// get member. no member selection is allowed. returns NULL if not found or if not a struct type
	virtual Type* getMemberInternal(const std::string& name);

	virtual void visitMembers(Visitor& visitor, std::string& prefix) = 0;
	void visitMembers(Visitor& visitor) {std::string prefix; this->visitMembers(visitor, prefix);}

	/// get array of all unrolled members
	void getMembers(std::vector<std::string>& members);

	/// get array (name, type) of all unrolled members
	void getMembers(std::vector<std::pair<std::string, std::string> >& members);

	/// get set of all unrolled members
	void getMembers(std::set<std::string>& members);

	/// get map (name, type) of all unrolled members
	void getMembers(std::map<std::string, std::string>& members);
	
	// returns true if the type is empty (empty struct or array of zero length)
	virtual bool isEmpty() = 0;

	// write type with given name (e.g. typedef int name;)
	virtual void writeType(CodeWriter& w, const std::string& name, AlignMode alignMode = VECTOR_ALIGN);

	// write variable with given name (e.g. int name;)
	void writeVariable(CodeWriter& w, const std::string& name, AlignMode alignMode = VECTOR_ALIGN);

	// write variable with given name (e.g. int name)
	virtual void writeVariableInternal(CodeWriter& w, const std::string& name, AlignMode alignMode = VECTOR_ALIGN) = 0;
	
	// writes a string representation of the type (e.g. "int[3]")
	virtual std::string toString() = 0;
};


/// a variable consisting of a name and a type
struct Variable
{
	std::string name;
	Pointer<Type> type;
	
	Variable() {}
	
	Variable(const std::string& name, Pointer<Type> type)
		: name(name), type(type) {}
		
	void writeCode(CodeWriter& w) const
	{
		this->type->writeVariable(w, this->name);
	}
};


/// named type for built-in types, e.g. int or float3
class NamedType : public Type
{
public:

	NamedType(const std::string& name) : name(name) {}

	virtual ~NamedType();

	// set name of type
	void setName(const std::string& name) {this->name = name;}
	
	// get name of type
	const std::string& getName() {return this->name;}


	//virtual uint getHash(uint hash);

	virtual void visitMembers(Visitor& visitor, std::string& prefix);
	using Type::visitMembers;
	
	// returns false since a named type is never empty
	virtual bool isEmpty();

	// write variable with given name (e.g. int name)
	virtual void writeVariableInternal(CodeWriter& w, const std::string& name, AlignMode alignMode = VECTOR_ALIGN);

	virtual std::string toString();

protected:

	// name of type, e.g. "int", "float3", "MyStruct"
	std::string name;
};


/// array type
class ArrayType : public Type
{
public:
	
	ArrayType() : numElements(0) {}
	ArrayType(Pointer<Type> elementType, size_t numElements)
		: elementType(elementType), numElements(numElements) {}
	virtual ~ArrayType();

	size_t getNumElements() {return this->numElements;}

	virtual void addMemberInternal(const std::string& name, Pointer<Type> type);
	//virtual uint getHash(uint hash);
	virtual Type* getMemberInternal(const std::string& name);

	virtual void visitMembers(Visitor& visitor, std::string& prefix);
	using Type::visitMembers;

	virtual bool isEmpty();
	

	// write variable with given name (e.g. int name[5])
	virtual void writeVariableInternal(CodeWriter& w, const std::string& name, AlignMode alignMode = VECTOR_ALIGN);

	virtual std::string toString();

	bool parseName(const std::string& name);

	// element type and length for array types
	Pointer<Type> elementType;
	size_t numElements;
};


/// struct type
class StructType : public Type
{
public:
	
	StructType() {}
	virtual ~StructType();

	virtual void addMemberInternal(const std::string& name, Pointer<Type> type);
	//virtual uint getHash(uint hash);
	virtual Type* getMemberInternal(const std::string& name);

	virtual void visitMembers(Visitor& visitor, std::string& prefix);
	using Type::visitMembers;

	void writeMembers(CodeWriter& w);

	virtual bool isEmpty();

	// write type with given name (e.g. struct name {..};)
	virtual void writeType(CodeWriter& w, const std::string& name, AlignMode alignMode = VECTOR_ALIGN);

	// write variable wigh given name (e.g. struct {..} name)
	virtual void writeVariableInternal(CodeWriter& w, const std::string& name, AlignMode alignMode = VECTOR_ALIGN);

	virtual std::string toString();


	// members
	std::vector<Variable> members;
};


/// @}

} // namespace digi

#endif
