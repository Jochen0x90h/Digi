# public header files (visible to users of this library)
set(HEADERS
	All.h
	AudioConverter.h
	AudioDecoder.h
	AudioException.h
	AudioFormat.h
	AudioIn.h
	AudioOut.h
	LineOut.h
)

# source files
set(FILES
	All.cpp
	AudioConverter.cpp
	AudioDecoder.cpp
	AudioException.cpp
	AudioFormat.cpp
	AudioIn.cpp
	AudioOut.cpp
	LineOut.cpp
)

# add files that depend on ogg/vorbis
if(OGG_FOUND)
	list(APPEND HEADERS OggVorbisDecoder.h)
	list(APPEND FILES OggVorbisDecoder.cpp)
endif()

# add platform dependent files
if(WIN32)
	set(FILES ${FILES}
		Win32/Win32WaveOut.cpp
	)
elseif(APPLE)
	set(FILES ${FILES}
		Apple/CoreAudioOut.cpp
	)
endif() 

# set include directories
include_directories(${Audio_INCLUDE_DIRS})

# add definitions
add_definitions(${Audio_DEFINITIONS})

# add library digiAudio
CREATE_LIBRARY(digi.Audio HEADERS ${HEADERS} FILES ${FILES} VERSIONIZE EXPORT Digi)


# add test
add_subdirectory(test)
