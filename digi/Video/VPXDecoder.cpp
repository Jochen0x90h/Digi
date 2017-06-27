#include <digi/Utility/VectorUtility.h>

#include "VPXDecoder.h"


namespace digi {
	
VPXDecoder::VPXDecoder(int width, int height, float framesPerSecond)
	: width(width), height(height), framesPerSecond(framesPerSecond)
{
	int flags = 0;
	vpx_codec_dec_init(&this->context, &vpx_codec_vp8_dx_algo, NULL, flags);
}

VPXDecoder::~VPXDecoder()
{
	vpx_codec_destroy(&this->context);
}

void VPXDecoder::decode(double time, const uint8_t* data, size_t length)
{
	vpx_codec_decode(&this->context, data, length, NULL, 0);
	
	vpx_codec_iter_t iter = NULL;
	vpx_image_t* image = vpx_codec_get_frame(&this->context, &iter);
	if (image != NULL)
	{
		VideoOut::Plane planes[3];
		planes[0].data = image->planes[0];
		planes[0].width = image->stride[0];
		planes[0].height = image->d_h;

		//int xs = image->x_chroma_shift;
		int ys = image->y_chroma_shift;
		planes[1].data = image->planes[1];
		planes[2].data = image->planes[2];
		planes[1].width = image->stride[1];
		planes[2].width = image->stride[2];
		planes[1].height = planes[2].height = (image->d_h + (1 << ys) - 1) >> ys;
		
		this->output->write(planes);
	}
}

void VPXDecoder::close()
{
	this->output->close();
}

} // namespace digi
