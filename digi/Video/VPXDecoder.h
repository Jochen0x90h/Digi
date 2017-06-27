#ifndef digi_Video_VPXDecoder_h
#define digi_Video_VPXDecoder_h

#include <vpx/vpx_decoder.h>
#include <vpx/vp8dx.h>

#include "MediaDecoder.h"
#include "VideoFormat.h"
#include "VideoOut.h"


namespace digi {

/// @addtogroup Video
/// @{

class VPXDecoder : public MediaDecoder
{
public:
	VPXDecoder(int width, int height, float framesPerSecond);

	virtual ~VPXDecoder();

	virtual void close();

	VideoFormat getFormat() {return VideoFormat(VideoFormat::YV12, this->width, this->height, this->framesPerSecond);}

	void setOutput(Pointer<VideoOut> output) {this->output = output;}

	virtual void decode(double time, const uint8_t* data, size_t length);

protected:
	
	int width;
	int height;
	float framesPerSecond;
	
	Pointer<VideoOut> output;
	
	vpx_codec_ctx_t context;
};

/// @}

} // namespace digi

#endif
