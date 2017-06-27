#include <llvm/LLVMContext.h>
#include <llvm/Module.h>

#include <llvm/Support/IRBuilder.h>
#include <llvm/Support/TypeBuilder.h>

#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ExecutionEngine/GenericValue.h>

#include <clang/Frontend/CodeGenOptions.h>
#include <clang/AST/RecordLayout.h>
#include <clang/CodeGen/ModuleBuilder.h>

#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/Find.h>
#include <digi/Utility/MapUtility.h>
#include <digi/Utility/foreach.h>
#include <digi/System/File.h>
#include <digi/Math/All.h>
#include <digi/Engine/Track.h>

#include "Compiler.h"
#include "CodeGenWrapper.h"
#include "GetStructs.h"
#include "VMFile.h"


namespace digi {

namespace
{
	// less functors

	template <typename Type>
	struct NameLess
	{
		bool operator ()(const Type& info, StringRef name)
		{
			return info.name < name;
		}

		bool operator ()(StringRef name, const Type& info)
		{
			return name < info.name;
		}
	};

	typedef NameLess<TextureInfo> TextureLess;


	void readCode(ObjectReader& r, std::string& str)
	{
		size_t length = readVarSize<size_t>(r);
		str.resize(13 + length + 2);
		char* data = (char*)getData(str);
		r.readData(data + 13, length);
		memcpy(data, "extern \"C\" {\n", 13);	
		memcpy(data + 13 + length, "}\n", 2);	
	}

	intptr_t getPointerToFunction(llvm::ExecutionEngine* executionEngine, llvm::Module* module, StringRef name)
	{
		llvm::Function* function = module->getFunction(llvm::StringRef(name.data(), name.length()));
		if (function == NULL)
			return 0;
		return (intptr_t)executionEngine->getPointerToFunction(function);
	}

	struct TypeOffset
	{
		const clang::Type* type;
		size_t offset;
		
		TypeOffset()
			: type(NULL), offset() {}
		TypeOffset(const clang::Type* type, size_t offset)
			: type(type), offset(offset) {}
	};

	TypeOffset getRecordTypeOffset(clang::ASTContext* astContext, clang::RecordDecl* recordDecl, const std::string& name)
	{
		clang::RecordDecl::field_iterator it = recordDecl->field_begin();
		clang::RecordDecl::field_iterator end = recordDecl->field_end();
		int fieldIndex = 0;
		for (; it != end; ++it, ++fieldIndex)
		{
			clang::FieldDecl* fieldDecl = *it;
			if (fieldDecl->getName() == name)
			{
				const clang::Type* type = fieldDecl->getType().getTypePtr();
				size_t offset = size_t(astContext->getASTRecordLayout(recordDecl).getFieldOffset(fieldIndex) / 8);
				return TypeOffset(type, offset);
			}
		}
		return TypeOffset();
	}

	size_t getTypeSize(clang::ASTContext* astContext, const clang::Type* type)
	{
		return size_t(astContext->getTypeSize(type)) / 8;
	}

