#include <boost/range.hpp>

#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/Find.h>
#include <digi/Utility/StringUtility.h>
#include <digi/Utility/UTFTranscode.h>
#include <digi/Utility/VectorUtility.h>
#include <digi/Utility/foreach.h>
#include <digi/System/Log.h>
#include <digi/OPenGL/GLWrapper.h>

#include "../Version.h"
#include "../InputDevice.h"
#include "../SimpleQueue.h"
#include "../Display.h"

#ifdef GL_ES
	#include <EGL/egl.h>
#else
	#include "wglext.h"
#endif

#include <Windows.h>
#include <Windowsx.h>
#include <Mmsystem.h>
#undef NO_ERROR
#undef ERROR

#define LSTR_(s) L#s
#define LSTR(s) LSTR_(s)

#ifdef _WIN32_WCE
	#define WS_OVERLAPPEDWINDOW 0
	#define WS_EX_APPWINDOW 0
	#define SetWindowLongPtr SetWindowLongW
	#define GetWindowLongPtr GetWindowLongW
#endif


namespace digi {
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
}

void digiDisplayInit()
{
	digi::VersionInfo::add(versionInfo);

	// create window class	
	HINSTANCE hInstance = GetModuleHandle(NULL);
	
	WNDCLASSW wc;
	wc.style = CS_HREDRAW | CS_VREDRAW; //! | CS_OWNDC; // redraw on resize, own DC for window
	wc.lpfnWndProc = (WNDPROC)digi::WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(void*); // can store one pointer using SetWindowLongPtr
	wc.hInstance = hInstance;
	wc.hIcon = LoadIconW(hInstance, L"App");
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"digi";
	RegisterClassW(&wc);
}

void digiDisplayDone()
{
}


namespace digi {
	
Display::Mode Display::getMode()
{
	HWND hWnd = GetDesktopWindow();
	HDC hDC = GetDC(hWnd);
	Mode mode(
		GetDeviceCaps(hDC, HORZRES), 
		GetDeviceCaps(hDC, VERTRES),
		GetDeviceCaps(hDC, BITSPIXEL));
	ReleaseDC(hWnd, hDC);
	return mode;
}

/*
void Display::getSupportedModes(std::vector<Mode>& modes, Mode minMode)
{
#ifndef _WIN32_WCE
	DEVMODEW devMode;
	memset(&devMode, 0, sizeof(devMode));
	devMode.dmSize = sizeof(devMode);
	DWORD index = 0;
	while (EnumDisplaySettingsW(NULL, index, (LPDEVMODEW)&devMode) != 0)
	{
		Mode mode(devMode.dmPelsWidth, devMode.dmPelsHeight, devMode.dmBitsPerPel);
		if (mode.width >= minMode.width && mode.height >= minMode.height && mode.bitsPerPixel >= minMode.bitsPerPixel
			&& !contains(modes, mode))
			modes += mode;
		++index;
	}
#endif
}

bool Display::setMode(const Mode& mode)
{
#ifndef _WIN32_WCE
	DEVMODE devMode;
	memset(&devMode, 0, sizeof(devMode));
	devMode.dmSize = sizeof(devMode);
	devMode.dmPelsWidth = mode.width;
	devMode.dmPelsHeight = mode.height;
	devMode.dmBitsPerPel = mode.bitsPerPixel;
	devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

	// try to set screen settings. note: CDS_FULLSCREEN gets rid of task bar
	return ChangeDisplaySettings(&devMode, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL;
#else
    return false;
#endif
}

void Display::resetMode()
{
#ifndef _WIN32_WCE
	ChangeDisplaySettings(NULL, 0);
#endif
}
*/

namespace
{
	// keyboard
	extern const InputDevice::AttributeInfo keyAttributeInfos[];
	struct KeyState
	{
		#include "numKeyAttributes.inc.h"
		
		// the state of each key exposed as attribute
		bool keys[NUM_ATTRIBUTES];

		// flags that indicate if a key state has changed
		uint32_t changed[(NUM_ATTRIBUTES + 31) / 32];

