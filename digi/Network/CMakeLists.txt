# public header files (visible to users of this library)
set(HEADERS
	All.h
	Socket.h
)

# source files
set(FILES
	All.cpp
	Socket.cpp
)

# platform dependent files
if(WIN32)
	SET(FILES ${FILES}
		Win32/Win32Socket.cpp
	)
else()
	SET(FILES ${FILES}
		POSIX/POSIXSocket.cpp
	)
endif()

# add definitions
add_definitions(${Network_DEFINITIONS})
 
# add library digiNetwork
CREATE_LIBRARY(digi.Network HEADERS ${HEADERS} FILES ${FILES} VERSIONIZE EXPORT Digi)


# add test
add_subdirectory(test)
