set(FILES
	main.cpp
)

# add executable target
add_executable(digiCodeGeneratorTool ${FILES})
source_group(Files FILES ${FILES})

# set dependencies
target_link_libraries(digiCodeGeneratorTool
	${Boost_LIBRARIES} ${System_LIBRARIES}
)

# ceate InitLibraries.h containing initLibraries() that calls init functions of all libraries
CREATE_INIT_LIBRARIES("${System_INIT_DONE}") 


# run tool after build
add_custom_command(TARGET digiCodeGeneratorTool
	POST_BUILD
	COMMAND digiCodeGeneratorTool
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
	COMMENT "*** digiCodeGeneratorTool ***"
)
