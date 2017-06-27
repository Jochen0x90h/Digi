#ifndef digi_Audio_AudioIn_h
#define digi_Audio_AudioIn_h

#include <digi/Utility/Object.h>
#include "AudioFormat.h"


namespace digi {

/// @addtogroup Audio
/// @{

/// base class for audio input stream
class AudioIn : public Object
{
public:

	/// audio buffer. read takes one buffer for INTERLEAVED, multiple buffers for SEPARATE formats
	typedef void* Buffer;

	/// destructor
	virtual ~AudioIn();

	/// get the format of the audio input
	virtual AudioFormat getFormat() = 0;

// state

	/// close the audio input
	virtual void close() = 0;

// read

	/// read samples from the audio input. returns number of samples read
	virtual size_t read(Buffer* buffers, size_t numSamples) = 0;

	/// read samples in interleaved format (getFormat().layout must be AudioFormat::INTERLEAVED)
	size_t readInterleaved(void* samples, size_t numSamples)
	{
		return this->read(&samples, numSamples);
	}
};

/// @}

} // namespace digi

#endif
