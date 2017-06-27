#include <llvm/DerivedTypes.h>

#include <digi/CodeGenerator/TypeInfo.h>

#include "ValueHelper.h"
#include "ShaderType.h"


namespace digi {

// ShaderType

ShaderType::ShaderType(const std::string& type)
	: numElements(0)
{
	VectorInfo t(type);
	switch (t.type)
	{
	case VectorInfo::BOOL:
		this->type = BOOL;
		break;
	case VectorInfo::INT:
		this->type = INT;
		break;
	case VectorInfo::FLOAT:
		this->type = FLOAT;
		break;
	}
	this->numRows = t.numRows;
}


ShaderType getShaderType(llvm::Type* type)
{
	// check for array type
	int numElements = 0;
	if (type->isArrayTy())
	{
		const llvm::ArrayType* arrayType = llvm::cast<llvm::ArrayType>(type);
		numElements = int(arrayType->getNumElements());
		type = arrayType->getElementType();
	}

	// check for vector type
	int numRows = 1;
	if (type->isVectorTy())
	{
		const llvm::VectorType* vectorType = llvm::cast<llvm::VectorType>(type);
		numRows = vectorType->getNumElements();
		type = vectorType->getElementType();
	}
	
	if (type->isIntegerTy())
	{
		return ShaderType(isBool(type) ? ShaderType::BOOL : ShaderType::INT, numRows, numElements);
	}
	if (type->isFloatingPointTy())
	{
		return ShaderType(ShaderType::FLOAT, numRows, numElements);
	}
	
	if (const llvm::PointerType* pointerType = llvm::dyn_cast<llvm::PointerType>(type))
	{
		// object types are modeled by address spaces
		// 2: 2D texture
		// 3: 3D texture
		// 4: cube texture
		int t = pointerType->getAddressSpace();
		if (t >= 2 && t <= 4)
		{
			return ShaderType(ShaderType::TEXTURE, t);
		}
	}

	return ShaderType();
}

} // namespace digi
