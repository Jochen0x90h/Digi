#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Assembly/PrintModulePass.h> // createPrintModulePass
#include <llvm/Bitcode/BitstreamWriter.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/FormattedStream.h>

//#include <clang/Frontend/PCHWriter.h>
#include <clang/Frontend/CodeGenOptions.h> // clang::CodeGenOptions
#include <clang/Sema/SemaConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/CodeGen/ModuleBuilder.h> // clang::CreateLLVMCodeGen

#include <digi/Utility/Ascii.h>
#include <digi/Utility/StringUtility.h>
#include <digi/Utility/VectorUtility.h>
#include <digi/Utility/foreach.h>
#include <digi/System/File.h>
#include <digi/System/Log.h>
#include <digi/System/ConsoleLogChannel.h>
#include <digi/System/MemoryDevices.h>
#include <digi/CodeGenerator/CodeWriter.h>
#include <digi/CodeGenerator/CodeWriterFunctions.h>
#include <digi/EngineVM/Compiler.h>
//#include <digi/SceneConvert/CompileHelper.h>
#include <digi/SceneConvert/GlobalCollector.h>
#include <digi/SceneConvert/SplittedGlobalPrinter.h>

#include "InitLibraries.h"

using namespace digi;


static void writeString(const std::string& str, const fs::path& path)
{
	Pointer<File> file = File::create(path);
	file->write(getData(str), str.size());
	file->close();
}


void compile(const std::string& code, const std::string& astVariables, const std::string& splitVariables,
	const fs::path& outputPath)
{
	llvm::LLVMContext context;
	Compiler compiler(Compiler::INTERMEDIATE);

	// code generator options
	clang::CodeGenOptions codeGenOptions = getCodeGenOptionsForWrite();

	std::string moduleName = "module";

	// create clang code generator (derives from clang::ASTConsumer)
	llvm::OwningPtr<GlobalCollector> astConsumer(new GlobalCollector(clang::CreateLLVMCodeGen(
		*compiler.diagnosticsEngine,
		moduleName,
		codeGenOptions,
		context)));

	// compile
	Pointer<CompileResult> compileResult = compiler.compileFromString(code, astConsumer.get());
	if (compileResult == null)
		return;

	// get compiled module
	llvm::OwningPtr<llvm::Module> module(astConsumer->ReleaseModule());
	if (!module)
		return;

	// create target data
	llvm::OwningPtr<llvm::TargetData> targetDataPtr(compileResult->getTargetData());
	llvm::TargetData* targetData = targetDataPtr.get();

	// get some references (no ownership is taken)
	std::map<std::string, clang::VarDecl*>& decls = astConsumer->decls;
	clang::ASTContext* astContext = compileResult->astContext.get();

	// ast global printer (variable names stay the same)
	std::map<std::string, GlobalPrinterAccess> globalPrinters;
	if (!astVariables.empty())
	{
		ASTGlobalPrinter* astGlobalPrinter = new ASTGlobalPrinter(astContext, decls, targetData);
		ptrdiff_t len = astVariables.length();
		ptrdiff_t s = 0;
		while (s < len)
		{
			ptrdiff_t e = find(astVariables, ',', s, len);

			globalPrinters[substring(astVariables, s, e)] = astGlobalPrinter;
			
			s = e + 1;
		}
	}
	
	// splitted global printer (variables get converted into arrays of int and float)
	{
		ptrdiff_t len = splitVariables.length();
		ptrdiff_t s = 0;
		while (s < len)
		{
			ptrdiff_t e = find(splitVariables, ',', s, len);
			std::string variable = substring(splitVariables, s, e);
			addSplittedGlobalPrinter(globalPrinters, astContext, decls, targetData, *module, variable, '$' + variable, false);
			
			s = e + 1;
		}
	}
	
	// scalarize
	module.reset(scalarizeModule(*module, compileResult));

	// print
	std::string outputCode;
	std::map<std::string, Pointer<IODevice> > deviceMap;
	deviceMap["main"] = new StringRefDevice(outputCode);
	printModule(*module, compileResult, globalPrinters, deviceMap, Language::JS);
	
	writeString(outputCode, outputPath);
}

void compileFromFile(const fs::path& path, const std::string& astVariables, const std::string& splitVariables,
	const fs::path& outputDir)
{
	Pointer<File> file = File::open(path, File::READ);
	std::string code(size_t(file->getSize()), 0);
	file->read((void*)code.data(), code.size());
	file->close();
	compile(code, astVariables, splitVariables, outputDir / (path.stem() + ".js"));
}


void genMatrixMul(const fs::path& outputDir)
{
	std::string code;
	{
		Pointer<IODevice> dev = new StringRefDevice(code);
		CodeWriter w2(dev);
		
		// variables
		writeBindingVariable(w2, "a", "float4x4");
		writeBindingVariable(w2, "b", "float4x4");
		writeBindingVariable(w2, "r", "float4x4");
		w2.writeLine();
		
		// function
		w2 << "extern \"C\" void main()\n";
		w2.beginScope();
		w2 << "r = a * b;\n";
		w2.endScope();

		w2.close();
	}

	compile(code, "", "a,b,r", outputDir / "matrixMul.js");
}

void genMatrixInv(const fs::path& outputDir)
{
	std::string code;
	{
		Pointer<IODevice> dev = new StringRefDevice(code);
		CodeWriter w2(dev);
		
		// variables
		writeBindingVariable(w2, "a", "float4x4");
		writeBindingVariable(w2, "r", "float4x4");
		w2.writeLine();
		
		// function
		w2 << "extern \"C\" void main()\n";
		w2.beginScope();
		w2 << "r = inv(a);\n";
		w2.endScope();

		w2.close();
	}

	compile(code, "", "a,r", outputDir / "matrixInv.js");
}


int main()
{
	Log::init();
	Log::addChannel(new ConsoleLogChannel());

	initLibraries();

	fs::path inputDir = "Source";
	fs::path outputDir = "Source";

	genMatrixMul(outputDir);
	genMatrixInv(outputDir);

	// create perspective or orthographic projection matrix from camera projection parameters
	compileFromFile(inputDir / "matrix4x4Projection.cpp", "aspect", "p,r", outputDir);

	// tracks
	// evalStepTrack is implemented by hand, loop in evalWeightedBezierTrack unrolled by hand
	compileFromFile(inputDir / "evalBezierTrack.cpp", "xValues,keys,numKeys,x,index,result", "", outputDir);
	compileFromFile(inputDir / "evalWeightedBezierTrack.cpp", "xValues,yValues,numKeys,x,index,result", "", outputDir);
	compileFromFile(inputDir / "evalCatmullRomTrack.cpp", "keys,x,result", "", outputDir);

	// create a very small frustum for 1x1 picking render target
	compileFromFile(inputDir / "pickArea.cpp", "x,y", "p,r", outputDir);
	
	// noise
	compileFromFile(inputDir / "noise2f.cpp", "x,y,result,perm,grad", "", outputDir);
	compileFromFile(inputDir / "noise3f.cpp", "x,y,z,result,perm,grad", "", outputDir);

	// camera control
	compileFromFile(inputDir / "cameraY.cpp", "tx,ty,tz,rx,ry,rz,di", "r", outputDir);
	compileFromFile(inputDir / "cameraZ.cpp", "tx,ty,tz,rx,ry,rz,di", "r", outputDir);
}
