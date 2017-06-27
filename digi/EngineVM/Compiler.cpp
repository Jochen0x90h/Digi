#include <iostream>

// llvm
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>

#include <llvm/Assembly/PrintModulePass.h> // createPrintModulePass

#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/Support/TypeBuilder.h>

#include <llvm/Transforms/Scalar.h> // otimizations, e.g. createScalarReplAggregatesPass, createInstructionCombiningPass, createLoopSimplifyPass
#include <llvm/Transforms/IPO.h> // createFunctionInliningPass

#include <llvm/Analysis/Dominators.h>
#include <llvm/Analysis/LoopInfo.h>

// clang
#include <clang/Basic/FileSystemOptions.h>

#include <clang/Lex/Preprocessor.h>

#include <clang/AST/ASTConsumer.h>
#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclGroup.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/RecordLayout.h>

#include <clang/Parse/ParseAST.h>


#include <digi/Utility/Ascii.h>
#include <digi/Utility/StringUtility.h>
#include <digi/Utility/VectorUtility.h>
#include <digi/Utility/foreach.h>
#include <digi/System/Log.h>
#include <digi/CodeGenerator/NameMangler.h>

#include "MemoryPass.h"
#include "TargetInfo.h"
#include "PrintModulePass.h"
#include "Compiler.h"


