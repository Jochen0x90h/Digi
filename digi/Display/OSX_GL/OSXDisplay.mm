#import <OpenGL/gl.h>
#import <Cocoa/Cocoa.h>

#import <boost/range.hpp>

#import <digi/Utility/ArrayUtility.h>
#import <digi/Utility/ArrayRef.h>
#import <digi/Utility/VectorUtility.h>
#import <digi/Utility/foreach.h>
#import <digi/System/Log.h>

#import "../Version.h"
#import "../Display.h"
#import "../InputDevice.h"
#import "../SimpleQueue.h"


// application
@interface DigiApplication : NSApplication
{
}
@end

@implementation DigiApplication

- (id) init
{
	self = [super init];
	
    // set application delegate (which is DigiApplication)
    //[self setDelegate: self];

	[self finishLaunching];
	
	// hack: mark app as running
	_running = 1;

	return self;
}

// application delegate methods
- (void)applicationWillFinishLaunching: (NSNotification *)notification
{
}

- (void)applicationDidFinishLaunching: (NSNotification *)notification
{
}

@end

void digiDisplayInit()
{
	digi::VersionInfo::add(versionInfo);

	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	NSApp = [DigiApplication sharedApplication];
			
	[pool drain];		
}

void digiDisplayDone()
{
}


namespace digi {

Display::Mode Display::getMode()
{
	NSScreen* mainScreen = [NSScreen mainScreen];
	NSRect rect = [mainScreen frame];
	NSWindowDepth depth = [mainScreen depth];
	
	return Mode(rect.size.width, rect.size.height, depth);
}

// forward declare display class
class OSXDisplay;

} // namespace digi


// window
@interface DigiWindow : NSWindow<NSWindowDelegate>
{
@public
	digi::OSXDisplay* display;
	NSRect frameForNonFullScreenMode;
}
@end

// opengl view
@interface DigiOpenGLView : NSOpenGLView
{
@public
	digi::OSXDisplay* display;
	NSCursor* cursor;
}
@end


namespace digi {

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
			// same sorted order as mouseAttributeInfos
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


class OSXDisplay : public Display
{
public:
	
	OSXDisplay(DigiWindow* window, DigiOpenGLView* view, bool fullscreen)
		: window(window), view(view), fullscreen(fullscreen)
	{
		[window setReleasedWhenClosed: false];
		[window setAcceptsMouseMovedEvents: true];
		window->display = this;
		view->display = this;
	
		// set window as its delegate
		//[window setDelegate: window];
		
		// mouse cursor
		NSSize size;
		size.width = size.height = 4;
		NSPoint point;
		point.x = point.y = 0;
		NSImage* image = [[NSImage alloc] initWithSize:size];
		this->noCursor = [[NSCursor alloc] initWithImage:image hotSpot:point];
		view->cursor = nil;

		// clear key and mouse state for event handling
		memset(&this->keyState, 0, sizeof(KeyState));
		memset(&this->mouseState, 0, sizeof(MouseState));
	}

	virtual ~OSXDisplay()
	{
		this->close();
	}
	
	virtual void close()
	{
		// release window
		if (this->window != NULL)
		{
			NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

			[this->window release];
			this->window = NULL;

			[pool drain];
		}
	}
		
	virtual void update()
	{
		// copy back buffer to front buffer (openGLContext is of type NSOpenGLContext)
		[[this->view openGLContext] flushBuffer];
	}

	virtual int getState()
	{
		if ([this->window isMiniaturized])
			return HIDDEN;

		int flags = 0;
		if ([this->window isKeyWindow])
			flags |= ACTIVE;
		if ([this->window styleMask] & NSFullScreenWindowMask)
			flags |= FULLSCREEN;
		return flags;
	}
	
	virtual int2 getSize()
	{
		NSRect rect = [this->view bounds];
		return vector2(int(rect.size.width), int(rect.size.height));
	}

	virtual void toggleFullscreen()
	{
		if ([DigiWindow instancesRespondToSelector:@selector(toggleFullScreen:)])
		{
			[this->window toggleFullScreen:nil];
		
			// workaround: when a key down initiates toggleFullscreen, key up is lost
			fill(this->keyState.keys, false);
		}
	}
	
