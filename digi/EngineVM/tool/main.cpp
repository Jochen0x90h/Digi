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
#include <digi/System/File.h>
#include <digi/System/Log.h>
#include <digi/System/ConsoleLogChannel.h>
#include <digi/CodeGenerator/CodeWriter.h>
#include <digi/CodeGenerator/CodeWriterFunctions.h>
#include <digi/EngineVM/Compiler.h>
#include <digi/EngineVM/TargetInfo.h>

#include "HeaderBinder.h"
#include "InitLibraries.h"

using namespace digi;

/*
class PCHGenerator : public clang::SemaConsumer
{
		clang::Sema* semaPtr;
		//clang::MemorizeStatCalls* statCalls; // owned by the FileManager

  public:
    
    PCHGenerator(clang::FileManager& fileManager)
		: semaPtr(NULL)//, statCalls(NULL)
    {
		// install a stat() listener to keep track of all of the stat() calls.
		//this->statCalls = new clang::MemorizeStatCalls();
		//fileManager.addStatCache(statCalls, / *AtBeginning=* /true); // takes ownership of statCalls
    }
    
	virtual void InitializeSema(clang::Sema &sema)
	{
		this->semaPtr = &sema;
	}

	virtual void HandleTranslationUnit(clang::ASTContext &Ctx)
	{
		//if (this->diagnostics->hasErrorOccurred())
		//	return;

		std::vector<unsigned char> buffer;
		llvm::BitstreamWriter stream(buffer);
		clang::PCHWriter writer(stream);

		llvm::sys::Path sysPath("./RenderLib/");
		sysPath.makeAbsolute();

		// emit the PCH file
		writer.WritePCH(*this->semaPtr,
			NULL, // stat cache (clang::MemorizeStatCalls)
			sysPath.c_str()); // isysroot

		Pointer<IODevice> file = File::open("RenderLib.pch", File::WRITE_NEW);
		file->writeData(getData(buffer), getLength(buffer));
		file->close();		
	}
};
*/

/*
void generatePCH()
{
	Compiler compiler;
	compiler.setSearchPaths("./RenderLib/");

	llvm::OwningPtr<PCHGenerator> pchGenerator(new PCHGenerator(*compiler.fileManager));
	
	compiler.compileFromFile("./RenderLib/PrecompiledHeader.h", pchGenerator.get());	
}
*/
bool generatePCH(const fs::path& headerFile, const fs::path& outFile)
{
	// language options
	clang::LangOptions langOptions;
	langOptions.Bool = 1;
	langOptions.CPlusPlus = 1;
	langOptions.RTTI = 0;
	langOptions.AltiVec = 1;
	langOptions.Freestanding = 1;

	// diagnostic options
	clang::DiagnosticOptions diagnosticOptions;
	diagnosticOptions.ShowColumn = 1;
	diagnosticOptions.ShowLocation = 1;
	diagnosticOptions.ShowCarets = 1;
	diagnosticOptions.ShowFixits = 1;
	diagnosticOptions.ShowSourceRanges = 1,
	diagnosticOptions.ShowColors = 1;
	diagnosticOptions.MessageLength = 80;

	// diagnostic
	llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs> ids(new clang::DiagnosticIDs());
	clang::DiagnosticConsumer* diagnosticConsumer = new clang::TextDiagnosticPrinter(
		llvm::errs(),
		diagnosticOptions);
	diagnosticConsumer->BeginSourceFile(langOptions);
	llvm::IntrusiveRefCntPtr<clang::DiagnosticsEngine> diagnosticsEngine(new clang::DiagnosticsEngine(ids, diagnosticConsumer)); // takes ownership of diagnosticClient
	
	// target info
	clang::TargetInfo* targetInfo = new TargetInfo();//clang::TargetInfo::CreateTargetInfo((LLVM_HOSTTRIPLE);
	
	// source and file manager
	clang::FileSystemOptions fileSystemOptions;
	clang::FileManager fileManager(fileSystemOptions);
	clang::SourceManager sourceManager(*diagnosticsEngine, fileManager);
	clang::HeaderSearch headerSearch(fileManager);

	ModuleLoader moduleLoader;

	// preprocessor
	clang::Preprocessor pp(
		*diagnosticsEngine,
		langOptions,
		targetInfo,
		sourceManager,
		headerSearch,
		moduleLoader);
	pp.setPredefines("#define __cplusplus\n");

	// Add input file
	const clang::FileEntry* file = fileManager.getFile(headerFile.string());
	if (!file)
	{
		std::cerr << "Failed to open \'" << headerFile.string() << "\'";
		return false;
	}
	sourceManager.createMainFileID(file);
	pp.EnterMainSourceFile();

	CodeWriter w2(outFile);
	std::stringstream w;
	
	// Parse it
	clang::Token token;
	char last = ' ';
	
	// flag for automatic return insertion
	bool ret = false;
	do
	{
		pp.Lex(token);
		if (diagnosticsEngine->hasErrorOccurred())
			return false;
		if (token.is(clang::tok::eof))
			break;

/*
		// automatic return insertion
		if (ret && !token.is(clang::tok::semi))
		{
			w << '\n';
			last = ' ';
			ret = false;
		}
*/
		std::string text = pp.getSpelling(token);
				
		if (!text.empty())
		{
			if (Ascii::isIdentifier(last) && Ascii::isIdentifier(text[0]))
				w << " ";
			
			w << text;
			
			last = text[text.length() - 1];
		}

		if (token.is(clang::tok::semi) || token.is(clang::tok::l_brace) || token.is(clang::tok::r_brace))
			ret = true;
	} while (true);
	w << '\n';
	
	std::string str = w.str();
	
	w2 << "static const char* " << headerFile.stem().string() << "[] =\n";
	w2.beginScope();
	do
	{
		size_t len = min(size_t(3000), str.length());
		
		w2 << "\"";
		w2 << replace(replace(str.substr(0, len), "\"", "\\\""), "\n", "\\n");
		w2 << "\",\n";
		str.erase(0, len);
	} while (!str.empty());
	w2.endScope(';');

	w2.close();
	return true;
}

