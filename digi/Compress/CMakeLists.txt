# public header files (visible to users of this library)
set(HEADERS
	All.h
	DLZSS.h
	#LocoStream.h
)

# source files
set(FILES
	All.cpp
	DLZSS.cpp
	#LocoStream.cpp
)

# add definitions
add_definitions(${Data_DEFINITIONS})
 
# add library digiData
CREATE_LIBRARY(digi.Compress HEADERS ${HEADERS} FILES ${FILES} VERSIONIZE EXPORT Digi)


# add test
add_subdirectory(test)