	virtual void showCursor(bool show)
	{
		this->view->cursor = show ? nil : this->noCursor;
		
		// invalidate cursor rects so that the new cursor gets set
		[this->window invalidateCursorRectsForView:this->view];
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
			NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
			NSRunLoop *runLoop = [[NSRunLoop currentRunLoop] retain];

			this->stopEvents = false;
			NSEvent* event;
			while (!this->stopEvents
				&& (event = [NSApp nextEventMatchingMask:NSAnyEventMask untilDate:nil inMode:NSDefaultRunLoopMode dequeue:YES]))
			{
				[NSApp sendEvent:event];
			}
			[runLoop release];
			[pool drain];

			// handle input events
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
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

		NSString* nsMessage = [[NSString alloc] initWithUTF8String:message.data()];
	
		NSAlert *alert = [[NSAlert alloc] init];
		[alert addButtonWithTitle:@"OK"];
	//[this->window getTitle];
		[alert setMessageText:nsMessage];
		//[alert setInformativeText:nsMessage];
		[alert setAlertStyle:NSWarningAlertStyle];
		[alert runModal];
		[alert release];
		
		[pool drain];
	}
	
	void processKeyEvent(NSEvent* event, bool repeat)
	{
		NSEventType type = [event type];
		uint key = [event keyCode];
		int modifiers = [event modifierFlags];
	//dNotify("processKeyEvent type " << int(type) << " key " << key << " modifiers " << std::hex << modifiers);

		// standard keys
		if (key < 256)
		{
			// modifiers are not included in key2AttributeIndex
			int attributeIndex = digi::key2AttributeIndex[key];
			if (attributeIndex != -1)
			{
				// set key state
				bool state = type == NSKeyDown;
				this->keyState.setKey(attributeIndex, state, repeat);
				this->stopEvents |= state & !repeat;
			}
		}

		// modifier keys
		this->keyState.setModifier(KeyState::SHIFT_INDEX, (modifiers & NSShiftKeyMask) != 0);
		this->keyState.setModifier(KeyState::LEFT_SHIFT_INDEX, (modifiers & 0x2) != 0);
		this->keyState.setModifier(KeyState::RIGHT_SHIFT_INDEX, (modifiers & 0x4) != 0);

		this->keyState.setModifier(KeyState::CONTROL_INDEX, (modifiers & NSControlKeyMask) != 0);
		this->keyState.setModifier(KeyState::LEFT_CONTROL_INDEX, (modifiers & 0x1) != 0);
		this->keyState.setModifier(KeyState::RIGHT_CONTROL_INDEX, (modifiers & 0x2000) != 0);

		this->keyState.setModifier(KeyState::ALT_INDEX, (modifiers & NSAlternateKeyMask) != 0);
		this->keyState.setModifier(KeyState::LEFT_ALT_INDEX, (modifiers & 0x20) != 0);
		this->keyState.setModifier(KeyState::RIGHT_ALT_INDEX, (modifiers & 0x40) != 0);

		this->keyState.setModifier(KeyState::COMMAND_INDEX, (modifiers & NSCommandKeyMask) != 0);
		this->keyState.setModifier(KeyState::LEFT_COMMAND_INDEX, (modifiers & 0x08) != 0);
		this->keyState.setModifier(KeyState::RIGHT_COMMAND_INDEX, (modifiers & 0x10) != 0);
	}
	
	
	// window and view
	DigiWindow* window;
	DigiOpenGLView* view;

	// window state
	bool fullscreen;

