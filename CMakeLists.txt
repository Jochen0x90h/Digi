cmake_minimum_required(VERSION 3.1)

# set project
project(Digi)

# set default postfixes (can be overridden on the command line)
set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "Postfix for output library name in Debug build")
set(CMAKE_RELWITHDEBINFO_POSTFIX "r" CACHE STRING "Postfix for output library name in RelWithDebInfo build")

# cmake defines NDEBUG for release builds


# import boost
set(Boost_USE_STATIC_LIBS ON)
set(Boost_COMPONENTS "")
if(NOT DEFINED NO_BOOST_FILESYSTEM)
	list(APPEND Boost_COMPONENTS system filesystem)
endif()
message("Boost COMPONENTS ${Boost_COMPONENTS}")
find_package(Boost COMPONENTS ${Boost_COMPONENTS})


# set directory for config.h which is local to the current cmake generator
set(DIGI_CONFIG_DIR ${CMAKE_CURRENT_BINARY_DIR})

# set include directories
include_directories("." ${DIGI_CONFIG_DIR} ${Boost_INCLUDE_DIRS})

# set preprocessor definitions and compiler switches
add_definitions(${Boost_DEFINITIONS})

# include cmake macros
include(cmake/Macros.cmake)

# list of component libraries to build
if(NOT LIBS)
	set(LIBS
		Audio
		Base
		Checksum
		CodeGenerator
		Compress
		Data
		Display
		Engine
		EngineVM
		Image
		ImageConvert
		ImageIO
		LLVMJit
		Math
		Mesh
		Network
		Nodes
		OpenGL
		Scene
		SceneConvert
		System
		Utility
		Video
	)
endif()
add_subdirectory(digi)

# install an auto-generated description of the targets
install(EXPORT Digi DESTINATION cmake/Digi)

# install cmake files
install(FILES
	cmake/ConfigTemplate.cmake
	cmake/InitLibraries.h.cmake
	cmake/Macros.cmake
	cmake/Version.h.cmake
	cmake/Versionize.cmake
	DESTINATION cmake/Digi
)
