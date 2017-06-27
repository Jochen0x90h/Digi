set(System_DEPENDENCIES Utility)
set(System_HAS_INIT_DONE YES)

# platform dependent libraries
if(WIN32)
	set(System_LIBRARIES winmm)
elseif(APPLE)
	find_library(CORESERVICES_LIBRARY CoreServices)
	set(System_LIBRARIES ${CORESERVICES_LIBRARY})
endif()

if(NO_BOOST_FILESYSTEM)
	list(APPEND System_DEFINITIONS -DNO_BOOST_FILESYSTEM)
endif()
