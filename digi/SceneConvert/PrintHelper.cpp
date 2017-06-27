#include <llvm/DerivedTypes.h>
#include <llvm/Constants.h>

#include <digi/Utility/foreach.h>
#include <digi/System/Log.h>

#include "PrintHelper.h"


namespace digi {


void printConstructor(CodeStream& w, const ShaderType& shaderType, InitializerType initializerType)
{
	Language language = w.getLanguage();
	 
	if (shaderType.numRows > 1 && (language.isCPP() || language.isGLSL()))
	{
		if (language.isCPP())
		{
			if (initializerType == INIT_SCALAR_LIST)
				w << "vector";
			else if (initializerType == INIT_SCALAR)
				w << "splat";
			else
			{
				w << "convert_";
				if (shaderType.type == ShaderType::INT)
					w << "int";
				else
					w << "float";		
			}
		}
		else
		{
			if (shaderType.type == ShaderType::INT)
				w << 'i';
			w << "vec";		
		}
	}
	else
	{
		if (shaderType.type == ShaderType::INT)
			w << "int";
		else
			w << "float";		
	}	

	if (shaderType.numRows > 1)
		w << ident(shaderType.numRows);
}

void printScope(CodeStream& w, ShaderScope scope)
{
	Language language = w.getLanguage();
	if (language.isOldGLSL())
	{
		switch (scope)
		{
			case SCOPE_UNIFORM:
				w << "uniform ";
				break;
			case SCOPE_ATTRIBUTE:
				w << "attribute ";
				break;
			case SCOPE_IN:
			case SCOPE_OUT:
				w << "varying ";
				break;
			default:
				// no shader scope, e.g. external binding code
				break;
		}
	}
	else if (language.isGLSL())
	{
		switch (scope)
		{
			case SCOPE_UNIFORM:
				w << "uniform ";
				break;
			case SCOPE_ATTRIBUTE:
			case SCOPE_IN:
				w << "in ";				
				break;
			case SCOPE_OUT:
				w << "out ";				
				break;
			default:
				// no shader scope, e.g. external binding code
				break;
		}
	}
}

void printHeader(CodeStream& w, Precision precision)
{
	Language language = w.getLanguage();
	
	if (language == Language::GLSL_1_2)
	{
		w << "#version 120\n";
	}
	else if (language == Language::GLSL_1_5)
	{
		w << "#version 150\n";
	}
	else if (language == Language::ESSL)
	{
		w << "precision ";
		switch (precision)
		{
		case PRECISION_LOW:
			w << "lowp";
			break;
		case PRECISION_MEDIUM:
			w << "mediump";
			break;
		case PRECISION_HIGH:
			w << "highp";
			break;
		}		
		w << " float;\n";
	}
}

struct ObjectTypeInfo
{
	const char* name;
	
