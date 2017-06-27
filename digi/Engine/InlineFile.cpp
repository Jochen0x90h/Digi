#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/Find.h>
#include <digi/Utility/MapUtility.h>
#include <digi/Utility/foreach.h>
#include <digi/System/File.h>

#include "InlineFile.h"


namespace digi {

// InlineFile

InlineFile::InlineFile(const fs::path& path, const EngineInfo& engineInfo)
	: engineInfo(engineInfo)
{
	// read data
	{
		// open file, throws exception if it failed
		Pointer<File> file = File::open(path, File::READ);
		size_t size = size_t(file->getSize());
		DataMemory data(size);
		file->read(data, size);
		file->close();
		swap(this->data, data);
	}
		
	// determine size of global memory
	size_t globalMemorySize = 0;
	foreach (const TextureInfo& textureInfo, engineInfo.textureInfos)
		globalMemorySize += textureInfo.globalSize;
	foreach (const SceneInfo& sceneInfo, engineInfo.sceneInfos)
		globalMemorySize += sceneInfo.globalSize;
	
	// allocate global memory
	DataMemory global(globalMemorySize);
	swap(this->global, global);
	uint8_t* g = this->global;
	int numTextures = int(engineInfo.textureInfos.size());
	int numScenes = int(engineInfo.sceneInfos.size());
	this->textureGlobals.resize(numTextures);
	this->sceneGlobals.resize(numScenes);
	
	// init textures
	for (int i = 0; i < numTextures; ++i)
	{
		const TextureInfo& textureInfo = engineInfo.textureInfos[i];
		this->textureGlobals[i] = g;
		textureInfo.initGlobal(g, data);
		g += textureInfo.globalSize;
	}
	
	// init scenes
	for (int i = 0; i < numScenes; ++i)
	{
		const SceneInfo& sceneInfo = engineInfo.sceneInfos[i];
		this->sceneGlobals[i] = g;
		sceneInfo.initGlobal(g, data);
		g += sceneInfo.globalSize;
	}	
}

InlineFile::~InlineFile()
{
	// in the destructor we can't know if the correct OpenGL context is current.
	// therefore destroy must have been called.
}

ArrayRef<const TextureInfo> InlineFile::getTextureInfos()
{
	return ArrayRef<const TextureInfo>(this->engineInfo.textureInfos.data, this->engineInfo.textureInfos.length);
}

void* InlineFile::getTextureGlobal(int index)
{
	return this->textureGlobals[index];
}

ArrayRef<const SceneInfo> InlineFile::getSceneInfos()
{
	return ArrayRef<const SceneInfo>(this->engineInfo.sceneInfos.data, this->engineInfo.sceneInfos.length);
}

void* InlineFile::getSceneGlobal(int index)
{
	return this->sceneGlobals[index];
}

void InlineFile::done()
{
	size_t numTextures = this->engineInfo.textureInfos.size();
	size_t numScenes = this->engineInfo.sceneInfos.size();
	
	// done textures
	for (size_t i = 0; i < numTextures; ++i)
	{
		const TextureInfo& textureInfo = this->engineInfo.textureInfos[i];
		textureInfo.doneGlobal(this->textureGlobals[i]);
	}
	
	// done scenes
	for (size_t i = 0; i < numScenes; ++i)
	{
		const SceneInfo& sceneInfo = this->engineInfo.sceneInfos[i];
		sceneInfo.doneGlobal(this->sceneGlobals[i]);
	}
}

} // namespace digi
