# create test project
ADD_GTEST(TestNetwork
	FILES
		TestNetwork.cpp
	LIBRARIES
		${Boost_LIBRARIES} ${Network_LIBRARIES}
)

# create InitLibraries.h containing initLibraries() that calls init functions of all libraries
CREATE_INIT_LIBRARIES(${Network_INIT_DONE})
