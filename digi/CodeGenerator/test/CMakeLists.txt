# create test project
ADD_GTEST(TestCodeGenerator
	FILES
		TestCodeGenerator.cpp
	LIBRARIES
		${Boost_LIBRARIES} ${CodeGenerator_LIBRARIES}
)

# create InitLibraries.h containing initLibraries() that calls init functions of all libraries
CREATE_INIT_LIBRARIES(${CodeGenerator_INIT_DONE})