	TypeOffset getTypeOffset(clang::ASTContext* astContext, clang::RecordDecl* recordDecl, const std::string& p)
	{
		// get offset of path in struct. example: path = "foo.bar[3].x"
		
		int length = int(p.length());

		// first path element (e.g. "foo")
		int pos = 0;
		do
			++pos;
		while (pos < length && p[pos] != '.' && p[pos] != '[');

		std::string element = p.substr(0, pos);
		TypeOffset typeOffset = getRecordTypeOffset(astContext, recordDecl, element);
		if (typeOffset.type == NULL)
			return TypeOffset();

		// following path elements (e.g. ".bar[3].x")
		while (pos < length)
		{	
			// get next path element
			int start = pos;
			do
				++pos;
			while (pos < length && p[pos] != '.' && p[pos] != '[');
			
			if (p[start] == '.')
			{
				// member selection on struct or vector (e.g. ".bar" or ".x")
				std::string element = p.substr(start + 1, pos - start - 1);

				if (const clang::RecordType* recordType = typeOffset.type->getAs<clang::RecordType>())
				{
					// struct
					clang::RecordDecl* recordDecl = recordType->getDecl();
					TypeOffset typeOffset2 = getRecordTypeOffset(astContext, recordDecl, element);
					if (typeOffset2.type == NULL)
						return TypeOffset();
					
					typeOffset.type = typeOffset2.type;
					typeOffset.offset += typeOffset2.offset;				
				}
				else if (const clang::VectorType* vectorType = typeOffset.type->getAs<clang::VectorType>())
				{
					// vector
					int index = 0;
					if (element == "x")
						index = 0;
					else if (element == "y")
						index = 1;
					else if (element == "z")
						index = 2;
					else if (element == "w")
						index = 3;
					else
					{
						// error: member not found
						return TypeOffset();
					}
					
					const clang::Type* elementType = vectorType->getElementType().getTypePtr();
					size_t size = getTypeSize(astContext, elementType);
					
					typeOffset.type = elementType;
					typeOffset.offset += index * size;
				}
				else
				{
					// error: not a struct or vector
					return TypeOffset();
				}
			}
			else
			{
				// array index (e.g. "[3]")
				int index = toInt(p.substr(start + 1, pos - start - 2));
			
				if (const clang::Type* elementType = typeOffset.type->getArrayElementTypeNoTypeQual())
				{
					size_t size = getTypeSize(astContext, elementType);
					typeOffset.type = elementType;
					typeOffset.offset += index * size;
				}
				else
				{
					// error: not an array
					return TypeOffset();
				}
			}
		}

		return typeOffset;
	}
} // anonymous namespace


// VMLoader

VMLoader::VMLoader()
{
	// create llvm context
	this->context = new llvm::LLVMContext();

	// create a dummy module
	this->module = new llvm::Module("dummy", *this->context);

	// create JIT compiler. takes ownership of the module
	this->executionEngine = llvm::ExecutionEngine::create(this->module, false, NULL,
		//llvm::CodeGenOpt::None);
		llvm::CodeGenOpt::Default);
}

VMLoader::~VMLoader()
{
	delete this->executionEngine;
	delete this->context;
}

Pointer<EngineFile> VMLoader::load(const fs::path& path)
{
	// open data file
	Pointer<IODevice> file = File::open(path, File::READ);

	return new VMFile(file, this->context, this->executionEngine);
}


// VMFile

VMFile::VMFile(Pointer<IODevice> file, llvm::LLVMContext* context, llvm::ExecutionEngine* executionEngine)
	: executionEngine(executionEngine)
{
	Compiler compiler(Compiler::VM_OPENGL);

	ObjectReader r(file);

	// code generator options
	clang::CodeGenOptions codeGenOptions;
	codeGenOptions.DisableLLVMOpts = 1;
	codeGenOptions.OptimizationLevel = 0;
	//codeGenOptions.OptimizationLevel = 2;
	codeGenOptions.Inlining = clang::CodeGenOptions::NormalInlining;

	// read textures
	uint numTextures;
	r & numTextures;
	this->textureInfos.resize(numTextures);
	this->textures.resize(numTextures);
	uint textureIndex = 0;
	for (uint k = 0; k < numTextures; ++k)
	{
		TextureInfo& info = this->textureInfos[textureIndex];
			
		// read name
		info.name = this->readString(r);
			
		// read type
		r & info.type;

		// read data
		size_t dataSize;
		r & dataSize;
		DataMemory data(dataSize);
		r.readData(data, dataSize);
			
		// read code (with extern "C")
		std::string code;		
		readCode(r, code);


		// compile
		llvm::OwningPtr<GetStructs> astConsumer(new GetStructs(clang::CreateLLVMCodeGen(
			*compiler.diagnosticsEngine,
			info.name, // module name
			codeGenOptions,
			*context)));

		Pointer<CompileResult> result = this->compile(compiler, code, astConsumer.get());
		llvm::Module* module;
				
		if (result == null)
		{
			dError("texture '" << info.name << "' failed to compile");
			goto error2;
		}
				
		// get module (take ownership)
		module = astConsumer->ReleaseModule();
		if (module == NULL)
			goto error2;

		{			
			// check if struct Global was found
			if (astConsumer->globalDecl == NULL)
				goto error2;

			// add module to execution engine and get function pointers
			this->executionEngine->addModule(module);
				
			// void initGlobal(uchar* data, void* pGlobal)
			info.initGlobal = (TextureInfo::InitGlobal)getPointerToFunction(
				this->executionEngine, module, "initGlobal");

			// void doneGlobal(void* pGlobal)
			info.doneGlobal = (TextureInfo::DoneGlobal)getPointerToFunction(
				this->executionEngine, module, "doneGlobal");

			// void copy(const void* pGlobal, void* pDestination)
			info.copy = (TextureInfo::Copy)getPointerToFunction(
				this->executionEngine, module, "copy");

			if (info.initGlobal == NULL || info.doneGlobal == NULL || info.copy == NULL)
				goto error1;

			// everything is ok
			info.globalSize = result->getSize(astConsumer->globalDecl);
				
			// set texture global
			Global& texture = this->textures[textureIndex];
			texture.module = module;
			swap(texture.data, data);
			DataMemory global(info.globalSize);
			swap(texture.global, global);
				
			// initialize
			info.initGlobal(texture.global, texture.data);
				
			++textureIndex;
		}
		continue;
		
		// error handling
	error1:
		// remove module again
		this->executionEngine->removeModule(module);
		delete module;
	error2:
		;
	}
	this->textureInfos.resize(textureIndex);
	this->textures.resize(textureIndex);

	// read scenes
	uint numScenes;
	r & numScenes;
	this->sceneInfos.resize(numScenes);
	this->scenes.resize(numScenes);
	uint sceneIndex = 0;
	for (uint k = 0; k < numScenes; ++k)
	{
		SceneInfo& info = this->sceneInfos[sceneIndex];
			
		// read name
		info.name = this->readString(r);
			
		// read global data
		size_t dataSize;
		r & dataSize;
		DataMemory data(dataSize);
		r.readData(data, dataSize);
			
		// read code (with extern "C")
		std::string code;		
		readCode(r, code);

#ifdef PROFILE
	std::cout << "profiling scene " << name << std::endl;
	int startTime = Timer::getMilliSeconds();
	int time;
#endif

		// compile
		llvm::OwningPtr<GetStructs> astConsumer(new GetStructs(clang::CreateLLVMCodeGen(
			*compiler.diagnosticsEngine,
			info.name, // module name
			codeGenOptions,
			*context)));

		Pointer<CompileResult> result = this->compile(compiler, code, astConsumer.get());
		llvm::Module* module;
			
		if (result == null)
		{
			dError("scene '" << info.name << "' failed to compile");
			goto error4;
		}
			
		// get module (take ownership)
		module = astConsumer->ReleaseModule();
		if (module == NULL)
			goto error4;

		{
			// check if structs Global and Instance were found
			if (astConsumer->globalDecl == NULL || astConsumer->instanceDecl == NULL)
				goto error4;

			// get type and offset of state in struct Instance
			TypeOffset stateTypeOffset = getRecordTypeOffset(result->astContext.get(), astConsumer->instanceDecl, "state");
			if (stateTypeOffset.type == NULL)
				goto error4;

			// state must be a struct
			const clang::RecordType* stateRecordType = stateTypeOffset.type->getAs<clang::RecordType>();
			if (stateRecordType == NULL)
				goto error4;
			clang::RecordDecl* stateDecl = stateRecordType->getDecl();

#ifdef PROFILE
	time = Timer::getMilliSeconds();
	std::cout << "clang " << time - startTime << std::endl;
	startTime = time;
#endif

			// add module to execution engine and get function pointers
			this->executionEngine->addModule(module);
				
			// void initGlobal(uchar* data, void* pGlobal)
			info.initGlobal = (SceneInfo::InitGlobal)getPointerToFunction(
				this->executionEngine, module, "initGlobal");
				
			// void doneGlobal(void* pGlobal)
			info.doneGlobal = (SceneInfo::DoneGlobal)getPointerToFunction(
				this->executionEngine, module, "doneGlobal");

			// void initInstance(const void* pGlobal, void* pInstance)
			info.initInstance = (SceneInfo::InitInstance)getPointerToFunction(
				this->executionEngine, module, "initInstance");

			// void doneInstance(void* pInstance)
			info.doneInstance = (SceneInfo::DoneInstance)getPointerToFunction(
				this->executionEngine, module, "doneInstance");
							
			// void addClip(void* pInstance, int index, float* tracks, float time, float weight)
			info.addClip = (SceneInfo::AddClip)getPointerToFunction(
				this->executionEngine, module, "addClip");

			// void update(void* pInstance, float time, float timeStep)
			info.update = (SceneInfo::Update)getPointerToFunction(
				this->executionEngine, module, "update");

			// void getBoundingBox(void* pInstance, float4x2& boundingBox)
			info.getBoundingBox = (SceneInfo::GetBoundingBox)getPointerToFunction(
				this->executionEngine, module, "getBoundingBox");

			// void render(void* pInstance, const float4x4& viewMatrix, const float4x4& projectionMatrix, float time, RenderQueues* renderQueues)
			info.render = (SceneInfo::Render)getPointerToFunction(
				this->executionEngine, module, "render");

			// int rayTest(void* pInstance, const float4x4& viewMatrix, const float4x4& projectionMatrix, float x, float y, float& minT)
//			info.rayTest = (SceneInfo::RayTest)getPointerToFunction(
//				this->executionEngine, module, "rayTest");

			if (info.initGlobal == NULL || info.doneGlobal == NULL || info.initInstance == NULL || info.doneInstance == NULL
				|| info.addClip == NULL || info.update == NULL || info.getBoundingBox == NULL || info.render == NULL// || info.rayTest == NULL
			)
				goto error3;

#ifdef PROFILE
	time = Timer::getMilliSeconds();
	std::cout << "llvm " << time - startTime << std::endl;
	startTime = time;
#endif

			// everything is ok
			info.globalSize = result->getSize(astConsumer->globalDecl);
			info.instanceSize = result->getSize(astConsumer->instanceDecl);
						
						
			// read node infos
			{
				uint numNodes;
				r & numNodes;
				NodeInfo* nodeInfos = this->nodeInfoPool.alloc(numNodes);
				info.nodeInfos.data = nodeInfos;
				info.nodeInfos.length = numNodes;
				for (uint i = 0; i < numNodes; ++i)
				{
					NodeInfo& nodeInfo = nodeInfos[i];

					// read name
					nodeInfo.name = this->readString(r);
						
					// read type
					r & nodeInfo.type;
				}				
			}

			// read attribute infos
			{
				uint numAttributes;
				r & numAttributes;
				AttributeInfo* attributeInfos = this->parameterInfoPool.alloc(numAttributes);
				info.attributeInfos.data = attributeInfos;
				uint attributeIndex = 0;
				for (uint i = 0; i < numAttributes; ++i)
				{
					AttributeInfo& attributeInfo = attributeInfos[attributeIndex];

					// read name
					attributeInfo.name = this->readString(r);
						
					// read type
					r & attributeInfo.type;

					// read path and convert to offset
					std::string path;
					r & path;
					TypeOffset typeOffset = getTypeOffset(result->astContext.get(), stateDecl, path);
					attributeInfo.offset = stateTypeOffset.offset + typeOffset.offset;

					// read semantic
					attributeInfo.semantic = this->readString(r);

					// attribute is valid only if path was found
					if (typeOffset.type != NULL)
						++attributeIndex;
				}
				info.attributeInfos.length = attributeIndex;
			}
						
			// read texture bindings
			{
				uint numTextureBindings;
				r & numTextureBindings;
				TextureBinding* textureBindings = this->textureBindingPool.alloc(numTextureBindings);
				info.textureBindings.data = textureBindings;
				uint textureBindingIndex = 0;
				for (uint i = 0; i < numTextureBindings; ++i)
				{
					TextureBinding& textureBinding = textureBindings[textureBindingIndex];

					std::string textureName;
					r & textureName;
						
					//! sync skip
					// find texture by name
					std::vector<TextureInfo>::iterator begin = this->textureInfos.begin();
					std::vector<TextureInfo>::iterator end = this->textureInfos.end();
					std::vector<TextureInfo>::iterator it = binaryFind(begin, end, textureName, TextureLess());
					textureBinding.textureIndex = uint(it - begin);

					r & textureBinding.type;

					// read path and convert to offset
					std::string path;
					r & path;
					TypeOffset typeOffset = getTypeOffset(result->astContext.get(), stateDecl, path);
					textureBinding.offset = stateTypeOffset.offset + typeOffset.offset;
						
					// texture binding is only valid if texture and path path were found
					if (it != end && typeOffset.type != NULL)
						++textureBindingIndex;
				}
				info.textureBindings.length = textureBindingIndex;
			}
				
			// read attribute set infos
			{
				uint numParameterSets;
				r & numParameterSets;
				AttributeSetInfo* attributeSetInfos = this->parameterSetInfoPool.alloc(numParameterSets);
				info.attributeSetInfos.data = attributeSetInfos;
				uint attributeSetIndex = 0;
				for (uint j = 0; j < numParameterSets; ++j)
				{
					AttributeSetInfo& attributeSetInfo = attributeSetInfos[attributeSetIndex];

					// read name
					attributeSetInfo.name = this->readString(r);
						
					// read path and convert to offset
					std::string path;
					r & path;
					TypeOffset typeOffset = getTypeOffset(result->astContext.get(), stateDecl, path);
					attributeSetInfo.offset = stateTypeOffset.offset + typeOffset.offset;

					r & attributeSetInfo.numTracks;
					r & attributeSetInfo.clipIndex;
					r & attributeSetInfo.numClips;

					// attribute set is valid only if path was found
					if (typeOffset.type != NULL)
						++attributeSetIndex;
				}			
				info.attributeSetInfos.length = attributeSetIndex;
			}

			// read clips
			{
				uint numClips;
				r & numClips;
				ClipInfo* clipInfos = this->clipInfoPool.alloc(numClips);
				info.clipInfos.data = clipInfos;
				info.clipInfos.length = numClips;
				for (uint i = 0; i < numClips; ++i)
				{
					ClipInfo& clipInfo = clipInfos[i];

					// read members
					clipInfo.name = this->readString(r);
					r & clipInfo.index;
					r & clipInfo.length;
				}
			}
				
			// read object infos
			{
				uint numObjects;
				r & numObjects;
				ObjectInfo* objectInfos = this->objectInfoPool.alloc(numObjects);
				info.objectInfos.data = objectInfos;
				info.objectInfos.length = numObjects;
				for (uint i = 0; i < numObjects; ++i)
				{
					ObjectInfo& objectInfo = objectInfos[i];
					objectInfo.name = this->readString(r);
					
					// read index and convert to offset of id in scene instance
					uint index;
					r & index;
					TypeOffset typeOffset = getTypeOffset(result->astContext.get(), astConsumer->instanceDecl,
						arg("ids[%0]", index));
					objectInfo.offset = typeOffset.offset;
				}
			}
				

			// set scene global
			Global& scene = this->scenes[sceneIndex];
			scene.module = module;
			swap(scene.data, data);
			DataMemory global(info.globalSize);
			swap(scene.global, global);
				
			// initialize
			info.initGlobal(scene.global, scene.data);
				
			++sceneIndex;
		}				
		continue;
		
		// error handling
	error3:
		// remove module again
		this->executionEngine->removeModule(module);
		delete module;
	error4:
		// skip nodes
		{
			uint numNodes;
			r & numNodes;
			for (uint i = 0; i < numNodes; ++i)
			{
				// read name, type and path
				std::string name;
				uint type;
				r & name;
				r & type;
			}
		}

		// skip attributes
		{
			uint numAttributes;
			r & numAttributes;
			for (uint i = 0; i < numAttributes; ++i)
			{
				// read name, type and path
				std::string name;
				uint type;
				std::string path;
				std::string semantic;
				r & name;
				r & type;
				r & path;
				r & semantic;
			}
		}

		// skip texture bindings
		{
			uint numTextureBindings;
			r & numTextureBindings;
			for (uint i = 0; i < numTextureBindings; ++i)
			{
				std::string textureName;
				uint type;
				std::string path;
				r & textureName;
				r & type;
				r & path;
			}
		}

		// skip attribute sets
		{
			uint numParameterSets;
			r & numParameterSets;
			for (uint i = 0; i < numParameterSets; ++i)
			{
				std::string name;
				std::string path;
				uint numTracks;
				uint clipIndex;
				uint numClips;
				r & name;
				r & path;
				r & numTracks;
				r & clipIndex;
				r & numClips;
			}			
		}
			
		// skip clips
		{
			uint numClips;
			r & numClips;
			for (uint i = 0; i < numClips; ++i)
			{
				std::string name;
				uint index;
				float length;
				r & name;
				r & index;
				r & length;
			}
		}
		
		// skip objects
		{
			uint numObjects;
			r & numObjects;
			for (uint i = 0; i < numObjects; ++i)
			{
				std::string name;
				uint index;
				r & name;
				r & index;
			}
		}
	}
	this->sceneInfos.resize(sceneIndex);
	this->scenes.resize(sceneIndex);

	r.close();
}

VMFile::~VMFile()
{
	int numTextures = int(this->textureInfos.size());
	int numScenes = int(this->sceneInfos.size());
	
	// done textures
	for (int i = 0; i < numTextures; ++i)
	{
		// done global
		const TextureInfo& textureInfo = this->textureInfos[i];
		Global& texture = this->textures[i];
		textureInfo.doneGlobal(texture.global);

		// delete module		
		llvm::Module* module = texture.module;
		this->executionEngine->removeModule(module);
		delete module;
	}
	
	// done scenes
	for (int i = 0; i < numScenes; ++i)
	{
		// done global
		const SceneInfo& sceneInfo = this->sceneInfos[i];
		Global& scene = this->scenes[i];
		sceneInfo.doneGlobal(scene.global);

		// delete module		
		llvm::Module* module = scene.module;
		this->executionEngine->removeModule(module);
		delete module;
	}
}

ArrayRef<const TextureInfo> VMFile::getTextureInfos()
{
	return ArrayRef<const TextureInfo>(this->textureInfos);
}

void* VMFile::getTextureGlobal(int index)
{
	return this->textures[index].global;
}

ArrayRef<const SceneInfo> VMFile::getSceneInfos()
{
	return ArrayRef<const SceneInfo>(this->sceneInfos);
}

void* VMFile::getSceneGlobal(int index)
{
	return this->scenes[index].global;
}

void VMFile::done()
{
	size_t numTextures = this->textureInfos.size();
	size_t numScenes = this->sceneInfos.size();
	
	// done textures
	for (size_t i = 0; i < numTextures; ++i)
	{
		const TextureInfo& textureInfo = this->textureInfos[i];
		textureInfo.doneGlobal(this->textures[i].global);
	}
	
	// done scenes
	for (size_t i = 0; i < numScenes; ++i)
	{
		const SceneInfo& sceneInfo = this->sceneInfos[i];
		sceneInfo.doneGlobal(this->scenes[i].global);
	}
}

const char* VMFile::readString(ObjectReader& r)
{
	size_t length = readVarSize<size_t>(r);
	char* str = this->stringPool.alloc(length + 1);
	r.readData(str, length);
	str[length] = 0;
	return str;
}


namespace
{
	// creates an llvm function inside a module that calls a system function
	void bindFunction(llvm::Module* module, const char* name,
		void* functionPointer, bool system = false)
	{
		llvm::LLVMContext& context = module->getContext();

		// get function
		llvm::Function* function = module->getFunction(name);
	
		// if the function is not used we don't need to implement it
		if (function == NULL)
			return;

		// set internal linkage that the functions are removed after inlining
		function->setLinkage(llvm::Function::InternalLinkage);

		// get arguments of the function
		llvm::SmallVector<llvm::Value*, 8> args;
		for (llvm::Function::arg_iterator argIt = function->arg_begin(); argIt != function->arg_end(); ++argIt)
		{
			args.push_back(argIt);
		}
		
		// create basic block
		llvm::BasicBlock* basicBlock = llvm::BasicBlock::Create(context, "Entry", function);
		llvm::IRBuilder<> builder(basicBlock);

		// pointer to function
		llvm::FunctionType* functionType = function->getFunctionType();
		llvm::Constant* functionInt = llvm::ConstantInt::get(llvm::TypeBuilder<size_t, false>::get(context), (size_t)functionPointer); // executionEngine->getTargetData()->getIntPtrType()
		llvm::Value* functionPtr = llvm::ConstantExpr::getIntToPtr(functionInt, llvm::PointerType::getUnqual(functionType)); 
	
		// create call (pass all arguments)
		llvm::CallInst* callInst = builder.CreateCall(functionPtr, args);

		// win32 system functions have stdcall calling convention on 32 bit
		#ifdef _WIN32
			if (system && sizeof(void*) == 4)
				callInst->setCallingConv(llvm::CallingConv::X86_StdCall);
		#endif

		// create return
		if (function->getReturnType()->isVoidTy())
			builder.CreateRetVoid();
		else
			builder.CreateRet(callInst);
	}

