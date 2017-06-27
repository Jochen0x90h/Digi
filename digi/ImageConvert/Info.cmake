set(ImageConvert_DEPENDENCIES Checksum CodeGenerator Image EngineVM)

# find dependencies
find_package(squish NO_SYSTEM_ENVIRONMENT_PATH)
find_package(PVRTexLib NO_SYSTEM_ENVIRONMENT_PATH)

if(SQUISH_FOUND)
	set(ImageConvert_PACKAGES squish)
	set(ImageConvert_INCLUDE_DIRS ${SQUISH_INCLUDE_DIRS})
	set(ImageConvert_DEFINITIONS -DHAVE_S3TC ${SQUISH_DEFINITIONS})
	set(ImageConvert_LIBRARIES ${SQUISH_LIBRARIES})
else()
	message("squish library not found: no support for S3TC texture compression")
endif()

if(PVRTEXLIB_FOUND)
	list(APPEND ImageConvert_PACKAGES PVRTexLib)
	list(APPEND ImageConvert_INCLUDE_DIRS ${PVRTEXLIB_INCLUDE_DIRS})
	list(APPEND ImageConvert_DEFINITIONS -DHAVE_PVRTC ${PVRTEXLIB_DEFINITIONS})
	list(APPEND ImageConvert_LIBRARIES ${PVRTEXLIB_LIBRARIES})
else()
	message("PVRTexLib not found: no support for PVRTC texture compression")
endif()

set(ImageConvert_HAS_INIT_DONE YES)
