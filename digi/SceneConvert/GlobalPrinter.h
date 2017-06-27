#ifndef digi_SceneConvert_GlobalPrinter_h
#define digi_SceneConvert_GlobalPrinter_h

#include <string>
#include <set>
#include <map>

#include <llvm/Value.h>
#include <llvm/Target/TargetData.h>

#include <clang/AST/ASTContext.h>

#include <digi/Utility/Object.h>
#include <digi/CodeGenerator/CodeWriter.h>
#include <digi/CodeGenerator/NameGenerator.h>

#include "ShaderOptions.h"
#include "ShaderType.h"


namespace digi {

/// @addtogroup SceneConvert
/// @{

class PrintWalker;

/// maps a global vairiable name found in llvm ir to a name for code output
class GlobalPrinter : public Object
{
public:

	virtual ~GlobalPrinter();

	// print a global for store. may only be called once
	virtual void printForStore(CodeStream& w, llvm::Value* value, llvm::Value* global,
		std::vector<llvm::Value*>& indices, PrintWalker& printWalker) = 0;
	
	// print a global for load. returns true if the global is a vector
	virtual void printForLoad(CodeStream& w, llvm::Type* type, llvm::Value* global,
		std::vector<llvm::Value*>& indices, PrintWalker& printWalker, int priority) = 0;

	// get variables
	virtual void getVariables(std::vector<ShaderVariable>& variables);
};

struct GlobalPrinterAccess
{
	enum AccessFlags
	{
		READ = 1,
		WRITE = 2,
		READ_WRITE = 3
	};

	Pointer<GlobalPrinter> globalPrinter;
	
	// access flags for the global
	int access;
	
	GlobalPrinterAccess()
		: access() {}
		
	GlobalPrinterAccess& operator =(Pointer<GlobalPrinter> globalPrinter)
	{
		this->globalPrinter = globalPrinter;
		return *this;
	}
};


/// collects variables into individual variables
class StandardGlobalPrinter : public GlobalPrinter
{
public:

	StandardGlobalPrinter(const std::vector<ShaderType>& types, StringRef storePrefix, StringRef loadPrefix,
		NameGenerator& nameGenerator);
	virtual ~StandardGlobalPrinter();

	virtual void printForStore(CodeStream& w, llvm::Value* value, llvm::Value* global,
		std::vector<llvm::Value*>& indices, PrintWalker& printWalker);
	virtual void printForLoad(CodeStream& w, llvm::Type* type, llvm::Value* global,
		std::vector<llvm::Value*>& indices, PrintWalker& printWalker, int priority);

	// get variables
	virtual void getVariables(std::vector<ShaderVariable>& variables);
		
	std::vector<ShaderVariable> variables;
	std::string storePrefix;
	std::string loadPrefix;
};


/// generate variables on the fly if possible (i.e. if not array)
class VariableGlobalPrinter : public GlobalPrinter
{
public:

	VariableGlobalPrinter(const std::vector<ShaderType>& types, StringRef storePrefix, StringRef loadPrefix,
		NameGenerator& nameGenerator);
	virtual ~VariableGlobalPrinter();

	virtual void printForStore(CodeStream& w, llvm::Value* value, llvm::Value* global,
		std::vector<llvm::Value*>& indices, PrintWalker& printWalker);
	virtual void printForLoad(CodeStream& w, llvm::Type* type, llvm::Value* global,
		std::vector<llvm::Value*>& indices, PrintWalker& printWalker, int priority);
	
	struct VariableInfo
	{
		ShaderType type;
		NameGenerator nameGenerator;
	};

	std::vector<VariableInfo> variables;
	std::string storePrefix;
	std::string loadPrefix;
};


/// packs variables into vectors of length 4 (for e.g. transfer from vertex to pixel shader)
class PackingGlobalPrinter : public GlobalPrinter
{
public:

	PackingGlobalPrinter(const std::vector<ShaderType>& types, StringRef storePrefix, StringRef loadPrefix,
		NameGenerator& nameGenerator);
	virtual ~PackingGlobalPrinter();
	
