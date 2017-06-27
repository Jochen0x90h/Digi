#ifndef digi_Display_InputDevice_h
#define digi_Display_InputDevice_h

#include <digi/Utility/ArrayRef.h>
#include <digi/Utility/Object.h>

#include "SimpleQueue.h"

namespace digi {

/// @addtogroup Display
/// @{

class EventQueue
{
public:
	EventQueue(const std::vector<int>& handles, SimpleQueue<int, 32>& events)
		: handles(handles), events(events) {}

	void add(int index)
	{
		// get handle for index
		int handle = this->handles[index];
		
		// check if handle is allocated
		if (handle != -1)
		{
			// post handle as event
			this->events.add(handle);
		}
	}

protected:
	const std::vector<int>& handles;
	SimpleQueue<int, 32>& events;
};

class InputDevice : public Object
{
public:
	enum Type
	{
		T_INVALID = -1,
		T_BOOL,
		T_BOOL2,
		T_BOOL3,
		T_BOOL4,
		T_INT,
		T_INT2,
		T_INT3,
		T_INT4,
		T_FLOAT,
		T_FLOAT2,
		T_FLOAT3,
		T_FLOAT4,		
	};

	struct AttributeInfo
	{	
		const char* name;

		// type of attribute (e.g. "T_INT")
		uint type;

		// offset in state
		size_t offset;
	};

	virtual ~InputDevice();

	// get alphabetically sorted list of parameters
	virtual ArrayRef<const AttributeInfo> getAttributeInfos() = 0;
	
	// get state of device
	virtual void* getState() = 0;

	// update state by reading from the device and post events into the queue
	virtual void update(EventQueue eventQueue) = 0;
};

/// @}

} // namespace digi

#endif
