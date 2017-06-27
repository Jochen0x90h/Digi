if(STRUCTIO)
	# generate structs and readers/writers for webm
	add_custom_command(
		DEPENDS webm.struct
		COMMAND ${STRUCTIO} -g ebml ${CMAKE_CURRENT_SOURCE_DIR}/webm.struct
		OUTPUT
			${CMAKE_CURRENT_SOURCE_DIR}/webm.struct.h
			${CMAKE_CURRENT_SOURCE_DIR}/webm.ebml.id.h
			${CMAKE_CURRENT_SOURCE_DIR}/webm.ebml.read.h
			${CMAKE_CURRENT_SOURCE_DIR}/webm.ebml.write.h
	)
endif()

# public header files (visible to users of this library)
set(HEADERS
	All.h
	MediaDecoder.h
	MediaEncoder.h
	VideoFormat.h
	VideoIn.h
	VideoOut.h
	VideoTexture.h
	webm.struct.h
	WebMDecoder.h
	WebMEncoder.h
)

# source files
set(FILES
	All.cpp
	MediaDecoder.cpp
	MediaEncoder.cpp
	VideoIn.cpp
	VideoOut.cpp
	VideoTexture.cpp
	WebMDecoder.cpp
	WebMEncoder.cpp
	webm.struct
	webm.ebml.id.h
	webm.ebml.read.h
	webm.ebml.write.h
)

# add files that depend on ogg/vorbis
if(OGG_FOUND)
	list(APPEND HEADERS
		VorbisDecoder.h
		VorbisEncoder.h
	)
	list(APPEND FILES
		VorbisDecoder.cpp
		VorbisEncoder.cpp
	)
endif() 

# add files that depend on libvpx
if(VPX_FOUND)
	list(APPEND HEADERS
		VPXDecoder.h
		VPXEncoder.h
	)
	list(APPEND FILES
		VPXDecoder.cpp
		VPXEncoder.cpp
	)
endif() 

# set include directories
include_directories(${Video_INCLUDE_DIRS})

# add definitions
add_definitions(${Video_DEFINITIONS})

# add library digiVideo
CREATE_LIBRARY(digi.Video HEADERS ${HEADERS} FILES ${FILES} VERSIONIZE EXPORT Digi)


# add test
add_subdirectory(test)
