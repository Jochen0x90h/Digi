# create test project
ADD_GTEST(TestImage
	FILES
		TestImage.cpp
	LIBRARIES
		${Boost_LIBRARIES}
		${Image_LIBRARIES}
)

# create InitLibraries.h containing initLibraries() that calls init functions of all libraries
CREATE_INIT_LIBRARIES(${Image_INIT_DONE})
