#ifndef digi_Engine_MCFile_h
#define digi_Engine_MCFile_h

#include <digi/Utility/MemoryPool.h>
#include <digi/Scene/ObjectReader.h>
#include <digi/Engine/Engine.h>

#include "CodeMemory.h"
#include "DataMemory.h"

namespace digi {

/// @addtogroup Engine
/// @{

// loader for machine code engine file (contains machine code for host machine)
// attention: opengl function pointers (see digi/OpenGL/gLWrapper.h) must already be initialized
class MCLoader : public EngineLoader
{
public:
	MCLoader() {}

	virtual ~MCLoader();

	virtual Pointer<EngineFile> load(const fs::path& path);
};
	
// machine code engine file
class MCFile : public EngineFile
{
public:
	
	// constructor. can be used directly to load a file "by hand" e.g. from network
	MCFile(Pointer<IODevice> dev);
	virtual ~MCFile();

	virtual ArrayRef<const TextureInfo> getTextureInfos();
	virtual void* getTextureGlobal(int index);

	virtual ArrayRef<const SceneInfo> getSceneInfos();
	virtual void* getSceneGlobal(int index);

	virtual void done();

protected:

	const char* readString(ObjectReader& r);


	std::vector<TextureInfo> textureInfos;
	std::vector<SceneInfo> sceneInfos;		

	struct Global
	{
		CodeMemory code;
		DataMemory data;
		DataMemory global;
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
