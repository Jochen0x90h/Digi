# public header files (visible to users of this library)
set(HEADERS
	All.h
)

# source files
set(FILES
	All.cpp
)

# add definitions
add_definitions(${LLVMJit_DEFINITIONS})

# add library digiLLVMJit
CREATE_LIBRARY(digi.LLVMJit HEADERS ${HEADERS} FILES ${FILES} EXPORT Digi)
