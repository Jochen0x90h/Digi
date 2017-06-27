#ifndef digi_Video_MediaDecoder_h
#define digi_Video_MediaDecoder_h

#include <digi/Base/Platform.h>
#include <digi/Utility/Object.h>


namespace digi {

/// @addtogroup Video
/// @{

class MediaDecoder : public Object
{
public:
	
	virtual ~MediaDecoder();

	/// close decoder and output devices (e.g. file)
	virtual void close() = 0;

	/// update. may be used to stream out e.g. pending audio data
	virtual void update();
	
	/// decode a block of data
	virtual void decode(double time, const uint8_t* data, size_t length) = 0;
	
	/// clear current state on seek. may be used to clear the current subtitle
	virtual void clear();
};

/// @}

} // namespace digi

#endif