namespace digi {

// CompileResult

CompileResult::CompileResult(clang::DiagnosticsEngine& diagnosticsEngine, clang::LangOptions& langOptions,
	clang::TargetInfo& targetInfo, clang::SourceManager& sourceManager, clang::HeaderSearch& headerSearch,
	const std::string& predefines)
{
	this->moduleLoader.reset(new ModuleLoader());

	// create preprocessor
	this->preprocessor.reset(new clang::Preprocessor(
		diagnosticsEngine,
		langOptions,
		&targetInfo,
		sourceManager,
		headerSearch,
		*this->moduleLoader));
   
	// parse with ParseAST()
	this->astContext.reset(new clang::ASTContext(
		langOptions,
		sourceManager,
		&targetInfo,
		this->preprocessor->getIdentifierTable(),
		this->preprocessor->getSelectorTable(),
		this->preprocessor->getBuiltinInfo(),
		0 /*size_reserve*/));
		
	this->preprocessor->setPredefines(predefines);
}

CompileResult::~CompileResult()
{
}

void CompileResult::addPasses(llvm::Module& module, llvm::PassManager& passManager, bool vm)
{
	// target data
	const char* targetDescription = this->getTargetDescription();

	// workaround: do inlining separately
	{
		llvm::PassManager passManager;
		passManager.add(new llvm::TargetData(targetDescription));
		#ifndef NDEBUG
			addPrintPass(passManager, "input.ll");
		#endif

		// inline functions (threshold, typical 200, very aggressive 2000)
		passManager.add(llvm::createFunctionInliningPass(2000));//vm ? 200 : 2000));
		#ifndef NDEBUG
			addPrintPass(passManager, "inline.ll");
		#endif

		passManager.run(module);	
	}
	
	passManager.add(new llvm::TargetData(targetDescription));
	
	if (!vm)
	{
		// replace calls to llvm.memcpy by element copy
		passManager.add(new MemoryPass());		
		#ifndef NDEBUG
			addPrintPass(passManager, "memory.ll");
		#endif
			
		// convert memory variables to registers
		passManager.add(llvm::createPromoteMemoryToRegisterPass());		
		#ifndef NDEBUG
			addPrintPass(passManager, "mem2reg.ll");
		#endif
	}
	else
	{
		// convert memory variables and structs to llvm registers (size of struct, typical 128)
		passManager.add(llvm::createScalarReplAggregatesPass(1000000));
	}	
	/*
		sparse conditional constant propagation
		1. Assumes values are constant unless proven otherwise
		2. Assumes BasicBlocks are dead unless proven otherwise
		3. Proves values to be constant, and replaces them with constants
		4. Proves conditional branches to be unconditional
	*/
	//passManager.add(llvm::createSCCPPass());

	/*
		InstructionCombining - Combine instructions to form fewer, simple instructions.  	
		1. If a binary operator has a constant operand, it is moved to the RHS
		2. Bitwise operators with constant operands are always grouped so that
			shifts are performed first, then or's, then and's, then xor's.
		3. Compare instructions are converted from <,>,<=,>= to ==,!= if possible
		4. All cmp instructions on boolean values are replaced with logical ops
		5. add X, X is represented as (X*2) => (X << 1)
		6. Multiplies with a power-of-two constant argument are transformed into shifts.
		... etc.
	*/
	passManager.add(llvm::createInstructionCombiningPass());
	#ifndef NDEBUG
		addPrintPass(passManager, "instCombine.ll");
	#endif

	// global value numbering
//	passManager.add(llvm::createGVNPass());
	
	// remove dead basic blocks and branches that are known at compile time
	passManager.add(llvm::createCFGSimplificationPass());

	// optimize again
	passManager.add(llvm::createInstructionCombiningPass());

	// dead code elimination
	passManager.add(llvm::createDeadInstEliminationPass());
	
	// dead store elimination
	passManager.add(llvm::createDeadStoreEliminationPass());
	
	// dead type elimination
	//passManager.add(llvm::createDeadTypeEliminationPass());

	// generate info passes
	passManager.add(new llvm::DominatorTree());
	passManager.add(new llvm::LoopInfo());
	
	// canonicalize natrual loops (other optimizations such as LICM = Loop Invariant Code Motion benefit from this)
	passManager.add(llvm::createLoopSimplifyPass());

	#ifndef NDEBUG
		addPrintPass(passManager, "output.ll");
	#endif
}

size_t CompileResult::getSize(clang::RecordDecl* recordDecl)
{
	//return size_t((this->astContext->getASTRecordLayout(recordDecl).getSize() + 7) / 8);
	return size_t(this->astContext->getASTRecordLayout(recordDecl).getSize().getQuantity());
}


// Compiler

static void defineType(clang::MacroBuilder& macroBuilder, const llvm::Twine& name, clang::TargetInfo::IntType type)
{
	macroBuilder.defineMacro(name, clang::TargetInfo::getTypeName(type));
}
/*
static void defineTypeWidth(clang::MacroBuilder& macroBuilder, llvm::StringRef name, clang::TargetInfo::IntType type,
	const clang::TargetInfo& targetInfo)
{
	macroBuilder.defineMacro(name, llvm::Twine(targetInfo.getTypeWidth(type)));
}
*/
static void defineExactWidthIntType(clang::MacroBuilder& macroBuilder, clang::TargetInfo::IntType type,
	const clang::TargetInfo& targetInfo)
{
	int typeWidth = targetInfo.getTypeWidth(type);
	defineType(macroBuilder, "__INT" + llvm::Twine(typeWidth) + "_TYPE__", type);

	llvm::StringRef constSuffix(clang::TargetInfo::getTypeConstantSuffix(type));
	if (!constSuffix.empty())
		macroBuilder.defineMacro("__INT" + llvm::Twine(typeWidth) + "_C_SUFFIX__", constSuffix);
}

Compiler::Compiler(LibType libType)
	#ifndef NDEBUG
		//: diagnosticFile("diagnostic.txt", diagnosticError, llvm::raw_fd_ostream::F_Binary)
	#endif
{
	// language options
	this->langOptions.Bool = 1;
	this->langOptions.CPlusPlus = 1;
	this->langOptions.RTTI = 0;
	this->langOptions.AltiVec = 1;
	this->langOptions.OpenCL = 0;
	this->langOptions.Freestanding = 1; // no magic for main

	// diagnostic options (clang::TextDiagnosticPrinter takes a pointer)
	this->diagnosticOptions.ShowColumn = 1;
	this->diagnosticOptions.ShowLocation = 1;
	this->diagnosticOptions.ShowCarets = 1;
	this->diagnosticOptions.ShowFixits = 1;
	this->diagnosticOptions.ShowSourceRanges = 1,
	this->diagnosticOptions.ShowColors = 0;
	this->diagnosticOptions.MessageLength = 80;

	// diagnostic (llvm/tools/clang/lib/Frontend/TextDiagnosticPrinter.cpp, TextDiagnosticPrinter::HandleDiagnostic) 
	this->ids = new clang::DiagnosticIDs();
	this->diagnosticConsumer = new clang::TextDiagnosticPrinter(
		#ifndef NDEBUG
			//diagnosticFile,
			llvm::errs(),
		#else
			llvm::nulls(),
		#endif
		diagnosticOptions);
	this->diagnosticConsumer->BeginSourceFile(langOptions);
	this->diagnosticsEngine = new clang::DiagnosticsEngine(this->ids, this->diagnosticConsumer); // takes ownership of diagnosticConsumer

	// target info (contains size of built-in types, e.g. int, long, pointer)
	this->targetInfo.reset(new TargetInfo());
//const char* td = this->targetInfo->getTargetDescription();

	// source and file manager
	clang::FileSystemOptions fileSystemOptions;
	this->fileManager.reset(new clang::FileManager(fileSystemOptions));
	this->headerSearch.reset(new clang::HeaderSearch(*fileManager));

	// build predefines
	{
		std::string predefineBuffer;
		predefineBuffer.reserve(4080);
		llvm::raw_string_ostream predefines(predefineBuffer); 
		clang::MacroBuilder macroBuilder(predefines);
		
		macroBuilder.defineMacro("__cplusplus");
		macroBuilder.defineMacro("__DIGI__");

		// fixed width types
		macroBuilder.defineMacro("__INT" + llvm::Twine(this->targetInfo->getCharWidth()) + "_TYPE__", "char"); 
			      
		if (this->targetInfo->getShortWidth() > this->targetInfo->getCharWidth())
			defineExactWidthIntType(macroBuilder, clang::TargetInfo::SignedShort, *this->targetInfo);

		if (this->targetInfo->getIntWidth() > this->targetInfo->getShortWidth())
			defineExactWidthIntType(macroBuilder, clang::TargetInfo::SignedInt, *this->targetInfo);

		if (this->targetInfo->getLongWidth() > this->targetInfo->getIntWidth())
			defineExactWidthIntType(macroBuilder, clang::TargetInfo::SignedLong, *this->targetInfo);

		if (this->targetInfo->getLongLongWidth() > this->targetInfo->getLongWidth())
			defineExactWidthIntType(macroBuilder, clang::TargetInfo::SignedLongLong, *this->targetInfo);
	    
		// set defines
		this->predefines = predefines.str();
	}

	// lib type
	if (libType != NO_LIB)
	{
		static const char* defines[] =
		{
			"#define NULL __null\n"
			"#define ALIGN(x) __attribute__((__aligned__(x)))\n",
		
			"#define make_bool2(x, y) (bool2)((x), (y))\n"
			"#define make_byte2(x, y) (byte2)((x), (y))\n"
			"#define make_ubyte2(x, y) (ubyte2)((x), (y))\n"
			"#define make_short2(x, y) (short2)((x), (y))\n"
			"#define make_ushort2(x, y) (ushort2)((x), (y))\n"
			"#define make_int2(x, y) (int2)((x), (y))\n"
			"#define make_uint2(x, y) (uint2)((x), (y))\n"
			"#define make_float2(x, y) (float2)((x), (y))\n",

			"#define make_bool3(x, y, z) (bool3)((x), (y), (z))\n"
			"#define make_byte3(x, y, z) (byte3)((x), (y), (z))\n"
			"#define make_ubyte3(x, y, z) (ubyte3)((x), (y), (z))\n"
			"#define make_short3(x, y, z) (short3)((x), (y), (z))\n"
			"#define make_ushort3(x, y, z) (ushort3)((x), (y), (z))\n"
			"#define make_int3(x, y, z) (int3)((x), (y), (z))\n"
			"#define make_uint3(x, y, z) (uint3)((x), (y), (z))\n"
			"#define make_float3(x, y, z) (float3)((x), (y), (z))\n",

			"#define make_bool4(x, y, z, w) (bool4)((x), (y), (z), (w))\n"
			"#define make_byte4(x, y, z, w) (byte4)((x), (y), (z), (w))\n"
			"#define make_ubyte4(x, y, z, w) (ubyte4)((x), (y), (z), (w))\n"
			"#define make_short4(x, y, z, w) (short4)((x), (y), (z), (w))\n"
			"#define make_ushort4(x, y, z, w) (ushort4)((x), (y), (z), (w))\n"
			"#define make_int4(x, y, z, w) (int4)((x), (y), (z), (w))\n"
			"#define make_uint4(x, y, z, w) (uint4)((x), (y), (z), (w))\n"
			"#define make_float4(x, y, z, w) (float4)((x), (y), (z), (w))\n",
		};

	/*
		#ifdef _WIN32
			this->setSearchPaths("c:/Digisnap/Coding/Digi/digi/EngineVM/tool/RenderLib");
		#else
			this->setSearchPaths("/Users/jwilhelmy/Coding/Digi/digi/EngineVM/tool/RenderLib");
		#endif
	
		if (libType == INTERMEDIATE)
		{
			this->predefines += "#include <Intermediate.h>\n";
		}
		else
		{
			this->predefines += "#include <VMOpenGL.h>\n";
		}
	*/

		if (libType == INTERMEDIATE)
		{
			#include "Intermediate.inc.h"

			foreach (const char* s, defines)
			{
				this->predefines += s;
			}

			foreach (const char* s, Intermediate)
			{
				this->predefines += s;
			}
		}
		else
		{
			#include "VMOpenGL.inc.h"

			foreach (const char* s, defines)
			{
				this->predefines += s;
			}
		
			foreach (const char* s, VMOpenGL)
			{
				this->predefines += s;
			}
		}
	}
}

void Compiler::setSearchPaths(const std::string& paths)
{
	std::vector<clang::DirectoryLookup> dirs;

	size_t len = paths.length();
	size_t s = 0;
	while (s < len)
	{
		size_t e = find(paths, ';', s, len);
		
		std::string path = substring(paths, s, e);

		if (!path.empty())
		{
			const clang::DirectoryEntry* directoryEntry = fileManager->getDirectory(path);
			dirs += clang::DirectoryLookup(directoryEntry, clang::SrcMgr::C_User, true, false);
		}
		
		s = e + 1;
	}

	this->headerSearch->SetSearchPaths(dirs, 0, 0, false);
}

Pointer<CompileResult> Compiler::compileFromFile(const fs::path& path, clang::ASTConsumer* astConsumer)
{
	// get input file
	const clang::FileEntry* file = this->fileManager->getFile(path.string());
	if (!file)
	{
		dError("Failed to open \'" << path.string() << "\'");
		return null;
	}
	clang::SourceManager sourceManager(*this->diagnosticsEngine, *this->fileManager);
	
	// load pch file
	//clang::PCHReader* pchReader = NULL;
	{
	}
	
	sourceManager.createMainFileID(file);

	return this->compile(astConsumer, sourceManager/*, pchReader*/);	
}

Pointer<CompileResult> Compiler::compileFromString(const std::string& str, clang::ASTConsumer* astConsumer)
{
	clang::SourceManager sourceManager(*this->diagnosticsEngine, *this->fileManager);

	// load pch file
//	clang::PCHReader* pchReader = NULL;
	if (!this->pch.empty())
	{
		/*pchReader = new clang::PCHReader(sourceManager, *this->fileManager, *this->diagnostic,
			"c:/Digisnap/Coding/digi/Tools/CompilerTool/RenderLib/" / *NULL* /);
		
		pchReader->ReadPCH(this->pch);
		*/
	}

	llvm::MemoryBuffer* buffer = llvm::MemoryBuffer::getMemBuffer(str);
	sourceManager.createMainFileIDForMemBuffer(buffer); // takes ownership of buffer
	//clang::FileID id = sourceManager.getMainFileID();

	return this->compile(astConsumer, sourceManager/*, pchReader*/);	
}


Pointer<CompileResult> Compiler::compile(clang::ASTConsumer* astConsumer, clang::SourceManager& sourceManager /*,
	clang::PCHReader* pchReader*/)
{
	// clear diagnostics engine
	this->diagnosticsEngine->Reset();
	
	Pointer<CompileResult> result = new CompileResult(
		*this->diagnosticsEngine,
		this->langOptions,
		*this->targetInfo,
		sourceManager,
		*this->headerSearch,
		this->predefines);

/*
	if (pchReader != NULL)
	{
		pchReader->setPreprocessor(*result->preprocessor);
		pchReader->InitializeContext(*result->astContext);

		llvm::OwningPtr<clang::ExternalASTSource> astSource(pchReader);
		result->astContext->setExternalSource(astSource); // takes ownership of pchReader		
	}
*/
	clang::ParseAST( // calls preprocessor.EnterMainSourceFile() for us
		*result->preprocessor,
		astConsumer,
		*result->astContext);
		
	// clear file id's that were collected during compilation
	this->headerSearch->ClearFileInfo();

	// check if an compilation error has occured
	if (this->diagnosticsEngine->hasErrorOccurred())
		return null;

	return result;
}

void addPrintPass(llvm::PassManager& passManager, StringRef name)
{
	passManager.add(new PrintModulePass(name));
}

namespace {