	// noise functions for vm
	float noise2f(float x, float y) {return digi::noise(vector2(x, y));}
	float noise3f(float x, float y, float z) {return digi::noise(vector3(x, y, z));}

	void bindSystemFunctions(llvm::Module* module)
	{	
		// macros for binding functions from virtual machine to host machine
		#define BIND_C_FUNCTION(module, vmName, hostName) \
			bindFunction(module, vmName, (void*)hostName)
		#define BIND_GL_FUNCTION(module, vmName, hostName) \
			bindFunction(module, vmName, (void*)hostName, true)

		// auto-generated calls to BIND_?_FUNCTION for all functions available to the virtual machine
		#include "stdlib.inc.h"
		#include "math.inc.h"
		#include "TrackC.inc.h"
		#include "NoiseC.inc.h"
		#include "gl.inc.h"
		#include "GLWrapper.inc.h"

		#undef BIND_C_FUNCTION
		#undef BIND_GL_FUNCTION
	}

} // anonymous namespace


Pointer<CompileResult> VMFile::compile(Compiler& compiler, const std::string& inputCode, clang::CodeGenerator* astConsumer)
{
	#ifndef NDEBUG
		Pointer<IODevice> file = File::create("input.cpp");
		file->write(getData(inputCode), inputCode.size());
		file->close();
	#endif

	// compile
	Pointer<CompileResult> compileResult = compiler.compileFromString(inputCode, astConsumer);
	if (compileResult == null)
		return null;
	
	// get compiled module
	llvm::Module* module = astConsumer->GetModule();
	if (module == NULL)
		return null;
		
	// add system functions
	bindSystemFunctions(module);
	implementConvertFunctions(module);
	
	// create pass manager and add passes
	llvm::PassManager passManager;

	// optimization (vm mode)
	compileResult->addPasses(*module, passManager, true);
	
	// run all passes
	passManager.run(*module);

	return compileResult;
}

} // namespace digi
