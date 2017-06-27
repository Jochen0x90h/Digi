#ifndef digi_EngineVM_Compiler_h
#define digi_EngineVM_Compiler_h

#include <map>

// note: llvm needs __STDC_LIMIT_MACROS and __STDC_CONSTANT_MACROS to be defined
#undef interface
#include <llvm/PassManager.h>

#include <llvm/Target/TargetData.h>

#include <clang/Basic/TargetOptions.h>
#include <clang/Basic/DiagnosticIDs.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Basic/FileManager.h>
#include <clang/Basic/Macrobuilder.h>

#include <clang/Frontend/DiagnosticOptions.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
//#include <clang/Frontend/PCHReader.h>

#include <clang/Lex/HeaderSearch.h>
#include <clang/Serialization/Module.h>
#include <clang/Lex/ModuleLoader.h>
#include <clang/Lex/Preprocessor.h>

#include <clang/Parse/Parser.h>

#include <clang/AST/ASTContext.h>

#include <digi/Utility/Pointer.h>
#include <digi/Utility/Object.h>
#include <digi/System/FileSystem.h>
#include <digi/CodeGenerator/TypeInfo.h>

#include "ModuleLoader.h"


namespace digi {

/// @addtogroup EngineVM
/// @{

class CompileResult : public Object
{
public:

	CompileResult(clang::DiagnosticsEngine& diagnosticsEngine, clang::LangOptions& langOptions,
		clang::TargetInfo& targetInfo, clang::SourceManager& sourceManager, clang::HeaderSearch& headerSearch,
		const std::string& predefines);
	
	virtual ~CompileResult();
	
	const char* getTargetDescription() {return preprocessor->getTargetInfo().getTargetDescription();}

	llvm::TargetData* getTargetData() {return new llvm::TargetData(this->getTargetDescription());}

	// add some standard optimization passes (workaround: do inline on module separately)
	void addPasses(llvm::Module& module, llvm::PassManager& passManager, bool vm);

	// get size of a struct
	size_t getSize(clang::RecordDecl* recordDecl);


	llvm::OwningPtr<ModuleLoader> moduleLoader;
	llvm::OwningPtr<clang::Preprocessor> preprocessor;
	llvm::OwningPtr<clang::ASTContext> astContext;
};

// uses clang/llvm 3.0
class Compiler
{
public:
	
	enum LibType
	{
		NO_LIB,
		
		// itermediate code
		INTERMEDIATE,
		
		// virtual machine with opengl
		VM_OPENGL
	};

	Compiler(LibType libType);

	// set ';'-separated include search paths (e.g. "./;./")
	void setSearchPaths(const std::string& paths);
	
	
	Pointer<CompileResult> compileFromFile(const fs::path& path, clang::ASTConsumer* astConsumer);
	Pointer<CompileResult> compileFromString(const std::string& str, clang::ASTConsumer* astConsumer);


	clang::LangOptions langOptions;
	clang::TargetOptions targetOptions;

	
	#ifndef NDEBUG
		// for printing diagnostic to file
		//std::string diagnosticError;
		//llvm::raw_fd_ostream diagnosticFile;
	#endif

	clang::DiagnosticOptions diagnosticOptions;
	llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs> ids;
	clang::DiagnosticConsumer* diagnosticConsumer;
	llvm::IntrusiveRefCntPtr<clang::DiagnosticsEngine> diagnosticsEngine;
	llvm::OwningPtr<clang::TargetInfo> targetInfo;
	llvm::OwningPtr<clang::FileManager> fileManager;
	llvm::OwningPtr<clang::HeaderSearch> headerSearch;

	std::string predefines;

protected:

	Pointer<CompileResult> compile(clang::ASTConsumer* astConsumer, clang::SourceManager& sourceManager/*, clang::PCHReader* pchReader*/);
	
	std::string pch;
};

void addPrintPass(llvm::PassManager& passManager, StringRef name);

// helper functions
void setStdCall(llvm::Module* module);
void implementConvertFunctions(llvm::Module* module);

/// @}

} // namespace digi

#endif
