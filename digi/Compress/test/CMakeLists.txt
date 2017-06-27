# create test project
ADD_GTEST(TestCompress
	FILES
		TestCompress.cpp
	LIBRARIES
		${Boost_LIBRARIES}
		${Compress_LIBRARIES}
)

# add definitions
add_definitions(${Compress_DEFINITIONS})

# create InitLibraries.h containing initLibraries() that calls init functions of all libraries
CREATE_INIT_LIBRARIES(${Compress_INIT_DONE})
