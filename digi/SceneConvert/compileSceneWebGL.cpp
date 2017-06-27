#include <llvm/Pass.h>
#include <llvm/PassManager.h>

#include <llvm/Target/TargetData.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Support/TargetRegistry.h>
//#include <llvm/Support/SubtargetFeature.h>

#include <llvm/Transforms/Scalar.h> // otimizations, e.g. createScalarReplAggregatesPass, createInstructionCombiningPass, createLoopSimplifyPass

#include <llvm/Analysis/Dominators.h>
#include <llvm/Analysis/PostDominators.h>
#include <llvm/Analysis/LoopInfo.h>

#include <llvm/CodeGen/Passes.h>

#include <llvm/Assembly/PrintModulePass.h>

#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/MapUtility.h>
#include <digi/Utility/foreach.h>
#include <digi/System/MemoryDevices.h>

#include "ParticlePass.h"
#include "Scalarizer.h"
#include "ReplacePass.h"
#include "PrintPass.h"
#include "ScalarizerPass.h"
#include "CompileHelper.h"
#include "compileSceneWebGL.h"


namespace digi {

bool compileSceneWebGL(
	Compiler& compiler,
	const std::string& inputCode,
	const std::map<std::string, int>& buffers,
	Pointer<SplittedGlobalPrinter>& statePrinter,
	Pointer<SplittedGlobalPrinter>& uniformsPrinter,
	Pointer<SplittedGlobalPrinter>& transformsPrinter,
	Pointer<SplittedGlobalPrinter>& boundingBoxesPrinter,	
	std::string& initStateCode,
	std::string& updateCode,
	int& useFlags)
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
		file->write(inputCode.c_str(), inputCode.length());
		file->close();
	}
	#endif
	
	// compile
	Pointer<CompileResult> compileResult = compiler.compileFromString(inputCode, astConsumer.get());
	if (compileResult == null)
		return false;
	
	// get compiled module (take ownership)
	llvm::OwningPtr<llvm::Module> module(astConsumer->ReleaseModule());
	if (!module)
		return false;


	// create target data
	llvm::OwningPtr<llvm::TargetData> targetDataPtr(compileResult->getTargetData());
	llvm::TargetData* targetData = targetDataPtr.get();

	// get some references (no ownership is taken)
	std::map<std::string, clang::VarDecl*>& decls = astConsumer->decls;
	clang::ASTContext* astContext = compileResult->astContext.get();

	// printers for global variables
	std::map<std::string, GlobalPrinterAccess> globalPrinters;

	// print variables as in c++ (e.g. "foo.bar[3]")
	ASTGlobalPrinter* astGlobalPrinter = new ASTGlobalPrinter(astContext, decls, targetData);
	globalPrinters["buffers"] = new AST2ArrayGlobalPrinter(astContext, decls, targetData, "buffers", buffers);//astGlobalPrinter;
	globalPrinters["seed"] = astGlobalPrinter;

	// language does not support structs. these get splitted into arrays of ints, floats and objects
	statePrinter = addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData,
		*module, "state", "$state", false);
	uniformsPrinter = addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData,
		*module, "uniforms", "$uniforms", false);
	transformsPrinter = addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData,
		*module, "transforms", "$transforms", false);
	boundingBoxesPrinter = addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData,
		*module, "boundingBoxes", "$boundingBoxes", false);
	
	// set output devices for the functions
	std::map<std::string, Pointer<IODevice> > deviceMap;
	deviceMap["initState"] = new StringRefDevice(initStateCode);
	deviceMap["update"] = new StringRefDevice(updateCode);

	// scalarize (with optimization)
	module.reset(scalarizeModule(*module, compileResult));

	// print (optimize result of scalarizer)
	printModule(*module, compileResult, globalPrinters, deviceMap, Language::JS);

	if (globalPrinters["seed"].access & GlobalPrinterAccess::READ_WRITE)
		useFlags |= SCENE_USES_SEED;

	return true;
}

