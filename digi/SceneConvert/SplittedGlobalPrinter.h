#ifndef digi_SceneConvert_SplittedGlobalPrinter_h
#define digi_SceneConvert_SplittedGlobalPrinter_h

#include <string>

#include <llvm/Value.h>

#include <digi/Utility/Object.h>
#include <digi/Math/All.h>

#include "CompileHelper.h"
#include "GlobalPrinter.h"


namespace digi {

/// @addtogroup SceneConvert
/// @{

class SplittedType;


/// splits globals into arrays of int, float and object (e.g. texture)
class SplittedGlobalPrinter : public GlobalPrinter
{
public:
	
	// constructor. A '$' placeholder in the name gets replaced by 'i', 'f' or 'o' according to type of output array.
	SplittedGlobalPrinter(StringRef name,
		clang::ASTContext* astContext, const clang::Type* clangType,
		llvm::TargetData* targetData, llvm::Type* llvmType, bool vectorized,
		int3 offset = int3());

	virtual ~SplittedGlobalPrinter();

	// print a global for store. may only be called once
	virtual void printForStore(CodeStream& w, llvm::Value* value, llvm::Value* global,
		std::vector<llvm::Value*>& indices, PrintWalker& printWalker);
	
	// print a global for load. returns true if the global is a vector
	virtual void printForLoad(CodeStream& w, llvm::Type* type, llvm::Value* global,
		std::vector<llvm::Value*>& indices, PrintWalker& printWalker, int priority);

	// get offset of a struct member given by path
	int3 getOffset(const std::string& path);

	// get size of struct
	int3 getSize() {return size;}
	
protected:
	
	std::string name;
	
	// extra offset in case the global is contained in another struct
	int3 offset;

	// result of slitting into int, float, object
	Pointer<SplittedType> types[3];
	int3 size;
};


// add splitted global printer to map of global printers
Pointer<SplittedGlobalPrinter> addSplittedGlobalPrinter(std::map<std::string, GlobalPrinterAccess>& globalPrinters,
	clang::ASTContext* astContext, std::map<std::string, clang::VarDecl*>& decls,
	llvm::TargetData* targetData, llvm::Module& module,
	const std::string& inputName, StringRef outputName, bool vectorized, int3 offset = int3());

inline Pointer<SplittedGlobalPrinter> addSplittedGlobalPrinter(
	std::map<std::string, GlobalPrinterAccess>& globalPrinters,
	clang::ASTContext* astContext, std::map<std::string, clang::VarDecl*>& decls,
	llvm::TargetData* targetData, llvm::Module& module,
	const std::string& name, bool vectorized, int3 offset = int3())
{
	return addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData, module, name, name, vectorized,
		offset);
}

/// @}

} // namespace digi

#endif
