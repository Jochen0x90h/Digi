# add generator tool
if(NOT DEFINED NO_TOOLS)
	add_subdirectory(tool)
endif()


# public header files (visible to users of this library)
set(HEADERS
	All.h
	Display.h
	InputDevice.h
	SimpleQueue.h
)

# source files
set(FILES
	Display.cpp
	InputDevice.cpp
)

# platform source files
set(PLATFORM_DIR)
if(WIN32)
	list(APPEND FILES Win32_GL/Win32Display.cpp)
	set(PLATFORM_DIR Win32_GL)
elseif(APPLE)
	list(APPEND FILES OSX_GL/OSXDisplay.mm)
	set(PLATFORM_DIR OSX_GL)
else()
	list(APPEND FILES Linux_EGL/LinuxDisplay.cpp)
endif()

# platform source files that are created by tool
if(PLATFORM_DIR)
	list(APPEND FILES
		${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_DIR}/key2AttributeIndex.inc.h
		${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_DIR}/keyAttributeInfos.inc.h
		${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_DIR}/numKeyAttributes.inc.h
	)
	if(NOT DEFINED NO_TOOLS)
		add_custom_command(
			OUTPUT
				${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_DIR}/key2AttributeIndex.inc.h
				${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_DIR}/keyAttributeInfos.inc.h
				${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_DIR}/numKeyAttributes.inc.h
			COMMAND digiDisplayTool
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${PLATFORM_DIR}
			DEPENDS digiDisplayTool
		)
	endif()
endif()


# set include directories
include_directories(${Display_INCLUDE_DIRS})

# add definitions
add_definitions(${Display_DEFINITIONS})
 
# add library digiDisplay
CREATE_LIBRARY(digi.Display HEADERS ${HEADERS} FILES ${FILES} VERSIONIZE EXPORT Digi)


# add test
add_subdirectory(test)
