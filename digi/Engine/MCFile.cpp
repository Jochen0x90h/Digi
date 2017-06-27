#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/Find.h>
#include <digi/Utility/MapUtility.h>
#include <digi/Utility/foreach.h>
#include <digi/System/File.h>
#include <digi/Data/WriteFunctions.h>
#include <digi/Math/All.h>
#include <digi/Engine/Track.h>

#include "MCFile.h"


#ifdef _WIN32
	#ifdef _M_X64
		// x64 (x86 64 bit)
		extern "C" size_t __chkstk(size_t);
	#else
		// x86 (x86 32 bit)
		extern "C" void* _alloca_probe_16(size_t);
	#endif
#endif


namespace digi {

namespace
{
	// noise functions for machine code loader
	float noise2f(float x, float y) {return digi::noise(vector2(x, y));}
	float noise3f(float x, float y, float z) {return digi::noise(vector3(x, y, z));}

	struct Function
	{
		const char* name;
		void* function;
	};

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

	typedef NameLess<Function> FunctionLess;
	
	bool relocate(ObjectReader& r, CodeMemory& code, Function* functionsBegin, Function* functionsEnd)
	{
		// read names of external symbols
		uint numExternalNames;
		r & numExternalNames;
		std::vector<std::string> externalNames(numExternalNames);
		for (uint i = 0; i < numExternalNames; ++i)
		{
			r & externalNames[i];
		}

		ReadLE<size_t> read;
		WriteLE<size_t> write;

		// read relocations for extrnal symbols
		uint numExternalRelocs;
		r & numExternalRelocs;
		for (uint i = 0; i < numExternalRelocs; ++i)
		{
			uint nameIndex;
			size_t offset;

			r & nameIndex;
			r & offset;

			StringRef functionName = externalNames[nameIndex];

			// get external symbol
			size_t s;
		#ifdef _WIN32
			// win32 uses _alloca/__chkstk to grow the stack if more than 4k (size of guard page) are allocated
		#ifdef _M_X64
			// 64 bit
			if (functionName == "__chkstk")
			{
				s = (size_t)(void*)__chkstk;
			}
		#else
			// 32 bit
			if (functionName == "_alloca")
			{
				s = (size_t)(void*)_alloca_probe_16;
			}
		#endif
			else
		#endif
			{
				Function* function = binaryFind(
					functionsBegin,
					functionsEnd,
					functionName, FunctionLess());

				// check if function was found
				if (function == functionsEnd)
				{
					// error
					dError("reference to unknown function '" << functionName << "'");
					return false;
				}
				s = (size_t)function->function;
			}

			// do relocation
			size_t a = read(&code[offset]);
			size_t p = 0;
			if (sizeof(size_t) == 4)
			{
				// 32 bit: pc-relative addressing
				p = (size_t)&code[offset];
			}

			write(s + a - p, &code[offset]);
		}
		
		// read internal relocations
		uint numInternalRelocs;
		r & numInternalRelocs;
		for (uint i = 0; i < numInternalRelocs; ++i)
		{
			size_t offset;
			//uint type;
			
			// offset of location where machine code has to be patched
			r & offset;
			//r & type;
			
			/*if (type == 0)
			{
				// 32 bit S + A
				int32_t s = (int32_t)&code[0];
				int32_t a = read(&code[offset]);
				write(&code[offset], s + a);
			}
			else*/
			{
				// S + A
				size_t s = (size_t)&code[0];
				size_t a = read(&code[offset]);
				write(s + a, &code[offset]);
			}
		}
		return true;
	}

} // anonymous namespace


// MCLoader

MCLoader::~MCLoader()
{
}

Pointer<EngineFile> MCLoader::load(const fs::path& path)
{
	Pointer<File> file = File::open(path, File::READ);
	return new MCFile(file);
}


// MCFile

MCFile::MCFile(Pointer<IODevice> dev)
{
	// initialize list of functions that the machine code can call
	// (math e.g. sin(), system e.g. malloc(), opengl e.g. glEnable())
	// on windows the gl function pointers must already be initialized.
	// some gl functions are not available dependent on GL_MAJOR and GL_ES defines.
	#include "MCNumFunctions.inc.h"
	Function functions[NUM_FUNCTIONS];
	Function* f = functions;
	#include "MCSetFunctions.inc.h"
	Function* functionsEnd = f;

	ObjectReader r(dev);

	// read textures
	uint numTextures;
	r & numTextures;
	this->textureInfos.resize(numTextures);
	this->textures.resize(numTextures);
	uint textureIndex = 0;
	std::vector<uint> textureRemap(numTextures, textureIndex);
	for (uint k = 0; k < numTextures; ++k)
	{
		TextureInfo& info = this->textureInfos[textureIndex];
		
		// read name
		info.name = this->readString(r);

		// if name is zero-length then something went wrong while writing the file
		if (*info.name != 0)
		{
			// read type
			r & info.type;

			// read data
			size_t dataSize;
			r & dataSize;
			DataMemory data(dataSize);
			r.readData(data, dataSize);

			// read code
			size_t codeSize;
			r & codeSize;
			CodeMemory code(codeSize);
			r.readData(code, codeSize);

			// read public symbols
			uint numPublics;
			r & numPublics;
			for (uint i = 0; i < numPublics; ++i)
			{
				std::string name;
				size_t offset;

				r & name;
				r & offset;

				if (name == "initGlobal")
					info.initGlobal = (TextureInfo::InitGlobal)&code[offset];
				else if (name == "doneGlobal")
					info.doneGlobal = (TextureInfo::DoneGlobal)&code[offset];
				else if (name == "copy")
					info.copy = (TextureInfo::Copy)&code[offset];
			}
			if (info.initGlobal != NULL && info.doneGlobal != NULL && info.copy != NULL
				&& relocate(r, code, functions, functionsEnd))
			{
				// everything is ok
				
				// read global size
				r & info.globalSize;

				// set texture global
				Global& texture = this->textures[textureIndex];
				swap(texture.code, code);
				swap(texture.data, data);
				DataMemory global(info.globalSize);
				swap(texture.global, global);

				// initialize
				info.initGlobal(texture.global, texture.data);

				textureRemap[k] = textureIndex;
				++textureIndex;
			}
			else
			{
				dError("texture '" << info.name << "' failed to load");
			}
		}
	}
	this->textureInfos.resize(textureIndex);

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

		// if name is zero-length then something went wrong while writing the file
		if (*info.name != 0)
		{			
			// read data
			size_t dataSize;
			r & dataSize;
			DataMemory data(dataSize);
			r.readData(data, dataSize);

			// read code
			size_t codeSize;
			r & codeSize;
			CodeMemory code(codeSize);
			r.readData(code, codeSize);

			// read public symbols
			uint numPublics;
			r & numPublics;
			for (uint i = 0; i < numPublics; ++i)
			{
				std::string name;
				size_t offset;

				r & name;
				r & offset;

				if (name == "initGlobal")
					info.initGlobal = (SceneInfo::InitGlobal)&code[offset];
				else if (name == "doneGlobal")
					info.doneGlobal = (SceneInfo::DoneGlobal)&code[offset];
				else if (name == "initInstance")
					info.initInstance = (SceneInfo::InitInstance)&code[offset];
				else if (name == "doneInstance")
					info.doneInstance = (SceneInfo::DoneInstance)&code[offset];
				else if (name == "addClip")
					info.addClip = (SceneInfo::AddClip)&code[offset];
				else if (name == "update")
					info.update = (SceneInfo::Update)&code[offset];
				else if (name == "getBoundingBox")
					info.getBoundingBox = (SceneInfo::GetBoundingBox)&code[offset];
				else if (name == "render")
					info.render = (SceneInfo::Render)&code[offset];
				//else if (name == "rayTest")
				//	info.rayTest = (SceneInfo::RayTest)&code[offset];
			}
			if (info.initGlobal != NULL && info.doneGlobal != NULL && info.initInstance != NULL
				&& info.doneInstance != NULL && info.addClip != NULL && info.update != NULL
				&& info.getBoundingBox != NULL && info.render != NULL// && info.rayTest != NULL
				&& relocate(r, code, functions, functionsEnd))
			{
				// everything is ok

				// read global and instance size
				r & info.globalSize;
				r & info.instanceSize;
				
				// read nodes
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
				
				// read parameters
				{
					uint numAttributes;
					r & numAttributes;
					AttributeInfo* attributeInfos = this->parameterInfoPool.alloc(numAttributes);
					info.attributeInfos.data = attributeInfos;
					info.attributeInfos.length = numAttributes;
					for (uint i = 0; i < numAttributes; ++i)
					{
						AttributeInfo& attributeInfo = attributeInfos[i];

						// read name
						attributeInfo.name = this->readString(r);
							
						// read type
						r & attributeInfo.type;

						// read offset
						r & attributeInfo.offset;

						// read semantic
						attributeInfo.semantic = this->readString(r);
					}
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

						uint textureIndex;
						r & textureIndex;
						r & textureBinding.type;
						r & textureBinding.offset;
					
						textureBinding.textureIndex = textureRemap[textureIndex];
						if (textureBinding.textureIndex < numTextures)
							++textureBindingIndex;
					}
					info.textureBindings.length = textureBindingIndex;
				}

				// read attribute sets
				{
					uint numParameterSets;
					r & numParameterSets;
					AttributeSetInfo* attributeSetInfos = this->parameterSetInfoPool.alloc(numParameterSets);
					info.attributeSetInfos.data = attributeSetInfos;
					info.attributeSetInfos.length = numParameterSets;
					for (uint i = 0; i < numParameterSets; ++i)
					{
						AttributeSetInfo& attributeSetInfo = attributeSetInfos[i];

						// read members
						attributeSetInfo.name = this->readString(r);
						r & attributeSetInfo.offset;
						r & attributeSetInfo.numTracks;
						r & attributeSetInfo.clipIndex;
						r & attributeSetInfo.numClips;
					}			
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
				
				// read objects
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
						r & objectInfo.offset;
					}
				}

				// set scene global
				Global& scene = this->scenes[sceneIndex];
				swap(scene.code, code);
				swap(scene.data, data);
				DataMemory global(info.globalSize);
				swap(scene.global, global);

				// initialize
				info.initGlobal(scene.global, scene.data);
			
				++sceneIndex;
			}
			else
			{
				dError("scene '" << info.name << "' failed to load");
			}
		}
	}
	this->sceneInfos.resize(sceneIndex);
}

MCFile::~MCFile()
{
}

ArrayRef<const TextureInfo> MCFile::getTextureInfos()
{
	return ArrayRef<const TextureInfo>(this->textureInfos);
}

void* MCFile::getTextureGlobal(int index)
{
	return this->textures[index].global;
}

ArrayRef<const SceneInfo> MCFile::getSceneInfos()
{
	return ArrayRef<const SceneInfo>(this->sceneInfos);
}

void* MCFile::getSceneGlobal(int index)
{
	return this->scenes[index].global;
}

void MCFile::done()
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

const char* MCFile::readString(ObjectReader& r)
{
	size_t length = readVarSize<size_t>(r);
	char* str = this->stringPool.alloc(length + 1);
	r.readData(str, length);
	str[length] = 0;
	return str;
}

} // namespace digi
