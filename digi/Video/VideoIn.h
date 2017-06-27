#ifndef digi_Video_VideoIn_h
#define digi_Video_VideoIn_h

#include <digi/Utility/Object.h>

#include "VideoFormat.h"


namespace digi {

/// @addtogroup Video
/// @{

/// base class for video input stream
class VideoIn : public Object
{
public:

	/// plane of video frame for one or multiple color components dependent on format
	struct Plane
	{
		// plane data
		void* data;
		
		// plane size
		int width;
		int height;
	};

	virtual ~VideoIn();

	/// get video format that is provided by this video input
	virtual VideoFormat getFormat() = 0;

// state

	/// close video input
	virtual void close() = 0;
	
// read

	/// read a frame
	virtual bool read(Plane* planes) = 0;
	
	/// reset to first frame. must be implemented only when using with a multipass encoder
	virtual void reset();
};

/// @}

} // namespace digi

#endif
