# this file gets loaded by find_package(@LIB@ COMPONENTS ...) and defines the following:
#
# @LIB@_FOUND - YES if @LIB@ was found
# @LIB@_PACKAGES - dependent packages that should be included using find_package()
# @LIB@_INCLUDE_DIRS - include directories for this lib and its dependencies
# @LIB@_DEFINITIONS - preprocessor definitions and compiler switches
# @LIB@_LIBRARIES - list of libraries for given components and their dependencies inside @LIB@
# @LIB@_INIT_DONE - list of libraries that have an init/done function for initialization/deinitialization


if(NOT @LIB@_FOUND) 
	# find and include cmake-generated description file of the library
	find_file(@LIB@_CMAKE_FILE @LIB@.cmake PATH_SUFFIXES cmake/@LIB@)
	include(${@LIB@_CMAKE_FILE})

	find_file(@LIB@_MACROS_FILE Macros.cmake PATH_SUFFIXES cmake/Digi)
	include(${@LIB@_MACROS_FILE})
endif()
set(@LIB@_FOUND 1)

# find include directory
find_path(@LIB@_INCLUDE_DIRS @FIND_INCLUDE@)

# initialize variables
set(@LIB@_PACKAGES)
set(@LIB@_DEFINITIONS -DBOOST_ALL_NO_LIB)
set(@LIB@_LIBRARIES)
set(@LIB@_INIT_DONE)

# the components and the libraries they need
@COMPONENT_INFO@

# determine the needed libraries for the given components
foreach(_COMPONENT ${@LIB@_FIND_COMPONENTS})
	list(APPEND @LIB@_PACKAGES ${_${_COMPONENT}_PACKAGES})
	list(APPEND @LIB@_INCLUDE_DIRS ${_${_COMPONENT}_INCLUDE_DIRS})
	list(APPEND @LIB@_DEFINITIONS ${_${_COMPONENT}_DEFINITIONS})
	list(APPEND @LIB@_LIBRARIES ${_${_COMPONENT}_LIBRARIES})
	list(APPEND @LIB@_INIT_DONE ${_${_COMPONENT}_INIT_DONE})
endforeach()

# remove duplicates
if(@LIB@_PACKAGES)
	list(REMOVE_DUPLICATES @LIB@_PACKAGES)
endif()
list(REMOVE_DUPLICATES @LIB@_INCLUDE_DIRS)
if(@LIB@_DEFINITIONS)
	list(REMOVE_DUPLICATES @LIB@_DEFINITIONS)
endif()
#list(REMOVE_DUPLICATES @LIB@_LIBRARIES)
if(@LIB@_INIT_DONE)
	list(REMOVE_DUPLICATES @LIB@_INIT_DONE)
endif()
