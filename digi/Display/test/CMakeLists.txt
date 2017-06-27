# create test project
ADD_GTEST(TestDisplay
	FILES
		TestDisplay.cpp
	LIBRARIES
		# order is important for gcc
		${Display_LIBRARIES} ${Boost_LIBRARIES}
	PROPERTIES MACOSX_BUNDLE TRUE
)

# create application bundle on macos
#set_target_properties(TestDisplay PROPERTIES MACOSX_BUNDLE TRUE)

# add definitions
add_definitions(${Display_DEFINITIONS})

# create InitLibraries.h containing initLibraries() that calls init functions of all libraries
CREATE_INIT_LIBRARIES(${Display_INIT_DONE})
