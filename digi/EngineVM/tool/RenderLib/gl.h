#ifndef gl_h
#define gl_h

#include "glTypes.h"

enum
{
	// value
	//GL_FALSE                              = 0,
	//GL_TRUE                               = 1,
	GL_ZERO                               = 0,
	GL_ONE                                = 1,

	// type
	GL_BYTE                               = 0x1400,
	GL_UNSIGNED_BYTE                      = 0x1401,
	GL_SHORT                              = 0x1402,
	GL_UNSIGNED_SHORT                     = 0x1403,
	GL_INT                                = 0x1404,
	GL_UNSIGNED_INT                       = 0x1405,
	GL_HALF_FLOAT                         = 0x140B,
	GL_FLOAT                              = 0x1406,
	GL_DOUBLE                             = 0x140A,

	// cull state
	GL_CULL_FACE                          = 0x0B44,

	GL_FRONT                              = 0x0404,
	GL_BACK                               = 0x0405,

	// blend state
	GL_BLEND                              = 0x0BE2,

	//GL_ZERO
	//GL_ONE
	GL_SRC_COLOR                          = 0x0300,
	GL_ONE_MINUS_SRC_COLOR                = 0x0301,
	GL_DST_COLOR                          = 0x0306,
	GL_ONE_MINUS_DST_COLOR                = 0x0307,
	GL_SRC_ALPHA                          = 0x0302,
	GL_ONE_MINUS_SRC_ALPHA                = 0x0303,
	GL_DST_ALPHA                          = 0x0304,
	GL_ONE_MINUS_DST_ALPHA                = 0x0305,
	GL_CONSTANT_COLOR                     = 0x8001,
	GL_ONE_MINUS_CONSTANT_COLOR           = 0x8002,
	GL_CONSTANT_ALPHA                     = 0x8003,
	GL_ONE_MINUS_CONSTANT_ALPHA           = 0x8004,
	GL_SRC_ALPHA_SATURATE                 = 0x0308,

	GL_FUNC_ADD                           = 0x8006,
	GL_FUNC_SUBTRACT                      = 0x800A,
	GL_FUNC_REVERSE_SUBTRACT              = 0x800B,
	GL_MIN                                = 0x8007,
	GL_MAX                                = 0x8008,

	// get state
	GL_VIEWPORT                           = 0x0BA2,

	// channels
	GL_RED                                = 0x1903,
	GL_GREEN                              = 0x1904,
	GL_BLUE                               = 0x1905,
	GL_ALPHA                              = 0x1906,

	// pixel format
	//GL_RED
	GL_RG                                 = 0x8227,
	GL_RGB                                = 0x1907,
	GL_RGBA                               = 0x1908,
	GL_LUMINANCE                          = 0x1909, // <= OpenGL 2.1
	GL_LUMINANCE_ALPHA                    = 0x190A, // <= OpenGL 2.1

	// pixel type
	GL_UNSIGNED_BYTE_2_3_3_REV            = 0x8362,
	GL_UNSIGNED_SHORT_5_6_5_REV           = 0x8364,
	GL_UNSIGNED_SHORT_4_4_4_4_REV         = 0x8365,
	GL_UNSIGNED_SHORT_1_5_5_5_REV         = 0x8366,
	GL_UNSIGNED_INT_8_8_8_8_REV           = 0x8367,
	GL_UNSIGNED_INT_2_10_10_10_REV        = 0x8368,
	GL_UNSIGNED_INT_5_9_9_9_REV           = 0x8C3E,
	GL_UNSIGNED_INT_10F_11F_11F_REV       = 0x8C3B,

	// internal pixel format
	GL_R8                                 = 0x8229,
	GL_R16                                = 0x822A,
	GL_R16F                               = 0x822D, // >= OpenGL 3.2
	GL_R32F                               = 0x822E, // >= OpenGL 3.2

	GL_RG8                                = 0x822B,
	GL_RG16                               = 0x822C,
	GL_RG16F                              = 0x822F, // >= OpenGL 3.2
	GL_RG32F                              = 0x8230, // >= OpenGL 3.2

	GL_RGB8                               = 0x8051,
	GL_RGB16                              = 0x8054,
	GL_RGB16F                             = 0x881B, // >= OpenGL 3.2
	GL_RGB32F                             = 0x8815, // >= OpenGL 3.2
	GL_RGB5                               = 0x8050,
	GL_R11F_G11F_B10F                     = 0x8C3A, // >= OpenGL 3.2
	GL_RGB9_E5                            = 0x8C3D, // >= OpenGL 3.2

	GL_RGBA8                              = 0x8058,
	GL_RGBA16                             = 0x805B,
	GL_RGBA16F                            = 0x881A, // >= OpenGL 3.2
	GL_RGBA32F                            = 0x8814, // >= OpenGL 3.2
	GL_RGB5_A1                            = 0x8057,
	GL_RGB10_A2                           = 0x8059,

