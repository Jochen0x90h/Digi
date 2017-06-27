#ifndef digi_Scene_SceneFile_h
#define digi_Scene_SceneFile_h

#include <digi/Image/Image.h>

#include "Texture.h"
#include "Scene.h"


namespace digi {

/// @addtogroup Scene
/// @{

class SceneFile : public Object
{
public:

	virtual ~SceneFile();

	std::vector<Pointer<Texture> > textures;
		
	std::vector<Pointer<Scene> > scenes;
	
	static Pointer<SceneFile> load(ObjectReader& r);
	void save(ObjectWriter& w);
};	

/// @}

} // namespace digi

#endif
