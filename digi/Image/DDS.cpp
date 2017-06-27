#include <digi/Utility/ArrayUtility.h>
#include <digi/Data/DataException.h>
#include <digi/Data/LittleEndianReader.h>
#include <digi/Data/LittleEndianWriter.h>
#include <digi/Image/Image.h>

#include "DDS.h"


namespace digi {

namespace
{
	uint32_t makeFourCC(char a, char b, char c, char d)
	{
		return ((uint32_t)(uint8_t)(a) | ((uint32_t)(uint8_t)(b) << 8) | ((uint32_t)(uint8_t)(c) << 16) | ((uint32_t)(uint8_t)(d) << 24));
	}

	/*
		DDS format

		int32 Magic (0x20534444, or "DDS ")
		DDSHeader ddsd (provides information about the surface format)
		BYTE bData1[] (surface data for the main surface)
		[BYTE bData2[]...] (surface data for attached surfaces, if any, follows)
	*/

	struct DDSPixelFormat
	{
		enum Flags
		{
			ALPHAPIXELS = 0x00000001,
			FOURCC = 0x00000004,
			RGB = 0x00000040
		};

		/*
			Size of structure. This member must be set to 32.
		*/
		int32_t size;

		/*
			Flags to indicate valid fields. Uncompressed formats will
			usually use DDPF_RGB to indicate an RGB format, while 
			compressed formats will use DDPF_FOURCC with a 
			four-character code.
		*/
		int32_t flags;

		/*
			This is the four-character code for compressed formats. 
			flags should include DDPF_FOURCC in this case. For 
			DXTn compression, this is set to "DXT1", "DXT2", "DXT3", 
			"DXT4", or "DXT5".		
		*/
		uint32_t fourCC;

		/*
			For RGB formats, this is the total number of bits in the
			format. flags should include DDPF_RGB in this case. 
			This value is usually 16, 24, or 32. For A8R8G8B8, this 
			value would be 32.
		*/
		int32_t rgbBitCount;
		
		/*
			For RGB formats, these three fields contain the masks for
			the red, green, and blue channels. For A8R8G8B8, these 
			values would be 0x00ff0000, 0x0000ff00, and 0x000000ff 
			respectively
		*/
		uint32_t rBitMask;
		uint32_t gBitMask;
		uint32_t bBitMask;
		
		/*
			For RGB formats, this contains the mask for the alpha 
			channel, if any. flags should include DDPF_ALPHAPIXELS
			in this case. For A8R8G8B8, this value would be 
			0xff000000.
		*/
		uint32_t aBitMask;
	};


	struct DDSHeader
	{
		enum Flags
		{
			CAPS = 0x00000001,
			HEIGHT = 0x00000002,
			WIDTH = 0x00000004,
			PITCH = 0x00000008,
			PIXELFORMAT = 0x00001000,
			MIPMAPCOUNT = 0x00020000,
			LINEARSIZE = 0x00080000,
			DEPTH = 0x00800000
		};

		enum Caps1
		{
			COMPLEX = 0x00000008,
			TEXTURE = 0x00001000,
			MIPMAP = 0x00400000
		};

		enum Caps2
		{
			CUBEMAP = 0x00000200,
			CUBEMAP_POSITIVEX = 0x00000400,
			CUBEMAP_NEGATIVEX = 0x00000800,
			CUBEMAP_POSITIVEY = 0x00001000,
			CUBEMAP_NEGATIVEY = 0x00002000,
			CUBEMAP_POSITIVEZ = 0x00004000,
			CUBEMAP_NEGATIVEZ = 0x00008000,
			VOLUME = 0x00200000
		};
		
		/*
			Size of structure. This is 124 for DX9 and 144 for DX10
		*/
		int32_t size; 

		/*
			Flags to indicate valid fields. A combination of members of the Flags enum
		*/
		int32_t flags; 
		
		/*
			Height of the main image in pixels
		*/
		int32_t height; 
		
