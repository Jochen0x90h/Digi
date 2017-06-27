# create test project
ADD_GTEST(TestSceneConvert
	FILES
		TestSceneConvert.cpp
	LIBRARIES
		${Boost_LIBRARIES}
		${SceneConvert_LIBRARIES}
)

# add definitions
add_definitions(${SceneConvert_DEFINITIONS})

# create InitLibraries.h containing initLibraries() that calls init functions of all libraries
CREATE_INIT_LIBRARIES(${SceneConvert_INIT_DONE})