// generate binding for all functions in the given header file
void generateBinding(const fs::path& headerFile, const fs::path& outputDir, StringRef macroName,
	const std::map<std::string, std::string>& conditionals,
	const std::map<std::string, std::string>& functionNames, Mode mode)
{
	Compiler compiler(Compiler::NO_LIB);
	//compiler.setSearchPaths("RenderLib/"); // not necessary since we #include with ""
	compiler.predefines +=
		"typedef signed char byte;"
		"typedef unsigned char ubyte;"
		"typedef unsigned short ushort;"
		"typedef unsigned int uint;"
		"\n";

	fs::path outputPath = outputDir / (headerFile.stem() + ".inc.h");
	llvm::OwningPtr<HeaderBinder> headerBinder(new HeaderBinder(File::create(outputPath),
		macroName, conditionals, functionNames, mode));
	compiler.compileFromFile(headerFile, headerBinder.get());
}

void collectFunctions(llvm::OwningPtr<FunctionCollector>& functionCollector, const fs::path& headerFile,
	const std::map<std::string, std::string>& conditionals,
	const std::map<std::string, std::string>& functionNames, Mode mode)
{
	Compiler compiler(Compiler::NO_LIB);
	//compiler.setSearchPaths("RenderLib/"); // not necessary since we #include with ""
	compiler.predefines +=
		"typedef signed char byte;"
		"typedef unsigned char ubyte;"
		"typedef unsigned short ushort;"
		"typedef unsigned int uint;"
		"\n";

	functionCollector->conditionals.insert(conditionals.begin(), conditionals.end());
	functionCollector->functionNames = &functionNames;
	functionCollector->mode = mode;
	compiler.compileFromFile(headerFile, functionCollector.get());
}

// generate binding for all functions in the given header file
void generateWrapper(const fs::path& headerFile, const fs::path& outFile)
{
	Compiler compiler(Compiler::NO_LIB);
	//compiler.setSearchPaths("RenderLib/"); // not necessary since we #include with ""

	llvm::OwningPtr<WrapperGenerator> wrapperGenerator(new WrapperGenerator(
		File::create(outFile)));
	compiler.compileFromFile(headerFile, wrapperGenerator.get());
}


// include math in a namespace that it doesn't interfere with system math.h
namespace math
{
	#undef M_PI
	#undef M_PI_2
	#define M_PI   3.1415927f
	#define M_PI_2 1.5707963f

 	#include "RenderLib/Math.inc.h"
}

static int compare(const char* name, float x, float a, float b)
{
	if (a == b)
		return 0;

	dNotify(name << " fails at " << x << ": " << a << ", " << b);
	return 1;
}

