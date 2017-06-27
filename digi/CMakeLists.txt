# find StructIO
find_program(STRUCTIO structio)
if(STRUCTIO)
	message("Using StructIO on .struct files")
endif()

# include Info.cmake for each library
foreach(LIB ${LIBS})
	include(${LIB}/Info.cmake)
endforeach()

# set library name and file for finding include directory
set(LIB Digi)
set(FIND_INCLUDE digi/Base/All.h)

# resolve dependencies and set COMPONENT_INFO variable
RESOLVE_DEPENDENCIES(digi "${LIBS}")

# create cmake config file from template (for other libraries that use this library)
# uses variables LIB, INCLUDE, COMPONENT_INFO
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/../cmake/ConfigTemplate.cmake
	${CMAKE_BINARY_DIR}/DigiConfig.cmake
	@ONLY)

# install cmake config file
install(FILES
	${CMAKE_BINARY_DIR}/DigiConfig.cmake
	DESTINATION cmake
)

# add the libraries
foreach(LIB ${LIBS})
	#message("adding lib ${LIB}")
	add_subdirectory(${LIB})
endforeach()
