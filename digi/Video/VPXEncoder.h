#ifndef digi_Video_VPXEncoder_h
#define digi_Video_VPXEncoder_h

#include <vpx/vpx_encoder.h>
#include <vpx/vp8cx.h>

#include "MediaEncoder.h"
#include "VideoIn.h"


namespace digi {

/// @addtogroup Video
/// @{

class VPXEncoder : public MediaEncoder
{
public:

	/// constructs vpx encoder that reads frames from given input
	VPXEncoder(Pointer<VideoIn> input);

	virtual ~VPXEncoder();

	virtual void close();

	/// initialiye for single pass
	void initForSinglePass(int bitrate = 256);

	/// do first pass to get statistics, then initialize for second pass
	bool doFirstPassAndInitForSecondPass(int bitrate, double duration);

	virtual void encode(Packet& packet);

protected:

	Pointer<VideoIn> input;
	
	int width;
	int height;
	double frames2Seconds;

	// two pass mode
	bool twopass;

	// vpx encoder context
	vpx_codec_ctx_t context;

	// frame counter
	int frameIndex;

	// iterator for getting packets
	vpx_codec_iter_t iter;
	
	// statistics for multipass
	std::vector<uint8_t> stats;
};

/// @}

} // namespace digi

#endif
