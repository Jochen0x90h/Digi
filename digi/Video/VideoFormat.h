#ifndef digi_Video_VideoFormat_h
#define digi_Video_VideoFormat_h


namespace digi {

/// @addtogroup Video
/// @{

struct VideoFormat
{
	enum Type
	{
		INVALID_TYPE = -1,
		
		/// luminance is sampled every pixel, chrominance every second pixel in x and y direction.
		/// an image is represented as three planes for y, u and v.
		YV12,
	};

	Type type;
	int width;
	int height;
	float frameRate;
	
	VideoFormat()
		: type(INVALID_TYPE), width(), height(), frameRate() {}
		
	VideoFormat(Type type, int width, int height, float frameRate)
		: type(type), width(width), height(height), frameRate(frameRate) {}	
};

/// @}

} // namespace digi

#endif
