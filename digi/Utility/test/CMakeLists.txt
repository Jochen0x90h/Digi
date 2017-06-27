# create test project
ADD_GTEST(TestUtility
	FILES 
		TestUtility.cpp
	LIBRARIES 
		${Utility_LIBRARIES}
)

# add definitions
add_definitions(${Utility_DEFINITIONS})

# create InitLibraries.h containing initLibraries() that calls init functions of all libraries
CREATE_INIT_LIBRARIES(${Utility_INIT_DONE})
