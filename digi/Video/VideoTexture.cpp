#include <boost/format.hpp>

#include <digi/Utility/Convert.h>

#include "VideoTexture.h"


namespace digi {

namespace
{
	GLuint createTexture()
	{
		GLuint tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		return tex;
	}
} // anonymous namespace

VideoTexture::VideoTexture(VideoFormat format)
	: format(format)
{
	// luminance and chrominance textures
	for (int i = 0; i < 3; ++i)
		this->planeInfos[i].texture = createTexture();

	// target texture
	this->texture = createTexture();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, format.width, format.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	// frame buffer object
	glGenFramebuffers(1, &this->fbo);	
	glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->texture, 0);
	uint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		dError("frame buffer not complete");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// quad
	glGenBuffers(1, &this->quadBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->quadBuffer);
	float quad[] = {0.0, 0.0,  1.0, 0.0,  0.0, 1.0,  1.0, 0.0,  0.0, 1.0,  1.0, 1.0};
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
#if GL_MAJOR == 2
	GLuint vertexShader = createVertexShader(
		"attribute vec2 p;"
		"varying vec2 uv;"
		"void main(){"
			"gl_Position = vec4(p * 2.0 - 1.0, 0.0, 1.0);"
			"uv = p;"
		"}", "quad");
#else
	glGenVertexArrays(1, &this->quadArray);
	glBindVertexArray(this->quadArray);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, this->quadBuffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
	GLuint vertexShader = createVertexShader("#version 150\n"
		"in vec2 p;"
		"out vec2 uv;"
		"void main(){"
			"gl_Position = vec4(p * 2.0 - 1.0, 0.0, 1.0);"
			"uv = p;"
		"}", "quad");
#endif

	// yuv compositor
#if GL_MAJOR == 2
	GLuint pixelShader = createPixelShader(
		"uniform sampler2D t0;"
		"uniform sampler2D t1;"
		"uniform sampler2D t2;"
		"uniform vec2 s0;"
		"uniform vec2 s1;"
		"uniform vec2 s2;"
		"varying vec2 uv;"
		"void main (void){"
			"float y = texture2D(t0, uv * s0).x;"
			"float u = texture2D(t1, uv * s1).x - 0.5;"
			"float v = texture2D(t2, uv * s2).x - 0.5;"
			"gl_FragColor = vec4(y + u * vec3(0, -0.39062, 2.0156) + v * vec3(1.5977, -0.8125, 0), 1);"
		"}", "yuv2rgb");
#else
	GLuint pixelShader = createPixelShader("#version 150\n"
		"uniform sampler2D t0;"
		"uniform sampler2D t1;"
		"uniform sampler2D t2;"
		"uniform vec2 s0;"
		"uniform vec2 s1;"
		"uniform vec2 s2;"
		"in vec2 uv;"
		"out vec4 color;"
		"void main (void){"
			"float y = texture(t0, uv * s0).x;"
			"float u = texture(t1, uv * s1).x - 0.5;"
			"float v = texture(t2, uv * s2).x - 0.5;"
			"color = vec4(y + u * vec3(0, -0.39062, 2.0156) + v * vec3(1.5977, -0.8125, 0), 1);"
		"}", "yuv2rgb");
#endif
	GLuint program = this->yuvCompositor = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, pixelShader);
	glBindAttribLocation(program, 0, "p");
	glLinkProgram(program);
	glDeleteShader(vertexShader);
	glDeleteShader(pixelShader);
	glUseProgram(program);
	boost::format t("t%1%");
	boost::format s("s%1%");
	for (int i = 0; i < 3; ++i)
	{			
		glUniform1i(glGetUniformLocation(program, str(t % i).c_str()), i);
		this->planeInfos[i].scale = glGetUniformLocation(program, str(s % i).c_str());
	}
}

VideoTexture::~VideoTexture()
{
	this->close();
}

VideoFormat VideoTexture::getFormat()
{
	return this->format;
}

void VideoTexture::close()
{
	if (this->yuvCompositor == 0)
		return;
		
	for (int i = 0; i < 3; ++i)
		glDeleteTextures(1, &this->planeInfos[i].texture);
	glDeleteTextures(1, &this->texture);
	glDeleteBuffers(1, &this->quadBuffer);
	glDeleteProgram(this->yuvCompositor);

	this->yuvCompositor = 0;
}

void VideoTexture::write(const Plane* planes)
{
	int width = this->format.width;
	int height = this->format.height;
	
	// update planes (e.g. Y, U and V)
	for (int i = 0; i < 3; ++i)
	{
	#if GL_MAJOR == 2
		const int format = GL_LUMINANCE;
	#else
		const int format = GL_RED;
	#endif
		const Plane& p = planes[i];
		PlaneInfo& pi = this->planeInfos[i];
		glBindTexture(GL_TEXTURE_2D, pi.texture);
		if (p.width != pi.width || p.height != pi.height)
		{
			// size changed: create new texture for this plane and copy plane into it
			pi.width = p.width;
			pi.height = p.height;
			glTexImage2D(GL_TEXTURE_2D, 0, format, p.width, p.height, 0, format, GL_UNSIGNED_BYTE, p.data);
		}
		else
		{
			// size stayed the same: just copy plane into texture
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, p.width, p.height, format, GL_UNSIGNED_BYTE, p.data);
		}
	}

	// update target texture
	/*if (width != this->width || height != this->height)
	{
		this->width = width;
		this->height = height;
		glBindTexture(GL_TEXTURE_2D, this->texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	}*/
	
	// save viewport
	//GLint viewport[4];
	//glGetIntegerv(GL_VIEWPORT, viewport);
	
	// set viewport
	glViewport(0, 0, width, height);

	// render to texture
	glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);

	glUseProgram(this->yuvCompositor);
	for (int i = 0; i < 3; ++i)
	{
		const Plane& p = planes[i];
		PlaneInfo& pi = this->planeInfos[i];
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, pi.texture);
		
		// calc displayed size of plane dependent on video format (plane may be larger, currently only YV12)
		float sizeX = float(width);
		float sizeY = float(height);
		if (i >= 1)
		{
			// U and V plane are half size
			sizeX *= 0.5f;
			sizeY *= 0.5f;
		}
		
		// set scale of plane that removes any padding on the right and bottom
		glUniform2f(pi.scale, sizeX / float(p.width), sizeY / float(p.height));
	}

	// set vertex array
#if GL_MAJOR == 2
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, this->quadBuffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
#else
	glBindVertexArray(this->quadArray);
#endif

	// draw quad to execute yuv2rgb shader
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// reset
#if GL_MAJOR == 2
	glDisableVertexAttribArray(0);	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
#else
	glBindVertexArray(0);
#endif
	for (int i = 2; i >= 0; --i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	// restore viewport
	//glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

} // namespace digi
