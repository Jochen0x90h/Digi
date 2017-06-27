# public header files (visible to users of this library)
set(HEADERS
	All.h
	Deformer.h
	Instancer.h
	Layer.h
	Mesh.h
	ObjectReader.h
	ObjectWriter.h
	ParticleSystem.h
	SamplerState.h
	Scene.h
	SceneFile.h
	Shader.h
	Shape.h
	Texture.h
	ValueWrapper.h
	VertexField.h
)

# source files
set(FILES
	All.cpp
	Deformer.cpp
	Instancer.cpp
	Layer.cpp
	Mesh.cpp
	ObjectReader.cpp
	ObjectWriter.cpp
	ParticleSystem.cpp
	SamplerState.cpp
	Scene.cpp
	SceneFile.cpp
	Shader.cpp
	Shape.cpp
	Texture.cpp
	VertexField.cpp
)

# add definitions
add_definitions(${Scene_DEFINITIONS})
 
# add library digiUtility
CREATE_LIBRARY(digi.Scene HEADERS ${HEADERS} FILES ${FILES} VERSIONIZE EXPORT Digi)
