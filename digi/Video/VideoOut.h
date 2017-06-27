#ifndef digi_Video_VideoOut_h
#define digi_Video_VideoOut_h

#include <digi/Utility/Object.h>

#include "VideoFormat.h"


namespace digi {

/// @addtogroup Video
/// @{

/// base class for video output stream
class VideoOut : public Object
{
public:

	/// plane of video frame for one or multiple color components dependent on format
	struct Plane
	{
		/// plane data
		const void* data;
		
		/// plane size
		int width;
		int height;
	};

	/// destructor
	virtual ~VideoOut();

	/// get the format of the video output
	virtual VideoFormat getFormat() = 0;

// state

	/// close video output
	virtual void close() = 0;

// write

	/// write a frame to the video output
	virtual void write(const Plane* planes) = 0;
};

/// @}

} // namespace digi

#endif
