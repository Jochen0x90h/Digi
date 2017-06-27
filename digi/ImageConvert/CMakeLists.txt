# public header files (visible to users of this library)
set(HEADERS
	All.h
	BufferConverter.h
	DataConverter.h
	ImageConverter.h
	ImageUtil.h
)

# source files
set(FILES
	All.cpp
	BufferConverter.cpp
	DataConverter.cpp
	ImageConverter.cpp
	ImageUtil.cpp
)

# set include directories
include_directories(${ImageConvert_INCLUDE_DIRS})

# add definitions
add_definitions(${ImageConvert_DEFINITIONS})

# add library digiImageConvert
CREATE_LIBRARY(digi.ImageConvert HEADERS ${HEADERS} FILES ${FILES} VERSIONIZE EXPORT Digi)


# add test
add_subdirectory(test)
