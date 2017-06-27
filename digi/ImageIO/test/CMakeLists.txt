# create test project
ADD_GTEST(TestImageIO
	FILES
		TestImageIO.cpp
	LIBRARIES
		${Boost_LIBRARIES}
		${ImageIO_LIBRARIES}
)

# create InitLibraries.h containing initLibraries() that calls init functions of all libraries
CREATE_INIT_LIBRARIES(${ImageIO_INIT_DONE})