		/*
			Width of the main image in pixels
		*/
		int32_t width; 

		/*
			For uncompressed formats, this is the number of bytes per scan 
			line (int32-aligned) for the main image. flags should include 
			PITCH in this case.
			For compressed formats, this is the total number of bytes 
			for the main image. flags should be include 
			LINEARSIZE in this case
		*/
		int32_t pitchOrLinearSize; 
		
		/*
			For volume textures, this is the depth of the volume. flags 
			should include DEPTH in this case
		*/
		int32_t depth; 
		
		/*
			For items with mipmap levels, this is the total number of 
			levels in the mipmap chain of the main image. flags 
			should include MIPMAPCOUNT in this case.
		*/
		int32_t mipmapCount;
		
		int32_t reserved[11];
		
		DDSPixelFormat pixelFormat;
		
		/*	
			DDS files should always include TEXTURE. If the
			file contains mipmaps, MIPMAP should be set. 
			For any DDS file with more than one main surface, such 
			as a mipmaps, cubic environment map, or volume texture, 
			COMPLEX should also be set.
		*/
		int32_t caps1;
		
		/*
			For cubic environment maps, DDSCAPS2_CUBEMAP should be 
			included as well as one or more faces of the map 
			(DDSCAPS2_CUBEMAP_POSITIVEX, DDSCAPS2_CUBEMAP_NEGATIVEX, 
			DDSCAPS2_CUBEMAP_POSITIVEY, DDSCAPS2_CUBEMAP_NEGATIVEY, 
			DDSCAPS2_CUBEMAP_POSITIVEZ, DDSCAPS2_CUBEMAP_NEGATIVEZ).
			For volume textures, DDSCAPS2_VOLUME should be included.		
		*/
		int32_t caps2;

		int32_t caps3;

		int32_t caps4;
		
		int32_t reserved2;
		
		// dx10 header
		int32_t dxgiFormat;
		int32_t resourceDimension;
		int32_t miscFlag;
		int32_t arraySize;
		int32_t reserved3; 
	};


	// DX10 formats
	enum DXGI_FORMAT
	{
		DXGI_FORMAT_UNKNOWN = 0,
		
		DXGI_FORMAT_R32G32B32A32_TYPELESS = 1,
		DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
		DXGI_FORMAT_R32G32B32A32_UINT = 3,
		DXGI_FORMAT_R32G32B32A32_SINT = 4,
		
		DXGI_FORMAT_R32G32B32_TYPELESS = 5,
		DXGI_FORMAT_R32G32B32_FLOAT = 6,
		DXGI_FORMAT_R32G32B32_UINT = 7,
		DXGI_FORMAT_R32G32B32_SINT = 8,
		
		DXGI_FORMAT_R16G16B16A16_TYPELESS = 9,
		DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
		DXGI_FORMAT_R16G16B16A16_UNORM = 11,
		DXGI_FORMAT_R16G16B16A16_UINT = 12,
		DXGI_FORMAT_R16G16B16A16_SNORM = 13,
		DXGI_FORMAT_R16G16B16A16_SINT = 14,
		
		DXGI_FORMAT_R32G32_TYPELESS = 15,
		DXGI_FORMAT_R32G32_FLOAT = 16,
		DXGI_FORMAT_R32G32_UINT = 17,
		DXGI_FORMAT_R32G32_SINT = 18,
		
		DXGI_FORMAT_R32G8X24_TYPELESS = 19,
		DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 20,
		DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
		DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
		
		DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
		DXGI_FORMAT_R10G10B10A2_UNORM = 24,
		DXGI_FORMAT_R10G10B10A2_UINT = 25,
		
		DXGI_FORMAT_R11G11B10_FLOAT = 26,
		
		DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
		DXGI_FORMAT_R8G8B8A8_UNORM = 28,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
		DXGI_FORMAT_R8G8B8A8_UINT = 30,
		DXGI_FORMAT_R8G8B8A8_SNORM = 31,
		DXGI_FORMAT_R8G8B8A8_SINT = 32,
		
