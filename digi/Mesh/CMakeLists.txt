# public header files (visible to users of this library)
set(HEADERS
	All.h
	Mesh.h
)

# source files
set(FILES
	All.cpp
	Mesh.cpp
)

# add definitions
add_definitions(${Mesh_DEFINITIONS})

# add library digiUtility
CREATE_LIBRARY(digi.Mesh HEADERS ${HEADERS} FILES ${FILES} VERSIONIZE EXPORT Digi)


# add test
add_subdirectory(test)
