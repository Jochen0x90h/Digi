#ifndef digi_Video_VorbisDecoder_h
#define digi_Video_VorbisDecoder_h

#include <vector>
#include <vorbis/codec.h>

#include <digi/Audio/AudioOut.h>

#include "MediaDecoder.h"


namespace digi {

/// @addtogroup Video
/// @{

class VorbisDecoder : public MediaDecoder
{
public:
	VorbisDecoder(const std::vector<uint8_t>& codecPrivate);

	virtual ~VorbisDecoder();

	virtual void close();

	AudioFormat getFormat()
	{
		return AudioFormat(AudioFormat::FLOAT32, AudioFormat::SEPARATE, this->info.channels, this->info.rate);
	}

	void setOutput(Pointer<AudioOut> output) {this->output = output;}

	virtual void update();

	virtual void decode(double time, const uint8_t* data, size_t length);
	
protected:
	
	Pointer<AudioOut> output;
	
	vorbis_info info;
	vorbis_comment comment;
	vorbis_dsp_state dspState;
	vorbis_block block;
	ogg_int64_t packetNumber;
};

/// @}

} // namespace digi

#endif