	// name for GLSL output
	const char* glslName;
};


// for shader code
void printShaderVariable(CodeStream& w, const ShaderVariable& variable, Precision defaultPrecision)
{
	// precision modifier
/*
	Language language = w.getLanguage();
	
	if (variable.precision != defaultPrecision && language == Language::ESSL)
	{
		switch (precision)
		{
		case PRECISION_LOW:
			w << "lowp ";
			break;
		case PRECISION_MEDIUM:
			w << "mediump ";
			break;
		case PRECISION_HIGH:
			w << "highp ";
			break;
		}
	}	
*/
	static const ObjectTypeInfo objectTypeInfos[] =
	{
		{"Texture2D",   "sampler2D"},
		{"Texture3D",   "sampler3D"},
		{"TextureCube", "samplerCube"},
	};

	Language language = w.getLanguage();

	if (language.isJS())
	{
		w << "var " << variable.name;
	}
	else
	{
		if (variable.type.type != ShaderType::TEXTURE)
		{
			// scalar or vector
			if (variable.type.numRows > 1 && language.isGLSL())
			{
				if (variable.type.type == ShaderType::BOOL)
					w << 'b';
				else if (variable.type.type == ShaderType::INT)
					w << 'i';
				w << "vec";
			}
			else
			{
				if (variable.type.type == ShaderType::BOOL)
					w << "bool";
				else if (variable.type.type == ShaderType::INT)
					w << "int";
				else
					w << "float";	
			}

			if (variable.type.numRows > 1)
				w << ident(variable.type.numRows);
		}
		else
		{
			// texture
			if (language.isGLSL())
				w << objectTypeInfos[variable.type.numRows - 2].glslName;
			else
				w << objectTypeInfos[variable.type.numRows - 2].name;
		}
		
		// variable name
		w << ' ' << variable.name;
		
		// array modifier (e.g. [5])
		if (variable.type.numElements > 0)
			w << '[' << variable.type.numElements << ']';
	}
}

void printShaderVariable(CodeStream& w, ShaderScope scope, const ShaderVariable& variable,  Precision defaultPrecision)
{
	// print shader scope (e.g. "varying ")
	printScope(w, scope);
	
	// print parameter (e.g. "vec4 a[5]" or "sampler2D")
	printShaderVariable(w, variable, defaultPrecision);
	w << ";\n";		
}

void printShaderVariables(CodeStream& w, ShaderScope scope, const std::vector<ShaderVariable>& variables,  Precision defaultPrecision)
{
	foreach (const ShaderVariable& variable, variables)
	{
		printShaderVariable(w, scope, variable, defaultPrecision);
	}
}

void printShaderStruct(CodeWriter& w, StringRef name, const std::vector<ShaderVariable>& variables)
{
	w.beginStruct(name);
	printShaderVariables(w, variables);
	w.endStruct();
}

void printShaderVariable(CodeWriter& w, StringRef name, const std::vector<ShaderVariable>& variables)
{
	w.beginStruct();
	printShaderVariables(w, variables);
	w.endStruct(name);
}


// print zero value
void printZero(CodeStream& w, llvm::Type* type)
{
	if (type->isIntegerTy())
		w << 0;
	else if (type->isFloatTy())
		w << 0.0f;
	else
		w << 0.0;
}

void printConstant(CodeStream& w, llvm::Value* value, Precision precision, Precision defaultPrecision)
{
	if (llvm::ConstantInt* constantInt = llvm::dyn_cast<llvm::ConstantInt>(value))
	{
		// constant int
		llvm::SmallVector<char, 16> number;
		constantInt->getValue().toString(number, 10, true);

		w.writeString(number.begin(), number.size());
	}	
	else if (llvm::ConstantFP* constantFP = llvm::dyn_cast<llvm::ConstantFP>(value))
	{
		// constant float
		llvm::APFloat f = constantFP->getValueAPF();
		bool reversible;
		f.convert(llvm::APFloat::IEEEdouble, llvm::APFloat::rmNearestTiesToEven, &reversible);

		double number = f.convertToDouble();
		if (constantFP->getType()->isFloatTy())
			w << float(number);
		else
			w << number;
	}
	else if (llvm::ConstantVector* constantVector = llvm::dyn_cast<llvm::ConstantVector>(value))
	{
		// constant vector

		// check for splat value
		if (llvm::Constant* splatValue = constantVector->getSplatValue())
		{
			// print as scalar (isVector() would return false)
			printConstant(w, splatValue, precision, defaultPrecision);
		}
		else
		{
			// is a vector value
			printConstructor(w, constantVector->getType(), INIT_SCALAR_LIST);

			llvm::SmallVector<llvm::Constant*, 4> elements;
			constantVector->getVectorElements(elements);
			int numElements = elements.size();
			w << '(';
			for (int i = 0; i < numElements; ++i)
			{
				if (i != 0)
					w << ", ";
				printConstant(w, elements[i], precision, defaultPrecision);
			}
			w << ')';
		}		
	}
	else if (llvm::isa<llvm::ConstantAggregateZero>(value) || llvm::isa<llvm::UndefValue>(value)
		|| llvm::isa<llvm::ConstantExpr>(value)) //! treat ConstantExpr as zero for now
	{
		// constant zero (scalar or vector) or undef
		if (llvm::isa<llvm::UndefValue>(value))
		{
			dWarning("encountered an undefined value");
			w << "/*undef*/";
		}
		else if (llvm::isa<llvm::ConstantExpr>(value))
		{
			dWarning("encountered a constant expression");
			w << "/*const*/";
		}

		// get type
		llvm::Type* type = value->getType();
		if (type->isVectorTy())
		{
			const llvm::VectorType* vectorType = llvm::cast<llvm::VectorType>(type);
			type = vectorType->getElementType();
		}

		// print as scalar (isVector() would return false)
		printZero(w, type);
	}
	else if (llvm::isa<llvm::ConstantPointerNull>(value))
	{
		// null pointer, e.g. null texture
		w << "null";
	}
	else
	{	
		dWarning("encountered unknown constant value");
		w << "/*unknown*/";
	}
}

} // namespace digi
