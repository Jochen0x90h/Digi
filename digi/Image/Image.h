#ifndef digi_Image_Image
#define digi_Image_Image

#include <string.h>

#include <digi/Utility/Object.h>
#include "ImageFormat.h"


namespace digi {

/// @addtogroup Image
/// @{

/// image or volume of format defined by ImageFormat
class Image : public Object
{
public:

	enum Type
	{
		TYPE_INVALID,
		
		// 2D image
		IMAGE,
		
		// volume
		VOLUME,
		
		// cube map. numImages is n*6 for n cube maps
		CUBEMAP
	};
	
	Image(Type type, ImageFormat format,
		int width, int height, int depth = 1, int numMipmaps = 1, int numImages = 1);

	Image(Type type, ImageFormat format,
		int3 size, int numMipmaps = 1, int numImages = 1);

	~Image();
	
	/// get type of image
	Type getType() {return this->type;}
	
	/// get format of image
	const ImageFormat& getFormat() {return this->format;}

	/// get width
	int getWidth() {return this->size.x;}

	/// get height
	int getHeight() {return this->size.y;}

	/// get depth
	int getDepth() {return this->size.z;}

	/// get width/height/depth
	int3 getSize() {return this->size;}

	// get number of mipmaps
	int getNumMipmaps() {return this->numMipmaps;}

	// get number of mipmaps for given width, height and depth
	static int calcNumMipmaps(int3 size);

	/// get number of images
	int getNumImages() {return this->numImages;}


	/// get number of pixels of one image, including mipmaps
	size_t getImageNumPixels();
	
	/// get number of pixels of all images, including mipmaps
	size_t getNumPixels();

	// calc number of pixels for given size
	static size_t calcNumPixels(int3 size) {return size_t(size.x) * size.y * size.z;}

	// calc number of pixels for given parameters
	static size_t calcNumPixels(int3 size, int numMipmaps, int numImages = 1);


	/// get size of one mipmap level
	size_t getMipmapMemorySize(int mipmapIndex);
	
	/// get size of one image in bytes, including mipmaps
	size_t getImageMemorySize();
	
	/// get size of all images in bytes, including mipmaps
	size_t getMemorySize();

	/// calc memory size for given parameters
	static size_t calcMemorySize(ImageFormat format, int3 size, int numMipmaps = 1, int numImages = 1);


	// get offset of an image
	size_t getOffset(int imageIndex);
	
	// get offset of a pixel
	size_t getOffset(int x, int y, int z, int mipmapIndex, int imageIndex);


	/// set data (warning: no type checking)
	void setData(const void* data) {memcpy(this->data, data, this->getMemorySize());}

	/// get data as pointer of given type (warning: no type checking)
	template <typename T>
	T* getData()
	{
		return (T*)this->data;
	}
	
	/// get data as pointer of given type (warning: no type checking)
	template <typename T>
	T* getData(int imageIndex)
	{
		return (T*)((uint8_t*)this->data + this->getOffset(imageIndex));
	}

	/// get data as pointer of given type (warning: no type checking)
	template <typename T>
	T* getData(int mipmapIndex, int imageIndex)
	{
		return (T*)((uint8_t*)this->data + this->getOffset(0, 0, 0, mipmapIndex, imageIndex));
	}
	
	// return true if the mipmap stack is complete
	bool checkMipmaps();

protected:

	Type type;

	// format of image
	ImageFormat format;
	
	// size of image (width, height, depth)
	int3 size;
	
	// int numMipmaps
	int numMipmaps;
	
	// number of images (for cube textures this counts the number of cube faces, i.e. multiple of 6)
	int numImages;
		
	// data
	void* data;
};


/// named image
struct NamedImage
{
	// name of image
	std::string name;

	// the image
	Pointer<Image> image;
	
	NamedImage() {}
	
	NamedImage(const std::string& name, Pointer<Image> image)
		: name(name), image(image) {}
};

/// @}

} // namespace digi

#endif
