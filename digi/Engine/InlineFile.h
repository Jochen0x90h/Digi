#ifndef digi_Engine_InlineFile_h
#define digi_Engine_InlineFile_h

#include "Engine.h"


namespace digi {

/// @addtogroup Engine
/// @{

/// engine file for scenes that are compiled into the c++ project
class InlineFile : public EngineFile
{
public:
	
	// constructor. allocates OpenGL resources, needs current OpenGL context
	InlineFile(const fs::path& path, const EngineInfo& engineInfo);
	virtual ~InlineFile();

	virtual ArrayRef<const TextureInfo> getTextureInfos();
	virtual void* getTextureGlobal(int index);

	virtual ArrayRef<const SceneInfo> getSceneInfos();
	virtual void* getSceneGlobal(int index);

	virtual void done();

protected:

	DataMemory data;
	DataMemory global;
	const EngineInfo& engineInfo;
	std::vector<void*> textureGlobals;
	std::vector<void*> sceneGlobals;
};

/// @}

} // namespace digi

#endif
