#ifndef digi_Engine_Watermark_h
#define digi_Engine_Watermark_h

#include <digi/OpenGL/GLWrapper.h>


namespace digi {

/// @addtogroup Engine
/// @{
			

class Watermark
{
public:
	
	Watermark();

	~Watermark();
	
	void render();

	GLuint texture;
	GLuint vertexBuffer;
	GLuint program;
};


/// @}

} // namespace digi

#endif