		void setKey(int attributeIndex, bool state, bool repeat)
		{
			//if (!repeat || this->keys[attributeIndex] != state)
			{
				// debug: output attribute name
			//dNotify("setKey " << keyAttributeInfos[attributeIndex].name << " state " << state << " repeat " << repeat);

				// set key state
				this->keys[attributeIndex] = state;


				// set changed flag
				this->changed[attributeIndex >> 5] |= 1 << (attributeIndex & 31);
			}
		}
		
		void setModifier(int attributeIndex, bool state)
		{
			if (this->keys[attributeIndex] != state)
				this->setKey(attributeIndex, state, true);
		}
	};
	const InputDevice::AttributeInfo keyAttributeInfos[] =
	{
		#include "keyAttributeInfos.inc.h"
	};
	const int key2AttributeIndex[] =
	{
		#include "key2AttributeIndex.inc.h"
	};

	
	// mouse
	struct MouseState
	{
		enum Flags
		{
			// same order as mouseAttributeInfos
			LEFT_FLAG = 1,
			MIDDLE_FLAG = 2,
			POSITION_FLAG = 4,
			RIGHT_FLAG = 8,
			WHEEL_FLAG = 16
		};
		
		// attributes
		bool left;
		bool right;
		bool middle;
		float2 position;
		int2 wheel;
		
		// flags that indicate if a mouse attribute has changed
		int changed;
	};
	const InputDevice::AttributeInfo mouseAttributeInfos[] =
	{
		// must be sorted alphabetically
		{".left",     InputDevice::T_BOOL, offsetof(MouseState, left)},
		{".middle",   InputDevice::T_BOOL, offsetof(MouseState, middle)},
		{".position", InputDevice::T_FLOAT2, offsetof(MouseState, position)},
		{".right",    InputDevice::T_BOOL, offsetof(MouseState, right)},
		{".wheel",    InputDevice::T_INT2, offsetof(MouseState, wheel)}
	};
} // anonymous namespace


// opengl helpers
#ifdef GL_ES
	static void destroy(HWND hWnd, HDC hDC, EGLDisplay display, EGLSurface surface, EGLContext context)
	{
		ShowCursor(TRUE);
		
		if (display != EGL_NO_DISPLAY)
		{
			eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

			// eglTerminate takes care of destroying any context or surface created with this
			// display, so we don't need to call eglDestroySurface or eglDestroyContext
			eglTerminate(display);
		}

		// release GDI context
		ReleaseDC(hWnd, hDC);

		// delete window
		DestroyWindow(hWnd);
	}
#else
	static int chooseMultisampleFormat(HDC hDC)
	{
		PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");	
		if (wglChoosePixelFormatARB == NULL)
			return 0;
			
		int iAttributes[] =
		{
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_STENCIL_BITS_ARB, 8,
			WGL_SAMPLE_BUFFERS_ARB, 1, //Number of buffers (must be 1 at time of writing)
			WGL_SAMPLES_ARB, 4,        //Number of samples
			0
		};
		float fAttributes[] =
		{
			0, 0
		};
		int pixelFormat;
		UINT numFormats;

		// try to get format with 4 samples
		int result = wglChoosePixelFormatARB(hDC, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);
		if (result && numFormats > 0)
		{
			return pixelFormat;	
		}

		// try to get format with 2 samples
		iAttributes[17] = 2;
		result = wglChoosePixelFormatARB(hDC, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);
		if (result && numFormats > 0)
		{
			return pixelFormat;
		}
		return 0;
	}

	static void destroy(HWND hWnd, HDC hDC, HGLRC context)
	{
		ShowCursor(TRUE);

		if (context != NULL)
		{
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(context);
		}

		// release GDI context
		ReleaseDC(hWnd, hDC);

		// delete window
		DestroyWindow(hWnd);
	}
#endif

const DWORD windowedStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE;
const DWORD windowedExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; // WS_EX_WINDOWEDGE = raised edge
const DWORD fullscreenStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE;
const DWORD fullscreenExStyle = WS_EX_APPWINDOW;

class Win32Display : public Display
{
public:

#ifdef GL_ES
	Win32Display(HWND hWnd, HDC hDC, EGLDisplay display, EGLSurface surface, EGLContext context, int state)
		: hWnd(hWnd), hDC(hDC), display(display), surface(surface), context(context), state(state & FULLSCREEN), cursor(true)
#else
	Win32Display(HWND hWnd, HDC hDC, HGLRC context, int state)
		: hWnd(hWnd), hDC(hDC), context(context), state(state & FULLSCREEN), cursor(true)
#endif
	{
		// store this pointer into window
		SetWindowLongPtr(hWnd, 0, (LONG_PTR)this);	

		// clear key/mouse state
		memset(&this->keyState, 0, sizeof(KeyState));
		memset(&this->mouseState, 0, sizeof(MouseState));
	}

