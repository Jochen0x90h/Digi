# public header files (visible to users of this library)
set(HEADERS
	All.h
	CRC32.h
)

# source files
set(FILES
	All.cpp
	CRC32.cpp
)

# add definitions
add_definitions(${Checksum_DEFINITIONS})

# add library digiChecksum
CREATE_LIBRARY(digi.Checksum HEADERS ${HEADERS} FILES ${FILES} VERSIONIZE EXPORT Digi)


# add test
add_subdirectory(test)
