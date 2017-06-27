set(FILES
	main.cpp
)

# add executable target
add_executable(digiSceneConvertTool ${FILES})
source_group(Files FILES ${FILES})

# set dependencies
target_link_libraries(digiSceneConvertTool
	${Boost_LIBRARIES}
	${SceneConvert_LIBRARIES}
)

# ceate InitLibraries.h containing initLibraries() that calls init functions of all libraries
CREATE_INIT_LIBRARIES(${SceneConvert_INIT_DONE})
