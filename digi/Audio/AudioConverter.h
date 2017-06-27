#ifndef digi_Audio_AudioConverter_h
#define digi_Audio_AudioConverter_h

#include <digi/Base/Platform.h>
#include "AudioIn.h"


namespace digi {

/// @addtogroup Audio
/// @{

/// base class for audio input stream
class AudioInConverter : public AudioIn
{
public:

	// construct an audio input converter with input, destination format and buffer size in samples
	AudioInConverter(Pointer<AudioIn> input, AudioFormat format, int numBufferSamples = 1024);

	/// destructor
	virtual ~AudioInConverter();

	/// get the format of the audio input
	virtual AudioFormat getFormat();

	/// close the audio input
	virtual void close();

	/// read samples from the audio input. returns number of samples read
	virtual size_t read(Buffer* buffers, size_t numSamples);

protected:
	
	// input
	Pointer<AudioIn> input;
	
	// output format
	AudioFormat format;
	
	// buffer
	size_t numBufferSamples;
	uint8_t* buffer;
	void* buffers[16];

	int numChannels;

	AudioFormat::Type srcType;
	int srcStride;

	bool dstInterleaved;
	int dstSize;
	int dstStride;
};

/// @}

} // namespace digi

#endif