	virtual ~Win32Display()
	{
		this->close();
	}
	
	virtual void close()
	{
		destroy(this->hWnd, this->hDC,
	#ifdef GL_ES
		this->display, this->surface,
	#endif
		this->context);

		this->hWnd = NULL;
		this->hDC = NULL;
	#ifdef GL_ES
		this->display = EGL_NO_DISPLAY;
		this->surface = EGL_NO_SURFACE;
		this->context = EGL_NO_CONTEXT;
	#else
		this->context = NULL;
	#endif
	}

	virtual void update()
	{
	#ifdef GL_ES
		eglSwapBuffers(this->display, this->surface);
	#else
		SwapBuffers(this->hDC);
	#endif
	}

	virtual int getState()
	{
		return this->state;
	}
	
	virtual int2 getSize()
	{
	#ifdef GL_ES
		EGLint width;
		EGLint height;
		eglQuerySurface(this->display, this->surface, EGL_WIDTH, &width);
		eglQuerySurface(this->display, this->surface, EGL_HEIGHT, &height);
		return vector2(int(width), int(height));
	#else
		RECT rect;
		GetClientRect(this->hWnd, &rect);
		return vector2(int(rect.right), int(rect.bottom));
	#endif
	}

	void toggleFullscreen()
	{
	#ifdef GL_ES
	#else
		if ((this->state & FULLSCREEN) == 0)
		{
			// to fullscreen

			// save current window rect
			GetWindowRect(this->hWnd, &this->windowRect);

			// get size of screen
			HWND hWnd = GetDesktopWindow();
			HDC hDC = GetDC(hWnd);
			int width = GetDeviceCaps(hDC, HORZRES);
			int height = GetDeviceCaps(hDC, VERTRES);
			ReleaseDC(hWnd, hDC);

			// update window
			SetWindowLongPtr(this->hWnd, GWL_STYLE, fullscreenStyle);
			SetWindowLongPtr(this->hWnd, GWL_EXSTYLE, fullscreenExStyle);
			MoveWindow(this->hWnd, 0, 0, width, height, TRUE);	
		}
		else
		{
			// to windowed
			int x = this->windowRect.left;
			int y = this->windowRect.top;
			int w = this->windowRect.right - x;
			int h = this->windowRect.bottom - y;
			SetWindowLongPtrW(this->hWnd, GWL_STYLE, windowedStyle);
			SetWindowLongPtrW(this->hWnd, GWL_EXSTYLE, windowedExStyle);
			MoveWindow(this->hWnd, x, y, w, h, true);
		}
		this->state ^= FULLSCREEN;
	#endif
	}
	
	void showCursor(bool show)
	{
		if (show != this->cursor)
		{
			this->cursor = show;

			//! force WM_SETCURSOR message
			SetCursor(NULL);
		}
	}

	virtual int readTextInput(char* data, int length)
	{
		int numRead = 0;
		while (length > 0 && !this->textInput.isEmpty())
		{
			*data = this->textInput.get(0);
			++data;
			--length;
			++numRead;
		}
		return numRead;		
	}

	virtual int getEvent()
	{		
		if (this->events.isEmpty())
		{
			// handle window system events
			this->stopEvents = false;
			MSG msg;
			while (!this->stopEvents && PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					return EVENT_CLOSE;

				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}

			// handle events from input devices
			foreach (InputsPair& input, this->inputs)
			{
				input.second.device->update(EventQueue(input.second.handles, this->events));
			}
		}
		
		// get an event (use NO_EVENT if queue is empty)
		return this->events.get(NO_EVENT);		
	}

	void showError(StringRef message)
	{
		// get title from window
		wchar_t title[128];
		GetWindowTextW(this->hWnd, title, 128); 

		// convert message to utf16
		std::wstring wmessage = utfString<wchar_t>(message);
		
		// show message box
		MessageBoxW(NULL, wmessage.c_str(), title, MB_OK | MB_ICONEXCLAMATION);
	}

/*
	void* getWindowHandle()
	{
		return Display::hWnd;
	}
*/
	