	void setStdCall(llvm::Module* module, llvm::StringRef name)
	{
		// get function
		llvm::Function* function = module->getFunction(name);

		// if the function is not used we don't need to set calling convention
		if (function == NULL)
			return;

		function->setCallingConv(llvm::CallingConv::X86_StdCall);
		
		llvm::Function::use_iterator it = function->use_begin();
		llvm::Function::use_iterator end = function->use_end();
		for (; it != end; ++it)
		{
			llvm::User* user = *it;
			
			if (llvm::CallInst* callInst = llvm::dyn_cast<llvm::CallInst>(user))
			{
				callInst->setCallingConv(llvm::CallingConv::X86_StdCall);
			}
		}
	}

	enum Base
	{
		BASE_BOOL,
		BASE_INT,
		BASE_UINT,
		BASE_FLOAT
	};

	struct ConvertInfo
	{	
		VectorInfo::Type type;
		Base base;
		int llvmType;
	};

	struct ReinterpretInfo
	{	
		VectorInfo::Type srcType;
		VectorInfo::Type destType;
		int llvmType;
	};

	llvm::Type* getMatchingIntType(llvm::Type* type)
	{
			llvm::Type* intType = llvm::Type::getIntNTy(type->getContext(), type->getScalarSizeInBits());
			if (const llvm::VectorType* vectorType = llvm::dyn_cast<llvm::VectorType>(type))
							intType = llvm::VectorType::get(intType, vectorType->getNumElements());
			return intType;
	}

