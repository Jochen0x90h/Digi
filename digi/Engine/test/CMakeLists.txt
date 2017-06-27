# create test project
ADD_GTEST(TestEngine
	FILES
		TestEngine.cpp
	LIBRARIES
		${Boost_LIBRARIES}
		${Engine_LIBRARIES}
)

# add definitions
add_definitions(${Engine_DEFINITIONS})

# create InitLibraries.h containing initLibraries() that calls init functions of all libraries
CREATE_INIT_LIBRARIES(${Engine_INIT_DONE})
