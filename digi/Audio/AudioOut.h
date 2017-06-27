#ifndef digi_Audio_AudioOut_h
#define digi_Audio_AudioOut_h

#include <digi/Utility/Object.h>
#include "AudioFormat.h"


namespace digi {

/// @addtogroup Audio
/// @{

/// base class for audio output stream
class AudioOut : public Object
{
public:

	/// audio buffer. write takes one buffer for INTERLEAVED, multiple buffers for SEPARATE formats
	typedef const void* Buffer;

	/// destructor
	virtual ~AudioOut();

	/// get the format of the audio output
	virtual AudioFormat getFormat() = 0;

// state

	/// close the audio output
	virtual void close() = 0;

// write

	/// write samples to the audio output. returns number of samples written
	virtual size_t write(const Buffer* buffers, size_t numSamples) = 0;
	
	/// write samples in interleaved format (getFormat().layout must be AudioFormat::INTERLEAVED)
	size_t writeInterleaved(const void* samples, size_t numSamples)
	{
		return this->write(&samples, numSamples);
	}
};

/// @}

} // namespace digi

#endif
