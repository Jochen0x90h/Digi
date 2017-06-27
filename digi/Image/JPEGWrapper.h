#ifndef digi_Image_JPEGWrapper_h
#define digi_Image_JPEGWrapper_h

#include <digi/System/FileSystem.h>
#include <digi/System/IODevice.h>
#include "Image.h"


namespace digi {

/// @addtogroup Image
/// @{

// load image
Pointer<Image> loadJPEG(const fs::path& path);
Pointer<Image> loadJPEG(Pointer<IODevice> dev);

// save image with given mipmap and image index. quality ranges from 0 (bad) to 100 (good)
// supports Y, RGB, uint8
void saveJPEG(const fs::path& path, Pointer<Image> image, int quality, int mipmapIndex = 0, int imageIndex = 0);
void saveJPEG(Pointer<IODevice> dev, Pointer<Image> image, int quality, int mipmapIndex = 0, int imageIndex = 0);

/// @}

} // namespace digi

#endif
