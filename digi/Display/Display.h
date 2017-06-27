#ifndef digi_Display_Display_h
#define digi_Display_Display_h

#include <map>

#include <digi/Utility/StringRef.h>
#include <digi/Utility/Signal.h>
#include <digi/Utility/Object.h>
#include <digi/Math/All.h>

#include "InputDevice.h"


namespace digi {

/// @addtogroup Display
/// @{

// display with rendering context (OpenGL context). On windows and macos this is a window with title bar that can be
// moved and resized. Also fullscreen can be toggled.
// note for mac os: application must be a bundle to make Display function correctly.
class Display : public Object
{
public:

	// display mode
	struct Mode
	{
		int width;
		int height;
		int bitsPerPixel;
		
		Mode()
			: width(0), height(0), bitsPerPixel(0) {}
		Mode(int width, int height, int bitsPerPixel)
			: width(width), height(height), bitsPerPixel(bitsPerPixel) {}
			
		// calculate the pixel aspect ratio for this mode if the physical screen size is known.
		// e.g. on a 16:9 screen do pixelAspect = mode.getPixelAspect(16.0f, 9.0f) if your screen is 16x9cm
		// if physical screen size is unknown, use 1.0f
		// aspect ratio of a display is size.x / size.y * pixelAspect
		float getPixelAspect(float physicalWidth, float physicalHeight)
		{
			return (physicalWidth * float(this->height)) / (physicalHeight * float(this->width));
		}
	};
	
	/// get current screen mode
	static Mode getMode();	

	
	// state flags
	enum State
	{
		// display is visible and has user input
		ACTIVE = 1,
		
		// display is fullscreen
		FULLSCREEN = 2,
		
		// display is hidden, e.g. minimized
		HIDDEN = 4
	};

	/// create a display (state can be FULLSCREEN).
	/// for this to work on macos the application must be an app bundle.
	/// cmake: set_target_properties(MyApp PROPERTIES MACOSX_BUNDLE TRUE)
	static Pointer<Display> open(StringRef title, int width, int height, int state = 0);
	
	virtual ~Display();
	
	/// close display
	virtual void close() = 0;
	
	/// update display (swap buffers)
	virtual void update() = 0;
	
	/// get display state flags
	virtual int getState() = 0;

	/// get current size of display in pixels (screen size for fullscreen, window size for windowed)
	virtual int2 getSize() = 0;

	/// toggle fullscreen state of window
	virtual void toggleFullscreen() = 0;
	
	/// show/hide mouse cursor
	virtual void showCursor(bool show) = 0;

	
	/// read text input (console device on linux)
	virtual int readTextInput(char* data, int length) = 0;


	/// set a device to the display. then handles can be retrieved by using <name>.<attribute>. erase device by passing null.
	void setDevice(const std::string& name, Pointer<InputDevice> device);
	
	/// get handle to an input (e.g. "key.A" or "mouse.left")
	int getHandle(StringRef name);
	
	/// get bool input value by handle
	bool getBool(int handle);

	/// get int input value by handle
	int getInt(int handle);
	
	/// get int2 input value by handle
	int2 getInt2(int handle);

	/// get float2 input value by handle
	float2 getFloat2(int handle);

	enum Event
	{
		NO_EVENT = -2,
		
		// the window was closed by the user
		EVENT_CLOSE = -3
	};

	/// return handle of input that has changed or NO_EVENT if no change or EVENT_QUIT if display was closed
	virtual int getEvent() = 0;

	/// show an error message, e.g. because a feature is not supported
	virtual void showError(StringRef message) = 0;
	
	
	/// signal that notifies size change
	Signal<void ()> onresize;

	/// signal that notifies paint request
	Signal<void ()> onpaint;

protected:

	Display()
	{
		dummyAttributeInfo.name = StringRef::emptyString;
		dummyAttributeInfo.type = InputDevice::T_INVALID;
	}

	struct Input
	{
		Pointer<InputDevice> device;
		std::vector<int> handles;

		void set(Pointer<InputDevice> device)
		{
			this->device = device;
			handles.assign(device->getAttributeInfos().size(), -1);
		}
	};
	typedef std::map<std::string, Input> Inputs;
	typedef std::pair<const std::string, Input> InputsPair;
	Inputs inputs;

	struct Attribute
	{
		const InputDevice::AttributeInfo* attributeInfo;
			
		// pointer to attribute (points into state of input device)
		uint8_t* pointer;
	};
	std::vector<Attribute> attributes;
	InputDevice::AttributeInfo dummyAttributeInfo;
};

inline bool operator ==(const Display::Mode& a, const Display::Mode& b)
{
	return a.width == b.width && a.height == b.height && a.bitsPerPixel == b.bitsPerPixel;
}	

/// @}

} // namespace digi

#endif
