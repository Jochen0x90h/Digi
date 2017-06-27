#ifndef digi_Image_ImageFormat_h
#define digi_Image_ImageFormat_h

#include "BufferFormat.h"


namespace digi {

/// @addtogroup Image
/// @{


/// ImageFormat is a BufferFormat and additionally assigns a meaning to each component,
/// e.g. a color, alpha or luminance
struct ImageFormat : public BufferFormat
{
	/// component mapping
	// note: keep synchronized with mappingInfos in ImageFormat.cpp
	enum Mapping
	{
		INVALID_MAPPING = 0,
		
		// red
		R = 1,

		// green
		G = 2,

		// blue
		B = 3,

		// alpha
		A = 4,

		// luminance
		Y = 5,
		
		// combinations
		RG = R | G << 4,
		RGB = R | G << 4 | B << 8,
		RGBA = R | G << 4 | B << 8 | A << 12,
		BGR = B | G << 4 | R << 8,
		BGRA = B | G << 4 | R << 8 | A << 12,
		YA = Y | A << 4,

		// luminance, chrominance
		//YUV,
		
		// cyan, magenta, yellow, black
		//CMYK,

		// compressed formats
		DXT1 = 0x10000, // 4bpp, RGB + alpha bit, layout must be BLOCK8
		DXT3 = 0x10001, // 8bpp, RGBA, layout must be BLOCK16
		DXT5 = 0x10002, // 8bpp, RGBA, layout must be BLOCK16

		PVRTC2 = 0x10003, // 2bpp, RGBA, layout must be BLOCK8
		PVRTC4 = 0x10004, // 4bpp, RGBA, layout must be BLOCK8
	};

	enum MappingFlags
	{
		R_FLAG = 1,
		G_FLAG = 2,
		B_FLAG = 4,
		A_FLAG = 8,
		Y_FLAG = 16,
	};

	Mapping mapping;


	ImageFormat()
		: mapping(INVALID_MAPPING) {}

	ImageFormat(BufferFormat dataFormat, Mapping mapping)
		: BufferFormat(dataFormat), mapping(mapping) {}

	ImageFormat(Layout layout, Type type, Mapping mapping)
		: BufferFormat(layout, type), mapping(mapping) {}

	ImageFormat(Layout baseLayout, int numChannels, Type type, Mapping mapping)
		: BufferFormat(baseLayout, numChannels, type), mapping(mapping) {}


	/// returns combination of mapping flags (valid if not compressed)
	int getMappingFlags() const;
	
	/// returns number of mappings (valid if not compressed)
	int getNumMappings() const;
	
	/// get mapping for a channel (valid if not compressed)
	int getMapping(int channelIndex) const {return (this->mapping >> channelIndex * 4) & 0xf;}

	/// returns true if this is a compressed format
	bool isCompressed() const {return (this->mapping & 0x10000) != 0;}


	struct BlockInfo
	{
		// dimensions of data block (e.g. 4x4x1 for DXT)
		int3 blockSize;

		// minimum number of blocks (e.g. 2x2x1 for PVRTC)
		int3 minNumBlocks;
	};

	// get block info
	const BlockInfo& getBlockInfo() const
	{
		return ImageFormat::blockInfos[std::max(this->mapping - 0xffff, 0)];
	}
	
	static const BlockInfo blockInfos[];
};


static inline bool operator ==(const ImageFormat& a, const ImageFormat& b)
{
	return a.layout == b.layout && a.type == b.type && a.mapping == b.mapping;
};

static inline bool operator !=(const ImageFormat& a, const ImageFormat& b)
{
	return a.layout != b.layout || a.type != b.type || a.mapping != b.mapping;
};

static inline bool operator <(const ImageFormat& a, const ImageFormat& b)
{
	return a.layout < b.layout || (a.layout == b.layout && (a.type < b.type || (a.type == b.type && a.mapping < b.mapping)));
};


/// @}
	
} // namespace digi

#endif