	// window handle
	HWND hWnd;
	
	// GDI context
	HDC hDC;

#ifdef GL_ES
	// display handle
	EGLDisplay display;
	
	// surface handle
	EGLSurface surface;

	// render context
	EGLContext context; 
#else
	// OpenGL render context
	HGLRC context;
#endif
	
	// window state
	int state;

	// cursor
	bool cursor;

	// text input queue
	SimpleQueue<char, 128> textInput;

	// event queue
	SimpleQueue<int, 32> events;

	// this flag is set to stop event processing in getEvent so that key or button presses are not missed
	bool stopEvents;

	// keyboard
	KeyState keyState;
	
	// mouse
	MouseState mouseState;

	// rect for windowed mode
	RECT windowRect;
};

class KeyInputDevice : public InputDevice
{
public:
	KeyInputDevice(KeyState& keyState)
		: keyState(keyState) {}

	virtual ~KeyInputDevice()
	{
	}

	virtual ArrayRef<const AttributeInfo> getAttributeInfos()
	{
		return keyAttributeInfos;
	}
	
	virtual void* getState()
	{
		return &this->keyState;
	}

	virtual void update(EventQueue eventQueue)
	{
		// iterate over flags that indicate a change
		int base = 0;
		foreach (uint32_t& changed, this->keyState.changed)
		{
			int attributeIndex = base;
			for (uint32_t f = changed; f != 0; f >>= 1)
			{
				// add to event queue if changed
				if (f & 1)
					eventQueue.add(attributeIndex);				
				++attributeIndex;
			}
			
			// clear changed indicator flags
			changed = 0;
			
			// increase base for attributeIndex
			base += 32;
		}
	}

	
	KeyState& keyState;
};

class MouseInputDevice : public InputDevice
{
public:
	MouseInputDevice(MouseState& mouseState)
		: mouseState(mouseState) {}

	virtual ~MouseInputDevice()
	{
	}

	virtual ArrayRef<const AttributeInfo> getAttributeInfos()
	{
		return mouseAttributeInfos;
	}
	
	virtual void* getState()
	{
		return &this->mouseState;
	}

