#include "glTypes.h"

extern "C" { 

GLuint createVertexShader(const char* source, const char* name);
GLuint createPixelShader(const char* source, const char* name);

typedef GLuint Texture;
typedef GLuint Texture2D;
typedef GLuint Texture3D;
typedef GLuint TextureCube;

}
