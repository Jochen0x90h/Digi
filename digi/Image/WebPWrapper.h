#ifndef digi_Image_WebPWrapper_h
#define digi_Image_WebPWrapper_h

#include <digi/System/FileSystem.h>
#include <digi/System/IODevice.h>
#include "Image.h"


namespace digi {

/// @addtogroup Image
/// @{

// load image
Pointer<Image> loadWebP(const fs::path& path);
Pointer<Image> loadWebP(Pointer<IODevice> dev);

// save image with given mipmap and image index. quality ranges from 0 (bad) to 100 (good)
// supports Y, YA, RGB, RGBA, uint8
void saveWebP(const fs::path& path, Pointer<Image> image, int quality, int mipmapIndex = 0, int imageIndex = 0);
void saveWebP(Pointer<IODevice> dev, Pointer<Image> image, int quality, int mipmapIndex = 0, int imageIndex = 0);

/// @}

} // namespace digi

#endif
