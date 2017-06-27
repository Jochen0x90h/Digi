set(FILES
	main.cpp
)

# add executable target
add_executable(digiDisplayTool ${FILES})
source_group(Files FILES ${FILES})

# set dependencies
target_link_libraries(digiDisplayTool
	${Boost_LIBRARIES} ${System_LIBRARIES}
)

# add definitions
add_definitions(${Display_DEFINITIONS})

# ceate InitLibraries.h containing initLibraries() that calls init functions of all libraries
CREATE_INIT_LIBRARIES("${System_INIT_DONE}") 
