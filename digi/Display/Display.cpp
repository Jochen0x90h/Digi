#include <digi/Utility/StringUtility.h>
#include <digi/Utility/Find.h>
#include <digi/Utility/VectorUtility.h>
#include <digi/Utility/foreach.h>

#include "Display.h"


namespace digi {

namespace
{
	// less functors

	template <typename Type>
	struct NameLess
	{
		bool operator ()(const Type& info, StringRef name)
		{
			return info.name < name;
		}

		bool operator ()(StringRef name, const Type& info)
		{
			return name < info.name;
		}
	};

	typedef NameLess<InputDevice::AttributeInfo> AttributeLess;

} // anonymous namespace


// Display

Display::~Display()
{
}

void Display::setDevice(const std::string& name, Pointer<InputDevice> device)
{
	Input& input = this->inputs[name];
	if (input.device != null)
	{
		// remove attributes of old device
		foreach (int handle, input.handles)
		{
			if (handle != -1)
			{
				Attribute& attribute = this->attributes[handle];
				attribute.attributeInfo = &this->dummyAttributeInfo;
				attribute.pointer = NULL;
			}
		}
	}
	if (device != null)
		input.set(device);
	else
		this->inputs.erase(name);
}

int Display::getHandle(StringRef name)
{
	int p = find(name, '.');
	if (p > 0)
	{
		std::string deviceName = substring(name, 0, p);
		Inputs::iterator inputIt = this->inputs.find(deviceName);
		if (inputIt != this->inputs.end())
		{
			std::string attributeName = substring(name, p);
			
			ArrayRef<const InputDevice::AttributeInfo> attributeInfos = inputIt->second.device->getAttributeInfos();
			
			ArrayRef<const InputDevice::AttributeInfo>::iterator begin = attributeInfos.begin();
			ArrayRef<const InputDevice::AttributeInfo>::iterator end = attributeInfos.end();
			ArrayRef<const InputDevice::AttributeInfo>::iterator it = binaryFind(begin, end,
				attributeName, AttributeLess());

			// check if attribute was found
			if (it != end)
			{
				const InputDevice::AttributeInfo* attributeInfo = &*it;
				size_t index = it - begin;
				
				// check if attribute already has a handle
				int handle = inputIt->second.handles[index];
				if (handle != -1)
					return handle;
				
				// no: search for free attribute handle
				std::vector<Attribute>::iterator it = this->attributes.begin();
				std::vector<Attribute>::iterator end = this->attributes.end();
				for (; it != end; ++it)
				{
					if (it->attributeInfo == &this->dummyAttributeInfo)
						break;
				}
				handle = int(it - this->attributes.begin());
				if (it == end)
				{
					// add attribute instance
					add(this->attributes);
				}
				
				// get pointer to attribute data
				uint8_t* pointer = (uint8_t*)inputIt->second.device->getState() + attributeInfo->offset;

				// set handle
				inputIt->second.handles[index] = handle;

				// add attribute instance
				Attribute& attribute = this->attributes[handle];
				attribute.attributeInfo = attributeInfo;
				attribute.pointer = pointer;
				
				return handle;
			}
		}		
	}	
	return -1;
}

bool Display::getBool(int handle)
{
	// check attribute handle
	if (uint(handle) < uint(this->attributes.size()))
	{
		Attribute attribute = this->attributes[handle];
		
		// set attribute if type matches
		if (attribute.attributeInfo->type >= InputDevice::T_BOOL && attribute.attributeInfo->type <= InputDevice::T_BOOL4)
			return *(bool*)attribute.pointer;
	}
	return bool();		
}

int Display::getInt(int handle)
{
	// check attribute handle
	if (uint(handle) < uint(this->attributes.size()))
	{		
		Attribute attribute = this->attributes[handle];
		
		// set attribute if type matches
		if (attribute.attributeInfo->type >= InputDevice::T_INT && attribute.attributeInfo->type <= InputDevice::T_INT4)
			return *(int*)attribute.pointer;
	}
	return int();
}

int2 Display::getInt2(int handle)
{
	// check attribute handle
	if (uint(handle) < uint(this->attributes.size()))
	{		
		Attribute attribute = this->attributes[handle];
		
		// set attribute if type matches
		if (attribute.attributeInfo->type >= InputDevice::T_INT2 && attribute.attributeInfo->type <= InputDevice::T_INT4)
			return *(int2*)attribute.pointer;
	}
	return int2();
}

float2 Display::getFloat2(int handle)
{
	// check attribute handle
	if (uint(handle) < uint(this->attributes.size()))
	{		
		Attribute attribute = this->attributes[handle];
		
		// set attribute if type matches
		if (attribute.attributeInfo->type >= InputDevice::T_FLOAT2 && attribute.attributeInfo->type <= InputDevice::T_FLOAT4)
			return *(float2*)attribute.pointer;
	}
	return float2();
}

} // namespace digi
