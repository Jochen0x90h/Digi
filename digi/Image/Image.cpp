#include <digi/Utility/malloc16.h>

#include "Image.h"


namespace digi {


// Image

Image::Image(Type type, ImageFormat format,
	int width, int height, int depth, int numMipmaps, int numImages)
	: type(type), format(format),
	size(vector3(width, height, depth)), numMipmaps(numMipmaps), numImages(numImages)
{
	size_t memSize = Image::calcMemorySize(format, this->size, numMipmaps, numImages);
	this->data = malloc16(memSize);
	memset(this->data, 0, memSize);
}

Image::Image(Type type, ImageFormat format,
	int3 size, int numMipmaps, int numImages)
	: type(type), format(format),
	size(size), numMipmaps(numMipmaps), numImages(numImages)
{
	size_t memSize = Image::calcMemorySize(format, size, numMipmaps, numImages);
	this->data = malloc16(memSize);
	memset(this->data, 0, memSize);
}

Image::~Image()
{
	free16(this->data);
}

int Image::calcNumMipmaps(int3 size)
{
	int dim = max(size);
	int i = 1;
	while (dim > 1)
	{
		++i;
		dim >>= 1;
	}
	return i;
}

size_t Image::getImageNumPixels()
{
	return Image::calcNumPixels(this->size, this->numMipmaps);
}

size_t Image::getNumPixels()
{
	return Image::calcNumPixels(this->size, this->numMipmaps, this->numImages);
}

size_t Image::calcNumPixels(int3 size, int numMipmaps, int numImages)
{
	size_t numPixels = 0;
	for (int i = 0; i < numMipmaps; ++i)
	{
		numPixels += Image::calcNumPixels(size);
		size = max(size >> 1, 1);
	}
	return numPixels * numImages;
}

size_t Image::getMipmapMemorySize(int mipmapIndex)
{
	return Image::calcMemorySize(this->format, max(this->size >> mipmapIndex, 1), 1);	
}

size_t Image::getImageMemorySize()
{
	return Image::calcMemorySize(this->format, this->size, this->numMipmaps);
}

size_t Image::getMemorySize()
{
	return Image::calcMemorySize(this->format, this->size, this->numMipmaps, this->numImages);
}

size_t Image::calcMemorySize(ImageFormat format, int3 size, int numMipmaps, int numImages)
{
	const ImageFormat::BlockInfo& info = format.getBlockInfo();
	
	size_t numBlocks = 0;
	for (int i = 0; i < numMipmaps; ++i)
	{
		// calc number of blocks in all 3 dimensions
		int3 b = max((size + info.blockSize - 1) / info.blockSize, info.minNumBlocks);
		numBlocks += Image::calcNumPixels(b);
		size = max(size >> 1, 1);
	}
	
	size_t numBytes = size_t(numImages) * numBlocks * format.getMemorySize();
	return numBytes;
}

size_t Image::getOffset(int imageIndex)
{
	// get size of complete images
	return Image::calcMemorySize(this->format, this->size, this->numMipmaps, imageIndex);
}

size_t Image::getOffset(int x, int y, int z, int mipmapIndex, int imageIndex)
{
	// calc size of complete images
	size_t offset = Image::calcMemorySize(this->format, this->size, this->numMipmaps, imageIndex);

	// calc size of complete mipmaps
	const ImageFormat::BlockInfo& info = format.getBlockInfo();
	int3 size = this->size;
	size_t numBlocks = 0;
	for (int i = 0; i < mipmapIndex; ++i)
	{
		int3 b = max((size + info.blockSize - 1) / info.blockSize, info.minNumBlocks);
		numBlocks += Image::calcNumPixels(b);
		size = max(size >> 1, 1);
	}

	// add number of blocks of incomplete mipmap
	size_t bw = (size.x + info.blockSize.x - 1) / info.blockSize.x;
	size_t bh = (size.y + info.blockSize.y - 1) / info.blockSize.y;
	numBlocks += ((z / info.blockSize.z) * bh + y / info.blockSize.y) * bw + x / info.blockSize.x;
	
	return offset + numBlocks * format.getMemorySize();
}


bool Image::checkMipmaps()
{
	int dim = max(this->getSize());
	int i = 1;
	while (dim > 1)
	{
		++i;
		dim >>= 1;
	}
	return i == this->numMipmaps;
}


} // namespace digi