bool compileRenderJobWebGL(
	Compiler& compiler,
	const std::string& inputCode,
	int3 transformOffset,
	int3 boundingBoxOffset,
	NameGenerator& nameGenerator,
	std::string& outputCode)
{
	llvm::LLVMContext context;

	// code generator options
	clang::CodeGenOptions codeGenOptions = getCodeGenOptionsForWrite();

	std::string moduleName = "renderJob";
	
	// create clang code generator (derives from clang::ASTConsumer)
	llvm::OwningPtr<GlobalCollector> astConsumer(new GlobalCollector(clang::CreateLLVMCodeGen(
		*compiler.diagnosticsEngine,
		moduleName,
		codeGenOptions,
		context)));

	// debug: write input code
	#ifndef NDEBUG
	{
		Pointer<IODevice> file = File::create("renderJob.cpp");
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

	// create target data
	llvm::OwningPtr<llvm::TargetData> targetDataPtr(compileResult->getTargetData());
	llvm::TargetData* targetData = targetDataPtr.get();

	// get some references (no ownership is taken)
	std::map<std::string, clang::VarDecl*>& decls = astConsumer->decls;
	clang::ASTContext* astContext = compileResult->astContext.get();
	
	// printers for global variables
	std::map<std::string, GlobalPrinterAccess> globalPrinters;

	std::vector<ShaderType> matrix(4, ShaderType(ShaderType::FLOAT, 4));
	StringRef prefix = "_";

	// trick: parentMatrix reuses the variable names of particleMatrix of parent call to compileShapeParticleInstancer
	if (contains(decls, "parentMatrix"))
		globalPrinters["parentMatrix"] = new VariableGlobalPrinter(matrix, prefix, prefix, nameGenerator);
	addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData,
		*module, "viewProjectionMatrix", false);
	addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData,
		*module, "transform", "$transforms", false, transformOffset);
	addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData,
		*module, "bb", "$boundingBoxes", false, boundingBoxOffset);
	addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData,
		*module, "data", false);
	if (contains(decls, "bbMatrix"))
		addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData,
			*module, "bbMatrix", false);

	// scalarize (with optimization)
	module.reset(scalarizeModule(*module, compileResult));

	// set output device for the main function
	std::map<std::string, Pointer<IODevice> > deviceMap;
	deviceMap["main"] = new StringRefDevice(outputCode);

	// print (optimize result of scalarizer)
	printModule(*module, compileResult, globalPrinters, deviceMap, Language::JS);
		
	return true;	
}

bool compileParticleSystem(
	Compiler& compiler,
	const std::string& inputCode,
	int3 emitterOffset,
	int3 uniformOffset,
	Pointer<SplittedGlobalPrinter>& particlePrinter,
	std::string& createUniformCode,	
	std::string& createParticleCode,	
	std::string& updateUniformCode,	
	std::string& updateParticleCode,
	int& useFlags,
	Language mainLanguage)
{
	llvm::LLVMContext context;
	
	// code generator options
	clang::CodeGenOptions codeGenOptions = getCodeGenOptionsForWrite();

	std::string moduleName = "particleSystem";	

	// create clang code generator (derives from clang::ASTConsumer)
	llvm::OwningPtr<GlobalCollector> astConsumer(new GlobalCollector(clang::CreateLLVMCodeGen(
		*compiler.diagnosticsEngine,
		moduleName,
		codeGenOptions,
		context)));

	// debug: write input code
	#ifndef NDEBUG
	{
		Pointer<IODevice> file = File::create("particleSystem.cpp");
		file->write(inputCode.c_str(), inputCode.length());
		file->close();
	}
	#endif
	
	// compile
	Pointer<CompileResult> compileResult = compiler.compileFromString(inputCode, astConsumer.get());
	if (compileResult == null)
		return false;
	
	// get compiled module (take ownership)
	llvm::OwningPtr<llvm::Module> module(astConsumer->ReleaseModule());
	if (!module)
		return false;


	// passmanager takes ownership of passes
	llvm::PassManager passManager;

	// optimization (high inline threshold)
	compileResult->addPasses(*module, passManager, false);

	// codegen
	passManager.add(new llvm::PostDominatorTree());

	// add shader pass (does the splitting into uniform and particle code)
	ParticlePass* particlePass = new ParticlePass();
	passManager.add(particlePass);
	
	// run all passes
	passManager.run(*module);


	// create target data
	llvm::OwningPtr<llvm::TargetData> targetDataPtr(compileResult->getTargetData());
	llvm::TargetData* targetData = targetDataPtr.get();

	// get some references (no ownership is taken)
	std::map<std::string, clang::VarDecl*>& decls = astConsumer->decls;
	clang::ASTContext* astContext = compileResult->astContext.get();

	// printers for global variables
	std::map<std::string, GlobalPrinterAccess> globalPrinters;

	// note: reference to nameGenerator is passed to global printers
	NameGenerator nameGenerator;

	{
		// language does not support structs. these get splitted into arrays of ints, floats and objects
		llvm::Module& uniformModule = *particlePass->modules[ParticlePass::UNIFORM];
		llvm::Module& particleModule = *particlePass->modules[ParticlePass::PARTICLE];
		addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData,
			uniformModule, "emitter", "$uniforms", false, emitterOffset);
		addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData,
			uniformModule, "uniform", "$uniforms", false, uniformOffset);

		// particle printer is returned so that we can create particles with correct number of ints and floats
		particlePrinter = addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData,
			particleModule, "particle", "$_p", false);
	}
	
	globalPrinters["index"] = new ASTGlobalPrinter(astContext, decls, targetData, "_6");
	globalPrinters["id"] = new ASTGlobalPrinter(astContext, decls, targetData, "_7");
	ASTGlobalPrinter* astGlobalPrinter = new ASTGlobalPrinter(astContext, decls, targetData);
	globalPrinters["seed"] = astGlobalPrinter;
	globalPrinters["alive"] = astGlobalPrinter;

	// uniform -> particle
	StringRef prefix = "_";
	globalPrinters["create_u2p"] = new VariableGlobalPrinter(particlePass->create_u2p, prefix, prefix, nameGenerator);
	globalPrinters["update_u2p"] = new VariableGlobalPrinter(particlePass->update_u2p, prefix, prefix, nameGenerator);
