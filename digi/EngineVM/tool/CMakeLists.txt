# ceate InitLibraries.h containing initLibraries() that calls init functions of all libraries
CREATE_INIT_LIBRARIES(${EngineVM_INIT_DONE})

set(FILES
	InitLibraries.h
	main.cpp
	HeaderBinder.cpp
	HeaderBinder2.cpp
	HeaderBinder.h
)

# disable rtti for classes that derive from llvm/clang
if(UNIX)
	set_source_files_properties(HeaderBinder.cpp
		PROPERTIES COMPILE_FLAGS -fno-rtti)
endif()

# add executable target
add_executable(digiEngineVMTool ${FILES})
source_group(Files FILES ${FILES})

# set include directories
include_directories(${EngineVM_INCLUDE_DIRS})

# add definitions
add_definitions(${EngineVM_DEFINITIONS})

# set dependencies
target_link_libraries(digiEngineVMTool
	${Boost_LIBRARIES}
	#${LLVM_LIBRARIES}
	#${CLANG_LIBRARIES}
	#LLVMCppBackend
	#LLVMCppBackendInfo
	${EngineVM_LIBRARIES}
	${CodeGenerator_LIBRARIES}
)
