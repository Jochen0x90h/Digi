set(Video_DEPENDENCIES Data Audio OpenGL)

# find dependencies
find_package(vpx NO_SYSTEM_ENVIRONMENT_PATH)

if(VPX_FOUND)
	set(Video_PACKAGES vpx)
	set(Video_INCLUDE_DIRS ${VPX_INCLUDE_DIRS})
	set(Video_DEFINITIONS ${VPX_DEFINITIONS} -DHAVE_VPX)
	set(Video_LIBRARIES ${VPX_LIBRARIES})  
endif()

set(Video_HAS_INIT_DONE YES)
