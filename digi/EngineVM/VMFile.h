#ifndef digi_EngineVM_VMFile_h
#define digi_EngineVM_VMFile_h

//#include <clang/CodeGen/ModuleBuilder.h>

#include <digi/Utility/MemoryPool.h>
#include <digi/Scene/ObjectReader.h>
#include <digi/Engine/Engine.h>


// forward-declare llvm and clang classes
namespace llvm
{
	class LLVMContext;
	class Module;
	class ExecutionEngine;
}
namespace clang
{
	class CodeGenerator;
}


namespace digi {

/// @addtogroup EngineVM
/// @{

class Compiler;
class CompileResult;

class VMLoader : public EngineLoader
{
public:
	VMLoader();

	virtual ~VMLoader();

	virtual Pointer<EngineFile> load(const fs::path& path);

protected:

	// context
	llvm::LLVMContext* context;

	// module
	llvm::Module* module;
		
	// JIT compiler
	llvm::ExecutionEngine* executionEngine;
};
	
class VMFile : public EngineFile
{
public:
	VMFile(Pointer<IODevice> file, llvm::LLVMContext* context, llvm::ExecutionEngine* executionEngine);
	virtual ~VMFile();

	virtual ArrayRef<const TextureInfo> getTextureInfos();
	virtual void* getTextureGlobal(int index);

	virtual ArrayRef<const SceneInfo> getSceneInfos();
	virtual void* getSceneGlobal(int index);

	virtual void done();

protected:

	const char* readString(ObjectReader& r);

public:
	static Pointer<CompileResult> compile(Compiler& compiler, const std::string& inputCode, clang::CodeGenerator* astConsumer);

protected:

	// JIT compiler
	llvm::ExecutionEngine* executionEngine;


	std::vector<TextureInfo> textureInfos;
	std::vector<SceneInfo> sceneInfos;		

	struct Global
	{
		llvm::Module* module;
		DataMemory data;
		DataMemory global;
		
		Global()
			: module(NULL) {}
	};

	std::vector<Global> textures;
	std::vector<Global> scenes;

	MemoryPool<char, 1024> stringPool;
	MemoryPool<NodeInfo, 128> nodeInfoPool;
	MemoryPool<AttributeInfo, 128> parameterInfoPool;
	MemoryPool<TextureBinding, 64> textureBindingPool;
	MemoryPool<AttributeSetInfo, 64> parameterSetInfoPool;
	MemoryPool<ClipInfo, 64> clipInfoPool;
	MemoryPool<ObjectInfo, 64> objectInfoPool;
};

/// @}

} // namespace digi

#endif
