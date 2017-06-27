# create test project
ADD_GTEST(TestData
	FILES
		TestData.cpp
	LIBRARIES
		${Boost_LIBRARIES}
		${Data_LIBRARIES}
)

# add definitions
add_definitions(${Data_DEFINITIONS})

# create InitLibraries.h containing initLibraries() that calls init functions of all libraries
CREATE_INIT_LIBRARIES(${Data_INIT_DONE})