	// empty mouse cursor
	NSCursor* noCursor;

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
		return ArrayRef<const AttributeInfo>(keyAttributeInfos);
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
		return ArrayRef<const AttributeInfo>(mouseAttributeInfos);
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
			for (ptrdiff_t attributeIndex = 0; attributeIndex < boost::size(mouseAttributeInfos); ++attributeIndex)
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

} // namespace digi


using namespace digi;


// window
@implementation DigiWindow

// needed for fullscreen (window style NSBorderlessWindowMask)
- (BOOL) canBecomeKeyWindow
{
    return YES;
}

- (void) close
{
	// the application has to close the window when it receives EVENT_CLOSE
	//[super close];
	
	self->display->events.add(Display::EVENT_CLOSE);
}
@end


// opengl view
@implementation DigiOpenGLView

// gets called when the size of the view changes
- (void) reshape
{
	[super reshape];
	self->display->onresize();
}

- (void)drawRect:(NSRect)dirtyRect
{
	self->display->onpaint();
}

// NSResponder protocol

- (BOOL) acceptsFirstResponder
{
	return YES;
}

- (void) keyDown:(NSEvent*)event
{
	self->display->processKeyEvent(event, [event isARepeat]);
	//[self interpretKeyEvents:[NSArray arrayWithObject:event]];

	// text input
	NSString* str = [event characters];
	const char* utf8 = [str UTF8String];
	self->display->textInput.add(utf8, strlen(utf8));
}

- (void) keyUp:(NSEvent*)event
{
	self->display->processKeyEvent(event, false);
}
/*
- (BOOL) performKeyEquivalent:(NSEvent*)event
{
	return YES;
}
*/
- (void) flagsChanged:(NSEvent*)event
{
	self->display->processKeyEvent(event, false);
}

/*
- (void) insertText:(id)aString
{
//int xx = 0;

	// text input
	NSString* str = (NSString*)aString;//[event characters];
	const char* utf8 = [str UTF8String];
	self->display->textInput.add(utf8, strlen(utf8));
std::cout << "insertText " << strlen(utf8) << std::endl;
}
*/

/*
// NSTextInputClient protocol
- (BOOL) hasMarkedText
{
}

- (NSRange) markedRange
{
}

- (NSRange) selectedRange
{
}

- (void) setMarkedText:(id)aString selectedRange:(NSRange)selectedRange replacementRange:(NSRange)replacementRange
{
}

- (void) unmarkText
{
}

- (NSArray*) validAttributesForMarkedText
{
}

- (NSAttributedString*) attributedSubstringForProposedRange:(NSRange)aRange actualRange:(NSRangePointer)actualRange
{
}

- (void) insertText:(id)aString replacementRange:(NSRange)replacementRange
{
	NSString* str = (NSString*)aString;//[event characters];
	std::cout << "insertText " << [str UTF8String] << " l " << replacementRange.location << " l " << replacementRange.length << std::endl;
}

- (NSUInteger) characterIndexForPoint:(NSPoint)aPoint
{
}

- (NSRect) firstRectForCharacterRange:(NSRange)aRange actualRange:(NSRangePointer)actualRange
{
}

- (void) doCommandBySelector:(SEL)aSelector
{
}
*/

- (void) mouseDown:(NSEvent*)event
{
	self->display->mouseState.left = true;
	self->display->mouseState.changed |= MouseState::LEFT_FLAG;
	self->display->stopEvents = true;
	//std::cout << "left down" << std::endl;
}

- (void) mouseUp:(NSEvent*)event
{
	self->display->mouseState.left = false;
	self->display->mouseState.changed |= MouseState::LEFT_FLAG;
	//std::cout << "left up" << std::endl;
}

- (void) rightMouseDown:(NSEvent*)event
{
	self->display->mouseState.right = true;
	self->display->mouseState.changed |= MouseState::RIGHT_FLAG;
	self->display->stopEvents = true;
	//std::cout << "right down" << std::endl;
}

- (void) rightMouseUp:(NSEvent*)event
{
	self->display->mouseState.right = false;
	self->display->mouseState.changed |= MouseState::RIGHT_FLAG;
	//std::cout << "right up" << std::endl;
}

- (void) otherMouseDown:(NSEvent*)event
{
	self->display->mouseState.middle = true;
	self->display->mouseState.changed |= MouseState::MIDDLE_FLAG;
	self->display->stopEvents = true;
}

- (void) otherMouseUp:(NSEvent*)event
{
	self->display->mouseState.middle = false;
	self->display->mouseState.changed |= MouseState::MIDDLE_FLAG;
}

- (void) mouseMoved:(NSEvent*)event
{
	// get size of view
	NSRect rect = [self bounds];
	int2 size = vector2(int(rect.size.width), int(rect.size.height));

	// get mouse position with origin at top/left
	NSPoint wp = [event locationInWindow];
	NSPoint vp = [self convertPoint:wp fromView:nil];
	int2 p = vector2(int(vp.x) - 1, size.y - 2 - int(vp.y)); //! - 2 ??
	//int2 p = vector2(int(vp.x), int(vp.y));
	
	// check if inside view
	if (uint(p.x) < uint(size.x) && uint(p.y) < uint(size.y))
	{
		// convert to device coordinates (-1 to 1)
		float2 position = vector2(float(p.x) + 0.5f, -float(p.y) - 0.5f)
			/ convert_float2(size) * 2.0f + vector2(-1.0f, 1.0f);

		self->display->mouseState.position = position;
		self->display->mouseState.changed |= MouseState::POSITION_FLAG;
		//std::cout << p << std::endl;
	}
}

- (void) mouseDragged:(NSEvent*)event
{
	// forward to mouseMoved method
	[self mouseMoved: event];
}

- (void) rightMouseDragged:(NSEvent*)event
{
	// forward to mouseMoved method
	[self mouseMoved: event];
}

- (void) otherMouseDragged:(NSEvent*)event
{
	// forward to mouseMoved method
	[self mouseMoved: event];
}

- (void) scrollWheel:(NSEvent*)event
{
	self->display->mouseState.wheel.x += int([event deltaX] * 120.0f);
	self->display->mouseState.wheel.y += int([event deltaY] * 120.0f);
	self->display->mouseState.changed |= MouseState::WHEEL_FLAG;

	//dNotify("wheel " << [event deltaX] << " " << [event deltaY] << " " << [event deltaZ]);
}
/*
- (void) mouseEntered:(NSEvent*)event
{
}

- (void) mouseExited:(NSEvent*)event
{
}
*/

- (void)resetCursorRects
{
	// set cursor for whole view
	if (self->cursor != nil)
		[self addCursorRect:[self bounds] cursor:self->cursor];
}

@end


namespace digi {

Pointer<Display> Display::open(StringRef title, int width, int height, int state)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	bool fullscreen = (state & FULLSCREEN) != 0;
	
