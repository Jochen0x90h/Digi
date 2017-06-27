# create test project
ADD_GTEST(TestAudio
	FILES
		TestAudio.cpp
	LIBRARIES
		${Boost_LIBRARIES}
		${Audio_LIBRARIES}
)

# create InitLibraries.h containing initLibraries() that calls init functions of all libraries
CREATE_INIT_LIBRARIES(${Audio_INIT_DONE})