	virtual void update(EventQueue eventQueue)
	{
		int flags = this->mouseState.changed;
		if (flags != 0)
		{
			for (int attributeIndex = 0; attributeIndex < int(boost::size(mouseAttributeInfos)); ++attributeIndex)
			{
				if (flags & (1 << attributeIndex))
				{
					eventQueue.add(attributeIndex);
				}
			}			

			// clear flags
			this->mouseState.changed = 0;
		}
	}

	
	MouseState& mouseState;
};

Pointer<Display> Display::open(StringRef title, int width, int height, int state)
{	
	HDC hDC = NULL;
#ifdef GL_ES

	EGLDisplay display = EGL_NO_DISPLAY;
	EGLSurface surface = EGL_NO_SURFACE;
	EGLContext context = EGL_NO_CONTEXT;

#else

	HGLRC context = NULL;

#endif

	// determine window style and size
	DWORD dwStyle;
	DWORD dwExStyle;
	RECT windowRect;
	if ((state & FULLSCREEN) != 0)
	{
		// fullscreen

		// window style. WS_CLIPSIBLINGS and WS_CLIPCHILDREN required for openGL
		dwStyle = fullscreenStyle;
		dwExStyle = fullscreenExStyle;

		// set screen size as window size
		HWND hWnd = GetDesktopWindow();
		HDC hDC = GetDC(hWnd);
		windowRect.left = 0;
		windowRect.top = 0;
		windowRect.right = GetDeviceCaps(hDC, HORZRES);
		windowRect.bottom = GetDeviceCaps(hDC, VERTRES);
		ReleaseDC(hWnd, hDC);		
	}
	else
	{
		// windowed

		// window style. WS_CLIPSIBLINGS and WS_CLIPCHILDREN required for openGL
		dwStyle = windowedStyle;
		dwExStyle = windowedExStyle;

		// set window size
		windowRect.left = 0;
		windowRect.top = 0;
		windowRect.right = long(width);
		windowRect.bottom = long(height);
	}	

	// adjust window size to contain title bar and borders
	AdjustWindowRectEx(&windowRect, dwStyle, false, dwExStyle);
	int winWidth = windowRect.right - windowRect.left;
	int winHeight = windowRect.bottom - windowRect.top;
	
	// transcode title to utf16
	std::wstring wtitle = utfString<wchar_t>(title);
	const wchar_t* ctitle = wtitle.c_str();
	
	// create window
	HINSTANCE hInstance = GetModuleHandle(NULL);
	HWND hWnd = CreateWindowExW(
		dwExStyle,    // extended style for the window
		L"digi",      // class name
		ctitle,       // window title
		dwStyle,      // window style
		0, 0,         // window position
		winWidth,     // window width
		winHeight,    // window height
		NULL,         // no parent window
		NULL,         // no menu
		hInstance,    // instance
		NULL);        // dont pass anything to WM_CREATE
	if (hWnd == NULL)
		goto error;

	// get gdi context
	hDC = GetDC(hWnd);

#ifdef GL_ES
	display = eglGetDisplay(hDC);

	// initialize EGL
	EGLint majorVersion, minorVersion;
	if (!eglInitialize(display, &majorVersion, &minorVersion))
	{
		goto error;
	}		

	// make OpenGL ES the current API
	if (eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE)
	{
		goto error;			
	}

	// specify the required configuration attributes
	static const EGLint configAttribs[] =
	{
		EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
		#if GL_MAJOR == 2
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		#else
			#error unsopported OpenGL ES version
		#endif
		EGL_DEPTH_SIZE,      16,
		EGL_NONE
	};

	// find a config that matches all requirements.
	EGLConfig config;
	EGLint numConfigs;
	if (!eglChooseConfig(display, configAttribs, &config, 1, &numConfigs) || (numConfigs == 0))
	{
		//printf("Error: eglChooseConfig() failed.\n");
		goto error;
	}

	// create a surface to draw to
	surface = eglCreateWindowSurface(display, config, hWnd, NULL);
	if (surface == EGL_NO_SURFACE)
	{
		goto error;			
	}

	// create a context
	static const EGLint contextAttribs[] =
	{
			EGL_CONTEXT_CLIENT_VERSION, 2,
			EGL_NONE
	};
	context = eglCreateContext(display, config, NULL, contextAttribs);
	if (context == EGL_NO_CONTEXT)
	{
		goto error;
	}

	// bind the context to the current thread
	eglMakeCurrent(display, surface, surface, context);

	// initialize pointers to opengl functions
	initGL();

	// create win32 display
	{
		Pointer<Win32Display> wd = new Win32Display(hWnd, hDC, display, surface, context, state);
		wd->inputs["key"].set(new KeyInputDevice(wd->keyState));
		wd->inputs["mouse"].set(new MouseInputDevice(wd->mouseState));

		//ShowWindow(hWnd, SW_SHOW);
		SetForegroundWindow(hWnd); // slightly higher priority
		SetFocus(hWnd);

		return wd;	
	}
error:
	destroy(hWnd, hDC, display, surface, context);
	MessageBoxW(NULL, L"Can't create an EGL window", L"Digi", MB_OK | MB_ICONEXCLAMATION);
	return null;
#else
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	// choose an OpenGL pixel format
	GLuint pixelFormat = ChoosePixelFormat(hDC, &pfd);
	if (pixelFormat == 0)
		goto error;

	// set pixel format
	SetPixelFormat(hDC, pixelFormat, &pfd);

	// create temporary OpenGL context
	HGLRC tempContext = wglCreateContext(hDC);
	if (tempContext == NULL)
		goto error;

	// set current
	wglMakeCurrent(hDC, tempContext);
	//const GLubyte* versionString = glGetString(GL_VERSION);

#if GL_MAJOR == 2
	// use old style OpenGL context
	context = tempContext;
#else
	// get address of wglCreateContextAttribsARB
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB =
		(PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	if (wglCreateContextAttribsARB == NULL)
		goto error;

	// choose multisample pixel format using wglChoosePixelFormatARB
	//pixelFormat = chooseMultisampleFormat(hDC);
	//if (pixelFormat != 0)
	//	SetPixelFormat(hDC, pixelFormat, &pfd);

	// version of gl context
	const int iAttribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, GL_MAJOR,
		WGL_CONTEXT_MINOR_VERSION_ARB, GL_MINOR,
		WGL_CONTEXT_FLAGS_ARB, 0,
	#ifdef GL_CORE
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
	#else
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
	#endif
		0, 0
	};

	// create new style OpenGL context
	context = wglCreateContextAttribsARB(hDC, 0, iAttribs);
	if (context == NULL)
		goto error;

	// delete temporary context
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(tempContext);
	wglMakeCurrent(hDC, context);
#endif

	// initialize pointers to opengl functions
	if (!initGL())
		goto error;

	// create win32 display
	{
		Pointer<Win32Display> wd = new Win32Display(hWnd, hDC, context, state);
		wd->inputs["key"].set(new KeyInputDevice(wd->keyState));
		wd->inputs["mouse"].set(new MouseInputDevice(wd->mouseState));

		// rect for windowed mode if we started fullscreen
		wd->windowRect.left = 0;
		wd->windowRect.right = long(width);
		wd->windowRect.top = 0;
		wd->windowRect.bottom = long(height);
		AdjustWindowRectEx(&wd->windowRect, windowedStyle, false, dwExStyle);
		if (wd->windowRect.left < 0)
		{
			wd->windowRect.right -= wd->windowRect.left;
			wd->windowRect.left = 0;
		}
		if (wd->windowRect.top < 0)
		{
			wd->windowRect.bottom -= wd->windowRect.top;
			wd->windowRect.top = 0;
		}

		//ShowWindow(hWnd, SW_SHOW);
		SetForegroundWindow(hWnd); // slightly higher priority
		SetFocus(hWnd);

		return wd;	
	}
error:
	destroy(hWnd, hDC, context);
	MessageBoxW(NULL, L"Can't create an OpenGL " LSTR(GL_MAJOR) L"." LSTR(GL_MINOR) L" window", ctitle, MB_OK | MB_ICONEXCLAMATION);
	return null;
#endif
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Win32Display* display = (Win32Display*)GetWindowLongPtr(hWnd, 0);
	if (display != NULL)
	{
		switch (message)
		{
		case WM_SYSCOMMAND:
			// system command: prevent screen saver and monitor power save
		#ifndef _WIN32_WCE
			switch (wParam)
			{
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
				return 0;
			}
		#endif
			break;

		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;

		case WM_ACTIVATE:
			{
				int state = display->state & ~(Display::ACTIVE | Display::HIDDEN);
				if (HIWORD(wParam))
				{
					// minimized
					state |= Display::HIDDEN;
				}
				else if (LOWORD(wParam) != WA_INACTIVE)
				{
					// active
					state |= Display::ACTIVE;
				}
				display->state = state;
			}
			return 0;

		case WM_SIZE:
			display->onresize();
			return 0;

		case WM_PAINT:
			display->onpaint();
			break;
	
		case WM_SETCURSOR:
			if (LOWORD(lParam) == HTCLIENT && !display->cursor)
			{
				SetCursor(NULL);
				return TRUE;
			}
			break;

	#ifndef _WIN32_WCE
		case WM_DISPLAYCHANGE:
			{
				// change of display resolution
				int width = LOWORD(lParam);
				int height = HIWORD(lParam);
								
				if (display->state & Display::FULLSCREEN)
				{
					// resize window to cover the screen again
					SetWindowPos(display->hWnd, NULL, 0, 0, width, height, 0);
				}
			}
			break;
	#endif
			
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
			// wParam is virtual key code
			if (uint(wParam) < 256)
			{
				int attributeIndex = key2AttributeIndex[wParam];
				if (attributeIndex != -1)
				{
					// set key state
					bool state = message == WM_KEYDOWN || message == WM_SYSKEYDOWN;
					bool repeat = state & display->keyState.keys[attributeIndex];
					display->keyState.setKey(attributeIndex, state, repeat);
					display->stopEvents |= state & !repeat;

					// modifier keys
					if (wParam == VK_SHIFT)
					{
						display->keyState.setModifier(KeyState::LEFT_SHIFT_INDEX, (GetKeyState(VK_LSHIFT) & 0x8000) != 0);
						display->keyState.setModifier(KeyState::RIGHT_SHIFT_INDEX, (GetKeyState(VK_RSHIFT) & 0x8000) != 0);
					}
					else if (wParam == VK_MENU) // alt
					{
						display->keyState.setModifier(KeyState::LEFT_ALT_INDEX, (GetKeyState(VK_LMENU) & 0x8000) != 0);
						display->keyState.setModifier(KeyState::RIGHT_ALT_INDEX, (GetKeyState(VK_RMENU) & 0x8000) != 0);
					}
					else if (wParam == VK_CONTROL)
					{
						display->keyState.setModifier(KeyState::LEFT_CONTROL_INDEX, (GetKeyState(VK_LCONTROL) & 0x8000) != 0);
						display->keyState.setModifier(KeyState::RIGHT_CONTROL_INDEX, (GetKeyState(VK_RCONTROL) & 0x8000) != 0);			
					}
					else if (wParam == VK_LWIN || wParam == VK_RWIN) // command
					{
						display->keyState.setModifier(KeyState::COMMAND_INDEX,
							((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000) != 0);
					}
				}
			}
			return 0;

		case WM_CHAR:
			{
				// utf16 -> utf8
				char buffer[4];
				UtfEncoder<1> encode;
				char* it = buffer;
				encode(it, wParam);
				
				display->textInput.add(buffer, it - buffer);
			}
			return 0;
	#ifndef _WIN32_WCE
		case WM_UNICHAR:
			// check for "no character" code
			if (wParam != 0xffff)
			{
				// utf32 -> utf8
				char buffer[8];
				UtfEncoder<1> encode;
				char* it = buffer;
				encode(it, wParam);
				
				display->textInput.add(buffer, it - buffer);
			}
			return 1; // 1 to indicate support of WM_UNICHAR
	#endif
		
		case WM_LBUTTONDOWN:
			display->mouseState.left = true;
			display->mouseState.changed |= MouseState::LEFT_FLAG;
			display->stopEvents = true;
			SetCapture(display->hWnd);
			return 0;
		case WM_LBUTTONUP:
			display->mouseState.left = false;
			display->mouseState.changed |= MouseState::LEFT_FLAG;
			SetCapture(NULL);
			return 0;
		case WM_RBUTTONDOWN:
			display->mouseState.right = true;
			display->mouseState.changed |= MouseState::RIGHT_FLAG;
			display->stopEvents = true;
			SetCapture(display->hWnd);
			return 0;
		case WM_RBUTTONUP:
			display->mouseState.right = false;
			display->mouseState.changed |= MouseState::RIGHT_FLAG;
			SetCapture(NULL);
			return 0;
		case WM_MBUTTONDOWN:
			display->mouseState.middle = true;
			display->mouseState.changed |= MouseState::MIDDLE_FLAG;
			display->stopEvents = true;
			SetCapture(display->hWnd);
			return 0;
		case WM_MBUTTONUP:
			display->mouseState.middle = false;
			display->mouseState.changed |= MouseState::MIDDLE_FLAG;
			SetCapture(NULL);
			return 0;
		case WM_MOUSEMOVE:
			{
				int2 p = vector2(int(GET_X_LPARAM(lParam)), int(GET_Y_LPARAM(lParam)));
				float2 position = vector2(float(p.x) + 0.5f, -float(p.y) - 0.5f)
					/ convert_float2(display->getSize()) * 2.0f + vector2(-1.0f, 1.0f); 
				if (any(position != display->mouseState.position) && !any(position < -1.0f) && !any(position > 1.0f))
				{
					display->mouseState.position = position;
					display->mouseState.changed |= MouseState::POSITION_FLAG;
				}
			}
			return 0;
	#ifndef _WIN32_WCE
		case WM_MOUSEWHEEL:
			// vertical mouse wheel
			display->mouseState.wheel.y += GET_WHEEL_DELTA_WPARAM(wParam);
			display->mouseState.changed |= MouseState::WHEEL_FLAG;
			return 0;
		case WM_MOUSEHWHEEL:
			// horizontal mouse wheel
			display->mouseState.wheel.x += GET_WHEEL_DELTA_WPARAM(wParam);
			display->mouseState.changed |= MouseState::WHEEL_FLAG;
			return 0;
	#endif
		}
	}
	
	// pass all unhandled messages to DefWindowProc
	return DefWindowProcW(hWnd, message, wParam, lParam);
}

} // namespace digi
