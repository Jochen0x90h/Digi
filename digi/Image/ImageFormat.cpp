#include "ImageFormat.h"


namespace digi {

int ImageFormat::getMappingFlags() const
{
	int flags = 0;
	int m = this->mapping;
	while (m != 0)
	{
		int shift = m & 0xf;
		if (m != 0)
			flags |= 1 << shift - 1;
		m >>= 4;
	}
	return flags;
}

int ImageFormat::getNumMappings() const
{
	int numMappings = 0;
	int m = this->mapping;
	while (m != 0)
	{
		if (m & 0xf)
			++numMappings;
		m >>= 4;
	}
	return numMappings;
}

const ImageFormat::BlockInfo ImageFormat::blockInfos[] =
{
	// standard formats
	{{1, 1, 1}, {1, 1, 1}},

	// compressed formats
	{{4, 4, 1}, {1, 1, 1}}, // DXT1
	{{4, 4, 1}, {1, 1, 1}}, // DXT3
	{{4, 4, 1}, {1, 1, 1}}, // DXT5
	{{8, 4, 1}, {2, 2, 1}}, // PVRTC2
	{{4, 4, 1}, {2, 2, 1}}, // PVRTC4
};

} // namespace digi
