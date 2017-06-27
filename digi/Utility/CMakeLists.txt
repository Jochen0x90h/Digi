# public header files (visible to users of this library)
set(HEADERS
	All.h
	ArrayUtility.h
	as.h
	Ascii.h
	Convert.h
	ArrayRef.h
	Find.h
	foreach.h
	IntUtility.h
	lexicalCast.h
	ListUtility.h
	malloc16.h
	MapUtility.h
	MemoryPool.h
	Object.h
	Pointer.h
	ScopedAssign.h
	SetUtility.h
	Signal.h
	Standard.h
	StringUtility.h
	StringRef.h
	UtfTranscode.h
	VectorUtility.h
)

# source files
set(FILES
	All.cpp
	Ascii.cpp
	Convert.cpp
	Object.cpp
	Pointer.cpp
	StringUtility.cpp
	StringRef.cpp
)

# add definitions
add_definitions(${Utility_DEFINITIONS})
 
# add library digiUtility
CREATE_LIBRARY(digi.Utility HEADERS ${HEADERS} FILES ${FILES} VERSIONIZE EXPORT Digi)


# add test
add_subdirectory(test)