		DXGI_FORMAT_R16G16_TYPELESS = 33,
		DXGI_FORMAT_R16G16_FLOAT = 34,
		DXGI_FORMAT_R16G16_UNORM = 35,
		DXGI_FORMAT_R16G16_UINT = 36,
		DXGI_FORMAT_R16G16_SNORM = 37,
		DXGI_FORMAT_R16G16_SINT = 38,
		
		DXGI_FORMAT_R32_TYPELESS = 39,
		DXGI_FORMAT_D32_FLOAT = 40,
		DXGI_FORMAT_R32_FLOAT = 41,
		DXGI_FORMAT_R32_UINT = 42,
		DXGI_FORMAT_R32_SINT = 43,
		
		DXGI_FORMAT_R24G8_TYPELESS = 44,
		DXGI_FORMAT_D24_UNORM_S8_UINT = 45,
		DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 46,
		DXGI_FORMAT_X24_TYPELESS_G8_UINT = 47,
		
		DXGI_FORMAT_R8G8_TYPELESS = 48,
		DXGI_FORMAT_R8G8_UNORM = 49,
		DXGI_FORMAT_R8G8_UINT = 50,
		DXGI_FORMAT_R8G8_SNORM = 51,
		DXGI_FORMAT_R8G8_SINT = 52,
		
		DXGI_FORMAT_R16_TYPELESS = 53,
		DXGI_FORMAT_R16_FLOAT = 54,
		DXGI_FORMAT_D16_UNORM = 55,
		DXGI_FORMAT_R16_UNORM = 56,
		DXGI_FORMAT_R16_UINT = 57,
		DXGI_FORMAT_R16_SNORM = 58,
		DXGI_FORMAT_R16_SINT = 59,
		
		DXGI_FORMAT_R8_TYPELESS = 60,
		DXGI_FORMAT_R8_UNORM = 61,
		DXGI_FORMAT_R8_UINT = 62,
		DXGI_FORMAT_R8_SNORM = 63,
		DXGI_FORMAT_R8_SINT = 64,
		DXGI_FORMAT_A8_UNORM = 65,
		
		DXGI_FORMAT_R1_UNORM = 66,
		
		DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 67,
		
		DXGI_FORMAT_R8G8_B8G8_UNORM = 68,
		DXGI_FORMAT_G8R8_G8B8_UNORM = 69,
		
		DXGI_FORMAT_BC1_TYPELESS = 70,
		DXGI_FORMAT_BC1_UNORM = 71,
		DXGI_FORMAT_BC1_UNORM_SRGB = 72,
		
		DXGI_FORMAT_BC2_TYPELESS = 73,
		DXGI_FORMAT_BC2_UNORM = 74,
		DXGI_FORMAT_BC2_UNORM_SRGB = 75,
		
		DXGI_FORMAT_BC3_TYPELESS = 76,
		DXGI_FORMAT_BC3_UNORM = 77,
		DXGI_FORMAT_BC3_UNORM_SRGB = 78,
		
		DXGI_FORMAT_BC4_TYPELESS = 79,
		DXGI_FORMAT_BC4_UNORM = 80,
		DXGI_FORMAT_BC4_SNORM = 81,
		
		DXGI_FORMAT_BC5_TYPELESS = 82,
		DXGI_FORMAT_BC5_UNORM = 83,
		DXGI_FORMAT_BC5_SNORM = 84,
		
