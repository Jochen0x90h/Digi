#include <digi/Utility/VectorUtility.h>

#include "SceneFile.h"


namespace digi {

// SceneFile

SceneFile::~SceneFile()
{
}

// serialize

template <typename Serializer>
void serialize(Serializer& s, SceneFile& sceneFile)
{
	s & sceneFile.textures;
	s & sceneFile.scenes;
}

Pointer<SceneFile> SceneFile::load(ObjectReader& r)
{
	Pointer<SceneFile> sceneFile = new SceneFile();
	VersionReader v(r);
	serialize(r, *sceneFile);
	return sceneFile;	
}

void SceneFile::save(ObjectWriter& w)
{
	VersionWriter v(w, 1);
	serialize(w, *this);
}

} // namespace digi