	//! implemented as round(), not round to even
	llvm::Value* createRoundToEven(llvm::IRBuilder<>& builder, llvm::Value* input, bool positive)
	{
		llvm::Type* inputType = input->getType();

		// create 0.5 (or vector of 0.5)
		llvm::Value* h = llvm::ConstantFP::get(inputType, 0.5);

		if (positive)
		{
			// input is only positive
			return builder.CreateFAdd(input, h);
		}
		else
		{
			// create integer type of same size as inputType
			llvm::Type* intType = getMatchingIntType(inputType);
			
			// create sign mask (32 bit for float, 64 bit for double)
			uint64_t intValue = inputType->getScalarSizeInBits() == 32 ? UINT64_C(0x7fffffff) : UINT64_C(0x7fffffffffffffff);
			llvm::Value* c = llvm::ConstantInt::get(intType, intValue);

			// bitcast input float to int
			llvm::Value* iValue1 = builder.CreateBitCast(input, intType);

			// split in abs and sign
			llvm::Value* absValue = builder.CreateAnd(iValue1, c);
			llvm::Value* signValue = builder.CreateAnd(iValue1, builder.CreateNot(c));

			// bitcast abs value to float
			llvm::Value* fValue1 = builder.CreateBitCast(absValue, inputType);

			// add 0.5
			llvm::Value* fValue2 = builder.CreateFAdd(fValue1, h);

			// bitcast to int
			llvm::Value* iValue2 = builder.CreateBitCast(fValue2, intType);

			// add sign
			llvm::Value* iValue3 = builder.CreateOr(iValue2, signValue);

			// bitcast to float
			return builder.CreateBitCast(iValue3, inputType);
		}
	}
	
