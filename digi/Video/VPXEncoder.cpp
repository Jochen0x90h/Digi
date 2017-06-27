#include <cmath>

#include <digi/Utility/VectorUtility.h>
#include <digi/System/Log.h>

#include "VPXEncoder.h"


namespace digi {

VPXEncoder::VPXEncoder(Pointer<VideoIn> input)
	: input(input), twopass(), frameIndex(-1), iter(NULL)
{
	VideoFormat format = input->getFormat();
	this->width = format.width;
	this->height = format.height;
	this->frames2Seconds = 1.0 / format.frameRate;
}

VPXEncoder::~VPXEncoder()
{
	vpx_codec_destroy(&this->context);
}

void VPXEncoder::initForSinglePass(int bitrate)
{
	this->twopass = false;

	vpx_codec_enc_cfg config;
	vpx_codec_enc_config_default(&vpx_codec_vp8_cx_algo, &config, 0);
	config.rc_target_bitrate = bitrate;
	config.g_w = this->width;
	config.g_h = this->height;
	config.g_timebase.num = 1;
	config.g_timebase.den = 1000;
	//config.kf_max_dist = int(5.0f * format.frameRate); // maximum key frame distance is 5 seconds
	config.kf_max_dist = int(0.5 / this->frames2Seconds); // maximum key frame distance is 0.5 seconds
	config.g_pass = VPX_RC_ONE_PASS;

	vpx_codec_enc_init(&this->context, &vpx_codec_vp8_cx_algo, &config, 0);
}

bool VPXEncoder::doFirstPassAndInitForSecondPass(int bitrate, double duration)
{
	this->twopass = true;
	
	vpx_codec_enc_cfg config;
	vpx_codec_enc_config_default(&vpx_codec_vp8_cx_algo, &config, 0);
	config.rc_target_bitrate = bitrate;
	config.g_w = this->width;
	config.g_h = this->height;
	config.g_timebase.num = 1;
	config.g_timebase.den = 1000;
	//config.kf_max_dist = int(5.0f * format.frameRate); // maximum key frame distance is 5 seconds
	config.kf_max_dist = int(0.5 / this->frames2Seconds); // maximum key frame distance is 0.5 seconds

	// init for first pass
	config.g_pass = VPX_RC_FIRST_PASS;
	vpx_codec_enc_init(&this->context, &vpx_codec_vp8_cx_algo, &config, 0);

	Packet packet;
	while (true)
	{
		this->encode(packet);
		
		// check for end packet
		if (packet.type == Packet::END)
			break;
		
		uint8_t* begin = (uint8_t*)packet.data;
		uint8_t* end = begin + packet.size;
		this->stats.insert(this->stats.end(), begin, end);

		// check if packet time exceeds duration
		if (packet.time >= duration)
			break;
	}

	// get end of stream packet
	vpx_codec_pts_t frameStart = vpx_codec_pts_t(floor(this->frameIndex * this->frames2Seconds * 1000.0 + 0.5));
	vpx_codec_encode(&this->context, NULL, frameStart, 1, 0, VPX_DL_BEST_QUALITY);
	const vpx_codec_cx_pkt_t* vpxPacket;
	vpx_codec_iter_t iter = NULL;
	while ((vpxPacket = vpx_codec_get_cx_data(&this->context, &iter)) != NULL)
	{
		if (vpxPacket->kind == VPX_CODEC_STATS_PKT)
		{
			uint8_t* begin = (uint8_t*)vpxPacket->data.twopass_stats.buf;
			uint8_t* end = begin + vpxPacket->data.twopass_stats.sz;
			this->stats.insert(this->stats.end(), begin, end);
		}
	}

	// destroy for first pass
	vpx_codec_destroy(&this->context);
	
	// reset input video stream to first frame
	this->input->reset();
	
	// init for second pass
	config.g_pass = VPX_RC_LAST_PASS;
	config.rc_twopass_stats_in.buf = this->stats.data();
	config.rc_twopass_stats_in.sz = this->stats.size();
	vpx_codec_err_t result = vpx_codec_enc_init(&this->context, &vpx_codec_vp8_cx_algo, &config, 0);
	
	this->frameIndex = -1;
	this->iter = NULL;
	
	return true;
}

void VPXEncoder::encode(Packet& packet)
{
	packet.time = this->frameIndex * this->frames2Seconds;
	while (true)
	{
		// get packet
		const vpx_codec_cx_pkt_t* vpxPacket;
		while ((vpxPacket = vpx_codec_get_cx_data(&this->context, &this->iter)) != NULL)
		{
			if (vpxPacket->kind == VPX_CODEC_STATS_PKT)
			{
				packet.type = Packet::NORMAL;
				packet.data = vpxPacket->data.twopass_stats.buf;
				packet.size = vpxPacket->data.twopass_stats.sz;
				return;
			}
			else if (vpxPacket->kind == VPX_CODEC_CX_FRAME_PKT)
			{
				packet.type = (vpxPacket->data.frame.flags & VPX_FRAME_IS_KEY) != 0 ? Packet::KEY : Packet::NORMAL;
				packet.data = vpxPacket->data.frame.buf;
				packet.size = vpxPacket->data.frame.sz;
				return;
			}
		}
		this->iter = NULL;
		
		// increment frame index
		++this->frameIndex;
		packet.time = this->frameIndex * this->frames2Seconds;

		// get input image
		VideoIn::Plane planes[3];
		if (!this->input->read(planes))
		{
			// end of frames: return null frame to indicate end
			packet.type = Packet::END;
			return;
		}

		// set vpx image
		vpx_image_t image;
		memset(&image, 0, sizeof(image));
		image.fmt = VPX_IMG_FMT_I420;
		image.d_w = this->width;
		image.d_h = this->height;
		image.x_chroma_shift = 1;
		image.y_chroma_shift = 1;
		image.planes[0] = (unsigned char*)planes[0].data;
		image.planes[1] = (unsigned char*)planes[1].data;
		image.planes[2] = (unsigned char*)planes[2].data;
		image.stride[0] = planes[0].width;
		image.stride[1] = planes[1].width;
		image.stride[2] = planes[2].width;
		
		// encode image (ip is NULL to indicate end of steam)
		vpx_codec_pts_t frameStart = vpx_codec_pts_t(floor(this->frameIndex * this->frames2Seconds * 1000.0 + 0.5));
		vpx_codec_pts_t frameEnd = vpx_codec_pts_t(floor((this->frameIndex + 1) * this->frames2Seconds * 1000.0 + 0.5));
		vpx_codec_encode(&this->context, &image, frameStart, uint(frameEnd - frameStart), 0,
			this->twopass ? VPX_DL_BEST_QUALITY : 0);
	}
}

void VPXEncoder::close()
{
	this->input->close();
}

} // namespace digi
