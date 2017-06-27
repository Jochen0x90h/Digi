#ifndef digi_ImageIO_ImageIO_h
#define digi_ImageIO_ImageIO_h

#include <string>

#include <digi/System/FileSystem.h>
#include <digi/Image/Image.h>


namespace digi {

/// @addtogroup ImageIO
/// @{

// load an image. the first pixel is the upper left corner. uses FreeImage to load many formats
Pointer<Image> loadImage(const fs::path& path);

// save given mipmap and image index. if mipmapIndex is -1 then all mipmaps are saved if possible
void saveImage(const fs::path& path, Pointer<Image> image, int mipmapIndex = -1, int imageIndex = 0);
	
/// @}

} // namespace digi

#endif