/*
{
	std::string error;
	llvm::raw_fd_ostream s("pre_scalarizeU", error, llvm::raw_fd_ostream::F_Binary);	
	s << *particlePass->modules[ParticlePass::UNIFORM];	
	s.close();
}		
{
	std::string error;
	llvm::raw_fd_ostream s("pre_scalarizeP", error, llvm::raw_fd_ostream::F_Binary);	
	s << *particlePass->modules[ParticlePass::PARTICLE];	
	s.close();
}		
*/
#ifndef NDEBUG
	{
		std::string error;
		llvm::raw_fd_ostream s("particleSystem.uniform.ll", error, llvm::raw_fd_ostream::F_Binary);
		s << *particlePass->modules[ParticlePass::UNIFORM];
		s.close();
	}
	{
		std::string error;
		llvm::raw_fd_ostream s("particleSystem.particle.ll", error, llvm::raw_fd_ostream::F_Binary);
		s << *particlePass->modules[ParticlePass::PARTICLE];
		s.close();
	}
#endif

	// scalarize uniform and particle code (no optimization necessary)
	particlePass->modules[ParticlePass::UNIFORM].reset(
		scalarizeModule(*particlePass->modules[ParticlePass::UNIFORM], null));
	particlePass->modules[ParticlePass::PARTICLE].reset(
		scalarizeModule(*particlePass->modules[ParticlePass::PARTICLE], null));

	// print the modules
	{
		std::map<std::string, Pointer<IODevice> > deviceMap;
		deviceMap["create"] = new StringRefDevice(createUniformCode);
		deviceMap["update"] = new StringRefDevice(updateUniformCode);

		// print (optimize result of scalarizer)
		printModule(*particlePass->modules[ParticlePass::UNIFORM], compileResult, globalPrinters, deviceMap, mainLanguage);
	}
	{
		std::map<std::string, Pointer<IODevice> > deviceMap;
		deviceMap["create"] = new StringRefDevice(createParticleCode);
		deviceMap["update"] = new StringRefDevice(updateParticleCode);

		// print (optimize result of scalarizer)
		printModule(*particlePass->modules[ParticlePass::PARTICLE], compileResult, globalPrinters, deviceMap, mainLanguage);
	}

	if (globalPrinters["seed"].access & GlobalPrinterAccess::READ_WRITE)
		useFlags |= SCENE_USES_SEED;

	return true;
}