		DXGI_FORMAT_B5G6R5_UNORM = 85,
		DXGI_FORMAT_B5G5R5A1_UNORM = 86,
		DXGI_FORMAT_B8G8R8A8_UNORM = 87,
		DXGI_FORMAT_B8G8R8X8_UNORM = 88,
	}; 
} // anonymous namespace

		
Pointer<Image> loadDDS(const fs::path& path)
{
	LittleEndianReader r(path);
	uint32_t magic = r.read<uint32_t>();
	if (magic != makeFourCC('D','D','S',' ')) 
		throw DataException(r.getDevice(), DataException::FORMAT_ERROR);

	// header
	DDSHeader header;
	header.size = r.read<int32_t>();
	header.flags = r.read<int32_t>();
	header.height = r.read<int32_t>();
	header.width = r.read<int32_t>();
	header.pitchOrLinearSize = r.read<int32_t>();
	header.depth = r.read<int32_t>();
	header.mipmapCount = r.read<int32_t>();
	r.read<uint32_t>(header.reserved, 11);

	// pixelFormat
	DDSPixelFormat& pixelFormat = header.pixelFormat;
	pixelFormat.size = r.read<int32_t>();
	pixelFormat.flags = r.read<int32_t>();
	pixelFormat.fourCC = r.read<int32_t>();
	pixelFormat.rgbBitCount = r.read<int32_t>();
	pixelFormat.rBitMask = r.read<int32_t>();
	pixelFormat.gBitMask = r.read<int32_t>();
	pixelFormat.bBitMask = r.read<int32_t>();
	pixelFormat.aBitMask = r.read<int32_t>();

	// caps
	header.caps1 = r.read<int32_t>();
	header.caps2 = r.read<int32_t>();
	header.caps3 = r.read<int32_t>();
	header.caps4 = r.read<int32_t>();

	// header
	header.reserved2 = r.read<int32_t>();

		
	// check if texture
	if ((header.caps1 & DDSHeader::TEXTURE) == 0)
		throw DataException(r.getDevice(), DataException::FORMAT_NOT_SUPPORTED);

	// format
	Image::Type imageType = Image::IMAGE;
	ImageFormat format;
	int numImages = 1;
		
	// dimensions
	int width = header.width;
	int height = header.height;
	int depth = 1;

	// check for cube map
	if ((header.caps2 & DDSHeader::CUBEMAP) != 0)
	{
		imageType = Image::CUBEMAP;
		numImages = 6;
	}
		
	// check for volume texture
	if ((header.caps2 & DDSHeader::VOLUME) != 0)
	{
		imageType = Image::VOLUME;
		depth = header.depth;
	}

	// number of mipmaps
	int numMipmaps = (header.caps1 & DDSHeader::MIPMAP) != 0 ? header.mipmapCount : 1;

	// get pixel format
	if ((pixelFormat.flags & DDSPixelFormat::RGB) != 0)
	{
		// RGB pixel format: search for the matching format	
		int n = pixelFormat.rgbBitCount;
		uint32_t r = pixelFormat.rBitMask;
		uint32_t g = pixelFormat.gBitMask;
		uint32_t b = pixelFormat.bBitMask;
		//uint32_t a = pixelFormat.aBitMask;
			
		ImageFormat::Mapping rgb = (pixelFormat.flags & DDSPixelFormat::ALPHAPIXELS) != 0 ? ImageFormat::RGBA : ImageFormat::RGB;
		ImageFormat::Mapping bgr = (pixelFormat.flags & DDSPixelFormat::ALPHAPIXELS) != 0 ? ImageFormat::BGRA : ImageFormat::BGR;
			
		// 16 bit
		if (n == 16 && r == 0x0f00 && g == 0x00f0 && b == 0x000f)
			format = ImageFormat(ImageFormat::X4Y4Z4W4, ImageFormat::UNORM, bgr); // B4G4R4(A4)
		else if (n == 16 && r == 0x7c00 && g == 0x03e0 && b == 0x001f)
			format = ImageFormat(ImageFormat::X5Y5Z5W1, ImageFormat::UNORM, bgr); // B5G5R5(A1)
		else if (n == 16 && r == 0xf800 && g == 0x07e0 && b ==  0x001f)
			format = ImageFormat(ImageFormat::X5Y6Z5, ImageFormat::UNORM, ImageFormat::BGR); // B5G6R5

		// 24 bit
		else if (n == 24 && r == 0xff0000 && g == 0x00ff00 && b == 0x0000ff)
			format = ImageFormat(ImageFormat::XYZ8, ImageFormat::UNORM, ImageFormat::RGB); // RGB8
		else if (n == 24 && r == 0xff0000 && g == 0x00ff00 && b == 0x0000ff)
			format = ImageFormat(ImageFormat::XYZ8, ImageFormat::UNORM, ImageFormat::BGR); // BGR8

		// 32 bit
		else if (n == 32 && r == 0x000000ff && g == 0x0000ff00 && b == 0x00ff0000)
			format = ImageFormat(ImageFormat::XYZW8, ImageFormat::UNORM, rgb); // RGB(A8)
		else if (n == 32 && r == 0x00ff0000 && g == 0x0000ff00 && b == 0x000000ff)
			format = ImageFormat(ImageFormat::XYZW8, ImageFormat::UNORM, bgr); // BGR(A8)
		else if (n == 32 && r == 0x000003ff && g == 0x000ffc00 && b == 0x3ff00000)
			format = ImageFormat(ImageFormat::X10Y10Z10W2, ImageFormat::UNORM, rgb); // R10G10B10(A2)
		else if (n == 32 && r == 0x0000ffff && g == 0xffff0000 && b == 0x00000000)
			format = ImageFormat(ImageFormat::XY16, ImageFormat::UNORM, ImageFormat::RGB); // RG16;
	}
	else if (pixelFormat.fourCC == makeFourCC('D','X','T','1'))
	{
		format = ImageFormat(ImageFormat::BLOCK8, ImageFormat::UNORM, ImageFormat::DXT1);
	}
	else if (pixelFormat.fourCC == makeFourCC('D','X','T','3'))
	{
		format = ImageFormat(ImageFormat::BLOCK16, ImageFormat::UNORM, ImageFormat::DXT3);
	}
	else if (pixelFormat.fourCC == makeFourCC('D','X','T','5'))
	{
		format = ImageFormat(ImageFormat::BLOCK16, ImageFormat::UNORM, ImageFormat::DXT5);
	}
	else if (pixelFormat.fourCC == makeFourCC('D','X','1','0'))
	{
		header.dxgiFormat = r.read<int32_t>();
		header.resourceDimension = r.read<int32_t>();
		header.miscFlag = r.read<int32_t>();
		header.arraySize = r.read<int32_t>();
		header.reserved3 = r.read<int32_t>();

		switch (header.dxgiFormat)
		{
			// 128 bit
			case DXGI_FORMAT_R32G32B32A32_FLOAT:
				format = ImageFormat(ImageFormat::XYZW32, ImageFormat::FLOAT, ImageFormat::RGBA); // R32G32B32A32
				break;
			case DXGI_FORMAT_R32G32B32A32_UINT:
				format = ImageFormat(ImageFormat::XYZW32, ImageFormat::UINT, ImageFormat::RGBA); // R32G32B32A32
				break;
			case DXGI_FORMAT_R32G32B32A32_SINT:
				format = ImageFormat(ImageFormat::XYZW32, ImageFormat::INT, ImageFormat::RGBA); // R32G32B32A32
				break;
		}
	}

	// check if format is supported
	if (format.layout == ImageFormat::INVALID_LAYOUT)
			throw DataException(r.getDevice(), DataException::FORMAT_NOT_SUPPORTED);

	// skip rest of header
	r.setPosition(4 + header.size);
		

	// create image
	Pointer<Image> image = new Image(imageType, format, width, height, depth, numMipmaps, numImages);
		
	// read data
	ImageFormat::LayoutInfo l = format.getLayoutInfo();
	ImageFormat::BlockInfo b = format.getBlockInfo();
				
	// iterate over images
	for (int imageIndex = 0; imageIndex < numImages; ++imageIndex)
	{
		// iterate over mipmaps
		int3 size = vector3(width, height, depth);
		for (int mipmapIndex = 0; mipmapIndex < numMipmaps; ++mipmapIndex)
		{
			uint8_t* data = image->getData<uint8_t>(mipmapIndex, imageIndex);
				
			// dimensions of image in blocks (e.g. DXT1 has 4x4x1 blocks)
			int3 s = (size + b.blockSize - 1) / b.blockSize;
				
			// number of components per line
			int numComponents = s.x * l.numComponents;

			// read image
			for (int z = 0; z < s.z; ++z)
			{
				for (int y = 0; y < s.y; ++y)
				{
					// read one line
					switch (l.componentSize)
					{
					case 1: // 8 bit
						r.read<uint8_t>(data, numComponents);
						r.skip(-numComponents & 3); // align 4
						data += numComponents;
						break;
					case 2: // 16 bit
						r.read<uint16_t>((uint16_t*)data, numComponents);
						r.skip(-numComponents & 1); // align 4
						data += numComponents * 2;
						break;
					case 4: // 32 bit
						r.read<uint32_t>((uint32_t*)data, numComponents);
						data += numComponents * 4;
						break;
					}
				}
			}

			// calc size of next mipmap level
			size = max(size >> 1, 1);
		}		
	}
	r.close();
		
	return image;
}


void saveDDS(const fs::path& path, Pointer<Image> image, int mipmapIndex, int imageIndex)
{
	LittleEndianWriter w(path);
	w.write<uint32_t>(makeFourCC('D','D','S',' '));

	Image::Type type = image->getType();
	ImageFormat format = image->getFormat();
		
	// mipmap range
	int mipmapStart = 0;
	int mipmapEnd = image->getNumMipmaps();
	if (mipmapIndex >= 0)
	{
		mipmapStart = mipmapIndex;
		mipmapEnd = mipmapIndex + 1;
	}
	int numMipmaps = mipmapEnd - mipmapStart;
		
	// image range
	int imageStart = imageIndex;
	int imageEnd = imageIndex + 1;

	// image size
	int3 imageSize = max(image->getSize() >> mipmapStart, 1);
			
		
	// header
	DDSHeader header;
	memset(&header, 0, sizeof(header));
	header.size = 124;
	header.flags = format.isCompressed() ? DDSHeader::LINEARSIZE : DDSHeader::PITCH;
	header.width = imageSize.x;
	header.height = imageSize.y;
	header.pitchOrLinearSize = format.isCompressed() ?
		int(Image::calcMemorySize(format, imageSize)) : imageSize.x * int(format.getMemorySize()) + 3 & ~3; 
	header.depth = imageSize.z;
	header.mipmapCount = numMipmaps > 1 ? numMipmaps : 0;

	// pixelFormat
	DDSPixelFormat& pixelFormat = header.pixelFormat;
	pixelFormat.size = 32;
	switch (format.mapping)
	{
	case ImageFormat::RGB:
		switch (format.type)
		{
		case ImageFormat::UNORM:
			switch (format.layout)
			{
			case ImageFormat::X5Y6Z5:
				pixelFormat.flags = DDSPixelFormat::RGB;
				pixelFormat.rgbBitCount = 16;
				pixelFormat.rBitMask = 0x001f;
				pixelFormat.gBitMask = 0x07e0;
				pixelFormat.bBitMask = 0xf800;
				break;
			case ImageFormat::XYZ8:
				pixelFormat.flags = DDSPixelFormat::RGB;
				pixelFormat.rgbBitCount = 24;
				pixelFormat.rBitMask = 0x0000ff;
				pixelFormat.gBitMask = 0x00ff00;
				pixelFormat.bBitMask = 0xff0000;
				break;
			case ImageFormat::XYZW8:
			case ImageFormat::X8Y8Z8W8:
				pixelFormat.flags = DDSPixelFormat::RGB;
				pixelFormat.rgbBitCount = 32;
				pixelFormat.rBitMask = 0x0000ff;
				pixelFormat.gBitMask = 0x00ff00;
				pixelFormat.bBitMask = 0xff0000;
				break;
			default:
				;
			}
			break;
		}
		break;
	case ImageFormat::BGR:
		switch (format.type)
		{
		case ImageFormat::UNORM:
			switch (format.layout)
			{
			case ImageFormat::X5Y6Z5:
				pixelFormat.flags = DDSPixelFormat::RGB;
				pixelFormat.rgbBitCount = 16;
				pixelFormat.rBitMask = 0xf800;
				pixelFormat.gBitMask = 0x07e0;
				pixelFormat.bBitMask = 0x001f;
				break;
			case ImageFormat::XYZ8:
				pixelFormat.flags = DDSPixelFormat::RGB;
				pixelFormat.rgbBitCount = 24;
				pixelFormat.rBitMask = 0xff0000;
				pixelFormat.gBitMask = 0x00ff00;
				pixelFormat.bBitMask = 0x0000ff;
				break;
			case ImageFormat::XYZW8:
			case ImageFormat::X8Y8Z8W8:
				pixelFormat.flags = DDSPixelFormat::RGB;
				pixelFormat.rgbBitCount = 32;
				pixelFormat.rBitMask = 0xff0000;
				pixelFormat.gBitMask = 0x00ff00;
				pixelFormat.bBitMask = 0x0000ff;
				break;
			default:
				;

			}
			break;
		}
		break;
	case ImageFormat::RGBA:
		switch (format.type)
		{
		case ImageFormat::UNORM:
			switch (format.layout)
			{
			case ImageFormat::X4Y4Z4W4:
				pixelFormat.flags = DDSPixelFormat::RGB | DDSPixelFormat::ALPHAPIXELS;
				pixelFormat.rgbBitCount = 16;
				pixelFormat.rBitMask = 0x000f;
				pixelFormat.gBitMask = 0x00f0;
				pixelFormat.bBitMask = 0x0f00;
				pixelFormat.aBitMask = 0xf000;
				break;
			case ImageFormat::XYZW8:
			case ImageFormat::X8Y8Z8W8:
				pixelFormat.flags = DDSPixelFormat::RGB | DDSPixelFormat::ALPHAPIXELS;
				pixelFormat.rgbBitCount = 32;
				pixelFormat.rBitMask = 0x000000ff;
				pixelFormat.gBitMask = 0x0000ff00;
				pixelFormat.bBitMask = 0x00ff0000;
				pixelFormat.aBitMask = 0xff000000;
				break;
			default:
				;					
			}
			break;
		}
		break;
	case ImageFormat::BGRA:
		switch (format.type)
		{
		case ImageFormat::UNORM:
			switch (format.layout)
			{
			case ImageFormat::X4Y4Z4W4:
				pixelFormat.flags = DDSPixelFormat::RGB | DDSPixelFormat::ALPHAPIXELS;
				pixelFormat.rgbBitCount = 16;
				pixelFormat.rBitMask = 0x0f00;
				pixelFormat.gBitMask = 0x00f0;
				pixelFormat.bBitMask = 0x000f;
				pixelFormat.aBitMask = 0xf000;
				break;
			case ImageFormat::XYZW8:
			case ImageFormat::X8Y8Z8W8:
				pixelFormat.flags = DDSPixelFormat::RGB | DDSPixelFormat::ALPHAPIXELS;
				pixelFormat.rgbBitCount = 32;
				pixelFormat.rBitMask = 0x00ff0000;
				pixelFormat.gBitMask = 0x0000ff00;
				pixelFormat.bBitMask = 0x000000ff;
				pixelFormat.aBitMask = 0xff000000;
				break;
			default:
				;
			}
			break;
		}
		break;
	case ImageFormat::DXT1:
		pixelFormat.flags = DDSPixelFormat::FOURCC;
		pixelFormat.fourCC = makeFourCC('D','X','T','1');
		break;
	case ImageFormat::DXT3:
		pixelFormat.flags = DDSPixelFormat::FOURCC;
		pixelFormat.fourCC = makeFourCC('D','X','T','3');
		break;
	case ImageFormat::DXT5:
		pixelFormat.flags = DDSPixelFormat::FOURCC;
		pixelFormat.fourCC = makeFourCC('D','X','T','5');
		break;
	default:
		;
	}
		
	// check if format is supported
	if (pixelFormat.flags == 0)
			throw DataException(w.getDevice(), DataException::FORMAT_NOT_SUPPORTED);

	// caps
	header.caps1 |= DDSHeader::TEXTURE;
	if (numMipmaps > 1)
	{
		header.caps1 |= DDSHeader::MIPMAP | DDSHeader::COMPLEX;
	}
	if (type == Image::VOLUME)
	{
		header.caps1 |= DDSHeader::COMPLEX;
		header.caps2 |= DDSHeader::VOLUME;
	}
	if (type == Image::CUBEMAP)
	{
		header.caps2 |= DDSHeader::CUBEMAP;
		header.caps2 |= 0x0000FC00;
			
		// 6 images make up one cube map
		imageEnd = imageStart + 6;
	}


	// write header
	w.write<int32_t>(header.size);
	w.write<int32_t>(header.flags);
	w.write<int32_t>(header.height);
	w.write<int32_t>(header.width);
	w.write<int32_t>(header.pitchOrLinearSize);
	w.write<int32_t>(header.depth);
	w.write<int32_t>(header.mipmapCount);
	w.write<uint32_t>(header.reserved, 11);
	w.write<int32_t>(pixelFormat.size);
	w.write<int32_t>(pixelFormat.flags);
	w.write<int32_t>(pixelFormat.fourCC);
	w.write<int32_t>(pixelFormat.rgbBitCount);
	w.write<int32_t>(pixelFormat.rBitMask);
	w.write<int32_t>(pixelFormat.gBitMask);
	w.write<int32_t>(pixelFormat.bBitMask);
	w.write<int32_t>(pixelFormat.aBitMask);
	w.write<int32_t>(header.caps1);
	w.write<int32_t>(header.caps2);
	w.write<int32_t>(header.caps3);
	w.write<int32_t>(header.caps4);
	w.write<int32_t>(header.reserved2);

	// check if this is a DX10 format
	if (header.size > 124)
	{
	}
		
	// write data
	ImageFormat::LayoutInfo l = format.getLayoutInfo();
	ImageFormat::BlockInfo b = format.getBlockInfo();
				
	// iterate over images
	for (int imageIndex = imageStart; imageIndex < imageEnd; ++imageIndex)
	{
		// iterate over mipmaps
		int3 size = imageSize;
		for (int mipmapIndex = mipmapStart; mipmapIndex < mipmapEnd; ++mipmapIndex)
		{
			uint8_t* data = image->getData<uint8_t>(mipmapIndex, imageIndex);
				
			// dimensions of image in blocks (e.g. DXT1 has 4x4x1 blocks)
			int3 s = (size + b.blockSize - 1) / b.blockSize;
				
			// number of components per line
			int numComponents = s.x * l.numComponents;

			// read image
			for (int z = 0; z < s.z; ++z)
			{
				for (int y = 0; y < s.y; ++y)
				{
					// read one line
					switch (l.componentSize)
					{
					case 1: // 8 bit
						w.write<uint8_t>(data, numComponents);
						w.skip(-numComponents & 3); // align 4
						data += numComponents;
						break;
					case 2: // 16 bit
						w.write<uint16_t>((uint16_t*)data, numComponents);
						w.skip(-numComponents & 1); // align 4
						data += numComponents * 2;
						break;
					case 4: // 32 bit
						w.write<uint32_t>((uint32_t*)data, numComponents);
						data += numComponents * 4;
						break;
					}
				}
			}

			// calc size of next mipmap level
			size = max(size >> 1, 1);
		}		
	}
	w.close();
}

} // namespace digi
