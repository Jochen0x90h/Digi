#ifndef digi_Video_VorbisEncoder_h
#define digi_Video_VorbisEncoder_h

#include <vector>
#include <vorbis/codec.h>

#include <digi/Audio/AudioIn.h>

#include "MediaEncoder.h"


namespace digi {

/// @addtogroup Video
/// @{

class VorbisEncoder : public MediaEncoder
{
public:

	/// construct vorbis encoder for given input. format is converted automatically.
	/// codecPrivate gets filled with the first three info packets in xiph lacing
	VorbisEncoder(Pointer<AudioIn> input, std::vector<uint8_t>& codecPrivate);

	virtual ~VorbisEncoder();

	virtual void close();

	virtual void encode(Packet& packet);
		
protected:
	
	Pointer<AudioIn> input;
	
	double samples2Seconds;

	vorbis_info info;
	vorbis_comment comment;
	vorbis_dsp_state dspState;
	vorbis_block block;
	
	AudioFormat::Type type;
	int numChannels;
};

/// @}

} // namespace digi

#endif
