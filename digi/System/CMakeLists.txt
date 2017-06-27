# public header files (visible to users of this library)
set(HEADERS
	All.h
	ConsoleLogChannel.h
	Log.h
	File.h
	FileLogChannel.h
	FileSystem.h
	IOCatcher.h
	IODevice.h
	IOException.h
	MemoryDevices.h
	Resource.h
	SerialPort.h
	Timer.h
)

# source files
set(FILES
	All.cpp
	ConsoleLogChannel.cpp
	File.cpp
	FileLogChannel.cpp
	FileSystem.cpp
	IODevice.cpp
	IOException.cpp
	Log.cpp
	SerialPort.cpp
)

# platform dependent files
if(WIN32)
	SET(FILES ${FILES}
		Win32/Resource.cpp
		Win32/Win32File.cpp
		Win32/Win32SerialPort.cpp
	)
elseif(APPLE)
	SET(FILES ${FILES}
		Apple/Resource.cpp
		POSIX/POSIXFile.cpp
		POSIX/POSIXSerialPort.cpp
	)
else()
	SET(FILES ${FILES}
		POSIX/POSIXFile.cpp
		POSIX/POSIXSerialPort.cpp
	)
endif()

# add definitions
add_definitions(${System_DEFINITIONS})
 
# add library digiSystem
CREATE_LIBRARY(digi.System HEADERS ${HEADERS} FILES ${FILES} VERSIONIZE EXPORT Digi)



# add test
add_subdirectory(test)
