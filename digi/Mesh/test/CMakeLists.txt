# create test project
ADD_GTEST(TestMesh
	FILES
		TestMesh.cpp
	LIBRARIES
		${Boost_LIBRARIES} ${Mesh_LIBRARIES}
)

# create InitLibraries.h containing initLibraries() that calls init functions of all libraries
CREATE_INIT_LIBRARIES(${Mesh_INIT_DONE})
