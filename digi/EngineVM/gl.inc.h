// glEnable
BIND_GL_FUNCTION(module, "glEnable", glEnable);

// glDisable
BIND_GL_FUNCTION(module, "glDisable", glDisable);

// glCullFace
BIND_GL_FUNCTION(module, "glCullFace", glCullFace);

// glBlendFunc
BIND_GL_FUNCTION(module, "glBlendFunc", glBlendFunc);

// glBlendFuncSeparate
BIND_GL_FUNCTION(module, "glBlendFuncSeparate", glBlendFuncSeparate);

// glBlendEquation
BIND_GL_FUNCTION(module, "glBlendEquation", glBlendEquation);

// glBlendEquationSeparate
BIND_GL_FUNCTION(module, "glBlendEquationSeparate", glBlendEquationSeparate);

// glBlendColor
BIND_GL_FUNCTION(module, "glBlendColor", glBlendColor);

// glGetFloatv
BIND_GL_FUNCTION(module, "glGetFloatv", glGetFloatv);

// glGenTextures
BIND_GL_FUNCTION(module, "glGenTextures", glGenTextures);

// glDeleteTextures
BIND_GL_FUNCTION(module, "glDeleteTextures", glDeleteTextures);

// glBindTexture
BIND_GL_FUNCTION(module, "glBindTexture", glBindTexture);

// glTexImage2D
BIND_GL_FUNCTION(module, "glTexImage2D", glTexImage2D);

// glTexSubImage2D
BIND_GL_FUNCTION(module, "glTexSubImage2D", glTexSubImage2D);

// glTexImage3D
#if !defined(GL_ES) || GL_MAJOR >= 3
	BIND_GL_FUNCTION(module, "glTexImage3D", glTexImage3D);
#endif

// glTexSubImage3D
#if !defined(GL_ES) || GL_MAJOR >= 3
	BIND_GL_FUNCTION(module, "glTexSubImage3D", glTexSubImage3D);
#endif

// glCompressedTexImage2D
BIND_GL_FUNCTION(module, "glCompressedTexImage2D", glCompressedTexImage2D);

// glGenerateMipmap
BIND_GL_FUNCTION(module, "glGenerateMipmap", glGenerateMipmap);

// glActiveTexture
BIND_GL_FUNCTION(module, "glActiveTexture", glActiveTexture);

// glTexParameteri
BIND_GL_FUNCTION(module, "glTexParameteri", glTexParameteri);

// glTexParameteriv
BIND_GL_FUNCTION(module, "glTexParameteriv", glTexParameteriv);

// glTexParameterfv
BIND_GL_FUNCTION(module, "glTexParameterfv", glTexParameterfv);

// glGenBuffers
BIND_GL_FUNCTION(module, "glGenBuffers", glGenBuffers);

// glDeleteBuffers
BIND_GL_FUNCTION(module, "glDeleteBuffers", glDeleteBuffers);

// glBindBuffer
BIND_GL_FUNCTION(module, "glBindBuffer", glBindBuffer);

// glBufferData
BIND_GL_FUNCTION(module, "glBufferData", glBufferData);

// glBufferSubData
BIND_GL_FUNCTION(module, "glBufferSubData", glBufferSubData);

// glMapBuffer
BIND_GL_FUNCTION(module, "glMapBuffer", glMapBuffer);

// glUnmapBuffer
BIND_GL_FUNCTION(module, "glUnmapBuffer", glUnmapBuffer);

// glGenVertexArrays
#if GL_MAJOR >= 3
	BIND_GL_FUNCTION(module, "glGenVertexArrays", glGenVertexArrays);
#endif

// glDeleteVertexArrays
#if GL_MAJOR >= 3
	BIND_GL_FUNCTION(module, "glDeleteVertexArrays", glDeleteVertexArrays);
#endif

// glBindVertexArray
#if GL_MAJOR >= 3
	BIND_GL_FUNCTION(module, "glBindVertexArray", glBindVertexArray);
#endif

// glCreateShader
BIND_GL_FUNCTION(module, "glCreateShader", glCreateShader);

// glDeleteShader
BIND_GL_FUNCTION(module, "glDeleteShader", glDeleteShader);

// glShaderSource
BIND_GL_FUNCTION(module, "glShaderSource", glShaderSource);

// glCompileShader
BIND_GL_FUNCTION(module, "glCompileShader", glCompileShader);

// glCreateProgram
BIND_GL_FUNCTION(module, "glCreateProgram", glCreateProgram);

// glDeleteProgram
BIND_GL_FUNCTION(module, "glDeleteProgram", glDeleteProgram);

// glAttachShader
BIND_GL_FUNCTION(module, "glAttachShader", glAttachShader);

// glBindAttribLocation
BIND_GL_FUNCTION(module, "glBindAttribLocation", glBindAttribLocation);

// glLinkProgram
BIND_GL_FUNCTION(module, "glLinkProgram", glLinkProgram);

// glUseProgram
BIND_GL_FUNCTION(module, "glUseProgram", glUseProgram);

// glGetUniformLocation
BIND_GL_FUNCTION(module, "glGetUniformLocation", glGetUniformLocation);

// glUniform1i
BIND_GL_FUNCTION(module, "glUniform1i", glUniform1i);

// glUniform4iv
BIND_GL_FUNCTION(module, "glUniform4iv", glUniform4iv);

// glUniform4fv
BIND_GL_FUNCTION(module, "glUniform4fv", glUniform4fv);

// glEnableVertexAttribArray
BIND_GL_FUNCTION(module, "glEnableVertexAttribArray", glEnableVertexAttribArray);

// glDisableVertexAttribArray
BIND_GL_FUNCTION(module, "glDisableVertexAttribArray", glDisableVertexAttribArray);

// glVertexAttribPointer
BIND_GL_FUNCTION(module, "glVertexAttribPointer", glVertexAttribPointer);

// glDrawArrays
BIND_GL_FUNCTION(module, "glDrawArrays", glDrawArrays);

// glDrawElements
BIND_GL_FUNCTION(module, "glDrawElements", glDrawElements);

