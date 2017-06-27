set(OpenGL_DEPENDENCIES System)
set(OpenGL_HAS_INIT_DONE YES)

# import OpenGL
if(GL_ES AND (GL_MAJOR GREATER 1))

	# opengl es
	set(OpenGL_DEFINITIONS -DGL_ES -DGL_CORE -DGL_MAJOR=${GL_MAJOR} -DGL_MINOR=${GL_MINOR})
	find_path(OpenGL_INCLUDE_DIRS EGL/egl.h)
	
	# check for #include <GLES2/gl2extimg.h>
	include(CheckIncludeFile)
	CHECK_INCLUDE_FILE(${OpenGL_INCLUDE_DIRS}/GLES2/gl2extimg.h HAVE_GL2EXTIMG_H)
	if(HAVE_GL2EXTIMG_H)
		list(APPEND OpenGL_DEFINITIONS -DHAVE_GL2EXTIMG_H)
	endif()

	# find libraries
	if(WIN32)
		find_library(LIBEGL libEGL)
		find_library(LIBGLESV2 libGLESv2)
	else()
		find_library(LIBEGL EGL)
		find_library(LIBGLESV2 GLESv2)
	endif()
	set(OpenGL_LIBRARIES ${LIBEGL} ${LIBGLESV2})

elseif(GL_MAJOR GREATER 1)

	# desktop opengl
	set(OpenGL_DEFINITIONS -DGL_MAJOR=${GL_MAJOR} -DGL_MINOR=${GL_MINOR})
	if(GL_CORE)
		list(APPEND OpenGL_DEFINITIONS -DGL_CORE)
	endif()
	find_package(OpenGL REQUIRED)
	set(OpenGL_LIBRARIES ${OPENGL_gl_LIBRARY})
	
else()

	# error: no opengl version selected
	message(FATAL_ERROR "No OpenGL version selected.\n"
		"define GL_MAJOR and GL_MINOR\n"
		"Desktop OpenGL: optionally define GL_CORE\n"
		"OpenGL ES: define GL_ES")

endif()