	// window content rectangle (i.e. area without title bar) and style
	NSRect rect;
	rect.origin.x = 0;
	rect.origin.y = 0;
	rect.size.width = width;
	rect.size.height = height;
	int windowStyle = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;
/*
	// old fullscreen
	{
		rect = [[NSScreen mainScreen] frame];
		windowStyle = NSBorderlessWindowMask;		
	}
*/
	// create window
	DigiWindow* window = [[DigiWindow alloc] initWithContentRect:rect styleMask:windowStyle
		//backing:NSBackingStoreNonretained // bug: corners are black
		backing:NSBackingStoreBuffered
		defer:YES];
		//defer:NO];

	// Mac OS X lion: add fullscreen button to window title bar
	if ([DigiWindow instancesRespondToSelector:@selector(setCollectionBehavior:)])
		[window setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
/*
	if (fullscreen)
	{
		// set the window level to be above the menu bar
		[window setLevel:NSMainMenuWindowLevel + 1];
		
		[window setOpaque:YES];
		[window setHidesOnDeactivate:YES];
	}
*/
	// create pixel format
	NSOpenGLPixelFormatAttribute attributes[24];
	int index = 0;
	NSOpenGLPixelFormat* pixelFormat;

	attributes[index++] = NSOpenGLPFADoubleBuffer;
	attributes[index++] = NSOpenGLPFAAccelerated;

	// opengl profile
	attributes[index++] = NSOpenGLPFAOpenGLProfile;
#if GL_MAJOR == 2
	attributes[index++] = NSOpenGLProfileVersionLegacy;
#elif GL_MAJOR == 3 && GL_MINOR <= 2 && defined(GL_CORE)
	attributes[index++] = NSOpenGLProfileVersion3_2Core;
#else
	#error "OpenGL version not supported"
#endif
	
	// color and depth/stencil resolution
	attributes[index++] = NSOpenGLPFAColorSize;
	attributes[index++] = 24; // colorBits;
	attributes[index++] = NSOpenGLPFADepthSize;
	attributes[index++] = 24; // depthBits;
	attributes[index++] = NSOpenGLPFAStencilSize;
	attributes[index++] = 8;
	
	// multisample
	//attributes[index++] = NSOpenGLPFAMultisample;
	//attributes[index++] = NSOpenGLPFASampleBuffers;
	//attributes[index++] = 1;
	//attributes[index++] = NSOpenGLPFASamples;
	//attributes[index++] = 4;
	
	// end
	attributes[index] = 0;
	
	pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
		
	// create view with rectangle and pixel format
	DigiOpenGLView* view = [[DigiOpenGLView alloc] initWithFrame:rect pixelFormat:pixelFormat];
	//[view setWantsBestResolutionOpenGLSurface:YES];
	
	// release pixel format
	[pixelFormat release];
	
	if (!view)
		return null;
	
	// create display and set to view
	OSXDisplay* display = new OSXDisplay(window, view, fullscreen);
	display->inputs["key"].set(new KeyInputDevice(display->keyState));
	display->inputs["mouse"].set(new MouseInputDevice(display->mouseState));

	// set view into window
	[window setContentView: view];
	
	// forward key and mouse events to view
	[window setInitialFirstResponder: view];
	[window makeFirstResponder: view];
	
	// make the opengl context current
	[[view openGLContext] makeCurrentContext];

	// set title
	NSString* nsTitle = [[NSString alloc] initWithUTF8String:title.data()];
	[window setTitle:nsTitle];
		
	// show window
	[window makeKeyAndOrderFront:nil];

	// set fullscreen
	if (fullscreen && [DigiWindow instancesRespondToSelector:@selector(toggleFullScreen:)])
		[window toggleFullScreen:nil];

	[pool drain];

	return display;	
}

} // namespace digi
