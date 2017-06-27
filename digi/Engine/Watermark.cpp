#include <digi/Math/All.h>

#include "Watermark.h"


namespace digi {

typedef uint16_t ushort;
#include "watermark.inc.h"

/*
static const float2 vertices[] =
{
	{0.0f, 0.0f},
	{1.0f, 0.0f},
	{1.0f, 1.0f},
	{0.0f, 1.0f},
};
*/
static const float2 vertices[] =
{
	{0.0f, 0.0f},
	{1.0f, 0.0f},
	{1.0f, 1.0f},

	{1.0f, 1.0f},
	{0.0f, 1.0f},
	{0.0f, 0.0f},
};

Watermark::Watermark()
{
	// texture
	glGenTextures(1, &this->texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA4, 64, 64, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, watermark);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	// vertex buffer
	glGenBuffers(1, &this->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// shader
	GLuint vertexShader = createVertexShader(
		"attribute vec2 pos;"
		"varying vec2 uv;"
		"void main()"
		"{"
		"  gl_Position = vec4(pos * 0.2 + vec2(0.8, -1), 0, 1);"
		"  uv = pos * vec2(1, -1) + vec2(0, 1);"
		"}",
		"watermark"
	);
	
	GLuint pixelShader = createPixelShader(
		"uniform sampler2D tex;"
		"varying vec2 uv;"
		"void main()"
		"{"
		"  gl_FragColor = texture2D(tex, uv);"
		"}",
		"watermark"
	);
	
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, pixelShader);
	glBindAttribLocation(program, 0, "pos");
	glLinkProgram(program);
	glDeleteShader(vertexShader);
	glDeleteShader(pixelShader);
	this->program = program;

}

Watermark::~Watermark()
{
	// texture
	glDeleteTextures(1, &this->texture);
	
	// vertex buffer
	glDeleteBuffers(1, &this->vertexBuffer);
	
	// shader
	glDeleteProgram(this->program);
}

void Watermark::render()
{
	// set blend mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(this->program);
	glEnableVertexAttribArray(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture);
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, sizeof(float2), (GLvoid*)0);
	
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableVertexAttribArray(0);
	glUseProgram(0);

	// reset blend mode
	glBlendFunc(GL_ONE, GL_ZERO);
	glDisable(GL_BLEND);
}

} // namespace digi
