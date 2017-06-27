#include <gtest/gtest.h>

#include <digi/System/Timer.h>
#include <digi/System/ConsoleLogChannel.h>
#include <digi/Display/Display.h>
#include <digi/OpenGL/GLWrapper.h>

#include "InitLibraries.h"

using namespace digi;


TEST(Display, InitLibraries)
{
	initLibraries();
	std::string versionInfo = VersionInfo::get();
	Log::addChannel(new ConsoleLogChannel());
}

Pointer<Display> display;
int2 size;

void resize()
{
	size = display->getSize();
	dNotify("display size " << size);
}

void draw()
{
	// render triangle
	glViewport(0, 0, size.x, size.y);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glDrawArrays(GL_TRIANGLES, 0, 3);

	display->update();
}

TEST(Display, Display)
{
	display = Display::open("TestDisplay", 1024, 768, 0);//Display::FULLSCREEN);
	display->onresize.connect(resize);
	display->onpaint.connect(draw);

	// call resize initially
	resize();

	int esc = display->getHandle("key.escape");
	int fKey = display->getHandle("key.f");
	int eKey = display->getHandle("key.e");

	int mouseButtonHandle = display->getHandle("mouse.left");
	int mousePosHandle = display->getHandle("mouse.position");
	int mouseWheelHandle = display->getHandle("mouse.wheel");
	int keyHandle = display->getHandle("key.rightShift");


	// shader
#if GL_MAJOR == 2
	GLuint vs = createVertexShader(
		"attribute vec4 position;"
		"uniform vec2 pos;"
		"void main(void){"
			"gl_Position = position + vec4(pos, 0, 0);"
		"}", "vs");
	GLuint ps = createPixelShader(
		"uniform vec3 col;"
		"void main (void){"
			"gl_FragColor = vec4(col, 1.0);"
		"}", "ps");
#else
	GLuint vs = createVertexShader("#version 150\n"
		"in vec4 position;"
		"uniform vec2 pos;"
		"void main(void){"
			"gl_Position = position + vec4(pos, 0, 0);"
		"}", "vs");
	GLuint ps = createPixelShader("#version 150\n"
		"uniform vec3 col;"
		"out vec4 color;"
		"void main (void){"
			"color = vec4(col, 1.0);"
		"}", "ps");
#endif
	GLuint program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, ps);
	glBindAttribLocation(program, 0, "position");
	glLinkProgram(program);
	glDeleteShader(vs);
	glDeleteShader(ps);

	// check link status
	GLint linkStatus;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (!linkStatus)
		dNotify("program failed to link");

	GLuint positionLocation = glGetUniformLocation(program, "pos");
	GLuint colorLocation = glGetUniformLocation(program, "col");
	glUseProgram(program);

	// vertex buffer
	const float vertices[] =
	{
		-0.4f,-0.4f,0.0f,
		0.4f ,-0.4f,0.0f,
		0.0f ,0.4f ,0.0f
	};
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// vertex array
#if GL_MAJOR >= 3
	GLuint array;
	glGenVertexArrays(1, &array);
	glBindVertexArray(array);
#endif
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// set background color
	glClearColor(0.12f, 0.02f, 0.02f, 1.0f);


	// event loop
	int event;
	int state = Display::ACTIVE;
	int startTime = Timer::getMilliSeconds();
	int time;
	bool currentCursorState = true;
	while ((time = Timer::getMilliSeconds() - startTime) < 3000)
	{
		float2 mousePos = display->getFloat2(mousePosHandle);
		glUniform2f(positionLocation, mousePos.x, mousePos.y);
	
		if (display->getBool(mouseButtonHandle))
			glUniform3f(colorLocation, 0.8f, 1.0f, 0.3f);
		else
			glUniform3f(colorLocation, 0.2f, 0.2f, 1.0f);
		
		// draw triangle
		draw();
		
		// get event
		event = display->getEvent();
		
		// reset startTime on event
		if (event != Display::NO_EVENT)
			startTime = Timer::getMilliSeconds();
		
		// show/hide cursor
		bool cursorState = time < 1000;
		if (cursorState != currentCursorState)
		{
			currentCursorState = cursorState;
			display->showCursor(cursorState);
		}
		
		if (event == Display::EVENT_CLOSE || event == esc)
			break;
		if (event == fKey && display->getBool(fKey))
			display->toggleFullscreen();
		if (event == eKey && display->getBool(eKey))
			display->showError("test error");
		
		if (event == mouseButtonHandle)
			std::cout << "mouseButton " << display->getBool(mouseButtonHandle) << std::endl;
		if (event == mousePosHandle)
			std::cout << "mousePos " << display->getFloat2(mousePosHandle) << std::endl;
		if (event == mouseWheelHandle)
			std::cout << "mouseWheel " << display->getInt2(mouseWheelHandle) << std::endl;
		if (event == keyHandle)
			std::cout << "key " << display->getBool(keyHandle) << std::endl;

		{
			char buffer[17];
			int numRead = display->readTextInput(buffer, 16);
			if (numRead > 0)
			{
				buffer[numRead] = 0;
				std::cout << "text " << buffer << std::endl;
			}
		}
		
		// print display state
		int newState = display->getState();
		if (newState != state)
		{
			state = newState;
			std::cout << "state ";
			if (state & Display::ACTIVE)
				std::cout << "active ";
			if (state & Display::FULLSCREEN)
				std::cout << "fullscreen ";
			if (state & Display::HIDDEN)
				std::cout << "hidden ";
			std::cout << std::endl;
		}
	};
		
	// exit
	display->close();
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
