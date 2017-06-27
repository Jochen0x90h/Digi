# create test project
ADD_GTEST(TestSystem
	FILES
		TestSystem.cpp
	LIBRARIES
		${Boost_LIBRARIES}
		${System_LIBRARIES}
)

# add definitions
add_definitions(${System_DEFINITIONS})

if(TEST_SERIAL_DEVICE)
	add_definitions(-DTEST_SERIAL_DEVICE=${TEST_SERIAL_DEVICE})
endif()

# create InitLibraries.h containing initLibraries() that calls init functions of all libraries
CREATE_INIT_LIBRARIES(${System_INIT_DONE})
