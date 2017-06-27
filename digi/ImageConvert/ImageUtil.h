#ifndef digi_ImageConvert_ImageUtil_h
#define digi_ImageConvert_ImageUtil_h

#include <digi/Image/Image.h>

namespace digi {

/// @addtogroup Data
/// @{


/// scale an image to the given size. must be float4 format (XYZW32, FLOAT).
/// only the top-level image is processed.
Pointer<Image> scaleFiltered(Pointer<Image> srcImage, int3 size);


/// @}

} // namespace digi

#endif
