set(Image_DEPENDENCIES Data Math)

# find dependencies
find_package(zlib NO_MODULE)
find_package(png NO_MODULE)
find_package(imagequant NO_MODULE)
find_package(jpeg NO_MODULE)
find_package(tiff NO_MODULE)
find_package(webp NO_MODULE)

# list all packages that Image depends on
set(Image_PACKAGES png imagequant jpeg tiff webp)

set(Image_INCLUDE_DIRS
	${ZLIB_INCLUDE_DIRS}
	${PNG_INCLUDE_DIRS}
	${IMAGEQUANT_INCLUDE_DIRS}
	${JPEG_INCLUDE_DIRS}
	${TIFF_INCLUDE_DIRS}
	${WEBP_INCLUDE_DIRS}
)

set(Image_DEFINITIONS
	${ZLIB_DEFINITIONS}
	${PNG_DEFINITIONS}
	${IMAGEQUANT_DEFINITIONS}
	${JPEG_DEFINITIONS}
	${TIFF_DEFINITIONS}
	${WEBP_DEFINITIONS}
)

set(Image_LIBRARIES
	${ZLIB_LIBRARIES}
	${PNG_LIBRARIES}
	${IMAGEQUANT_LIBRARIES}
	${JPEG_LIBRARIES}
	${TIFF_LIBRARIES}
	${WEBP_LIBRARIES}
) 

set(Image_HAS_INIT_DONE YES)
