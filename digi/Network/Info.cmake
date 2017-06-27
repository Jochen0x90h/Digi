set(Network_DEPENDENCIES System)
set(Network_HAS_INIT_DONE YES)

# platform dependent libraries
if(WIN32)
	set(Network_LIBRARIES ws2_32)
endif()
