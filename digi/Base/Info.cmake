# this is the root, therefore no dependencies
set(Base_DEPENDENCIES )

if(WIN32)
	# disable security warnings
	# disable min max macros from windows.h
	# disable auto-linking of boost libraries
	set(Base_DEFINITIONS -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -DNOMINMAX -DBOOST_ALL_NO_LIB)
elseif(UNIX)
	# cocoa: prevent definition of assert macros such as check that conflict with boost
	set(Base_DEFINITIONS -fvisibility=hidden -D__ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES=0)
endif()
