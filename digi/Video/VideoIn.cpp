#include <stdexcept>

#include "VideoIn.h"


namespace digi {

VideoIn::~VideoIn()
{
}

void VideoIn::reset()
{
	throw std::runtime_error("VideoIn: reset not supported");
}

} // namespace digi
