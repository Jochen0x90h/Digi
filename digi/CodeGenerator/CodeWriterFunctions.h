#ifndef digi_CodeWriter_CodeWriterFunctions_h
#define digi_CodeWriter_CodeWriterFunctions_h

#include <string>

#include "CodeWriter.h"
#include "Operators.h"

namespace digi {

/// @addtogroup CodeGenerator
/// @{


struct Code
{
	std::string str;
	StringRefCodeStream s;

	Code(Language language = Language::CPP)
		: s(str, language) {}

	template <typename Type>
	Code& operator <<(const Type& value)
	{
		this->s << value;
		return *this;
	}
	
	operator std::string() const
	{
		return this->str;
	}
	operator StringRef() const
	{
		return this->str;
	}
};


/// get type name from type
template <typename Type>
struct TypeName
{
};

template <>
struct TypeName<bool>
{
	static const char* get() {return "bool";}
};

template <>
struct TypeName<char>
{
	static const char* get() {return "char";}
};

template <>
struct TypeName<int8_t>
{
	static const char* get() {return "byte";}
};

template <>
struct TypeName<uint8_t>
{
	static const char* get() {return "ubyte";}
};

template <>
struct TypeName<int16_t>
{
	static const char* get() {return "short";}
};

template <>
struct TypeName<uint16_t>
{
	static const char* get() {return "ushort";}
};

template <>
struct TypeName<int32_t>
{
	static const char* get() {return "int";}
};

template <>
struct TypeName<uint32_t>
{
	static const char* get() {return "uint";}
};

template <>
struct TypeName<float>
{
	static const char* get() {return "float";}
};
/*
template <typename Type>
struct TypeName<Vector2<Type> >
{
	static std::string get() {return std::string(TypeName<Type>::get()) + '2';}
};

template <typename Type>
struct TypeName<Vector3<Type> >
{
	static std::string get() {return std::string(TypeName<Type>::get()) + '3';}
};

template <typename Type>
struct TypeName<Vector4<Type> >
{
	static std::string get() {return std::string(TypeName<Type>::get()) + '4';}
};
*/

template <>
struct TypeName<float2>
{
	static const char* get() {return "float2";}
};

template <>
struct TypeName<float3>
{
	static const char* get() {return "float3";}
};

template <>
struct TypeName<float4>
{
	static const char* get() {return "float4";}
};

template <>
struct TypeName<float4x4>
{
	static const char* get() {return "float4x4";}
};


/*
	convet integer to identifier (e.g. 0 = "a", 53 = "ab")
	the first character is a-z and A-Z,
	the other characters are a-z, A-Z and 0-9.
*/
char* convertIntToIdentifier(char* str, int v);


/// create a variable name from a string (e.g. a file path) using escape sequences
std::string createVariableName(const std::string& str);


/// @}

} // namespace digi

#endif
