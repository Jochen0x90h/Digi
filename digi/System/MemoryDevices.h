/*
	I/O devices that operate on memory
*/

#ifndef digi_System_MemoryDevices_h
#define digi_System_MemoryDevices_h

#include <string>
#include <vector>

#include "IODevice.h"

namespace digi {

/// @addtogroup System
/// @{


/**
	I/O device that operates on a stl-compatible random access container
	or a reference of a container
*/
template <typename Container>
class ContainerDevice : public IODevice
{
public:

	ContainerDevice()
		: position(0), state(OPEN) {}
	ContainerDevice(Container container, size_t position = 0)
		: container(container), position(position), state(OPEN) {}
	
	virtual ~ContainerDevice()
	{
	}
		
	virtual int getState()
	{
		return this->state;
	}

	virtual void close()
	{
		this->state = 0;
	}

	virtual size_t read(void* data, size_t length)
	{
		size_t numRead = std::min(length, this->container.size() - this->position);
		uint8_t* dst = (uint8_t*)data;
		std::copy(this->container.begin() + this->position, this->container.begin() + this->position + numRead, dst); 
		this->position += numRead;
		return numRead;		
	}
	
	virtual size_t write(const void* data, size_t length)
	{
		size_t newPosition = this->position + length;
		
		if (newPosition > this->container.size())
			this->container.resize(newPosition);
		
		const uint8_t* src = (uint8_t*)data;
		std::copy(src, src + length, this->container.begin() + this->position);
		
		this->position = newPosition;
		return length;		
	}

	virtual int64_t seek(int64_t position, PositionMode positionMode)
	{
		switch (positionMode)
		{
		case BEGIN:
			this->position = size_t(position);
			break;
		case CURRENT:
			this->position = size_t(this->position + position);
			break;
		case END:
			this->position = size_t(this->container.size() + position);
			break;
		}
				
		return this->position;		
	}
	
	virtual void setSize(int64_t size)
	{
		this->container.resize((size_t)size);
	}
	
	virtual int64_t getSize()
	{
		return this->container.size();
	}
	
	
	Container container;
	size_t position;
	int state;
};


typedef ContainerDevice<std::string&> StringRefDevice;
typedef ContainerDevice<std::vector<uint8_t> > MemoryDevice;

/// @}

} // namespace digi

#endif
