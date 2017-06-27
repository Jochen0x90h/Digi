#include <llvm/LLVMContext.h>
#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/Module.h>

#include <llvm/Assembly/PrintModulePass.h>

#include <llvm/CodeGen/Passes.h>

#include <digi/Utility/Find.h>
#include <digi/Utility/VectorUtility.h>
#include <digi/Utility/foreach.h>
#include <digi/System/MemoryDevices.h>
#include <digi/System/Log.h>

#include "GlobalCollector.h"
#include "ShaderWalker.h"
#include "ShaderPass.h"
#include "ShaderGlobalPrinter.h"
#include "PrintHelper.h"
#include "CompileHelper.h"
#include "PrintPass.h"

#include "compileShader.h"


namespace digi {

namespace {
	class FrontFacingGlobalPrinter : public GlobalPrinter
	{
	public:
		
		FrontFacingGlobalPrinter() : used(false) {}
		virtual ~FrontFacingGlobalPrinter() {}
		
		virtual void printForStore(CodeStream& w, llvm::Value* value, llvm::Value* global,
			std::vector<llvm::Value*>& indices, PrintWalker& printWalker)
		{
		}
		
		virtual void printForLoad(CodeStream& w, llvm::Type* type, llvm::Value* global,
			std::vector<llvm::Value*>& indices, PrintWalker& printWalker, int priority)
		{
			w << "f_f";
			this->used = true;
		}

