#include <stdexcept>

#include "IODevice.h"


namespace digi {

// IODevice

IODevice::~IODevice()
{
}

std::string IODevice::getResource()
{
	return std::string();
}

int64_t IODevice::seek(int64_t position, PositionMode mode)
{
	throw std::runtime_error("IODevice: seek not supported");
}

void IODevice::setSize(int64_t size)
{
	throw std::runtime_error("IODevice: setSize not supported");
}

int64_t IODevice::getSize()
{
	throw std::runtime_error("IODevice: getSize not supported");
}

} // namespace digi
