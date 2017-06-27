# add generator tool
add_subdirectory(tool)
 
 
# public header files (visible to users of this library)
set(HEADERS
	All.h
	CodeGenWrapper.h
	Compiler.h
	GetStructs.h
	MemoryPass.h
	ModuleLoader.h
	PrintModulePass.h
	TargetInfo.h
	VMFile.h
)

# source files
set(FILES
	All.cpp
	CodeGenWrapper.cpp
	Compiler.cpp
	GetStructs.cpp
	MemoryPass.cpp
	ModuleLoader.cpp
	PrintModulePass.cpp
	TargetInfo.cpp
	VMFile.cpp
)

# disable rtti for classes that derive from llvm/clang
if(UNIX)
	set_source_files_properties(
		CodeGenWrapper.cpp
		GetStructs.cpp
		ModuleLoader.cpp
		PrintModulePass.cpp
		TargetInfo.cpp
		PROPERTIES COMPILE_FLAGS -fno-rtti)
endif()

# add definitions
add_definitions(${EngineVM_DEFINITIONS})
 
# add library digiEngineVM
CREATE_LIBRARY(digi.EngineVM HEADERS ${HEADERS} FILES ${FILES} VERSIONIZE EXPORT Digi)