	GL_LUMINANCE8                         = 0x8040, // <= OpenGL 2.1
	GL_LUMINANCE16                        = 0x8042, // <= OpenGL 2.1

	GL_LUMINANCE8_ALPHA8                  = 0x8045, // <= OpenGL 2.1
	GL_LUMINANCE16_ALPHA16                = 0x8048, // <= OpenGL 2.1

	// compressed format
	GL_COMPRESSED_RGBA_S3TC_DXT1_EXT      = 0x83F1,
	GL_COMPRESSED_RGBA_S3TC_DXT3_EXT      = 0x83F2,
	GL_COMPRESSED_RGBA_S3TC_DXT5_EXT      = 0x83F3,

	// texture
	//GL_TEXTURE_1D                         = 0x0DE0,
	GL_TEXTURE_2D                         = 0x0DE1,
	GL_TEXTURE_3D                         = 0x806F,
	GL_TEXTURE_CUBE_MAP                   = 0x8513,
	GL_TEXTURE_CUBE_MAP_POSITIVE_X        = 0x8515,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X        = 0x8516,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y        = 0x8517,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y        = 0x8518,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z        = 0x8519,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z        = 0x851A,

	GL_TEXTURE0                           = 0x84C0,
	GL_TEXTURE1                           = 0x84C1,
	GL_TEXTURE2                           = 0x84C2,
	GL_TEXTURE3                           = 0x84C3,
	GL_TEXTURE4                           = 0x84C4,
	GL_TEXTURE5                           = 0x84C5,
	GL_TEXTURE6                           = 0x84C6,
	GL_TEXTURE7                           = 0x84C7,
	GL_TEXTURE8                           = 0x84C8,
	GL_TEXTURE9                           = 0x84C9,
	GL_TEXTURE10                          = 0x84CA,
	GL_TEXTURE11                          = 0x84CB,
	GL_TEXTURE12                          = 0x84CC,
	GL_TEXTURE13                          = 0x84CD,
	GL_TEXTURE14                          = 0x84CE,
	GL_TEXTURE15                          = 0x84CF,
	GL_TEXTURE16                          = 0x84D0,
	GL_TEXTURE17                          = 0x84D1,
	GL_TEXTURE18                          = 0x84D2,
	GL_TEXTURE19                          = 0x84D3,
	GL_TEXTURE20                          = 0x84D4,
	GL_TEXTURE21                          = 0x84D5,
	GL_TEXTURE22                          = 0x84D6,
	GL_TEXTURE23                          = 0x84D7,
	GL_TEXTURE24                          = 0x84D8,
	GL_TEXTURE25                          = 0x84D9,
	GL_TEXTURE26                          = 0x84DA,
	GL_TEXTURE27                          = 0x84DB,
	GL_TEXTURE28                          = 0x84DC,
	GL_TEXTURE29                          = 0x84DD,
	GL_TEXTURE30                          = 0x84DE,
	GL_TEXTURE31                          = 0x84DF,

	// texture filtering
	GL_TEXTURE_MAG_FILTER                 = 0x2800,
	GL_TEXTURE_MIN_FILTER                 = 0x2801,
	
	GL_NEAREST                            = 0x2600,
	GL_LINEAR                             = 0x2601,
	//GL_NEAREST_MIPMAP_NEAREST             = 0x2700,
	//GL_LINEAR_MIPMAP_NEAREST              = 0x2701,
	//GL_NEAREST_MIPMAP_LINEAR              = 0x2702,
	GL_LINEAR_MIPMAP_LINEAR               = 0x2703,
								       
	// texture address mode
	GL_TEXTURE_WRAP_S                     = 0x2802,
	GL_TEXTURE_WRAP_T                     = 0x2803,
	GL_TEXTURE_WRAP_R                     = 0x8072,
	
	GL_REPEAT                             = 0x2901,
	GL_CLAMP_TO_EDGE                      = 0x812F,
	//GL_CLAMP_TO_BORDER                    = 0x812D,
	GL_MIRRORED_REPEAT                    = 0x8370,
	//GL_MIRROR_CLAMP_TO_EDGE_EXT           = 0x8743,
	//GL_MIRROR_CLAMP_TO_BORDER_EXT         = 0x8912,
	
	// texture swizzle
	GL_TEXTURE_SWIZZLE_RGBA               = 0x8E46, // >= OpenGL 3.2

	// buffer
	GL_ARRAY_BUFFER                       = 0x8892,
	GL_ELEMENT_ARRAY_BUFFER               = 0x8893,

