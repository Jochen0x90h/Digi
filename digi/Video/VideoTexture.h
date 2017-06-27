#ifndef digi_Video_VideoTexture_h
#define digi_Video_VideoTexture_h

#include <digi/OpenGL/GLWrapper.h>

#include "VideoOut.h"


namespace digi {

/// @addtogroup Video
/// @{

class VideoTexture : public VideoOut
{
public:

	VideoTexture(VideoFormat format);

	virtual ~VideoTexture();

	virtual VideoFormat getFormat();

	virtual void close();
	
	virtual void write(const Plane* planes);
	
	GLuint getTexture() {return this->texture;}
	
protected:

	VideoFormat format;

	struct PlaneInfo
	{
		GLuint texture;
		int width;
		int height;
		GLuint scale;
		
		PlaneInfo()
			: texture(), width(), height(), scale() {}
	};
	PlaneInfo planeInfos[3];
	
	GLuint texture;
	GLuint fbo;
	
	GLuint quadBuffer;
	GLuint quadArray;
	GLuint yuvCompositor;
};

/// @}

} // namespace digi

#endif
