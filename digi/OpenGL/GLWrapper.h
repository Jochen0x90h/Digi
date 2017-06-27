#ifndef digi_OpenGL_GLWrapper_h
#define digi_OpenGL_GLWrapper_h

#include <digi/System/Log.h>

#if defined(GL_ES)

	// OpenGL ES
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#ifdef HAVE_GL2EXTIMG_H
		#include <GLES2/gl2extimg.h>
	#endif
	
	extern PFNGLMAPBUFFEROESPROC glMapBuffer;
	extern PFNGLUNMAPBUFFEROESPROC glUnmapBuffer;

	#define GL_WRITE_ONLY GL_WRITE_ONLY_OES

#elif defined(_WIN32)

	// Windows
	#include <Windows.h>
	#undef interface

	#include <gl/gl.h>	
	#include "glext.h"

	/*
		On Windows the following OpenGL functions are made available via global pointers.
		If you create a display with Display::create() they are initialized automatically.
		If you use some other display (e.g. wxWidgets/wxGLCanvas or Java/LWJGL) then call
		Display::initGL()
	*/

	// point sprite
	#if !defined(GL_CORE) || GL_MAJOR <= 2
		extern PFNGLPOINTPARAMETERIPROC glPointParameteri;
	#endif

	// blend state
	extern PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
	extern PFNGLBLENDEQUATIONPROC glBlendEquation;
	extern PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate;
	extern PFNGLBLENDCOLORPROC glBlendColor;
	
	// texture
	extern PFNGLACTIVETEXTUREPROC glActiveTexture;
	extern PFNGLTEXIMAGE3DPROC glTexImage3D;
	extern PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;
	extern PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;
	
	// buffer
	extern PFNGLGENBUFFERSPROC glGenBuffers;
	extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
	extern PFNGLBINDBUFFERPROC glBindBuffer;
	extern PFNGLBUFFERDATAPROC glBufferData;
	extern PFNGLBUFFERSUBDATAPROC glBufferSubData;
	extern PFNGLMAPBUFFERPROC glMapBuffer;
	extern PFNGLUNMAPBUFFERPROC glUnmapBuffer;

	// vertex array (opengl 3.0)
	#if GL_MAJOR >= 3
		extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
		extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
		extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
	#endif

	// shader
	extern PFNGLCREATESHADERPROC glCreateShader;
	extern PFNGLDELETESHADERPROC glDeleteShader;
	extern PFNGLSHADERSOURCEPROC glShaderSource;
	extern PFNGLCOMPILESHADERPROC glCompileShader;
	extern PFNGLCREATEPROGRAMPROC glCreateProgram;
	extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
	extern PFNGLATTACHSHADERPROC glAttachShader;
	extern PFNGLLINKPROGRAMPROC glLinkProgram;
	extern PFNGLUSEPROGRAMPROC glUseProgram;

	extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
	extern PFNGLUNIFORM1IPROC glUniform1i;
	extern PFNGLUNIFORM2IPROC glUniform2i;
	extern PFNGLUNIFORM3IPROC glUniform3i;
	extern PFNGLUNIFORM4IPROC glUniform4i;
	extern PFNGLUNIFORM1FPROC glUniform1f;
	extern PFNGLUNIFORM2FPROC glUniform2f;
	extern PFNGLUNIFORM3FPROC glUniform3f;
	extern PFNGLUNIFORM4FPROC glUniform4f;
	extern PFNGLUNIFORM1IVPROC glUniform1iv;
	extern PFNGLUNIFORM2IVPROC glUniform2iv;
	extern PFNGLUNIFORM3IVPROC glUniform3iv;
	extern PFNGLUNIFORM4IVPROC glUniform4iv;
	extern PFNGLUNIFORM1FVPROC glUniform1fv;
	extern PFNGLUNIFORM2FVPROC glUniform2fv;
	extern PFNGLUNIFORM3FVPROC glUniform3fv;
	extern PFNGLUNIFORM4FVPROC glUniform4fv;
	extern PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv;
	extern PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv;
	extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;

	extern PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
	extern PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;

	extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
	extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
	extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;

	extern PFNGLGETSHADERIVPROC glGetShaderiv;
	extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
	extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
	extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;

	// multiple render targets
	extern PFNGLDRAWBUFFERSPROC glDrawBuffers;

	// frame buffer, render buffer (opengl 2.x extension, opengl 3.0)
	extern PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
	extern PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
	extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
	extern PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
	extern PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
	extern PFNGLFRAMEBUFFERTEXTURE3DPROC glFramebufferTexture3D;
	extern PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
	#if GL_MAJOR >= 3
		extern PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;
	#endif

	extern PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
	extern PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers;
	extern PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
	extern PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
	#if GL_MAJOR >= 3
		extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample;
	#endif

	extern PFNGLGENERATEMIPMAPPROC glGenerateMipmap;

#elif defined(__APPLE__)

	// MacOS X
	#if GL_MAJOR <= 2
		#include <OpenGL/gl.h>
	#else
		#include <OpenGL/OpenGL.h>
		#include <OpenGL/gl3.h>
	#endif

#endif

namespace digi {

// create and compile vertex shader and log error
inline GLuint createVertexShader(const char* source, const char* name)
{
	GLuint shader = glCreateShader(GL_VERTEX_SHADER);	
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	// check for compile errors
	GLint result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
	#ifndef NDEBUG
		// print log
		GLsizei length;
		char infoLog[4096];
		glGetShaderInfoLog(shader, 4096, &length, infoLog); 
		dError("vertex shader '" << name << "':" << std::endl << infoLog);
	#else
		dError("shader '" << name << "' failed to compile");		
	#endif
	}
	return shader;
}

// create and compile pixel shader and log error
inline GLuint createPixelShader(const char* source, const char* name)
{
	GLuint shader = glCreateShader(GL_FRAGMENT_SHADER);	
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	// check for compile errors
	GLint result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
	#ifndef NDEBUG
		// print log
		GLsizei length;
		char infoLog[4096];
		glGetShaderInfoLog(shader, 4096, &length, infoLog); 
		dError("pixel shader '" << name << "':" << std::endl << infoLog);
	#else
		dError("shader '" << name << "' failed to compile");		
	#endif
	}
	return shader;
}
	
// texture types
typedef GLuint Texture2D;
typedef GLuint Texture3D;
typedef GLuint TextureCube;

// call initGL to init pointers to gl functions (digi::Display does this automatically). needs current GL context
bool initGL();
	
} // namespace digi
	
#endif
