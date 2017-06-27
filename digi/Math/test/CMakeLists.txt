# create test project
ADD_GTEST(TestMath
	FILES
		TestMath.cpp
	LIBRARIES
		${Math_LIBRARIES}
)

# add definitions
add_definitions(${Math_DEFINITIONS})

# create InitLibraries.h containing initLibraries() that calls init functions of all libraries
CREATE_INIT_LIBRARIES(${Math_INIT_DONE})
