set(ImageIO_DEPENDENCIES Image)

# find dependencies
find_package(freeimage)

set(ImageIO_PACKAGES freeimage)
set(ImageIO_INCLUDE_DIRS ${FREEIMAGE_INCLUDE_DIRS})
set(ImageIO_DEFINITIONS ${FREEIMAGE_DEFINITIONS})
set(ImageIO_LIBRARIES ${FREEIMAGE_LIBRARIES}) 

set(ImageIO_HAS_INIT_DONE YES)