bool compileShapeParticleInstancer(
	Compiler& compiler,
	const std::string& inputCode,
	int3 transformOffset,
	int3 uniformOffset,
	NameGenerator& nameGenerator,
	std::string& uniformCode,
	std::string& particleCode,
	int& useFlags,
	Language mainLanguage)
{
	llvm::LLVMContext context;
	
	// code generator options
	clang::CodeGenOptions codeGenOptions = getCodeGenOptionsForWrite();

	std::string moduleName = "shapeParticle";	

	// create clang code generator (derives from clang::ASTConsumer)
	llvm::OwningPtr<GlobalCollector> astConsumer(new GlobalCollector(clang::CreateLLVMCodeGen(
		*compiler.diagnosticsEngine,
		moduleName,
		codeGenOptions,
		context)));

	// debug: write input code
	#ifndef NDEBUG
	{
		Pointer<IODevice> file = File::create("shapeParticle.cpp");
		file->write(inputCode.c_str(), inputCode.length());
		file->close();
	}
	#endif
	
	// compile
	Pointer<CompileResult> compileResult = compiler.compileFromString(inputCode, astConsumer.get());
	if (compileResult == null)
		return false;
	
	// get compiled module (take ownership)
	llvm::OwningPtr<llvm::Module> module(astConsumer->ReleaseModule());
	if (!module)
		return false;


	// passmanager takes ownership of passes
	llvm::PassManager passManager;

	// optimization (high inline threshold)
	compileResult->addPasses(*module, passManager, false);

	// codegen
	passManager.add(new llvm::PostDominatorTree());

	// add shader pass (does the splitting into material, transform, vertex shader and pixel shader)
	ShapeParticlePass* particlePass = new ShapeParticlePass();
	passManager.add(particlePass);
	
	// run all passes
	passManager.run(*module);


	// create target data
	llvm::OwningPtr<llvm::TargetData> targetDataPtr(compileResult->getTargetData());
	llvm::TargetData* targetData = targetDataPtr.get();

	// get some references (no ownership is taken)
	std::map<std::string, clang::VarDecl*>& decls = astConsumer->decls;
	clang::ASTContext* astContext = compileResult->astContext.get();

	// printers for global variables
	std::map<std::string, GlobalPrinterAccess> globalPrinters;

	// note: reference to nameGenerator is passed to global printers
	//NameGenerator nameGenerator;
	std::vector<ShaderType> matrix(4, ShaderType(ShaderType::FLOAT, 4));
	StringRef prefix = "_";

	// trick: parentMatrix reuses the variable names of particleMatrix of parent call to compileShapeParticleInstancer
	if (contains(decls, "parentMatrix"))
		globalPrinters["parentMatrix"] = new VariableGlobalPrinter(matrix, prefix, prefix, nameGenerator);
	
	llvm::Module& uniformModule = *particlePass->modules[ParticlePass::UNIFORM];
	llvm::Module& particleModule = *particlePass->modules[ParticlePass::PARTICLE];
	addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData,
		uniformModule, "transform", "$transforms", false, transformOffset);
	addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData,
		uniformModule, "uniform", "$uniforms", false, uniformOffset);
	addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData,
		particleModule, "particle", "$_p", false);
	globalPrinters["index"] = new ASTGlobalPrinter(astContext, decls, targetData);

	// uniform -> particle
	globalPrinters["u2p"] = new VariableGlobalPrinter(particlePass->u2p, prefix, prefix, nameGenerator);

	// output matrix. names must match parentMatrix of next recursion level, therefore copy the name generator
	NameGenerator nameGenerator2 = nameGenerator;
	globalPrinters["particleMatrix"] = new VariableGlobalPrinter(matrix, prefix, prefix, nameGenerator2);

	// scalarize material and transform (no optimization necessary)
	particlePass->modules[ParticlePass::UNIFORM].reset(
		scalarizeModule(*particlePass->modules[ParticlePass::UNIFORM], null));
	particlePass->modules[ParticlePass::PARTICLE].reset(
		scalarizeModule(*particlePass->modules[ParticlePass::PARTICLE], null));

	// print the modules
	{
		std::map<std::string, Pointer<IODevice> > deviceMap;
		deviceMap["main"] = new StringRefDevice(uniformCode);

		// print (optimize result of scalarizer)
		printModule(*particlePass->modules[ParticlePass::UNIFORM], compileResult, globalPrinters, deviceMap, mainLanguage);
	}
	{
		std::map<std::string, Pointer<IODevice> > deviceMap;
		deviceMap["main"] = new StringRefDevice(particleCode);

		// print (optimize result of scalarizer)
		printModule(*particlePass->modules[ParticlePass::PARTICLE], compileResult, globalPrinters, deviceMap, mainLanguage);
	}
	
	if (globalPrinters["index"].access & GlobalPrinterAccess::READ_WRITE)
		useFlags |= SHAPE_PARTICLE_USES_INDEX;

	return true;
}

} // namespace digi