		bool used;
	};
}

bool compileShader(
	Compiler& compiler,
	const std::string& inputCode,
	int3 deformerOffset,
	int3 materialOffset,
	std::string& materialCode,
	std::string& transformCode,
	std::string& vertexShader,
	std::string& pixelShader,
	std::vector<ShaderVariable>& transferVariables,
	std::vector<ShaderVariable>& materialVariables,
	std::vector<ShaderVariable>& materialLargeArrays,
	std::vector<ShaderVariable>& transformVariables,
	std::map<std::string, ShaderVariable>& vertexBindings,
	SortMode& sortMode,
	int& useFlags,
	int2& numTextures,
	Language mainLanguage,
	Language shaderLanguage,
	ShaderOptions options,
	bool flipY)
{
	llvm::LLVMContext context;
	
	// code generator options
	clang::CodeGenOptions codeGenOptions = getCodeGenOptionsForWrite();

	std::string moduleName = "shader";	
	
	// create clang code generator and wrapper to capture toplevel decls
	llvm::OwningPtr<GlobalCollector> astConsumer(new GlobalCollector(clang::CreateLLVMCodeGen(
		*compiler.diagnosticsEngine,
		moduleName,
		codeGenOptions,
		context)));

	// debug: write input code
	#ifndef NDEBUG
	{
		Pointer<IODevice> file = File::create("shader.cpp");
		file->write(inputCode.data(), inputCode.length());
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

	// prevent fragmentation of uniform arrays if language does not support precision
	if (shaderLanguage != Language::ESSL)
	{
		options.positionPrecision = PRECISION_HIGH;
		options.colorPrecision = PRECISION_HIGH;
		options.samplePrecision = PRECISION_HIGH;
	}


	// passmanager takes ownership of passes
	llvm::PassManager passManager;

	// optimization (high inline threshold)
	compileResult->addPasses(*module, passManager, false);

	// codegen
	passManager.add(new llvm::PostDominatorTree());

	// add shader pass (does the splitting into material, transform, vertex shader and pixel shader)
	// material ---> vertex
	//    |      \ /   |
	//    |       X    |
	//    v      / \   v
	// transform --> pixel
	bool frontFacingIsUniform = options.doubleSidedMode != ShaderOptions::ONE_PASS;
	ShaderPass* shaderPass = new ShaderPass(options.avoidPixelShader, frontFacingIsUniform, flipY);
	passManager.add(shaderPass);
	
	// run all passes
	passManager.run(*module);

	// get sort mode that was determined by the shader pass
	sortMode = shaderPass->sortMode;


	// create target data
	llvm::OwningPtr<llvm::TargetData> targetDataPtr(compileResult->getTargetData());
	llvm::TargetData* targetData = targetDataPtr.get();

	// get some references (no ownership is taken)
	std::map<std::string, clang::VarDecl*>& decls = astConsumer->decls;
	clang::ASTContext* astContext = compileResult->astContext.get();

	// printers for global variables
	std::map<std::string, GlobalPrinterAccess> globalPrinters;

	// name generator for uniform, vertex -> pixel and pixel output variables
	// note: reference to nameGenerator is passed to global printers
	NameGenerator nameGenerator;

	// global variable printers for uniform input variables
	Pointer<ASTGlobalPrinter> astGlobalPrinter = new ASTGlobalPrinter(astContext, decls, targetData);
	if (mainLanguage.supportsStruct())
	{
		// print variables to output as in c++ input (e.g. "foo.bar[3]")
		globalPrinters["viewMatrix"] = astGlobalPrinter;
		globalPrinters["projectionMatrix"] = astGlobalPrinter;
		globalPrinters["viewProjectionMatrix"] = astGlobalPrinter;
		globalPrinters["deformer"] = astGlobalPrinter;
		globalPrinters["material"] = astGlobalPrinter;
		globalPrinters["matrix"] = astGlobalPrinter;
		//globalPrinters["transform"] = astGlobalPrinter;
	}
	else
	{
		// main language does not support structs. these get splitted into arrays of ints, floats and objects
		llvm::Module& materialModule = *shaderPass->modules[ShaderWalker::MATERIAL];
		llvm::Module& transformModule = *shaderPass->modules[ShaderWalker::TRANSFORM];
		addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData,
			materialModule, "viewMatrix", false);
		addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData,
			materialModule, "projectionMatrix", false);
		addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData,
			materialModule, "viewProjectionMatrix", false);
		addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData,
			materialModule, "deformer", "$uniforms", false, deformerOffset);
		addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData,
			materialModule, "material", "$uniforms", false, materialOffset);
		addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData,
			transformModule, "matrix", false);
		//addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData,
		//	transformModule, "transform", "$transform", false);
	}

	// transform of packed vertex data
	llvm::Module& vertexModule = *shaderPass->modules[ShaderWalker::VERTEX];
	Pointer<SplittedGlobalPrinter> scaleOffset = addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData,
		vertexModule, "s_o", true);

	globalPrinters["viewport"] = astGlobalPrinter;
	globalPrinters["sort"] = astGlobalPrinter;
	globalPrinters["flip"] = astGlobalPrinter;
	globalPrinters["objectId"] = astGlobalPrinter;

	// vertex attributes
	Pointer<VertexGlobalPrinter> vertexGlobalPrinter;
	globalPrinters["vertex"] = vertexGlobalPrinter = new VertexGlobalPrinter(astContext, decls, targetData);
		
	// material -> transform
	Pointer<GlobalPrinter> m2t;
	if (mainLanguage.supportsStruct())
	{
		m2t = new StandardGlobalPrinter(shaderPass->m2t, "transfer.", "transfer.", nameGenerator);
	}
	else
	{
		// no struct support: have to use arrays to transfer from material to transform
		int maxArraySize = 0xffffff;
		m2t = new ArrayPackingGlobalPrinter(shaderPass->m2t, std::string(), std::string(),
			maxArraySize, nameGenerator);
	}
	globalPrinters["m2t"] = m2t;

	const char* uniform = mainLanguage.supportsStruct() ? "uniform." : "";

	int maxVertexUniforms = options.maxVertexUniforms;
	int maxPixelUniforms = 9999;

	// reserve uniforms for scale/offset
	if (scaleOffset != null)
		maxVertexUniforms -= scaleOffset->getSize().y; // only float

	// material -> vertex
	Pointer<ArrayPackingGlobalPrinter> m2v;
	globalPrinters["m2v"] = m2v = new ArrayPackingGlobalPrinter(shaderPass->m2v, uniform, std::string(),
		maxVertexUniforms, nameGenerator);
	
	// material -> pixel
	Pointer<ArrayPackingGlobalPrinter> m2p;
	globalPrinters["m2p"] = m2p = new ArrayPackingGlobalPrinter(shaderPass->m2p, uniform, std::string(),
		maxPixelUniforms, nameGenerator);
	Pointer<StandardGlobalPrinter> m2po;
	globalPrinters["m2po"] = m2po = new StandardGlobalPrinter(shaderPass->m2po, "uniform.", std::string(),
		nameGenerator);

	// transform -> vertex
	Pointer<ArrayPackingGlobalPrinter> t2v;
	globalPrinters["t2v"] = t2v = new ArrayPackingGlobalPrinter(shaderPass->t2v, std::string(), std::string(),
		maxVertexUniforms, nameGenerator);

	// transform -> pixel
	Pointer<ArrayPackingGlobalPrinter> t2p;
	globalPrinters["t2p"] = t2p = new ArrayPackingGlobalPrinter(shaderPass->t2p, std::string(), std::string(),
		maxPixelUniforms, nameGenerator);

	// vertex -> pixel
	Pointer<PackingGlobalPrinter> v2p;
	globalPrinters["v2p"] = v2p = new PackingGlobalPrinter(shaderPass->v2p, std::string(), std::string(),
		nameGenerator);

	// now allocate arrays either into remaining uniforms or into textures if too large
	m2v->allocateArrays(maxVertexUniforms, nameGenerator);

	// global variable printers for shader input/output variables
	Pointer<ShaderGlobalPrinter> shaderGlobalPrinter = new ShaderGlobalPrinter(nameGenerator);
	Pointer<FrontFacingGlobalPrinter> frontFacingGlobalPrinter = new FrontFacingGlobalPrinter();
	globalPrinters["pPosition"] = shaderGlobalPrinter; // output: projected position
	globalPrinters["vPosition"] = shaderGlobalPrinter; // output: view position (currently not used)
	globalPrinters["pointSize"] = shaderGlobalPrinter; // output: sprite size
	if (frontFacingIsUniform)
	{
		// frontFacing is a uniform variable
		globalPrinters["frontFacing"] = frontFacingGlobalPrinter;
	}
	else
	{
		// frontFacing is a build-in pixel shader variable
		globalPrinters["frontFacing"] = shaderGlobalPrinter; // input
	}
	globalPrinters["pointCoord"] = shaderGlobalPrinter; // input: position on sprite
	globalPrinters["fragCoord"] = shaderGlobalPrinter; // input: fragment coordinate, x is [0 .. w], y is [0 .. h], z is [0 .. 1], w is 1/w
	globalPrinters["discard"] = shaderGlobalPrinter;
	globalPrinters["output"] = shaderGlobalPrinter; // output: color output
	globalPrinters["outputs"] = shaderGlobalPrinter; // output: output array for multiple render targets

	
	// scalarize material and transform (no optimization necessary)
	// do after addSplittedGlobalPrinter, otherwise clang and llvm types do not match any more
	if (!mainLanguage.supportsVector())
	{
		shaderPass->modules[ShaderWalker::MATERIAL].reset(
			scalarizeModule(*shaderPass->modules[ShaderWalker::MATERIAL], null));
		shaderPass->modules[ShaderWalker::TRANSFORM].reset(
			scalarizeModule(*shaderPass->modules[ShaderWalker::TRANSFORM], null));
	}
	
	
	// do the printing
	std::string vertexShaderCode;
	std::string pixelShaderCode;
	std::map<std::string, Pointer<IODevice> > deviceMap;
	Pointer<IODevice>& mainDev = deviceMap["main"];

	// print (no optimization necessary)
	mainDev = new StringRefDevice(materialCode);
	printModule(*shaderPass->modules[ShaderWalker::MATERIAL], null, globalPrinters, deviceMap, mainLanguage);
	mainDev = new StringRefDevice(transformCode);
	printModule(*shaderPass->modules[ShaderWalker::TRANSFORM], null, globalPrinters, deviceMap, mainLanguage);
	mainDev = new StringRefDevice(vertexShaderCode);
	printModule(*shaderPass->modules[ShaderWalker::VERTEX], null, globalPrinters, deviceMap, shaderLanguage);
	mainDev = new StringRefDevice(pixelShaderCode);
	printModule(*shaderPass->modules[ShaderWalker::PIXEL], null, globalPrinters, deviceMap, shaderLanguage);


	// material -> transform
	m2t->getVariables(transferVariables);

	// material -> vertex
	std::vector<ShaderVariable> m2vVariables;
	m2v->getVariables(m2vVariables);
	
	// material -> vertex, large arrays that are stored in textures
	std::vector<ShaderVariable> m2vLargeArrays;
	m2v->getLargeArrays(m2vLargeArrays);

	// material -> pixel
	std::vector<ShaderVariable> m2pVariables;
	m2p->getVariables(m2pVariables);

	// material -> pixel, objects (textures)
	std::vector<ShaderVariable> m2poVariables;
	m2po->getVariables(m2poVariables);

	// transform -> vertex
	std::vector<ShaderVariable> t2vVariables;
	t2v->getVariables(t2vVariables);

	// transform -> pixel
	std::vector<ShaderVariable> t2pVariables;
	t2p->getVariables(t2pVariables);

	// vertex -> pixel
	std::vector<ShaderVariable> v2pVariables;
	v2p->getVariables(v2pVariables);

	// get use flags
	if (globalPrinters["viewport"].access & GlobalPrinterAccess::READ)
		useFlags |= SHADER_USES_VIEWPORT;
	if (globalPrinters["objectId"].access & GlobalPrinterAccess::READ)
		useFlags |= SHADER_USES_ID;
	if (globalPrinters["s_o"].access & GlobalPrinterAccess::READ)
		useFlags |= SHADER_USES_SCALE_OFFSET;
	if (frontFacingGlobalPrinter->used)
		useFlags |= SHADER_USES_FRONT_FACING;
	if (globalPrinters["flip"].access & GlobalPrinterAccess::WRITE)
		useFlags |= SHADER_USES_FLIP;


	// generate vertex shader consisting of uniform, input and output variables and main function
	{
		CodeWriter w(new StringRefDevice(vertexShader), shaderLanguage);
		printHeader(w, options.positionPrecision);

		printShaderVariables(w, SCOPE_UNIFORM, m2vVariables, options.positionPrecision);
		printShaderVariables(w, SCOPE_UNIFORM, t2vVariables, options.positionPrecision);
		if (useFlags & SHADER_USES_SCALE_OFFSET)
		{
			int arraySize = (scaleOffset->getSize().y + 3) >> 2;
			printShaderVariable(w, SCOPE_UNIFORM, ShaderVariable("s_o", ShaderType::FLOAT, 4, arraySize));
		}
		if (frontFacingGlobalPrinter->used && (options.avoidPixelShader && frontFacingIsUniform))
		{
			// uniform boolean variable for front fracing 
			printShaderVariable(w, SCOPE_UNIFORM, ShaderVariable("f_f", ShaderType::FLOAT, 1));
		}
		
		// vertex attributes
		vertexGlobalPrinter->printVariables(w, options.positionPrecision);

		printShaderVariables(w, SCOPE_OUT, v2pVariables, options.colorPrecision);
		
		w.beginFunction("void", "main");
		w << vertexShaderCode;
		w.endFunction();

		// number of textures in vertex shader
		numTextures.x = int(m2vLargeArrays.size());
	}
	
	// generate pixel shader consisting of uniform and input variables and main function
	{
		CodeWriter w(new StringRefDevice(pixelShader), shaderLanguage);
		printHeader(w, options.colorPrecision);

		printShaderVariables(w, SCOPE_UNIFORM, m2pVariables, options.colorPrecision);
		printShaderVariables(w, SCOPE_UNIFORM, m2poVariables, options.colorPrecision);
		printShaderVariables(w, SCOPE_UNIFORM, t2pVariables, options.colorPrecision);
		if (frontFacingGlobalPrinter->used && !(options.avoidPixelShader && frontFacingIsUniform))
		{
			// uniform boolean variable for front fracing 
			printShaderVariable(w, SCOPE_UNIFORM, ShaderVariable("f_f", ShaderType::FLOAT, 1));
		}
		
		printShaderVariables(w, SCOPE_IN, v2pVariables, options.colorPrecision);
		
		// write output variable if necessary
		if (!w.getLanguage().isOldGLSL())
		{
			printScope(w, SCOPE_OUT);
			printShaderVariable(w, shaderGlobalPrinter->output);
			w << ";\n";
		}
		
		w.beginFunction("void", "main");
		w << pixelShaderCode;
		w.endFunction();
		
		// number of textures in pixel shader
		numTextures.y = int(m2poVariables.size());
	}

	materialVariables.swap(m2vVariables); // uniforms for material -> vertex
	add(materialVariables, m2pVariables); // uniforms for material -> pixel
	add(materialVariables, m2poVariables); // textures
	materialLargeArrays.swap(m2vLargeArrays); // uniforms for material -> pixel that are stored in textures
	
	transformVariables.swap(t2vVariables); // uniforms for transform -> vertex
	add(transformVariables, t2pVariables); // uniforms for transform -> pixel

	vertexGlobalPrinter->vertexBindings.swap(vertexBindings);

	return true;	
}

} // namespace digi
