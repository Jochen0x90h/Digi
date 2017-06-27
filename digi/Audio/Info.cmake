set(Audio_DEPENDENCIES Data)

# platform dependent libs
if(APPLE)
	find_library(AUDIOTOOLBOX_LIBRARY AudioToolbox)
	list(APPEND Audio_LIBRARIES ${AUDIOTOOLBOX_LIBRARY})
endif()

# find ogg vorbis
find_package(ogg)
if(OGG_FOUND)
	list(APPEND Audio_PACKAGES ogg)
	list(APPEND Audio_INCLUDE_DIRS ${OGG_INCLUDE_DIRS})
	list(APPEND Audio_DEFINITIONS ${OGG_DEFINITIONS} -DHAVE_OGG)
	list(APPEND Audio_LIBRARIES ${OGG_LIBRARIES})  
endif()

set(Audio_HAS_INIT_DONE YES)
