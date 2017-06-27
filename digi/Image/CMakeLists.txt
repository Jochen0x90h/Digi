# public header files (visible to users of this library)
set(HEADERS
	All.h
	Buffer.h
	BufferFormat.h
	DDS.h
	Image.h
	ImageFormat.h
	JPEGWrapper.h
	PNGWrapper.h
	TIFFWrapper.h
	WebPWrapper.h
)

# source files
set(FILES
	All.cpp
	Buffer.cpp
	BufferFormat.cpp
	DDS.cpp
	Image.cpp
	ImageFormat.cpp
	JPEGWrapper.cpp
	PNGWrapper.cpp
	TIFFWrapper.cpp
	WebPWrapper.cpp
)

# set include directories
include_directories(${Image_INCLUDE_DIRS})

# add definitions
add_definitions(${Image_DEFINITIONS})

# add library digiImage
CREATE_LIBRARY(digi.Image HEADERS ${HEADERS} FILES ${FILES} VERSIONIZE EXPORT Digi)


# add test
add_subdirectory(test)
