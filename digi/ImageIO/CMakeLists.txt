# public header files (visible to users of this library)
set(HEADERS
	All.h
	ImageIO.h
)

# source files
set(FILES
	All.cpp
	ImageIO.cpp
)

# set include directories
include_directories(${ImageIO_INCLUDE_DIRS})

# add definitions
add_definitions(${ImageIO_DEFINITIONS})

# add library digiImage
CREATE_LIBRARY(digi.ImageIO HEADERS ${HEADERS} FILES ${FILES} VERSIONIZE EXPORT Digi)


# add test
add_subdirectory(test)