	virtual void printForStore(CodeStream& w, llvm::Value* value, llvm::Value* global,
		std::vector<llvm::Value*>& indices, PrintWalker& printWalker);
	virtual void printForLoad(CodeStream& w, llvm::Type* type, llvm::Value* global,
		std::vector<llvm::Value*>& indices, PrintWalker& printWalker, int priority);

	// get shader parameters
	virtual void getVariables(std::vector<ShaderVariable>& parameters);


	struct PackingVariable
	{
		std::string name;
		
		// vector size
		int numRows;
		
		PackingVariable()
			: numRows() {}
		PackingVariable(std::string name, int numRows)
			: name(name), numRows(numRows) {}
	};
	typedef std::vector<PackingVariable> PackingArray;
	
	typedef std::pair<ShaderType::Type, Precision> TypePair; 
	typedef std::pair<const TypePair, PackingArray> VariablePair;

	// for each type the flags to indicate which elements are already occupied
	std::map<TypePair, std::vector<PackingVariable> > vectors;

	struct Variable
	{
		ShaderType type;
		
		// offset of variable in PackingArray
		int offset;
		
		// start component in variable (x, y, z or w)
		int start;
		
		Variable()
			: offset(), start() {}
	};

	// variables
	std::vector<Variable> variables;
	std::string storePrefix;
	std::string loadPrefix;
	NameGenerator& nameGenerator;
};


/// packs variables into arrays of vectors of length 4 (used for uniform shader variables)
class ArrayPackingGlobalPrinter : public GlobalPrinter
{
public:

	ArrayPackingGlobalPrinter(const std::vector<ShaderType>& types, StringRef storePrefix, StringRef loadPrefix,
		int& maxArraySize, NameGenerator& nameGenerator);
	virtual ~ArrayPackingGlobalPrinter();

	// allocate array variables maybe into textures if too large
	void allocateArrays(int& maxArraySize, NameGenerator& nameGenerator);
	
	virtual void printForStore(CodeStream& w, llvm::Value* value, llvm::Value* global,
		std::vector<llvm::Value*>& indices, PrintWalker& printWalker);
	virtual void printForLoad(CodeStream& w, llvm::Type* type, llvm::Value* global,
		std::vector<llvm::Value*>& indices, PrintWalker& printWalker, int priority);

	// get array variables
	virtual void getVariables(std::vector<ShaderVariable>& variables);

	// get textures that store large arrays
	void getLargeArrays(std::vector<ShaderVariable>& textures);
	

	struct Variable
	{
		ShaderType type;
				
		// flag to indicate if variable is a large array
		bool isLargeArray;
		
		// offset of variable in packed array
		int offset;

		// start component of variable in packed array (0-3 for x, y, z or w)
		int start;
		
		Variable()
			: isLargeArray(false), offset(), start() {}
	};

	struct PackingArray
	{
		std::string name;
		std::vector<uint8_t> flags;
	};

	typedef std::pair<ShaderType::Type, Precision> TypePair; 
	typedef std::pair<const TypePair, PackingArray> ArrayPair;

	// allocate variable in array. returns number of newly used elements or -1 if no space
	static int allocate(Variable& variable, int maxNewElements, PackingArray& array, NameGenerator& nameGenerator);

	static void printIndexAndSwizzle(CodeStream& w, const PackingArray& array, const Variable& variable,
		const std::vector<llvm::Value*>& indices, PrintWalker& printWalker);

	static void printSampleAndSwizzle(CodeStream& w,
		const std::string& loadPrefix, const PackingArray& array, const Variable& variable,
		const std::vector<llvm::Value*>& indices, PrintWalker& printWalker);


	// one variable for each type passed in the constructor
	std::vector<Variable> variables;
	
	// store/load prefixes
	std::string storePrefix;
	std::string loadPrefix;

	// for each type the flags to indicate which elements are already occupied
	std::map<TypePair, PackingArray> arrays;

	// large arrays (e.g. bone matrix palette) have to be put into textures
	std::map<TypePair, PackingArray> largeArrays;
};


/// retrieves the global name from the AST generated by clang
class ASTGlobalPrinter : public GlobalPrinter
{
public:

	ASTGlobalPrinter(clang::ASTContext* astContext, std::map<std::string, clang::VarDecl*>& decls,
		llvm::TargetData* targetData, StringRef name = StringRef());
	virtual ~ASTGlobalPrinter();
	
	virtual void printForStore(CodeStream& w, llvm::Value* value, llvm::Value* global,
		std::vector<llvm::Value*>& indices, PrintWalker& printWalker);
	virtual void printForLoad(CodeStream& w, llvm::Type* type, llvm::Value* global,
		std::vector<llvm::Value*>& indices, PrintWalker& printWalker, int priority);

	llvm::Type* handleVariable(CodeStream& w, const clang::Type* type, llvm::Type* llvmType,
		std::vector<llvm::Value*>::const_iterator& indexIt, std::vector<llvm::Value*>::const_iterator indexEnd,
			PrintWalker& printWalker);


	// ast context for accessing the original structures of the global variables
	clang::ASTContext* astContext;

	// map global variables from name to declaration
	std::map<std::string, clang::VarDecl*>& decls;
	
	// target data for getting llvm::StructLayout
	llvm::TargetData* targetData;
	
	// output name of global variable (use original if empty)
	std::string name;
};

/*
/// like ASTGlobalPrinter, but keeps track of which fields of a global were read
class TrackingASTGlobalPrinter : public ASTGlobalPrinter
{
public:

	TrackingASTGlobalPrinter(clang::ASTContext* astContext, std::map<std::string, clang::VarDecl*>& decls,
		llvm::TargetData* targetData, std::set<std::string>& fields);
	virtual ~TrackingASTGlobalPrinter();

	virtual bool printForLoad(llvm::Type* type, llvm::Value* global, std::vector<llvm::Value*>& indices,
		PrintWalker& printWalker, CodeStream& w);

	// set of accessed members (e.g. "position")
	std::set<std::string>& fields;
};
*/

/// replaces structure members by variables and stores mapping. uses names "_0", "_1" and so on.
/// (used for vertex attibutes of vertex shader)
class VertexGlobalPrinter : public ASTGlobalPrinter
{
public:

	/// constructor. note: ensure that the references passed to the constructor live longer than this object
	VertexGlobalPrinter(clang::ASTContext* astContext, std::map<std::string, clang::VarDecl*>& decls,
		llvm::TargetData* targetData, StringRef prefix = "_");
	virtual ~VertexGlobalPrinter();

	virtual void printForStore(CodeStream& w, llvm::Value* value, llvm::Value* global,
		std::vector<llvm::Value*>& indices, PrintWalker& printWalker);
	virtual void printForLoad(CodeStream& w, llvm::Type* type, llvm::Value* global,
		std::vector<llvm::Value*>& indices, PrintWalker& printWalker, int priority);

	void printVariables(CodeStream& w, Precision defaultPrecision);


	// assignment from vertex struct to shader attribute (e.g. "position" -> "_0")
	typedef std::map<std::string, ShaderVariable> VertexBindingMap;
	VertexBindingMap vertexBindings;
	typedef std::pair<const std::string, ShaderVariable> VertexBindingPair;
};

/// replaces structure members by array indices (used for named buffers of e.g. animations)
class AST2ArrayGlobalPrinter : public ASTGlobalPrinter
{
public:

	AST2ArrayGlobalPrinter(clang::ASTContext* astContext, std::map<std::string, clang::VarDecl*>& decls,
		llvm::TargetData* targetData, StringRef name, const std::map<std::string, int>& arrayElements);
	virtual ~AST2ArrayGlobalPrinter();
	
	virtual void printForStore(CodeStream& w, llvm::Value* value, llvm::Value* global,
		std::vector<llvm::Value*>& indices, PrintWalker& printWalker);
	virtual void printForLoad(CodeStream& w, llvm::Type* type, llvm::Value* global,
		std::vector<llvm::Value*>& indices, PrintWalker& printWalker, int priority);

	const std::map<std::string, int>& arrayElements;
};

/// @}

} // namespace digi

#endif
