# create test project
ADD_GTEST(TestVideo
	FILES
		TestVideo.cpp
	LIBRARIES
		${Boost_LIBRARIES}
		${Display_LIBRARIES}
		${Video_LIBRARIES}
)

# create application bundle on macos
set_target_properties(TestVideo PROPERTIES MACOSX_BUNDLE TRUE)

# create InitLibraries.h containing initLibraries() that calls init functions of all libraries
CREATE_INIT_LIBRARIES(${Display_INIT_DONE} ${Video_INIT_DONE})
