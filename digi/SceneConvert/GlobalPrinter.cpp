#include <llvm/Target/TargetData.h> // StructLayout

#include <clang/AST/RecordLayout.h>

#include <digi/Utility/foreach.h>
#include <digi/Utility/lexicalCast.h>
#include <digi/Utility/MapUtility.h>
#include <digi/Utility/VectorUtility.h>
#include <digi/System/Log.h>

#include "PrintWalker.h"
#include "GlobalPrinter.h"


namespace digi {


// GlobalPrinter
	
GlobalPrinter::~GlobalPrinter()
{
}

void GlobalPrinter::getVariables(std::vector<ShaderVariable>& variables)
{
}


// StandardGlobalPrinter

StandardGlobalPrinter::StandardGlobalPrinter(const std::vector<ShaderType>& types,
	StringRef storePrefix, StringRef loadPrefix, NameGenerator& nameGenerator)
	: variables(types.size()), storePrefix(storePrefix), loadPrefix(loadPrefix)
{
	size_t numVariables = types.size();
	for (size_t i = 0; i < numVariables; ++i)
	{
		this->variables[i].name = '_' + nameGenerator.get();
		this->variables[i].type = types[i];
	}
}

StandardGlobalPrinter::~StandardGlobalPrinter()
{
}

// print array index for array variables
static void printIndex(CodeStream& w, const ShaderVariable& variable,
	const std::vector<llvm::Value*>& indices, PrintWalker& printWalker)
{
	if (variable.type.numElements > 0 && indices.size() >= 2)
	{
		// array variabe
		w << '[';
		int index = getConstantIndex(indices[1]);
		if (index == -1)
		{
			// index is variable
			printWalker.printValue(w, indices[1], PRIO_ADD);
		}
		else
		{
			// index is constant
			w << index;
		}
		w << ']';
	}
	else
	{
		// scalar or vector variable
	}
}

void StandardGlobalPrinter::printForStore(CodeStream& w, llvm::Value* value, llvm::Value* global,
	std::vector<llvm::Value*>& indices, PrintWalker& printWalker)
{
	bool vectorized = w.getLanguage().supportsVector();

	int index = getConstantIndex(indices[0]);
	ShaderVariable& variable = this->variables[index];
	
	// print
	w << this->storePrefix << variable.name;
	printIndex(w, variable, indices, printWalker);
	w << " = ";
	printWalker.printValue(w, value, PRIO_ASSIGN);
	w << ";\n";
}

void StandardGlobalPrinter::printForLoad(CodeStream& w, llvm::Type*, llvm::Value* global,
	std::vector<llvm::Value*>& indices, PrintWalker& printWalker, int priority)
{
	int index = getConstantIndex(indices[0]);
	ShaderVariable& variable = this->variables[index];

	// print
	w << this->loadPrefix << variable.name;
	printIndex(w, variable, indices, printWalker);
}

void StandardGlobalPrinter::getVariables(std::vector<ShaderVariable>& variables)
{
	add(variables, this->variables);
}


// VariableGlobalPrinter

/// global printer that generates individual scalar variables
VariableGlobalPrinter::VariableGlobalPrinter(const std::vector<ShaderType>& types,
	StringRef storePrefix, StringRef loadPrefix, NameGenerator& nameGenerator)
	: variables(types.size()), storePrefix(storePrefix), loadPrefix(loadPrefix)
{
	size_t numVariables = types.size();
	for (size_t i = 0; i < numVariables; ++i)
	{
		const ShaderType& type = types[i];
		this->variables[i].type = type;
		this->variables[i].nameGenerator = nameGenerator;

		nameGenerator.next(type.numRows);
	}
}

VariableGlobalPrinter::~VariableGlobalPrinter()
{
}

void VariableGlobalPrinter::printForStore(CodeStream& w, llvm::Value* value, llvm::Value* global,
	std::vector<llvm::Value*>& indices, PrintWalker& printWalker)
{
	bool vectorized = w.getLanguage().supportsVector();

	int index = getConstantIndex(indices[0]);
	VariableInfo& variableInfo = this->variables[index];

	NameGenerator nameGenerator = variableInfo.nameGenerator;
	if (indices.size() >= 2)
	{
		int index = getConstantIndex(indices[1]);
		nameGenerator.next(index);
	}

	// print
	printShaderVariable(w, ShaderVariable(this->storePrefix + nameGenerator.getCurrent(), variableInfo.type));
	w << " = ";
	printWalker.printValue(w, value, PRIO_ASSIGN);
	w << ";\n";
}

void VariableGlobalPrinter::printForLoad(CodeStream& w, llvm::Type*, llvm::Value* global,
	std::vector<llvm::Value*>& indices, PrintWalker& printWalker, int priority)
{
	int index = getConstantIndex(indices[0]);
	VariableInfo& variableInfo = this->variables[index];

	NameGenerator nameGenerator = variableInfo.nameGenerator;
	if (indices.size() >= 2)
	{
		int index = getConstantIndex(indices[1]);
		for (int i = 0; i < index; ++i)
			nameGenerator.next();
	}

	// print
	w << this->loadPrefix << nameGenerator.getCurrent();
}


// PackingGlobalPrinter

PackingGlobalPrinter::PackingGlobalPrinter(const std::vector<ShaderType>& types,
	StringRef storePrefix, StringRef loadPrefix, NameGenerator& nameGenerator)
	: variables(types.size()), storePrefix(storePrefix), loadPrefix(loadPrefix), nameGenerator(nameGenerator)
{
Precision precision = PRECISION_HIGH;
	size_t numVariables = types.size();
	for (size_t variableIndex = 0; variableIndex < numVariables; ++variableIndex)
	{
		ShaderType type = types[variableIndex];
		
		// store bool into float vector
		if (type.type == ShaderType::BOOL)
			type.type = ShaderType::FLOAT;

		Variable& variable = this->variables[variableIndex];

		// set type
		variable.type = type;

		// get number of rows (1, 2, 3 or 4)
		int numRows = variable.type.numRows;

		// get array of vectors of given base type (int or float) and precision
		std::vector<PackingVariable>& vectors = this->vectors[TypePair(variable.type.type, precision)];

		// find free space in array of 4-vectors
		size_t numVectors = vectors.size();
		size_t offset;
		int start;
		for (offset = 0; offset < numVectors; ++offset)
		{
			int& nr = vectors[offset].numRows;

			// check for free space in this vector
			if (4 - nr >= numRows)
			{
				start = nr;
				nr += numRows;
				break;
			}
		}

		if (offset == numVectors)
		{
			// no free space found: create new variable
			start = 0;
			vectors += PackingVariable('_' + this->nameGenerator.get(), numRows);
		}
		
		// offset and start contain the location where to insert the variable into the vectors
		variable.offset = offset;
		variable.start = start;
	}
}

PackingGlobalPrinter::~PackingGlobalPrinter()
{
}

void PackingGlobalPrinter::printForStore(CodeStream& w, llvm::Value* value, llvm::Value* global,
	std::vector<llvm::Value*>& indices, PrintWalker& printWalker)
{
Precision precision = PRECISION_HIGH;

	// get variable
	int variableIndex = getConstantIndex(indices[0]);	
	Variable& variable = this->variables[variableIndex];

	// get number of rows (1, 2, 3 or 4)
	int numRows = variable.type.numRows;

	// get array of vectors of given base type (int or float) and precision
	PackingVariable& vector = this->vectors[TypePair(variable.type.type, precision)][variable.offset];

	// check if bool has to be converted to float
	bool b = !w.getLanguage().isJS() && isBool(value->getType());

	// print (e.g. "_m.yzw")
	w << this->storePrefix << vector.name;
	
	// check if variable uses vector alone. if not, add a swizzle
	if (numRows != vector.numRows)
		w << '.' << substring("xyzw", variable.start, variable.start + numRows);
	w << " = ";
	printWalker.printValue(w, value, b ? PRIO_CONDITIONAL : PRIO_ASSIGN);
	
	if (b)
		w << " ? " << 1.0f << " : " << 0.0f;

	w << ";\n";
}

void PackingGlobalPrinter::printForLoad(CodeStream& w, llvm::Type* type, llvm::Value* global,
	std::vector<llvm::Value*>& indices, PrintWalker& printWalker, int priority)
{
Precision precision = PRECISION_HIGH;

	// get variable
	int variableIndex = getConstantIndex(indices[0]);
	Variable& variable = this->variables[variableIndex];

	// get number of rows (1, 2, 3 or 4)
	int numRows = variable.type.numRows;

	// get vector for given base type (int or float), precision and offset
	PackingVariable& vector = this->vectors[TypePair(variable.type.type, precision)][variable.offset];

	bool b = isBool(type);
	bool p = priority > PRIO_EQUALS;
	if (b && p)
		w << '(';
	
	// print (e.g. "_m.yzw")
	w << this->loadPrefix << vector.name;

	// check if variable uses vector alone. if not, add a swizzle
	if (numRows != vector.numRows)
		w << '.' << substring("xyzw", variable.start, variable.start + numRows);

	if (b)
	{
		w << " != " << 0.0f;
		if (p)
			w << ')';
	}
}

void PackingGlobalPrinter::getVariables(std::vector<ShaderVariable>& variables)
{
	// iterate over vectors
	foreach (VariablePair& variablePair, this->vectors)
	{
		ShaderType::Type type = variablePair.first.first;
		Precision precision = variablePair.first.second;
		
		// iterate over variables
		foreach (const PackingVariable& variable, variablePair.second)
		{
			variables += ShaderVariable(variable.name, type, variable.numRows, 0);
		}
	}
}


// ArrayPackingGlobalPrinter

ArrayPackingGlobalPrinter::ArrayPackingGlobalPrinter(const std::vector<ShaderType>& types,
	StringRef storePrefix, StringRef loadPrefix, int& maxArraySize, NameGenerator& nameGenerator)
	: variables(types.size()), storePrefix(storePrefix), loadPrefix(loadPrefix)
{
Precision precision = PRECISION_HIGH;
	
	size_t numVariables = types.size();
	for (size_t variableIndex = 0; variableIndex < numVariables; ++variableIndex)
	{
		ShaderType type = types[variableIndex];
		
		// store bool into float array
		if (type.type == ShaderType::BOOL)
			type.type = ShaderType::FLOAT;

		Variable& variable = this->variables[variableIndex];
		variable.type = type;

		// arrange scalars and vectors in arrays
		if (type.numElements == 0)
		{
			// variable is scalar or vector: allocate immediately
			
			// get array
			PackingArray& array = this->arrays[TypePair(variable.type.type, precision)];
		
			// allocate variable in array (also generate name if array used the first time).
			// note that maxArraySize is a reference so that multiple arrays can share one limited pool (e.g. shader uniforms)
			maxArraySize -= ArrayPackingGlobalPrinter::allocate(variable, maxArraySize, array, nameGenerator);
		}
		else
		{
			// variable is an array: allocation is deferred until allocateArrays() gets called
		}
	}
}

void ArrayPackingGlobalPrinter::allocateArrays(int& maxArraySize, NameGenerator& nameGenerator)
{
Precision precision = PRECISION_HIGH;

	// arrange array variables in arrays or large arrays
	foreach (Variable& variable, this->variables)
	{
		if (variable.type.numElements > 0)
		{
			// get array
			PackingArray& array = this->arrays[TypePair(variable.type.type, precision)];

			// allocate variable in array
			int numNewElements = ArrayPackingGlobalPrinter::allocate(variable, maxArraySize, array, nameGenerator);
			if (numNewElements >= 0)
			{
				// variable fitted into array
				maxArraySize -= numNewElements;
			}
			else
			{
				// allocate variable in large array (texture)
				variable.isLargeArray = true;
				PackingArray& array = this->largeArrays[TypePair(variable.type.type, precision)];
				ArrayPackingGlobalPrinter::allocate(variable, 0xffffff, array, nameGenerator);
			}
		}
	}
}

int ArrayPackingGlobalPrinter::allocate(Variable& variable, int maxNewElements, PackingArray& array,
	NameGenerator& nameGenerator)
{
	// number of array elements (1 for scalar/vector)
	int numElements = max(variable.type.numElements, 1);

	// vector size (1 - 4)
	int dimension = variable.type.numRows;
	
	// mask that indicates which of the 4 vector components are used by this variable
	int mask;

	// number of elements that are allocated so far
	size_t numFlags = array.flags.size();
	int counters[4] = {};
	
	// offset in array
	int offset = 0;
	
	// position of first component (0 - 3)
	int start = 0;
	
	int maxCount = 0;
	int numNewElements = 0;
	for (size_t index = 0; index < numFlags; ++index)
	{
		uint8_t& flags = array.flags[index];

		// check which components are available
		uint8_t countFlags = 0;
		for (int i = 0; i < 4; ++i)
		{
			if ((flags & (1 << i)) == 0)
			{
				// component is available: increment counter
				++counters[i];
				if (counters[i] >= numElements)
					countFlags |= 1 << i;
			}
			else
			{
				// component is not available: reset counter
				counters[i] = 0;
			}
		}
		
		mask = ~(~0 << dimension);
		for (int i = 0; i <= 4 - dimension; ++i)
		{
			if ((countFlags & mask) == mask)
			{
				// found free space in existing array elements
				offset = int(index) + 1 - numElements;
				start = i;
				goto found;
			}
			mask <<= 1;
		}
	}
	
	// no free space found: add new array elements
	for (int j = 0; j <= 4 - dimension; ++j)
	{
		int minCount = 0xffffff;
		for (int i = 0; i < dimension; ++i)
		{
			minCount = min(counters[j + i], minCount);
		}
		if (minCount > maxCount)
		{
			maxCount = minCount;
			start = j;
		}
	}	
	offset = numFlags - maxCount;
	numNewElements = numElements - maxCount;
	
	if (numNewElements > maxNewElements)
		return -1;
	
	mask = ~(~0 << dimension) << start;
	array.flags.insert(array.flags.end(), numNewElements, 0);

found:			

	// set flags to indicate that the elements are occupied
	for (int i = 0; i < numElements; ++i)
	{
		array.flags[offset + i] |= mask;
	}

	variable.offset = offset;
	variable.start = start;

	// create name if new array
	if (array.name.empty())
		array.name = '_' + nameGenerator.get();
	
	return numNewElements;
}

ArrayPackingGlobalPrinter::~ArrayPackingGlobalPrinter()
{
}

void ArrayPackingGlobalPrinter::printIndexAndSwizzle(CodeStream& w, const PackingArray& array, const Variable& variable,
	const std::vector<llvm::Value*>& indices, PrintWalker& printWalker)
{
	// see if this is a vectorized language (e.g. glsl) or not (e.g. javascript)
	bool vectorized = w.getLanguage().supportsVector();
	
	// offset of variable in array
	int variableOffset = variable.offset;
	if (!vectorized)
	{
		variableOffset = variableOffset * 4 + variable.start;
		if (variable.type.numRows > 1)
		{
			// is a vector: last index indexes vector element (x, y, z or w)
			variableOffset += getConstantIndex(indices.back());
		}
	}
			
	// check if array with more than one element
	if (array.flags.size() > 1 || !vectorized)
	{
		// get access index if array variable
		int index = 0;
		if (variable.type.numElements > 0 && indices.size() >= 2)
			index = getConstantIndex(indices[1]);
		
		w << '[';
		if (index == -1)
		{
			// variable index
			if (variableOffset != 0)
				w << variableOffset << " + ";
			if (!vectorized)
			{
				printWalker.printValue(w, indices[1], PRIO_MUL);
				w << " * 4";
			}
			else
			{
				printWalker.printValue(w, indices[1], PRIO_ADD);
			}
		}
		else
		{
			// constant index
			if (!vectorized)
				index *= 4;
			w << variableOffset + index;
		}
		w << ']';
	}
	if (vectorized && variable.type.numRows < 4)
		w << '.' << substring("xyzw", variable.start, variable.start + variable.type.numRows);
}

void ArrayPackingGlobalPrinter::printSampleAndSwizzle(CodeStream& w,
	const std::string& loadPrefix, const PackingArray& array, const Variable& variable,
	const std::vector<llvm::Value*>& indices, PrintWalker& printWalker)
{
	// width of texture
	size_t textureWidth = array.flags.size();

	// offset of variable in array
	int variableOffset = variable.offset;
	
	w << "texture2D(" << loadPrefix << array.name << ", vec2(";
		
	// get access index if array variable
	int index = 0;
	if (variable.type.numElements > 0 && indices.size() >= 2)
		index = getConstantIndex(indices[1]);
	
	if (index == -1)
	{
		// variable index
		w << "(" << float(variableOffset) + 0.5f << " + float(";
		printWalker.printValue(w, indices[1], PRIO_MUL);
			
		// scale for texture width
		w << ")) * " << 1.0f / float(textureWidth);
	}
	else
	{
		// no index or constant index
		w << (float(variableOffset + index) + 0.5f) / float(textureWidth);
	}

	w << ", " << 0.5f << "))";
	if (variable.type.numRows < 4)
		w << "." << substring("xyzw", variable.start, variable.start + variable.type.numRows);
}

void ArrayPackingGlobalPrinter::printForStore(CodeStream& w, llvm::Value* value, llvm::Value* global,
	std::vector<llvm::Value*>& indices, PrintWalker& printWalker)
{
Precision precision = PRECISION_HIGH;
	Language language = w.getLanguage();

	int variableIndex = getConstantIndex(indices[0]);
	Variable& variable = this->variables[variableIndex];

	// get array
	PackingArray& array = !variable.isLargeArray ? this->arrays[TypePair(variable.type.type, precision)]
		: this->largeArrays[TypePair(variable.type.type, precision)];

	// check if bool has to be converted to float
	bool b = !language.isJS() && isBool(value->getType());

	// print (e.g. "_m[2].yzw")
	if (!variable.isLargeArray)
		w << this->storePrefix;
	w << array.name;
	printIndexAndSwizzle(w, array, variable, indices, printWalker);
	w << " = ";
	printWalker.printValue(w, value, b ? PRIO_CONDITIONAL : PRIO_ASSIGN);

	if (b)
		w << " ? " << 1.0f << " : " << 0.0f;

	w << ";\n";
}

void ArrayPackingGlobalPrinter::printForLoad(CodeStream& w, llvm::Type* type, llvm::Value* global,
	std::vector<llvm::Value*>& indices, PrintWalker& printWalker, int priority)
{
Precision precision = PRECISION_HIGH;

	int variableIndex = getConstantIndex(indices[0]);
	Variable& variable = this->variables[variableIndex];

	// get array
	PackingArray& array = !variable.isLargeArray ? this->arrays[TypePair(variable.type.type, precision)]
		: this->largeArrays[TypePair(variable.type.type, precision)];
	
	bool b = isBool(type);
	bool p = priority > PRIO_EQUALS;
	if (b && p)
		w << '(';

	if (!variable.isLargeArray)
	{
		// print (e.g. "_m[2].yzw")
		w << this->loadPrefix << array.name;
		printIndexAndSwizzle(w, array, variable, indices, printWalker);
	}
	else
	{
		printSampleAndSwizzle(w, this->loadPrefix, array, variable, indices, printWalker);
	}

	if (b)
	{
		w << " != " << 0.0f;
		if (p)
			w << ')';
	}
}

void ArrayPackingGlobalPrinter::getVariables(std::vector<ShaderVariable>& variables)
{
	// add textures for large array variables
	foreach (ArrayPair& arrayPair, this->largeArrays)
	{
		const std::string& name = arrayPair.second.name;
		
		variables += ShaderVariable(name, ShaderType::TEXTURE, 2); // 2D texture
	}
	
	// add array variables
	foreach (ArrayPair& arrayPair, this->arrays)
	{
		const std::string& name = arrayPair.second.name;
		ShaderType::Type type = arrayPair.first.first;
		Precision precision = arrayPair.first.second;
		int numElements = int(arrayPair.second.flags.size());
		
		// arrays of length 1 are converted to a single vector
		if (numElements == 1)
			variables += ShaderVariable(name, type, 4, 0);
		if (numElements > 1)
			variables += ShaderVariable(name, type, 4, numElements);
	}
}

void ArrayPackingGlobalPrinter::getLargeArrays(std::vector<ShaderVariable>& variables)
{
	foreach (ArrayPair& arrayPair, this->largeArrays)
	{
		const std::string& name = arrayPair.second.name;
		ShaderType::Type type = arrayPair.first.first;
		Precision precision = arrayPair.first.second;
		int numElements = int(arrayPair.second.flags.size());

		variables += ShaderVariable(name, type, 4, numElements);
	}
}


// ASTGlobalPrinter

ASTGlobalPrinter::ASTGlobalPrinter(clang::ASTContext* astContext, std::map<std::string, clang::VarDecl*>& decls,
	llvm::TargetData* targetData, StringRef name)
	: astContext(astContext), decls(decls), targetData(targetData), name(name.data(), name.length())
{
}

ASTGlobalPrinter::~ASTGlobalPrinter()
{
}

void ASTGlobalPrinter::printForStore(CodeStream& w, llvm::Value* value, llvm::Value* global,
	std::vector<llvm::Value*>& indices, PrintWalker& printWalker)
{
	this->printForLoad(w, value->getType(), global, indices, printWalker, PRIO_ASSIGN);
	w << " = ";
	printWalker.printValue(w, value, PRIO_ASSIGN);
	w << ";\n";
}

void ASTGlobalPrinter::printForLoad(CodeStream& w, llvm::Type*, llvm::Value* global,
	std::vector<llvm::Value*>& indices, PrintWalker& printWalker, int priority)
{
	std::string name = global->getNameStr();

	// get the declaration and collector of the global variable by name
	clang::VarDecl* decl = getValue(this->decls, name);
	if (decl != NULL)
	{
		w << (this->name.empty() ? name : this->name);
		
		// type is always a pointer type, therefore we can cast to llvm::SequentialType
		const llvm::SequentialType* pointerType = llvm::cast<llvm::SequentialType>(global->getType());
		std::vector<llvm::Value*>::const_iterator indexIt = indices.begin();
		if (this->handleVariable(w, decl->getType().getTypePtr(), pointerType->getElementType(),
			indexIt, indices.end(), printWalker) == NULL)
		{
			// error: field not found
		}
	}
	else
	{	
		// error: global variable not found
	}
}

llvm::Type* ASTGlobalPrinter::handleVariable(CodeStream& w, const clang::Type* type, llvm::Type* llvmType,
	std::vector<llvm::Value*>::const_iterator& indexIt, std::vector<llvm::Value*>::const_iterator indexEnd, PrintWalker& printWalker)
{
	if (indexIt == indexEnd)
		return llvmType;
		
	// note: clang::QualType has an operator -> that results in clang::Type*
	if (const clang::RecordType* recordType = type->getAs<clang::RecordType>())
	{					
		// record type
		clang::RecordDecl* recordDecl = recordType->getDecl();
		llvm::StructType* llvmStruct = llvm::cast<llvm::StructType>(llvmType);

		// get offset of field in llvm type
		int llvmFieldIndex = getConstantIndex(*indexIt);
		const llvm::StructLayout* llvmLayout = this->targetData->getStructLayout(llvmStruct);		
		uint64_t llvmOffset = llvmLayout->getElementOffsetInBits(llvmFieldIndex);
		
		// find field in clang type by comparing the offset
		//const clang::ASTRecordLayout& layout = this->astContext->getASTRecordLayout(recordDecl);
		
		// iterate over fields
		clang::RecordDecl::field_iterator it = recordDecl->field_begin();
		clang::RecordDecl::field_iterator end = recordDecl->field_end();
		int fieldIndex = 0;
		for (; it != end; ++it, ++fieldIndex)
		{
//! when llvm module is scalarized, the offsets do not match for float3
			// compare field offset (in bits) of clang and llvm struct element
			//if (layout.getFieldOffset(fieldIndex) == llvmOffset)
			if (fieldIndex == llvmFieldIndex)
			{
				// found field
				clang::FieldDecl* fieldDecl = *it;
				llvm::StringRef fieldName = fieldDecl->getName();
				w << '.' << fieldName;
				++indexIt;
				return this->handleVariable(w, fieldDecl->getType().getTypePtr(), llvmStruct->getElementType(llvmFieldIndex),
					indexIt, indexEnd, printWalker);
			}
		}

		// error: field not found
		return NULL;
	}
	else if (const clang::Type* elementType = type->getArrayElementTypeNoTypeQual())
	{
		// array type
		const llvm::ArrayType* llvmArrayType = llvm::cast<llvm::ArrayType>(llvmType);

		w << '[';
		printWalker.printValue(w, *indexIt, PRIO_NONE);
		w << ']';
		++indexIt;
		return this->handleVariable(w, elementType, llvmArrayType->getElementType(),
			indexIt, indexEnd, printWalker);
	}
	else
	{
		// scalar or vector type
		return llvmType;
	}
}

/*
// TrackingASTGlobalPrinter

TrackingASTGlobalPrinter::TrackingASTGlobalPrinter(clang::ASTContext* astContext, std::map<std::string, clang::VarDecl*>& decls,
	llvm::TargetData* targetData, std::set<std::string>& fields)
	: ASTGlobalPrinter(astContext, decls, targetData), fields(fields)
{
}

TrackingASTGlobalPrinter::~TrackingASTGlobalPrinter()
{
}

bool TrackingASTGlobalPrinter::printForLoad(llvm::Type* type, llvm::Value* global, std::vector<llvm::Value*>& indices, PrintWalker& printWalker, CodeStream& w)
{
	// get name of global, e.g. "vertex"
	std::string name = global->getNameStr();

	// get the declaration and collector of the global variable by name
	clang::VarDecl* decl = getValue(this->decls, name);
	if (decl != NULL)
	{
		StringCodeStream temp;
		//tmp << name;
		
		// type is always a pointer type, therefore we can cast to llvm::CompositeType
		const llvm::CompositeType* pointerType = llvm::cast<llvm::CompositeType>(global->getType());
		llvm::Type* elementType = this->handleVariable(temp, decl->getType().getTypePtr(),
			pointerType->getTypeAtIndex(uint(0)), indices, 0, printWalker);
	
		// store member (e.g. "position")
		this->fields.insert(temp.getString().substr(1)); // remove leading '.'
	}

	// call inherited method
	return this->ASTGlobalPrinter::printForLoad(type, global, indices, printWalker, w);
}
*/

// VertexGlobalPrinter

VertexGlobalPrinter::VertexGlobalPrinter(clang::ASTContext* astContext, std::map<std::string, clang::VarDecl*>& decls,
	llvm::TargetData* targetData, StringRef prefix)
	: ASTGlobalPrinter(astContext, decls, targetData, prefix)
{
}

VertexGlobalPrinter::~VertexGlobalPrinter()
{
}

void VertexGlobalPrinter::printForStore(CodeStream& w, llvm::Value* value, llvm::Value* global,
	std::vector<llvm::Value*>& indices, PrintWalker& printWalker)
{	
	this->printForLoad(w, value->getType(), global, indices, printWalker, PRIO_ASSIGN);
	w << " = ";
	printWalker.printValue(w, value, PRIO_ASSIGN);
	w << ";\n";
}

void VertexGlobalPrinter::printForLoad(CodeStream& w, llvm::Type* type, llvm::Value* global,
	std::vector<llvm::Value*>& indices, PrintWalker& printWalker, int priority)
{
	// get name of global, e.g. "vertex"
	std::string name = global->getNameStr();

	// get the declaration and collector of the global variable by name
	clang::VarDecl* decl = getValue(this->decls, name);
	if (decl != NULL)
	{
		StringCodeStream temp;
		
		// type is always a pointer type, therefore we can cast to llvm::CompositeType
		llvm::CompositeType* pointerType = llvm::cast<llvm::CompositeType>(global->getType());
		std::vector<llvm::Value*>::const_iterator indexIt = indices.begin();
		llvm::Type* fieldType = this->handleVariable(temp, decl->getType().getTypePtr(),
			pointerType->getTypeAtIndex(uint(0)), indexIt, indices.end(), printWalker);
		if (fieldType != NULL)
		{
			// store field binding (e.g. "position" -> "_0")
			ShaderVariable& variable = this->vertexBindings[temp.getString().substr(1)]; // remove leading '.'
			if (variable.name.empty())
			{
				// use index of binding as name (e.g. "_0")
				variable.name = this->name + lexicalCast<std::string>(this->vertexBindings.size() - 1);
				variable.type = getShaderType(fieldType);
			}			
			w << variable.name;
			
			// check if a vector component is indexed (may happen if module is scalarized)
			if (fieldType->isArrayTy() && indexIt < indices.end())
			{
				int index = getConstantIndex(*indexIt);
				w << "xyzw"[index];
			}
		}
		else
		{
			// error: field not found
		}
	}
	else
	{
		// error: global variable not found
	}
}

void VertexGlobalPrinter::printVariables(CodeStream& w, Precision defaultPrecision)
{
	foreach (VertexBindingPair& p, this->vertexBindings)
	{
		printScope(w, SCOPE_ATTRIBUTE);
		printShaderVariable(w, p.second, defaultPrecision);
		w << ";\n";
	}
}

// AST2ArrayGlobalPrinter

AST2ArrayGlobalPrinter::AST2ArrayGlobalPrinter(clang::ASTContext* astContext, std::map<std::string, clang::VarDecl*>& decls,
	llvm::TargetData* targetData, StringRef name, const std::map<std::string, int>& arrayElements)
	: ASTGlobalPrinter(astContext, decls, targetData, name), arrayElements(arrayElements)
{
}

AST2ArrayGlobalPrinter::~AST2ArrayGlobalPrinter()
{
}

void AST2ArrayGlobalPrinter::printForStore(CodeStream& w, llvm::Value* value, llvm::Value* global,
	std::vector<llvm::Value*>& indices, PrintWalker& printWalker)
{	
	this->printForLoad(w, value->getType(), global, indices, printWalker, PRIO_ASSIGN);
	w << " = ";
	printWalker.printValue(w, value, PRIO_ASSIGN);
	w << ";\n";
}

void AST2ArrayGlobalPrinter::printForLoad(CodeStream& w, llvm::Type* type, llvm::Value* global,
	std::vector<llvm::Value*>& indices, PrintWalker& printWalker, int priority)
{
	// get name of global, e.g. "vertex"
	std::string name = global->getNameStr();

	// get the declaration and collector of the global variable by name
	clang::VarDecl* decl = getValue(this->decls, name);
	if (decl != NULL)
	{
		StringCodeStream temp;
		
		// type is always a pointer type, therefore we can cast to llvm::CompositeType
		llvm::CompositeType* pointerType = llvm::cast<llvm::CompositeType>(global->getType());
		std::vector<llvm::Value*>::const_iterator indexIt = indices.begin();
		llvm::Type* fieldType = this->handleVariable(temp, decl->getType().getTypePtr(),
			pointerType->getTypeAtIndex(uint(0)), indexIt, indices.end(), printWalker);
		if (fieldType != NULL)
		{
			// replace member by array index (e.g. "BVH.Hips_rotateX_keys" -> 3)
			int index = getValue(this->arrayElements, temp.getString().substr(1), -1); // remove leading '.'
			w << this->name << '[' << index << ']';
		}
		else
		{
			// error: field not found
		}
	}
	else
	{
		// error: global variable not found
	}
}

} // namespace digi
