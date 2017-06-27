#include "GLWrapper.h"

#if defined(GL_ES)

	// OpenGL ES
	#include <EGL/egl.h>

	PFNGLMAPBUFFEROESPROC glMapBuffer = NULL;
	PFNGLUNMAPBUFFEROESPROC glUnmapBuffer = NULL;

	// note: eglGetProcAddress needs current GL context
	#define OES(type, name) name = (type)eglGetProcAddress(#name"OES")

	namespace digi {

	bool initGL()
	{
		// map buffer
		OES(PFNGLMAPBUFFEROESPROC, glMapBuffer);
		OES(PFNGLUNMAPBUFFEROESPROC, glUnmapBuffer);
		return true;
	}

	} // namespace digi

#elif defined(_WIN32)

	// point sprite
	#if !defined(GL_CORE) || GL_MAJOR <= 2
		PFNGLPOINTPARAMETERIPROC glPointParameteri = NULL;
	#endif

	// blend state
	PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate = NULL;
	PFNGLBLENDEQUATIONPROC glBlendEquation = NULL;
	PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate = NULL;
	PFNGLBLENDCOLORPROC glBlendColor = NULL;

	// texture
	PFNGLACTIVETEXTUREPROC glActiveTexture = NULL;
	PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture = NULL;
	PFNGLTEXIMAGE3DPROC glTexImage3D = NULL;
	PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D = NULL;
	PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D = NULL;

	// buffer
	PFNGLGENBUFFERSPROC glGenBuffers = NULL;
	PFNGLDELETEBUFFERSPROC glDeleteBuffers = NULL;
	PFNGLBINDBUFFERPROC glBindBuffer = NULL;
	PFNGLBUFFERDATAPROC glBufferData = NULL;
	PFNGLBUFFERSUBDATAPROC glBufferSubData = NULL;
	PFNGLMAPBUFFERPROC glMapBuffer = NULL;
	PFNGLUNMAPBUFFERPROC glUnmapBuffer = NULL;

	// vertex array (opengl 3.0)
	#if GL_MAJOR >= 3
		PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = NULL;
		PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = NULL;
		PFNGLBINDVERTEXARRAYPROC glBindVertexArray = NULL;
	#endif

	// shader
	PFNGLCREATESHADERPROC glCreateShader = NULL;
	PFNGLDELETESHADERPROC glDeleteShader = NULL;
	PFNGLSHADERSOURCEPROC glShaderSource = NULL;
	PFNGLCOMPILESHADERPROC glCompileShader = NULL;
	PFNGLATTACHSHADERPROC glAttachShader = NULL;
	PFNGLCREATEPROGRAMPROC glCreateProgram = NULL;
	PFNGLDELETEPROGRAMPROC glDeleteProgram = NULL;
	PFNGLLINKPROGRAMPROC glLinkProgram = NULL;
	PFNGLUSEPROGRAMPROC glUseProgram = NULL;

	PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;
	PFNGLUNIFORM1IPROC glUniform1i = NULL;
	PFNGLUNIFORM2IPROC glUniform2i = NULL;
	PFNGLUNIFORM3IPROC glUniform3i = NULL;
	PFNGLUNIFORM4IPROC glUniform4i = NULL;
	PFNGLUNIFORM1FPROC glUniform1f = NULL;
	PFNGLUNIFORM2FPROC glUniform2f = NULL;
	PFNGLUNIFORM3FPROC glUniform3f = NULL;
	PFNGLUNIFORM4FPROC glUniform4f = NULL;
	PFNGLUNIFORM1IVPROC glUniform1iv = NULL;
	PFNGLUNIFORM2IVPROC glUniform2iv = NULL;
	PFNGLUNIFORM3IVPROC glUniform3iv = NULL;
	PFNGLUNIFORM4IVPROC glUniform4iv = NULL;
	PFNGLUNIFORM1FVPROC glUniform1fv = NULL;
	PFNGLUNIFORM2FVPROC glUniform2fv = NULL;
	PFNGLUNIFORM3FVPROC glUniform3fv = NULL;
	PFNGLUNIFORM4FVPROC glUniform4fv = NULL;
	PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv = NULL;
	PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv = NULL;
	PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = NULL;

	PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation = NULL;
	PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation = NULL;

	PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = NULL;
	PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = NULL;
	PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = NULL;

	PFNGLGETSHADERIVPROC glGetShaderiv = NULL;
	PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = NULL;
	PFNGLGETPROGRAMIVPROC glGetProgramiv = NULL;
	PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = NULL;

	// multiple render targets
	PFNGLDRAWBUFFERSPROC glDrawBuffers = NULL;

	// frame buffer, render buffer (opengl 3.0)
	PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = NULL;
	PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers = NULL;
	PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = NULL;
	PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer = NULL;
	PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D = NULL;
	PFNGLFRAMEBUFFERTEXTURE3DPROC glFramebufferTexture3D = NULL;
	PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus = NULL;
	#if GL_MAJOR >= 3
		PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer = NULL;
	#endif

	PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers = NULL;
	PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers = NULL;
	PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer = NULL;
	PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage = NULL;
	#if GL_MAJOR >= 3
		PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample = NULL;
	#endif

	PFNGLGENERATEMIPMAPPROC glGenerateMipmap = NULL;


	namespace digi {

	bool initGL()
	{
		// note: wglGetProcAddress needs current GL context
		#define GL(type, name) name = (type)wglGetProcAddress(#name)
		#define GLEXT(type, name) name = (type)wglGetProcAddress(#name"EXT")

		// point sprite (up to opengl 2.x)
	#if !defined(GL_CORE) || GL_MAJOR <= 2
		GL(PFNGLPOINTPARAMETERIPROC, glPointParameteri);
	#endif

		// blend state
		GL(PFNGLBLENDFUNCSEPARATEPROC, glBlendFuncSeparate);
		GL(PFNGLBLENDEQUATIONPROC, glBlendEquation);
		GL(PFNGLBLENDEQUATIONSEPARATEPROC, glBlendEquationSeparate);
		GL(PFNGLBLENDCOLORPROC, glBlendColor);
	
		// texture
		GL(PFNGLACTIVETEXTUREPROC, glActiveTexture);
		GL(PFNGLCLIENTACTIVETEXTUREPROC, glClientActiveTexture); 
		GL(PFNGLTEXIMAGE3DPROC, glTexImage3D); 
		GL(PFNGLTEXSUBIMAGE3DPROC, glTexSubImage3D); 
		GL(PFNGLCOMPRESSEDTEXIMAGE2DPROC, glCompressedTexImage2D); 
	
		// buffer
		GL(PFNGLGENBUFFERSPROC, glGenBuffers);
		GL(PFNGLDELETEBUFFERSPROC, glDeleteBuffers);
		GL(PFNGLBINDBUFFERPROC, glBindBuffer);
		GL(PFNGLBUFFERDATAPROC, glBufferData);
		GL(PFNGLBUFFERSUBDATAPROC, glBufferSubData);
		GL(PFNGLMAPBUFFERPROC, glMapBuffer);
		GL(PFNGLUNMAPBUFFERPROC, glUnmapBuffer);

		// vertex array (opengl 3.0)
	#if GL_MAJOR >= 3
		GL(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays);
		GL(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays);
		GL(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray);
	#endif
	
		// shader
		GL(PFNGLCREATESHADERPROC, glCreateShader);
		GL(PFNGLDELETESHADERPROC, glDeleteShader);
		GL(PFNGLSHADERSOURCEPROC, glShaderSource);
		GL(PFNGLCOMPILESHADERPROC, glCompileShader);
		GL(PFNGLATTACHSHADERPROC, glAttachShader);
		GL(PFNGLCREATEPROGRAMPROC, glCreateProgram);
		GL(PFNGLDELETEPROGRAMPROC, glDeleteProgram);
		GL(PFNGLLINKPROGRAMPROC, glLinkProgram);
		GL(PFNGLUSEPROGRAMPROC, glUseProgram);
	
		GL(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);
		GL(PFNGLUNIFORM1IPROC, glUniform1i);
		GL(PFNGLUNIFORM2IPROC, glUniform2i);
		GL(PFNGLUNIFORM3IPROC, glUniform3i);
		GL(PFNGLUNIFORM4IPROC, glUniform4i);
		GL(PFNGLUNIFORM1FPROC, glUniform1f);
		GL(PFNGLUNIFORM2FPROC, glUniform2f);
		GL(PFNGLUNIFORM3FPROC, glUniform3f);
		GL(PFNGLUNIFORM4FPROC, glUniform4f);
		GL(PFNGLUNIFORM1IVPROC, glUniform1iv);
		GL(PFNGLUNIFORM2IVPROC, glUniform2iv);
		GL(PFNGLUNIFORM3IVPROC, glUniform3iv);
		GL(PFNGLUNIFORM4IVPROC, glUniform4iv);
		GL(PFNGLUNIFORM1FVPROC, glUniform1fv);
		GL(PFNGLUNIFORM2FVPROC, glUniform2fv);
		GL(PFNGLUNIFORM3FVPROC, glUniform3fv);
		GL(PFNGLUNIFORM4FVPROC, glUniform4fv);
		GL(PFNGLUNIFORMMATRIX2FVPROC, glUniformMatrix2fv);
		GL(PFNGLUNIFORMMATRIX3FVPROC, glUniformMatrix3fv);
		GL(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv);
	
		GL(PFNGLGETATTRIBLOCATIONPROC, glGetAttribLocation);
		GL(PFNGLBINDATTRIBLOCATIONPROC, glBindAttribLocation);
	
		GL(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);
		GL(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);
		GL(PFNGLDISABLEVERTEXATTRIBARRAYPROC, glDisableVertexAttribArray);
	
		GL(PFNGLGETSHADERIVPROC, glGetShaderiv);
		GL(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog);
		GL(PFNGLGETPROGRAMIVPROC, glGetProgramiv);
		GL(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog);

		// multiple render targets
		GL(PFNGLDRAWBUFFERSPROC, glDrawBuffers);

		const char* extensions = (const char*)glGetString(GL_EXTENSIONS);

		// frame buffer, render buffer (opengl 2.x extension, opengl 3.0)
	#if GL_MAJOR == 2
		if (strstr(extensions, "ARB_framebuffer_object") != 0)
		{
			// http://www.opengl.org/registry/specs/ARB/framebuffer_object.txt
	#endif
			GL(PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers);
			GL(PFNGLDELETEFRAMEBUFFERSPROC, glDeleteFramebuffers);
			GL(PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer);
			GL(PFNGLFRAMEBUFFERRENDERBUFFERPROC, glFramebufferRenderbuffer);
			GL(PFNGLFRAMEBUFFERTEXTURE2DPROC, glFramebufferTexture2D);
			GL(PFNGLFRAMEBUFFERTEXTURE3DPROC, glFramebufferTexture3D);
			GL(PFNGLCHECKFRAMEBUFFERSTATUSPROC, glCheckFramebufferStatus);
		#if GL_MAJOR >= 3
			GL(PFNGLBLITFRAMEBUFFERPROC, glBlitFramebuffer);
		#endif

			GL(PFNGLGENRENDERBUFFERSPROC, glGenRenderbuffers);
			GL(PFNGLDELETERENDERBUFFERSPROC, glDeleteRenderbuffers);
			GL(PFNGLBINDRENDERBUFFERPROC, glBindRenderbuffer);
			GL(PFNGLRENDERBUFFERSTORAGEPROC, glRenderbufferStorage);
		#if GL_MAJOR >= 3
			GL(PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC, glRenderbufferStorageMultisample);
		#endif

			GL(PFNGLGENERATEMIPMAPPROC, glGenerateMipmap); 

	#if GL_MAJOR == 2
		}
		else if (strstr(extensions, "EXT_framebuffer_object") != 0)
		{
			// http://www.opengl.org/registry/specs/EXT/framebuffer_object.txt
			GLEXT(PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers);
			GLEXT(PFNGLDELETEFRAMEBUFFERSPROC, glDeleteFramebuffers);
			GLEXT(PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer);
			GLEXT(PFNGLFRAMEBUFFERRENDERBUFFERPROC, glFramebufferRenderbuffer);
			GLEXT(PFNGLFRAMEBUFFERTEXTURE2DPROC, glFramebufferTexture2D);
			GLEXT(PFNGLFRAMEBUFFERTEXTURE3DPROC, glFramebufferTexture3D);
			GLEXT(PFNGLCHECKFRAMEBUFFERSTATUSPROC, glCheckFramebufferStatus);

			GLEXT(PFNGLGENRENDERBUFFERSPROC, glGenRenderbuffers);
			GLEXT(PFNGLDELETERENDERBUFFERSPROC, glDeleteRenderbuffers);
			GLEXT(PFNGLBINDRENDERBUFFERPROC, glBindRenderbuffer);
			GLEXT(PFNGLRENDERBUFFERSTORAGEPROC, glRenderbufferStorage);

			GLEXT(PFNGLGENERATEMIPMAPPROC, glGenerateMipmap); 
		}
		else
			return false;
	#endif
		return true;
	}

	} // namespace digi

#else

	namespace digi {

	bool initGL()
	{
		return true;
	}

	} // namespace digi

#endif
