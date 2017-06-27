#ifndef digi_Image_TIFFWrapper_h
#define digi_Image_TIFFWrapper_h

#include <digi/System/FileSystem.h>
#include <digi/System/IODevice.h>
#include "Image.h"


namespace digi {

/// @addtogroup Image
/// @{

// load image
Pointer<Image> loadTIFF(const fs::path& path);
Pointer<Image> loadTIFF(Pointer<IODevice> dev);

// save image with given mipmap and image index
// supports Y, YA, RGB, RGBA, uint8, uint16, float32
void saveTIFF(const fs::path& path, Pointer<Image> image, int mipmapIndex = 0, int imageIndex = 0);
void saveTIFF(Pointer<IODevice> dev, Pointer<Image> image, int mipmapIndex = 0, int imageIndex = 0);

/// @}

} // namespace digi

#endif
