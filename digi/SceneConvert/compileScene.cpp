#include <llvm/LLVMContext.h>
#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/Module.h>

#include <llvm/Target/TargetData.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
//#include <llvm/Target/TargetRegistry.h>
//#include <llvm/Target/SubtargetFeature.h>

#include <llvm/Transforms/Scalar.h> // otimizations, e.g. createScalarReplAggregatesPass, createInstructionCombiningPass, createLoopSimplifyPass

#include <llvm/Analysis/Dominators.h>
#include <llvm/Analysis/PostDominators.h>
#include <llvm/Analysis/LoopInfo.h>

#include <llvm/CodeGen/Passes.h>

#include <llvm/Assembly/PrintModulePass.h>

#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/foreach.h>
#include <digi/System/MemoryDevices.h>

#include "CompileHelper.h"
#include "GlobalCollector.h"
#include "SplittedGlobalPrinter.h"
#include "DeformerPass.h"
#include "PrintPass.h"

#include "compileScene.h"


namespace digi {

bool compileScene(
	Compiler& compiler, const std::string& inputCode,
	std::string& outputCode,
	Language language)
{
	llvm::LLVMContext context;

	// code generator options
	clang::CodeGenOptions codeGenOptions = getCodeGenOptionsForWrite();

	std::string moduleName = "scene";
	
	// create clang code generator (derives from clang::ASTConsumer)
	llvm::OwningPtr<GlobalCollector> astConsumer(new GlobalCollector(clang::CreateLLVMCodeGen(
		*compiler.diagnosticsEngine,
		moduleName,
		codeGenOptions,
		context)));

	// debug: write input code
	#ifndef NDEBUG
	{
		Pointer<IODevice> file = File::create("scene.cpp");
		file->write(inputCode.data(), inputCode.length());
		file->close();
	}
	#endif

	// compile
	Pointer<CompileResult> compileResult = compiler.compileFromString(inputCode, astConsumer.get());
	if (compileResult == null)
		return false;

	// get compiled module
	llvm::OwningPtr<llvm::Module> module(astConsumer->ReleaseModule());
	if (!module)
		return false;

	return true;
}

bool compileDeformer(
	Compiler& compiler,
	const std::string& inputCode,
	int3 deformerOffset,
	std::string& uniformCode,
	std::string& vertexCode,
	std::vector<ShaderVariable>& uniformVariables,
	std::map<std::string, ShaderVariable>& vertexBindings,
	std::map<std::string, ShaderVariable>& resultBindings,
	Language mainLanguage, Language deformerLanguage)
{
	llvm::LLVMContext context;

	// code generator options
	clang::CodeGenOptions codeGenOptions = getCodeGenOptionsForWrite();

	std::string moduleName = "deformer";
	
	// create clang code generator (derives from clang::ASTConsumer)
	llvm::OwningPtr<GlobalCollector> astConsumer(new GlobalCollector(clang::CreateLLVMCodeGen(
		*compiler.diagnosticsEngine,
		moduleName,
		codeGenOptions,
		context)));

	// debug: write input code
	#ifndef NDEBUG
	{
		Pointer<IODevice> file = File::create("deformer.cpp");
		file->write(inputCode.c_str(), inputCode.length());
		file->close();
	}
	#endif

	// compile
	Pointer<CompileResult> compileResult = compiler.compileFromString(inputCode, astConsumer.get());
	if (compileResult == null)
		return false;

	// get compiled module
	llvm::OwningPtr<llvm::Module> module(astConsumer->ReleaseModule());
	if (!module)
		return false;


	// passmanager takes ownership of passes
	llvm::PassManager passManager;

	// optimization (high inline threshold)
	compileResult->addPasses(*module, passManager, false);

	// codegen
	passManager.add(new llvm::PostDominatorTree());

	// add deformer pass (does the splitting into uniform and vertex code)
	DeformerPass* deformerPass = new DeformerPass();
	passManager.add(deformerPass);
	
	// run all passes
	passManager.run(*module);


	// create target data. note: c-pointer to targetData is passed to global printers while this pointer keeps ownership
	llvm::OwningPtr<llvm::TargetData> targetDataPtr(compileResult->getTargetData());
	llvm::TargetData* targetData = targetDataPtr.get();

	// get some references (no ownership is taken)
	std::map<std::string, clang::VarDecl*>& decls = astConsumer->decls;
	clang::ASTContext* astContext = compileResult->astContext.get();

	// printers for global variables
	std::map<std::string, GlobalPrinterAccess> globalPrinters;

	// note: reference to nameGenerator is passed to global printers
	NameGenerator nameGenerator;

	// global variable printers for input/output structures
	Pointer<ASTGlobalPrinter> astGlobalPrinter = new ASTGlobalPrinter(astContext, decls, targetData);
	if (mainLanguage.supportsStruct())
	{
		// print variables as in c++ (e.g. "foo.bar[3]")
		globalPrinters["deformer"] = astGlobalPrinter;
	}
	else
	{
		// main language does not support structs. these get splitted into arrays of ints, floats and objects
		llvm::Module& uniformModule = *deformerPass->modules[DeformerPass::UNIFORM];
		addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData,
			uniformModule, "deformer", "$uniforms", false, deformerOffset);
	}

	// vertex input
	Pointer<VertexGlobalPrinter> vertexGlobalPrinter;
	globalPrinters["vertex"] = vertexGlobalPrinter = new VertexGlobalPrinter(astContext, decls, targetData);

	// result
	Pointer<VertexGlobalPrinter> resultGlobalPrinter;
	globalPrinters["result"] = resultGlobalPrinter = new VertexGlobalPrinter(astContext, decls, targetData, "res");

	// uniform -> vertex
	Pointer<StandardGlobalPrinter> u2v;
	globalPrinters["u2v"] = u2v = new StandardGlobalPrinter(deformerPass->u2v, std::string(), std::string(), nameGenerator);


	// scalarize material and transform (no optimization necessary)
	// do after addSplittedGlobalPrinter, otherwise clang and llvm types do not match any more
	if (!mainLanguage.supportsVector())
		deformerPass->modules[DeformerPass::UNIFORM].reset(
			scalarizeModule(*deformerPass->modules[DeformerPass::UNIFORM], null));
	if (!deformerLanguage.supportsVector())
		deformerPass->modules[DeformerPass::VERTEX].reset(
			scalarizeModule(*deformerPass->modules[DeformerPass::VERTEX], null));


	// print the modules
	std::map<std::string, Pointer<IODevice> > deviceMap;
	Pointer<IODevice>& mainDev = deviceMap["main"];

	// print (no optimization necessary)
	mainDev = new StringRefDevice(uniformCode);
	printModule(*deformerPass->modules[DeformerPass::UNIFORM], null, globalPrinters, deviceMap, mainLanguage);
	mainDev = new StringRefDevice(vertexCode);
	printModule(*deformerPass->modules[DeformerPass::VERTEX], null, globalPrinters, deviceMap, deformerLanguage);

	// get variables for uniform -> vertex
	u2v->getVariables(uniformVariables);

	vertexGlobalPrinter->vertexBindings.swap(vertexBindings);
	resultGlobalPrinter->vertexBindings.swap(resultBindings);
		
	return true;	
}

} // namespace digi
