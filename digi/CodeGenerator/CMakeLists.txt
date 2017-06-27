# public header files (visible to users of this library)
set(HEADERS
	All.h
	Attribute.h
	BinOpNode.h
	CodeWriter.h
	CodeWriterFunctions.h
	ConditionNode.h
	Language.h
	LeafNode.h
	MultiplyAddNode.h
	NameGenerator.h
	NameMangler.h
	Node.h
	NodeFunctions.h
	NodeVisitor.h
	NodeWriter.h
	Operators.h
	Path.h
	ScriptNode.h
	TreeNode.h
	Type.h
	TypeInfo.h
)

# source files
set(FILES
	All.cpp
	Attribute.cpp
	BinOpNode.cpp
	CodeWriter.cpp
	CodeWriterFunctions.cpp
	ConditionNode.cpp
	forbiddenVariables.h
	LeafNode.cpp
	MultiplyAddNode.cpp
	NameGenerator.cpp
	NameMangler.cpp
	Node.cpp
	NodeFunctions.cpp
	NodeVisitor.cpp
	NodeWriter.cpp
	Path.cpp
	ScriptNode.cpp
	TreeNode.cpp
	Type.cpp
	TypeInfo.cpp
)

# add definitions
add_definitions(${CodeGenerator_DEFINITIONS})

# add library digiCodeGenerator
CREATE_LIBRARY(digi.CodeGenerator HEADERS ${HEADERS} FILES ${FILES} VERSIONIZE EXPORT Digi)


# add test
add_subdirectory(test)

# add tool
if(NOT NO_TOOLS)
	add_subdirectory(tool)
endif()
