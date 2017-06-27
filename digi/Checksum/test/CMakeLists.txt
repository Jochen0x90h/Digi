# create test project
ADD_GTEST(TestChecksum
	FILES
		TestChecksum.cpp
	LIBRARIES
		${Boost_LIBRARIES} ${Checksum_LIBRARIES}
)

# create InitLibraries.h containing initLibraries() that calls init functions of all libraries
CREATE_INIT_LIBRARIES(${Checksum_INIT_DONE})