static int compare(const char* name, float x, float a, float b, float epsilon)
{
	if (!(fabs(a - b) > epsilon))
		return 0;

	dNotify(name << " fails at " << x << ": " << a << ", " << b);
	return 1;
}

void testMath()
{
	int fabsErrors = 0;
	int floorErrors = 0;
	int ceilErrors = 0;
	int fmodErrors = 0;
	int sqrtErrors = 0;
	int expErrors = 0;
	int powErrors = 0;
	int logErrors = 0;
	int log10Errors = 0;
	int sinErrors = 0;
	int cosErrors = 0;
	int tanErrors = 0;
	int atan2Errors = 0;
	
	const float e = 0.01f;
	for (int i = -1000; i < 1000; ++i)
	{
		float x = float(i) * 0.01f;
		
		fabsErrors += compare("fabs", x, fabs(x), math::fabs(x));
		floorErrors += compare("floor", x, floor(x), math::floor(x));
		ceilErrors += compare("ceil", x, ceil(x), math::ceil(x));
		fmodErrors += compare("fmod", x, fmod(x, 5.5f), math::fmod(x, 5.5f), e);
		sqrtErrors += compare("sqrt", x, sqrt(x), math::sqrt(x), e);
		expErrors += compare("exp", x, exp(x), math::exp(x), e);
		powErrors += compare("pow", x, pow(x, 3.3f), math::pow(x, 3.3f), e);
		logErrors += compare("log", x, log(x), math::log(x), e);
		log10Errors += compare("log10", x, log10(x), math::log10(x), e);
		sinErrors += compare("sin", x, sin(x), math::sin(x), e);
		cosErrors += compare("cos", x, cos(x), math::cos(x), e);
		tanErrors += compare("tan", x, tan(x), math::tan(x), e);
		atan2Errors += compare("atan2", x, atan2(x, 0.5f), math::atan2(x, 0.5f), e);
	}
	
	// this fails (results 9.2e27)
	float y = math::pow(0.0f, 3.3f);
}

bool generateImplementation(const fs::path& sourceFile, const std::string& name)
{
	llvm::LLVMContext context;

	Compiler compiler(Compiler::NO_LIB);
	compiler.setSearchPaths("./RenderLib/");

	// code generator options (let clang optimize)
	clang::CodeGenOptions codeGenOptions;
	codeGenOptions.DisableLLVMOpts = 1;
	//codeGenOptions.DisableAggregateOpts = 1;
	codeGenOptions.OptimizationLevel = 0;
	//codeGenOptions.OptimizationLevel = 2;
	codeGenOptions.Inlining = clang::CodeGenOptions::NormalInlining;
	//codeGenOptions.Inlining = clang::CodeGenOptions::OnlyAlwaysInlining;
	//codeGenOptions.Inlining = clang::CodeGenOptions::NoInlining;
	
	llvm::OwningPtr<clang::CodeGenerator> astConsumer(clang::CreateLLVMCodeGen(
		*compiler.diagnosticsEngine,
		name,
		codeGenOptions,
		context));	

	// compile
	Pointer<CompileResult> compileResult = compiler.compileFromFile(sourceFile, astConsumer.get());
	if (compileResult == null)
		return false;

	// get compiled module
	llvm::OwningPtr<llvm::Module> module(astConsumer->ReleaseModule());
	if (!module)
		return false;


 	// create pass manager and add passes
	llvm::PassManager passManager;

	compileResult->addPasses(*module, passManager, true);

	// write as llvm assembly
	addPrintPass(passManager, "ll");

	// write as cpp using the cpp backend if it is available
	std::string error;
	llvm::raw_fd_ostream stream((name + ".cpp").c_str(), error, llvm::raw_fd_ostream::F_Binary);
	llvm::formatted_raw_ostream cpp(stream);
	
	llvm::TargetRegistry::iterator it = llvm::TargetRegistry::begin();
	llvm::TargetRegistry::iterator end = llvm::TargetRegistry::end();
	for (; it != end; ++it)
	{
		const llvm::Target& target = *it;
		
		const char* name = target.getName();
		
		if (strcmp(name, "cpp") == 0)
		{
			llvm::TargetMachine* targetMachine = target.createTargetMachine(std::string(), std::string(), std::string());
			targetMachine->addPassesToEmitFile(passManager, cpp, llvm::TargetMachine::CGFT_AssemblyFile, llvm::CodeGenOpt::Default);
		}
	}
	
	passManager.run(*module);
	
	return true;
}

