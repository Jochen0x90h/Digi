#ifndef digi_SceneConvert_CompileHelper_h
#define digi_SceneConvert_CompileHelper_h

#include <string>

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>

#include <clang/Frontend/CodeGenOptions.h>

#include <digi/System/IODevice.h>
#include <digi/CodeGenerator/CodeWriter.h>
#include <digi/CodeGenerator/Type.h>
#include <digi/EngineVM/Compiler.h>

#include "GlobalPrinter.h"


namespace digi {

/// @addtogroup SceneConvert
/// @{

inline void writeBindingVariable(CodeWriter& w, StringRef name, StringRef type)
{
	w << type << " " << name << ";\n";
}

// write binding variable. does not write if null or empty
inline void writeBindingVariable(CodeWriter& w, const std::string& name, Pointer<Type> type)
{
	if (type != null)
		type->writeVariable(w, name);
}

inline clang::CodeGenOptions getCodeGenOptionsForWrite()
{
	clang::CodeGenOptions codeGenOptions;
	codeGenOptions.DisableLLVMOpts = 1;
	codeGenOptions.OptimizationLevel = 0;
	codeGenOptions.Inlining = clang::CodeGenOptions::NormalInlining;
	return codeGenOptions;
}


// scalarize module
llvm::Module* scalarizeModule(llvm::Module& module, Pointer<CompileResult> compileResult);


// print module
void printModule(llvm::Module& module, Pointer<CompileResult> compileResult,
	std::map<std::string, GlobalPrinterAccess>& globalPrinters, std::map<std::string, Pointer<IODevice> >& deviceMap,
	Language language);

/// @}

} // namespace digi

#endif
