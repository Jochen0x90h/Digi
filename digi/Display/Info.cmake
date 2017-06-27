set(Display_DEPENDENCIES System OpenGL)
set(Display_HAS_INIT_DONE YES)

if(APPLE)
	find_library(COCOA_LIBRARY Cocoa)
	set(Display_LIBRARIES ${COCOA_LIBRARY})
endif()