	llvm::Value* createClamp(llvm::IRBuilder<>& builder, llvm::Value* input, double minValue, double maxValue)
	{
		llvm::Type* inputType = input->getType();

		//! llvm workaround, test again with llvm 3.0
		int numElements = 1;
		if (const llvm::VectorType* vectorType = llvm::dyn_cast<llvm::VectorType>(inputType))
		{
			inputType = vectorType->getElementType();
			numElements = vectorType->getNumElements();
		}

		// create min/max values (or vector)
		llvm::Value* minV = llvm::ConstantFP::get(inputType, minValue);
		llvm::Value* maxV = llvm::ConstantFP::get(inputType, maxValue);

		if (numElements == 1)
		{
			// input < minValue ? minValue : input
			llvm::Value* fValue1 = builder.CreateSelect(builder.CreateFCmpOLT(input, minV), minV, input);

			// fValue1 < maxValue ? fValue1 : maxValue
			return builder.CreateSelect(builder.CreateFCmpOLT(fValue1, maxV), fValue1, maxV);
		}
		else
		{
			llvm::Value* fValue1 = input;
			
			for (int i = 0; i < numElements; ++i)
			{
				llvm::Value* index = builder.getInt32(i);
				
				llvm::Value* fValue2 = builder.CreateExtractElement(fValue1, index);
				
				llvm::Value* fValue3 = builder.CreateSelect(builder.CreateFCmpOLT(fValue2, minV), minV, fValue2);
				llvm::Value* fValue4 = builder.CreateSelect(builder.CreateFCmpOLT(fValue3, maxV), fValue3, maxV);
				
				fValue1 = builder.CreateInsertElement(fValue1, fValue4, index);
			}
			
			return fValue1;
		}
	}

} // anonymous namespace


// for writeForMC.cpp
void setStdCall(llvm::Module* module)
{
	#define BIND_GL_FUNCTION(module, vmName, hostName) \
		setStdCall(module, vmName)

	#include "gl.inc.h"

	#undef BIND_GL_FUNCTION
}

void implementConvertFunctions(llvm::Module* module)
{
	llvm::LLVMContext& context = module->getContext();

	//std::vector<llvm::Type*> argumentTypes(1);

	llvm::Type* llvmTypes[7];
	llvmTypes[0] = llvm::Type::getInt1Ty(context);
	llvmTypes[1] = llvm::Type::getInt8Ty(context);
	llvmTypes[2] = llvm::Type::getInt16Ty(context);
	llvmTypes[3] = llvm::Type::getInt32Ty(context);
	//llvmTypes[4] = llvm::Type::getInt64Ty(context);
	llvmTypes[5] = llvm::Type::getFloatTy(context);
	//llvmTypes[6] = llvm::Type::getDoubleTy(context);

	const ConvertInfo types[] =
	{
		{VectorInfo::BOOL,   BASE_BOOL,  0},
		{VectorInfo::BYTE,   BASE_INT,   1},
		{VectorInfo::UBYTE,  BASE_UINT,  1},
		{VectorInfo::SHORT,  BASE_INT,   2},
		{VectorInfo::USHORT, BASE_UINT,  2},
		{VectorInfo::INT,    BASE_INT,   3},
		{VectorInfo::UINT,   BASE_UINT,  3},
		//{VectorInfo::LONG,   BASE_INT,   4},
		//{VectorInfo::ULONG,  BASE_UINT,  4},
		{VectorInfo::FLOAT,  BASE_FLOAT, 5},
		//{VectorInfo::DOUBLE, BASE_FLOAT, 6}
	};
	
	// iterate over vector lengths
	for (int length = 1; length <= 4; ++length)
	{
		foreach (const ConvertInfo& dt, types)
		{
			if (dt.base == BASE_FLOAT)
			{
				// create abs(<dt>)

				// create mangled function name	
				std::string name = NameMangler::mangle("abs", VectorInfo(dt.type, length));
		
				// implement the function only if it is used
				if (llvm::Function* function = module->getFunction(name))
				{
					// set internal linkage that the functions are removed after inlining
					function->setLinkage(llvm::Function::InternalLinkage);

					// create basic block
					llvm::BasicBlock* basicBlock = llvm::BasicBlock::Create(context, "Entry", function);
					llvm::IRBuilder<> builder(basicBlock);

					// abs
					llvm::Value* input = function->arg_begin();
					llvm::Type* inputType = input->getType();
					llvm::Type* intType = getMatchingIntType(inputType);
					
					// bit-cast to int
					llvm::Value* iValue = builder.CreateBitCast(input, intType);

					// mask out sign bit
					uint64_t intValue = dt.type == VectorInfo::FLOAT ? UINT64_C(0x7fffffff) : UINT64_C(0x7fffffffffffffff);
					llvm::Value* c = llvm::ConstantInt::get(intType, intValue);
					llvm::Value* aValue = builder.CreateAnd(iValue, c);

					// bit-cast back to float
					llvm::Value* absValue = builder.CreateBitCast(aValue, input->getType());

					builder.CreateRet(absValue);
				}
			}
			
			foreach (const ConvertInfo& st, types)
			{
				// create convert_<dt>(<st>)
				if (dt.base != BASE_BOOL) // && st.base != BASE_BOOL) 
				{
					// create mangled function name (e.g. convert_float3(int3) -> _Z14convert_float3Dv3_i)
					std::string name = NameMangler::mangle("convert_" + VectorInfo(dt.type, length).toString(),
						VectorInfo(st.type, length));

					// implement the function only if it is used
					if (llvm::Function* function = module->getFunction(name))
					{
						// set internal linkage that the functions are removed after inlining
						function->setLinkage(llvm::Function::InternalLinkage);

						// create return type
						llvm::Type* returnType = llvmTypes[dt.llvmType];//llvm::Type::getFloatTy(context);
						if (length > 1)
							returnType = llvm::VectorType::get(returnType, length);
						
						// create basic block
						llvm::BasicBlock* basicBlock = llvm::BasicBlock::Create(context, "Entry", function);
						llvm::IRBuilder<> builder(basicBlock);

						// convert
						llvm::Value* input = function->arg_begin();
						bool smaller = dt.llvmType < st.llvmType;
						llvm::Value* converted;
						if (dt.llvmType == st.llvmType)
						{
							converted = input;
						}
						else
						{
							// switch result type
							switch (dt.base)
							{
							/*case BASE_BOOL:
								switch (st.base)
								{
								case BASE_BOOL:
								case BASE_INT:
								case BASE_UINT:
									// result = input != 0
									converted = builder.CreateICmpNE(input, llvm::ConstantInt::get(input->getType(), 0));
									break;
								case BASE_FLOAT:
									// result = input != 0.0
									converted = builder.CreateFCmpONE(input, llvm::ConstantFP::get(input->getType(), 0.0));
								}
								break;*/
							case BASE_INT:
								// destination is integer
								switch (st.base)
								{
								case BASE_INT:
									if (smaller)
										converted = builder.CreateTrunc(input, returnType);
									else
										converted = builder.CreateSExt(input, returnType);
									break;
								case BASE_BOOL:
								case BASE_UINT:
									if (smaller)
										converted = builder.CreateTrunc(input, returnType);
									else
										converted = builder.CreateZExt(input, returnType);
									break;
								case BASE_FLOAT:
									converted = builder.CreateFPToSI(input, returnType);
									break;
								}
								break;
							case BASE_UINT:
								// destination is unsigned integer
								switch (st.base)
								{
								case BASE_INT:
									if (smaller)
										converted = builder.CreateTrunc(input, returnType);
									else
										converted = builder.CreateSExt(input, returnType);
									break;
								case BASE_BOOL:
								case BASE_UINT:
									if (smaller)
										converted = builder.CreateTrunc(input, returnType);
									else
										converted = builder.CreateZExt(input, returnType);
									break;
								case BASE_FLOAT:
									converted = builder.CreateFPToUI(input, returnType);
									break;
								}
								break;
							case BASE_FLOAT:
								// destination is floating point
								switch (st.base)
								{
								case BASE_INT:
									converted = builder.CreateSIToFP(input, returnType);
									break;
								case BASE_BOOL:
								case BASE_UINT:
									converted = builder.CreateUIToFP(input, returnType);
									break;
								case BASE_FLOAT:
									if (smaller)
										converted = builder.CreateFPTrunc(input, returnType);
									else
										converted = builder.CreateFPExt(input, returnType);
									break;
								}					
								break;
							}
						}
						builder.CreateRet(converted);
					}
				}
				
				// create convert_<dt>_rte(<st>)
				//! current implementation rounds 0.5 always up
				if ((dt.base == BASE_INT || dt.base == BASE_UINT) && st.base == BASE_FLOAT)
				{
					// create mangled function name (e.g. convert_int3_rte(float3) -> _Z16convert_int3_rteDv3_f)
					std::string name = NameMangler::mangle("convert_" + VectorInfo(dt.type, length).toString() + "_rte",
						VectorInfo(st.type, length));

					// implement the function only if it is used
					if (llvm::Function* function = module->getFunction(name))
					{
						// set internal linkage that the functions are removed after inlining
						function->setLinkage(llvm::Function::InternalLinkage);
						
						// create basic block
						llvm::BasicBlock* basicBlock = llvm::BasicBlock::Create(context, "Entry", function);
						llvm::IRBuilder<> builder(basicBlock);

						// get input
						llvm::Value* input = function->arg_begin();
						
						// create return type
						llvm::Type* returnType = llvmTypes[dt.llvmType];
						if (length > 1)
							returnType = llvm::VectorType::get(returnType, length);
						
						llvm::Value* converted;
						
						// switch result type
						switch (dt.base)
						{
						case BASE_INT:
							{								
								// round
								llvm::Value* fValue = createRoundToEven(builder, input, false);
								
								// convert to int
								converted = builder.CreateFPToSI(fValue, returnType);
							}
							break;
						case BASE_UINT:
							{
								// round
								llvm::Value* fValue = createRoundToEven(builder, input, true);
								
								// convert to uint
								converted = builder.CreateFPToUI(fValue, returnType);
							}
							break;
						}
						builder.CreateRet(converted);				
					}
				}

				// create convert_<dt>_sat_rte(<st>)
				//! current implementation rounds 0.5 always up
				if ((dt.base == BASE_INT || dt.base == BASE_UINT) && st.base == BASE_FLOAT)
				{
					// create mangled function name (e.g. convert_int3_sat_rte(float3) -> _Z20convert_int3_sat_rteDv3_f)
					std::string name = NameMangler::mangle("convert_" + VectorInfo(dt.type, length).toString() + "_sat_rte",
						VectorInfo(st.type, length));

					// implement the function only if it is used
					if (llvm::Function* function = module->getFunction(name))
					{
						// set internal linkage that the functions are removed after inlining
						function->setLinkage(llvm::Function::InternalLinkage);
						
						// create basic block
						llvm::BasicBlock* basicBlock = llvm::BasicBlock::Create(context, "Entry", function);
						llvm::IRBuilder<> builder(basicBlock);

						// get input
						llvm::Value* input = function->arg_begin();
						//llvm::Type* inputType = input->getType();
						
						// create return type
						llvm::Type* returnType = llvmTypes[dt.llvmType];
						if (length > 1)
							returnType = llvm::VectorType::get(returnType, length);
						
						llvm::Value* converted;
						
						// switch result type
						switch (dt.base)
						{
						case BASE_INT:
							{
								// round
								llvm::Value* fValue1 = createRoundToEven(builder, input, false);
								
								// clamp
								int numBits = returnType->getScalarSizeInBits() - 1;
								double minValue = -double(uint64_t(1) << (numBits));
								double maxValue = double((uint64_t(1) << numBits) - 1);
								llvm::Value* fValue2 = createClamp(builder, fValue1, minValue, maxValue);
								
								// convert to int
								converted = builder.CreateFPToSI(fValue2, returnType);
							}
							break;
						case BASE_UINT:
							{
								// round
								llvm::Value* fValue1 = createRoundToEven(builder, input, true);
								
								// clamp
								double minValue = 0.0;
								double maxValue = double((uint64_t(1) << returnType->getScalarSizeInBits()) - 1);
								llvm::Value* fValue2 = createClamp(builder, fValue1, minValue, maxValue);
								
								// convert to uint
								converted = builder.CreateFPToUI(fValue2, returnType);
							}
							break;
						}
						builder.CreateRet(converted);				
					}
				}

			}
		}
	}
	
	
	// reinterpreting types
	const ReinterpretInfo bitCasts[] =
	{
		{VectorInfo::INT,   VectorInfo::FLOAT, 5},
		{VectorInfo::UINT,  VectorInfo::FLOAT, 5},
		{VectorInfo::FLOAT, VectorInfo::INT,   3},
		{VectorInfo::FLOAT, VectorInfo::UINT,  3},
	};

	for (int length = 1; length <= 4; ++length)
	{
		foreach (const ReinterpretInfo& bc, bitCasts)
		{
			// create mangled function name (e.g. as_int(float) -> _Z6as_intf)
			std::string name = NameMangler::mangle("as_" + VectorInfo(bc.destType, length).toString(),
				VectorInfo(bc.srcType, length));

			if (llvm::Function* function = module->getFunction(name))
			{
				// set internal linkage that the functions are removed after inlining
				function->setLinkage(llvm::Function::InternalLinkage);
				
				// create basic block
				llvm::BasicBlock* basicBlock = llvm::BasicBlock::Create(context, "Entry", function);
				llvm::IRBuilder<> builder(basicBlock);

				// get input
				llvm::Value* input = function->arg_begin();
				
				// create return type
				llvm::Type* returnType = llvmTypes[bc.llvmType];
				if (length > 1)
					returnType = llvm::VectorType::get(returnType, length);
				
				// bitcast
				builder.CreateRet(builder.CreateBitCast(input, returnType));
			}
		}
	}
	
}

} // namespace digi
