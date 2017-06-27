# public header files (visible to users of this library)
set(HEADERS
	All.h
	CodeMemory.h
	DataMemory.h
	Engine.h
	EngineInfo.h
	IK.h
	InlineFile.h
	MCFile.h
	MCNumFunctions.inc.h
	ParameterType.h
	RenderJob.h
	RenderTypes.h
	Text.h
	Track.h
	Watermark.h
)

# source files
set(FILES
	All.cpp
	CodeMemory.cpp
	Engine.cpp
	IK.cpp
	InlineFile.cpp
	MCFile.cpp
	MCSetFunctions.inc.h
	RenderJob.cpp
	Text.cpp
	Track.cpp
	Watermark.cpp
)

# set include directories
include_directories(${Engine_INCLUDE_DIRS})

# add definitions
add_definitions(${Engine_DEFINITIONS})

# add library digiEngine
CREATE_LIBRARY(digi.Engine HEADERS ${HEADERS} FILES ${FILES} VERSIONIZE EXPORT Digi)


# add test
add_subdirectory(test)
