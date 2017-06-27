# create test project
ADD_GTEST(TestImageConvert
	FILES
		TestImageConvert.cpp
	LIBRARIES
		${Boost_LIBRARIES}
		${ImageConvert_LIBRARIES}
)

# create InitLibraries.h containing initLibraries() that calls init functions of all libraries
CREATE_INIT_LIBRARIES(${ImageConvert_INIT_DONE})
