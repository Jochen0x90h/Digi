#ifndef digi_SceneConvert_PrintHelper_h
#define digi_SceneConvert_PrintHelper_h

#include <string>

#include <llvm/Value.h>

#include <digi/CodeGenerator/CodeWriter.h>
#include <digi/CodeGenerator/TypeInfo.h>

#include "ShaderOptions.h"
#include "ShaderType.h"


namespace digi {

/// @addtogroup SceneConvert
/// @{

enum InitializerType
{
	// construct vector from scalar, e.g. "splat3(1)" or "vec2(1)"
	INIT_SCALAR,
	
	// construct vector from scalar list, e.g. "vector3(1, 2, 3)" or "vec2(1, 2)"
	INIT_SCALAR_LIST,
	
	// construct vector from vector of other type, e.g. "convert_float3(i3)" or "vec3(i3)"
	INIT_VECTOR
};

// print constructor (e.g. "vector3" or "ivec2")
void printConstructor(CodeStream& w, const ShaderType& shaderType, InitializerType initializerType);

inline void printConstructor(CodeStream& w, llvm::Type* type, InitializerType initializerType)
{
	printConstructor(w, getShaderType(type), initializerType);
}


// print scope with trailing space (e.g. "uniform ")
void printScope(CodeStream& w, ShaderScope scope);

// print header (e.g. version, default precision for ESSL)
void printHeader(CodeStream& w, Precision defaultPrecision);


// print a shader variable (scalar, vector, array of scalar/vector or texture)
void printShaderVariable(CodeStream& w, const ShaderVariable& variable, Precision defaultPrecision = PRECISION_HIGH);

// print a shader variable
void printShaderVariable(CodeStream& w, ShaderScope scope, const ShaderVariable& variable,
	Precision defaultPrecision = PRECISION_HIGH);

// print array of shader variables
void printShaderVariables(CodeStream& w, ShaderScope scope, const std::vector<ShaderVariable>& variables,
	Precision defaultPrecision = PRECISION_HIGH);

inline void printShaderVariables(CodeStream& w, const std::vector<ShaderVariable>& variables,
	Precision defaultPrecision = PRECISION_HIGH)
{
	printShaderVariables(w, SCOPE_NONE, variables, defaultPrecision);
}

// print struct type with shader variables for binding code outside of shader
void printShaderStruct(CodeWriter& w, StringRef name, const std::vector<ShaderVariable>& variables);

// print struct variable with shader variables for binding code outside of shader
void printShaderVariable(CodeWriter& w, StringRef name, const std::vector<ShaderVariable>& variables);


// operator priorities
enum Priority
{
	PRIO_NONE        =  0,
	PRIO_ASSIGN      =  1, // x = y
	PRIO_CONDITIONAL =  2, // c ? x : y
	PRIO_OR          =  5, // x | y
	PRIO_XOR         =  6, // x ^ y
	PRIO_AND         =  7, // x & y
	PRIO_EQUALS      =  8, // x == y, x != y
	PRIO_LESS        =  9, // x < y, x > y, x <= y, x >= y
	PRIO_SHIFT       = 10, // x << y, x >> y
	PRIO_ADD         = 11, // x + y	
	PRIO_MUL         = 13, // x * y
	PRIO_UNARY       = 15, // !x
	PRIO_CAST        = 15, // (x)y
	PRIO_POSTFIX     = 16, // x++
	PRIO_MEMBER      = 17, // x.y
};


// print zero value
void printZero(CodeStream& w, llvm::Type* type);

// print constant value. returns true if a vector was printed. 
void printConstant(CodeStream& w, llvm::Value* value, Precision precision, Precision defaultPrecision);


// operator << for streaming llvm::StringRef into std::ostream
inline std::ostream& operator <<(std::ostream& w, llvm::StringRef str)
{
	w.write(str.data(), str.size());
	return w;
}

// operator << for streaming llvm::StringRef into CodeStream
inline CodeStream& operator <<(CodeStream& w, llvm::StringRef str)
{
	w.writeString(str.data(), str.size());
	return w;
}

/// @}

} // namespace digi

#endif