	//GL_READ_ONLY                          = 0x88B8,
	GL_WRITE_ONLY                         = 0x88B9,
	//GL_READ_WRITE                         = 0x88BA,
	//GL_BUFFER_ACCESS                      = 0x88BB,
	//GL_BUFFER_MAPPED                      = 0x88BC,
	//GL_BUFFER_MAP_POINTER                 = 0x88BD,
	//GL_STREAM_DRAW                        = 0x88E0,
	//GL_STREAM_READ                        = 0x88E1,
	//GL_STREAM_COPY                        = 0x88E2,
	GL_STATIC_DRAW                        = 0x88E4,
	//GL_STATIC_READ                        = 0x88E5,
	//GL_STATIC_COPY                        = 0x88E6,
	GL_DYNAMIC_DRAW                       = 0x88E8,
	//GL_DYNAMIC_READ                       = 0x88E9,
	//GL_DYNAMIC_COPY                       = 0x88EA,

	// shader
	GL_FRAGMENT_SHADER                    = 0x8B30,
	GL_VERTEX_SHADER                      = 0x8B31,

	// draw
	GL_POINTS                             = 0x0000,
	GL_LINES                              = 0x0001,
	//GL_LINE_LOOP                          = 0x0002,
	//GL_LINE_STRIP                         = 0x0003,
	GL_TRIANGLES                          = 0x0004,
	//GL_TRIANGLE_STRIP                     = 0x0005,
	//GL_TRIANGLE_FAN                       = 0x0006,
	//GL_QUADS                              = 0x0007,
	//GL_QUAD_STRIP                         = 0x0008,
	//GL_POLYGON                            = 0x0009,
};


extern "C" {

// state
void glEnable(GLenum cap);
void glDisable(GLenum cap);
	
// cull state
void glCullFace(GLenum mode);
	
// blend state
void glBlendFunc(GLenum sfactor, GLenum dfactor);
void glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
void glBlendEquation(GLenum mode);
void glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha);
void glBlendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	
// get state
void glGetFloatv(GLenum pname, GLfloat *params);

// texture
void glGenTextures(GLsizei n, GLuint *textures);
void glDeleteTextures(GLsizei n, const GLuint *textures);
void glBindTexture(GLenum target, GLuint texture);
void glTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
void glTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid* pixels);
void glTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid* pixels);
void glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
void glGenerateMipmap(GLenum target);
void glActiveTexture(GLenum texture);

// texture state
void glTexParameteri(GLenum target, GLenum pname, GLint param);
void glTexParameteriv(GLenum target, GLenum pname, const GLint *params);
void glTexParameterfv(GLenum target, GLenum pname, const GLfloat *params);

// buffer
void glGenBuffers(GLsizei n, GLuint* buffers);
void glDeleteBuffers(GLsizei n, const GLuint* buffers);
void glBindBuffer(GLenum target, GLuint buffer);
void glBufferData(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid* data);
GLvoid* glMapBuffer(GLenum target, GLenum access);
GLboolean glUnmapBuffer(GLenum target);

// vertex array
void glGenVertexArrays(GLsizei n, GLuint *arrays);
void glDeleteVertexArrays(GLsizei n, const GLuint *arrays);
void glBindVertexArray(GLuint array);

// shader
GLuint glCreateShader(GLenum type);
void glDeleteShader(GLuint shader);
void glShaderSource(GLuint shader, GLsizei count, const char** string, const GLint* length);
void glCompileShader(GLuint shader);
GLuint glCreateProgram(void);
void glDeleteProgram(GLuint program);
void glAttachShader(GLuint program, GLuint shader);
void glBindAttribLocation(GLuint program, GLuint index, const char* name);
void glLinkProgram(GLuint program);
void glUseProgram(GLuint program);

// shader uniform binding
GLint glGetUniformLocation(GLuint program, const char* name);
void glUniform1i(GLint location, GLint x);
//void glUniform2i(GLint location, GLint x, GLint y);
//void glUniform3i(GLint location, GLint x, GLint y, GLint z);
//void glUniform4i(GLint location, GLint x, GLint y, GLint z, GLint w);
//void glUniform1f(GLint location, GLfloat x);
//void glUniform2f(GLint location, GLfloat x, GLfloat y);
//void glUniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z);
//void glUniform4f(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
//void glUniform1iv(GLint location, GLsizei count, const GLint* v);
//void glUniform2iv(GLint location, GLsizei count, const GLint* v);
//void glUniform3iv(GLint location, GLsizei count, const GLint* v);
void glUniform4iv(GLint location, GLsizei count, const GLint* v);
//void glUniform1fv(GLint location, GLsizei count, const GLfloat* v);
//void glUniform2fv(GLint location, GLsizei count, const GLfloat* v);
//void glUniform3fv(GLint location, GLsizei count, const GLfloat* v);
void glUniform4fv(GLint location, GLsizei count, const GLfloat* v);
//void glUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
//void glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
//void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

// shader vertex buffer binding
void glEnableVertexAttribArray(GLuint index);
void glDisableVertexAttribArray(GLuint index);
void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);

// draw
void glDrawArrays(GLenum mode, GLint first, GLsizei count);
void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);

}

#endif
