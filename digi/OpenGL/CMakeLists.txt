# public header files (visible to users of this library)
set(HEADERS
	All.h
	GLWrapper.h
	glext.h
)

# source files
set(FILES
	All.cpp
	GLWrapper.cpp
)

# set include directories
include_directories(${OpenGL_INCLUDE_DIRS})

# add definitions
add_definitions(${OpenGL_DEFINITIONS})

# add library digiOpenGL
CREATE_LIBRARY(digi.OpenGL HEADERS ${HEADERS} FILES ${FILES} VERSIONIZE EXPORT Digi)
