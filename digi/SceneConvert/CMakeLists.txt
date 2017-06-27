# public header files (visible to users of this library)
set(HEADERS
	All.h
	BufferInfo.h
	CompileHelper.h
	compileScene.h
	compileSceneWebGL.h
	compileShader.h
	CopyWalker.h
	DeformerPass.h
	generateSceneOpenGL.h
	generateSceneWebGL.h
	generateTextureOpenGL.h
	generateTextureWebGL.h
	GeneratorCollector.h
	GlobalCollector.h
	GlobalPrinter.h
	IndexBuffer.h
	InstructionWalker.h
	MCCache.h
	ParticlePass.h
	PrintHelper.h
	PrintPass.h
	PrintWalker.h
	ReplacePass.h
	Scalarizer.h
	ScalarizerPass.h
	SceneOptions.h
	ShaderOptions.h
	ShaderGlobalPrinter.h
	ShaderPass.h
	ShaderType.h
	ShaderWalker.h
	SplittedGlobalPrinter.h
	TextureDataInfo.h
	TextureFileInfo.h
	TextureFormat.h
	ValueHelper.h
	VertexBuffer.h
	writeForMC.h
	writeForVM.h
	writeForWebGL.h
)

# source files
set(FILES
	All.cpp
	BufferInfo.cpp
	CompileHelper.cpp
	compileScene.cpp
	compileSceneWebGL.cpp
	compileShader.cpp
	CopyWalker.cpp
	DeformerPass.cpp
	generateSceneOpenGL.cpp
	generateSceneWebGL.cpp
	generateTextureOpenGL.cpp
	generateTextureWebGL.cpp
	GeneratorCollector.cpp
	GlobalCollector.cpp
	GlobalPrinter.cpp
	IndexBuffer.cpp
	InstructionWalker.cpp
	MCCache.cpp
	ParticlePass.cpp
	PrintHelper.cpp
	PrintPass.cpp
	PrintWalker.cpp
	ReplacePass.cpp
	Scalarizer.cpp
	ScalarizerPass.cpp
	ShaderGlobalPrinter.cpp
	ShaderPass.cpp
	ShaderType.cpp
	ShaderWalker.cpp
	SplittedGlobalPrinter.cpp
	TextureDataInfo.cpp
	TextureFileInfo.cpp
	TextureFormat.cpp
	ValueHelper.cpp
	VertexBuffer.cpp
	writeForMC.cpp
	writeForVM.cpp
	writeForWebGL.cpp
)

# disable rtti for classes that derive from llvm/clang
if(UNIX)
	set_source_files_properties(GlobalCollector.cpp
		PROPERTIES COMPILE_FLAGS -fno-rtti)
endif()

# set include directories
include_directories(${SceneConvert_INCLUDE_DIRS})

# add definitions
add_definitions(${SceneConvert_DEFINITIONS})

# add library digiSceneConvert
CREATE_LIBRARY(digi.SceneConvert HEADERS ${HEADERS} FILES ${FILES} VERSIONIZE EXPORT Digi)


# add test
add_subdirectory(test)

# add tool
if(NOT NO_TOOLS)
	add_subdirectory(tool)
endif()
