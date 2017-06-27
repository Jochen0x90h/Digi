#ifndef digi_SceneConvert_ShaderType_h
#define digi_SceneConvert_ShaderType_h

#include <string>

#include <llvm/Type.h>


namespace digi {

/// @addtogroup SceneConvert
/// @{

enum ShaderScope
{
	SCOPE_NONE,
	SCOPE_UNIFORM,
	SCOPE_ATTRIBUTE,
	SCOPE_IN,
	SCOPE_OUT,
};

// basic type for shader, e.g. "int", "float3" or "texture2D"
struct ShaderType
{
	enum Type
	{
		BOOL,
		INT,
		FLOAT,
		TEXTURE
	};		

	Type type;
	
	// number of vector rows or texture dimensions (2 = 2D, 3 = 3D or 4 = Cube)
	int numRows;
	
	// number of array elements, 0 = no array
	int numElements;
	
	ShaderType()
		: type(), numRows(), numElements() {}

	ShaderType(Type type, int numRows, int numElements = 0)
		: type(type), numRows(numRows), numElements(numElements) {}

	explicit ShaderType(const std::string& type);
};

// shader variable, name and type
struct ShaderVariable
{
	std::string name;
	ShaderType type;
	
	ShaderVariable()
		{}
	
	ShaderVariable(const std::string& name, const ShaderType& type)
		: name(name), type(type) {}	

	ShaderVariable(const std::string& name, ShaderType::Type type, int numRows, int numElements = 0)
		: name(name), type(type, numRows, numElements) {}	
};


// get shader parameter type from llvm type
ShaderType getShaderType(llvm::Type* type);

/// @}

} // namespace digi

#endif
