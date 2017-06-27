#include <digi/Utility/foreach.h>
#include <digi/System/Log.h>

#include "../Version.h"
#include "../Display.h"

#include <EGL/egl.h>


// http://www.khronos.org/opengles/documentation/opengles1_0/html/


void digiDisplayInit()
{
	digi::VersionInfo::add(versionInfo);
}

void digiDisplayDone()
{
}


namespace digi {

Display::Mode Display::getMode()
{
	//! http://de.wikipedia.org/wiki/Framebuffer
	return Mode();
}

static void destroy(EGLDisplay display, EGLSurface surface, EGLContext context)
{
	eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	
	// eglTerminate takes care of destroying any context or surface created with this
	// display, so we don't need to call eglDestroySurface or eglDestroyContext
	eglTerminate(display);
}

class LinuxDisplay : public Display
{
public:
	
	LinuxDisplay(EGLDisplay display, EGLSurface surface, EGLContext context)
		: display(display), surface(surface), context(context)
	{
	}
	virtual ~LinuxDisplay()
	{
		destroy(this->display, this->surface, this->context);
	}
	
	virtual void close()
	{
		destroy(this->display, this->surface, this->context);

		this->display = EGL_NO_DISPLAY;
		this->surface = EGL_NO_SURFACE;
		this->context = EGL_NO_CONTEXT;
	}

	virtual void update()
	{
		eglSwapBuffers(this->display, this->surface);
	}

	virtual State getState()
	{
		// always active and fullscreen
		return ACTIVE | FULLSCREEN;
	}
	
	virtual int2 getSize()
	{
		EGLint width;
		EGLint height;
		eglQuerySurface(this->display, this->surface, EGL_WIDTH, &width);
		eglQuerySurface(this->display, this->surface, EGL_HEIGHT, &height);
		return vector2(int(width), int(height));
	}

	virtual int readTextInput(char* data, int length)
	{
		return 0;
	}

	virtual int getEvent()
	{
		if (this->events.isEmpty())
		{
			// handle events from input devices
			foreach (InputsPair& input, this->inputs)
			{
				input.second.device->update(EventQueue(input.second.handles, this->events));
			}
		}
		
		// get an event (use NO_EVENT if queue is empty)
		return this->events.get(NO_EVENT);
	}

	
	// display handle
	EGLDisplay display;
		
	// surface handle
	EGLSurface surface;
	
	// context handle
	EGLContext context;

	// event queue
	SimpleQueue<int, 32> events;
};

Pointer<Display> Display::open(StringRef title, int width, int height, int state)
{
	EGLSurface surface = EGL_NO_SURFACE;
	EGLContext context = EGL_NO_CONTEXT;
	
	// get the default display
	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (display == EGL_NO_DISPLAY)
	{
		return false;
	}
	
	// initialize EGL
	EGLint majorVersion, minorVersion;
	if (!eglInitialize(display, &majorVersion, &minorVersion))
	{
		//printf("Error: eglInitialize() failed.\n");
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
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
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
	surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)NULL, NULL);
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
		
	// crate linux display
	{
		Pointer<LinuxDisplay> ld = new LinuxDisplay(display, surface, context);
		
		return ld;
	}
	
error:
	destroy(display, surface, context);
	dNotify("Can't create an EGL window");
	return false;
}
	
} // namespace digi
