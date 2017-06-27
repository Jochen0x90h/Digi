include(CheckIncludeFile)

# check for #include <stdint.h>
CHECK_INCLUDE_FILE(stdint.h HAVE_STDINT_H)

# determine sizeof(wchar_t)
INCLUDE(CheckTypeSize)
SET(CMAKE_REQUIRED_INCLUDES stddef.h)

# detect C99 math
try_compile(HAVE_C99_MATH ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/detectC99Math.cpp OUTPUT_VARIABLE COMPILE_OUT)
#message("HAVE_C99_MATH: ${HAVE_C99_MATH}")
#message("COMPILE_OUT: ${COMPILE_OUT}")

# detect std locale
try_compile(HAVE_LOCALE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/detectLocale.cpp OUTPUT_VARIABLE COMPILE_OUT)
#message("HAVE_LOCALE: ${HAVE_LOCALE}")
#message("COMPILE_OUT: ${COMPILE_OUT}")

# create Config.h from Config.h.cmake using the above info
configure_file(Config.h.cmake ${DIGI_CONFIG_DIR}/Config.h)


# public header files (visible to users of this library)
set(HEADERS
	All.h
	${DIGI_CONFIG_DIR}/Config.h
	Platform.h
	VersionInfo.h
)

# source files
set(FILES
	VersionInfo.cpp
)

# add library digiBase
CREATE_LIBRARY(digi.Base HEADERS ${HEADERS} FILES ${FILES} EXPORT Digi)
