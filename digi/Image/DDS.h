#ifndef digi_Image_DDS_h
#define digi_Image_DDS_h

#include <digi/System/FileSystem.h>
#include "Image.h"


namespace digi {

/// @addtogroup Image
/// @{

// load DX9 or DX10 dds file
Pointer<Image> loadDDS(const fs::path& path);

// save given mipmap and image index. if mipmapIndex is -1 then all mipmaps are saved if possible
void saveDDS(const fs::path& path, Pointer<Image> image, int mipmapIndex = -1, int imageIndex = 0);

/// @}

} // namespace digi

#endif
