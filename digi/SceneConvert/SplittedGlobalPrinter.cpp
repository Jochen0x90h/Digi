#include <clang/AST/RecordLayout.h>

#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/MapUtility.h>

#include "PrintWalker.h"
#include "SplittedGlobalPrinter.h"


namespace digi {

class SplittedType : public Object
{
public:
	virtual ~SplittedType();
	
	virtual int print(CodeStream& w, const std::vector<llvm::Value*>& indices, int ii, int offset, PrintWalker& printWalker);
};

SplittedType::~SplittedType()
{
}

int SplittedType::print(CodeStream& w, const std::vector<llvm::Value*>& indices, int ii, int offset, PrintWalker& printWalker)
{
	if (w.getLanguage().supportsVector())
	{
		w << (offset >> 2);
		return offset & 3;
	}
	
	w << offset;
	return 0;
}

namespace
{
	// type count: int, float, object
	const int NUM_TYPES = 3;

	// get type index for LLVM type: 0 = int, 1 = float, 2 = object
	int getTypeIndex(llvm::Type* type)
	{
		return type->isFloatingPointTy() ? 1 : (type->isPointerTy() ? 2 : 0);
	}

	
	// SplittedArray
	class SplittedArray : public SplittedType
	{
	public:
		SplittedArray(Pointer<SplittedType> element, int elementSize)
			: element(element), elementSize(elementSize)
		{
		}
	
		virtual ~SplittedArray()
		{
		}

		virtual int print(CodeStream& w, const std::vector<llvm::Value*>& indices, int ii, int offset, PrintWalker& printWalker)
		{
			int index = getConstantIndex(indices[ii]);
			if (index != -1)
			{
				// index is constant

				// print offset of indexed element
				return this->element->print(w, indices, ii + 1, offset + index * this->elementSize, printWalker);
			}
			else
			{
				// index is variable
			
				// print base offset of array (offset of first element)
				int componentOffset = this->element->print(w, indices, ii + 1, offset, printWalker);
				
				// add array index times element size
				int elementSize = w.getLanguage().supportsVector() ? this->elementSize >> 2 : this->elementSize;
				w << " + ";
				printWalker.printValue(w, indices[ii], PRIO_MUL);
				w << " * " << elementSize;
				
				return componentOffset;
			}
		}

		Pointer<SplittedType> element;
		int elementSize;
	};

	
	struct SplittedElement
	{
		// element type
		Pointer<SplittedType> type;
		
		// offset of element in structure
		int offset;
				
		SplittedElement()
			: offset() {}

		SplittedElement(Pointer<SplittedType> type, int offset)
			: type(type), offset(offset) {}
	};

	// splitted structure
	class SplittedStruct : public SplittedType
	{
	public:
		virtual ~SplittedStruct()
		{
		}
		
		virtual int print(CodeStream& w, const std::vector<llvm::Value*>& indices, int ii, int offset, PrintWalker& printWalker)
		{
			int index = getConstantIndex(indices[ii]);
			const SplittedElement& element = this->elements[index];
			return element.type->print(w, indices, ii + 1, offset + element.offset, printWalker);
		}

		// llvm element index -> element
		std::map<int, SplittedElement> elements;
		
		// clang name -> llvm element index
		std::map<std::string, int> names;
	};


	struct SplittedResult
	{
		Pointer<SplittedType> type;
		int size;
		int align;
	
		SplittedResult() : size(), align() {}
	};

