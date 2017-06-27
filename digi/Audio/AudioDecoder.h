#ifndef digi_Audio_AudioDecoder_h
#define digi_Audio_AudioDecoder_h

#include <digi/Utility/Object.h>
#include "AudioIn.h"


namespace digi {

/// @addtogroup Audio
/// @{

/// decodes audio from uncompressed or compressed data or files
class AudioDecoder : public AudioIn
{
public:

	/// destructor
	virtual ~AudioDecoder();

// time

	/// set the position of the audio file in seconds
	virtual void setTime(double time) = 0;

	/// get the position of the audio file in seconds
	virtual double getTime() = 0;

	/// get duration of file in seconds
	virtual double getDuration() = 0;
};

/// @}

} // namespace digi

#endif