//extern "C" void LLVMInitializeCppBackendTargetInfo();
//extern "C" void LLVMInitializeCppBackendTarget();

// working directory must be digi/EngineVM/tool
int main()
{
	Log::init();
	Log::addChannel(new ConsoleLogChannel());

	initLibraries();
	//LLVMInitializeCppBackendTargetInfo();
	//LLVMInitializeCppBackendTarget();

	fs::path inputDir = "RenderLib";
	fs::path outputDir = "..";

	// generate precompiled headers using clang
	generatePCH(inputDir / "Intermediate.h", outputDir / "Intermediate.inc.h");
	generatePCH(inputDir / "VMOpenGL.h", outputDir / "VMOpenGL.inc.h");

	// function conditional maps
	std::map<std::string, std::string> glConditionals;
	glConditionals["glTexImage3D"] = "#if !defined(GL_ES) || GL_MAJOR >= 3";
	glConditionals["glTexSubImage3D"] = "#if !defined(GL_ES) || GL_MAJOR >= 3";
	glConditionals["glGenVertexArrays"] = "#if GL_MAJOR >= 3";
	glConditionals["glDeleteVertexArrays"] = "#if GL_MAJOR >= 3";
	glConditionals["glBindVertexArray"] = "#if GL_MAJOR >= 3";
	
	// function name maps
	std::map<std::string, std::string> stdlibNames;
	stdlibNames["malloc"] = "malloc16";
	stdlibNames["free"] = "free16";

	std::map<std::string, std::string> trackNames;
	trackNames["eSTF"] = "evalStepTrack";
	trackNames["eBTF"] = "evalBezierTrack";
	trackNames["eWBTF"] = "evalWeightedBezierTrack";
	trackNames["eCTF"] = "evalCatmullRomTrack";
	trackNames["eSTS"] = "evalStepTrack";
	trackNames["eBTS"] = "evalBezierTrack";
	trackNames["eWBTS"] = "evalWeightedBezierTrack";
	trackNames["eCTS"] = "evalCatmullRomTrack";

	std::map<std::string, std::string> empty;

	// generate binding of a llvm virtual machine to system functions (get included in EngineVM/VMFile.cpp)
	generateBinding(inputDir / "stdlib.h", outputDir, "BIND_C_FUNCTION", empty, stdlibNames, USE);
	generateBinding(inputDir / "math.h", outputDir, "BIND_C_FUNCTION", empty, empty, CAST_TO_FUNCTION_TYPE);
	generateBinding(inputDir / "gl.h", outputDir, "BIND_GL_FUNCTION", glConditionals, empty, USE);
	generateBinding(inputDir / "GLWrapper.h", outputDir, "BIND_C_FUNCTION", empty, empty, USE);
	generateBinding(inputDir / "TrackC.h", outputDir, "BIND_C_FUNCTION", empty, trackNames, CAST_TO_FUNCTION_TYPE);
	generateBinding(inputDir / "NoiseC.h", outputDir, "BIND_C_FUNCTION", empty, empty, USE);

	// generate list of functions for machine code cache (gets included in Engine/MCFile.cpp)
	{
		fs::path outputDir = "../../Engine";
		llvm::OwningPtr<FunctionCollector> functionCollector(new FunctionCollector());
		collectFunctions(functionCollector, inputDir / "stdlib.h", empty, stdlibNames, USE);
		collectFunctions(functionCollector, inputDir / "math.h", empty, empty, CAST_TO_FUNCTION_TYPE);
		collectFunctions(functionCollector, inputDir / "gl.h", glConditionals, empty, USE);
		collectFunctions(functionCollector, inputDir / "GLWrapper.h", empty, empty, USE);
		collectFunctions(functionCollector, inputDir / "TrackC.h", empty, trackNames, CAST_TO_FUNCTION_TYPE);
		collectFunctions(functionCollector, inputDir / "NoiseC.h", empty, empty, USE);
		functionCollector->writeSetFunctions(outputDir / "MCSetFunctions.inc.h");
		functionCollector->writeNumFunctions(outputDir / "MCNumFunctions.inc.h");
	}

	// generate wrapper for all gl functions which has cdecl calling convention
	//generateWrapper(inputDir / "gl.h", outputDir / "gl.wrapper.h");

	// test
	//testMath();
	
	//generateImplementation(inputDir / "math.cpp", "math");
}