	void convertType(clang::ASTContext* astContext, const clang::Type* clangType,
		llvm::TargetData* targetData, llvm::Type* llvmType, bool vectorized, SplittedResult* results)
	{
		if (llvm::StructType* llvmStructType = llvm::dyn_cast<llvm::StructType>(llvmType))
		{
			if (const clang::RecordType* clangRecordType = clangType->getAs<clang::RecordType>())
			{
				// struct type
				clang::RecordDecl* clangRecordDecl = clangRecordType->getDecl();
				const llvm::StructLayout* llvmLayout = targetData->getStructLayout(llvmStructType);		

				// iterate over clang fields
				clang::RecordDecl::field_iterator clangIt = clangRecordDecl->field_begin();
				clang::RecordDecl::field_iterator clangEnd = clangRecordDecl->field_end();
				llvm::StructType::element_iterator llvmIt = llvmStructType->element_begin();
				//llvm::StructType::element_iterator llvmEnd = llvmStructType->element_end();
				int clangFieldIndex = 0;
				int llvmElementIndex = 0;
				for (; clangIt != clangEnd; ++clangIt, ++clangFieldIndex)
				{
					uint64_t offset = astContext->getASTRecordLayout(clangRecordDecl).getFieldOffset(clangFieldIndex);

					// "catch up" with llvm field index until offset is reached
					while (llvmLayout->getElementOffsetInBits(llvmElementIndex) < offset)
					{
						++llvmElementIndex;
						++llvmIt;
					}
				
					// get clang and llvm type of field
					clang::FieldDecl* fieldDecl = *clangIt;
					const clang::Type* clangFieldType = fieldDecl->getType().getTypePtr();
					llvm::Type* llvmElementType = *llvmIt;

					// recursively convert field
					SplittedResult elementResults[NUM_TYPES];
					convertType(astContext, clangFieldType, targetData, llvmElementType, vectorized, elementResults);
					for (int i = 0; i < NUM_TYPES; ++i)
					{
						SplittedResult& r = results[i];
						SplittedResult& e = elementResults[i];
						
						// types[i] is null if no child element is of current type (int, float, object)
						if (e.type != null)
						{
							// create struct type at first time
							if (r.type == null)
								r.type = new SplittedStruct();
							Pointer<SplittedStruct> splittedStruct = staticCast<SplittedStruct>(r.type);
							
							int elementOffset = r.size;
							int elementSize = e.size;
							
							// align element offset if vectorized
							elementOffset = (elementOffset + e.align - 1) & ~(e.align - 1);
							
							// create element with element type and offset
							SplittedElement element(e.type, elementOffset);

							// map llvm element index -> element
							splittedStruct->elements[llvmElementIndex] = element;

							// map clang name -> llvm element index
							splittedStruct->names[fieldDecl->getName()] = llvmElementIndex;
							
							// accumulate size of structure
							r.size = elementOffset + elementSize;
							r.align = std::max(r.align, e.align);
						}
					}
				}
				return;
			}
		}
		
		if (const llvm::SequentialType* sequentialType = llvm::dyn_cast<llvm::SequentialType>(llvmType))
		{
			llvm::Type* llvmElementType = sequentialType->getElementType();

			// get number of elements of either array or vector type
			int numElements = -1;
			const clang::Type* clangElementType;
			if (const llvm::ArrayType* llvmArrayType = llvm::dyn_cast<llvm::ArrayType>(llvmType))
			{
				// array type
				if ((clangElementType = clangType->getArrayElementTypeNoTypeQual()))
					numElements = int(llvmArrayType->getNumElements());
			}
			else if (const llvm::VectorType* llvmVectorType = llvm::dyn_cast<llvm::VectorType>(llvmType))
			{
				// vector type
				if (const clang::VectorType* clangVectorType = clangType->getAs<clang::VectorType>())
				{
					numElements = int(llvmVectorType->getNumElements());
					clangElementType = clangVectorType->getElementType().getTypePtr();
				
					// treat vector as atomic type if vectorized
					if (vectorized)
					{
						int i = getTypeIndex(llvmElementType);
						SplittedResult& r = results[i];
						r.type = new SplittedType();
						r.size = numElements;
						r.align = numElements == 3 ? 4 : numElements;
						return;
					}
				}
			}

			if (numElements != -1)
			{
				// array or vector
				
				// recursively convert element
				SplittedResult elementResults[NUM_TYPES];
				convertType(astContext, clangElementType, targetData, llvmElementType, vectorized, elementResults);
				for (int i = 0; i < NUM_TYPES; ++i)
				{
					SplittedResult& r = results[i];
					SplittedResult& e = elementResults[i];
					if (e.type != null)
					{
						int elementSize = e.size;
						
						// align element size to 4 if vectorized
						if (vectorized)
							elementSize = (elementSize + 3) & ~3;
						
						// use SplittedArray for both array and vector
						r.type = new SplittedArray(e.type, elementSize);
						r.size = numElements * elementSize;
						r.align = vectorized ? 4 : 1;
					}
				}
				return;
			}
		}

		// scalar or object type
		int i = getTypeIndex(llvmType);
		SplittedResult& r = results[i];
		r.type = new SplittedType();
		r.size = 1;
		r.align = 1;
	}

} // anonymous namespace


// SplittedGlobalPrinter

SplittedGlobalPrinter::SplittedGlobalPrinter(StringRef name,
	clang::ASTContext* astContext, const clang::Type* clangType,
	llvm::TargetData* targetData, llvm::Type* llvmType, bool vectorized,
	int3 offset)
	: name(name), offset(offset)
{
	SplittedResult results[NUM_TYPES];
	convertType(astContext, clangType, targetData, llvmType, vectorized, results);
	
	for (int i = 0; i < NUM_TYPES; ++i)
	{
		this->types[i] = results[i].type;
		this->size[i] = results[i].size;
	}
}

SplittedGlobalPrinter::~SplittedGlobalPrinter()
{
}

void SplittedGlobalPrinter::printForStore(CodeStream& w, llvm::Value* value, llvm::Value* global,
	std::vector<llvm::Value*>& indices, PrintWalker& printWalker)
{
	printForLoad(w, value->getType(), global, indices, printWalker, PRIO_ASSIGN);
	w << " = ";
	printWalker.printValue(w, value, PRIO_ASSIGN);
	w << ";\n";
}

void SplittedGlobalPrinter::printForLoad(CodeStream& w, llvm::Type* type, llvm::Value* global,
	std::vector<llvm::Value*>& indices, PrintWalker& printWalker, int priority)
{
	int numComponents = 1;
	if (const llvm::VectorType* vectorType = llvm::dyn_cast<llvm::VectorType>(type))
	{
		// vector type
		numComponents = int(vectorType->getNumElements());
		type = vectorType->getElementType();
	}
					
	int i = getTypeIndex(type);
	w << replace(this->name, '$', "ifo"[i]);
	w << '[';
	int componentOffset = this->types[i]->print(w, indices, 0, this->offset[i], printWalker);
	w << ']';
	if (numComponents < 4 && w.getLanguage().supportsVector())
		w << '.' << StringRef(&"xyzw"[componentOffset], numComponents);
}

namespace
{
	int getOffsetInternal(const std::string& p, Pointer<SplittedType> splittedType, int offset)
	{
		// get offset of path in struct. example: path = "foo.bar[3].x"
		if (splittedType == null)
			return 0;

		// following path elements (e.g. ".bar[3].x")
		int pos = 0;
		int length = int(p.length());
		while (pos < length)
		{	
			// get next path element
			int start = pos;		
			do
				++pos;
			while (pos < length && p[pos] != '.' && p[pos] != '[');
			
			if (p[start] == '.')
			{
				// member selection on struct or vector (e.g. ".bar" or ".x")
				std::string element = p.substr(start + 1, pos - start - 1);
				
				if (Pointer<SplittedStruct> splittedStruct = dynamicCast<SplittedStruct>(splittedType))
				{
					// struct (e.g. ".foo")
					
					// find llvm element index by name
					std::map<std::string, int>::iterator it1 = splittedStruct->names.find(element);
					if (it1 == splittedStruct->names.end())
						return 0;
					int llvmElementIndex = it1->second;
					
					// find offset by llvm element index
					std::map<int, SplittedElement>::iterator it2 = splittedStruct->elements.find(llvmElementIndex);
					if (it2 == splittedStruct->elements.end())
						return 0;
					
					splittedType = it2->second.type;
					offset += it2->second.offset;
				}
				else if (Pointer<SplittedArray> splittedArray = dynamicCast<SplittedArray>(splittedType))
				{
					// vector (e.g. ".x")
					int index = 0;
					if (element == "x")
						index = 0;
					else if (element == "y")
						index = 1;
					else if (element == "z")
						index = 2;
					else if (element == "w")
						index = 3;
					else
					{
						// error: member not found
						return 0;
					}
					
					splittedType = splittedArray->element;
					offset += index;
				}
				else
				{
					// error: not a struct or vector
					return 0;
				}
			}
			else
			{
				// array index (e.g. "[3]")
				int index = toInt(p.substr(start + 1, pos - start - 2));
			
				if (Pointer<SplittedArray> splittedArray = dynamicCast<SplittedArray>(splittedType))
				{
					splittedType = splittedArray->element;
					offset += index * splittedArray->elementSize;
				}
				else
				{
					// error: not an array
					return 0;
				}
			}
		}

		return offset;
	}
} // anonymous namespace


int3 SplittedGlobalPrinter::getOffset(const std::string& path)
{
	// get offset of member with given path in struct. example: path = ".foo.bar[3].x"

	int3 offset;
	for (int i = 0; i < NUM_TYPES; ++i)
	{
		offset[i] = getOffsetInternal(path, this->types[i], this->offset[i]);
	}
	return offset;
}

Pointer<SplittedGlobalPrinter> addSplittedGlobalPrinter(std::map<std::string, GlobalPrinterAccess>& globalPrinters,
	clang::ASTContext* astContext, std::map<std::string, clang::VarDecl*>& decls,
	llvm::TargetData* targetData, llvm::Module& module,
	const std::string& inputName, StringRef outputName, bool vectorized, int3 offset)
{
	// get clang type
	clang::VarDecl* decl = getValue(decls, inputName);
	if (decl == NULL)
		return null;
	const clang::Type* clangType = decl->getType().getTypePtr();	

	// get llvm type
	llvm::GlobalVariable* global = module.getGlobalVariable(inputName, true);
	if (global == NULL)
		return null;
	llvm::Type* llvmType = llvm::cast<llvm::SequentialType>(global->getType())->getElementType();	
	
	// splits global into array of float and int. can also be used to query offsets and size of the variable
	Pointer<SplittedGlobalPrinter> p = new SplittedGlobalPrinter(outputName,
		astContext, clangType,
		targetData, llvmType, vectorized,
		offset);
	globalPrinters[inputName] = p;
	return p;
}

} // namespace digi
