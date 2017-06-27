#ifndef digi_Image_BufferFormat_h
#define digi_Image_BufferFormat_h

#include <digi/Math/All.h>


namespace digi {

/// @addtogroup Image
/// @{


/// BufferFormat describes data with 1 to 4 components in various layouts and formats
struct BufferFormat
{
	/// layout of 1 to 4 channels
	// note: keep synchronized with layoutInfos in BufferFormat.cpp
	enum Layout
	{
		INVALID_LAYOUT = -1,
		
		// standard layouts (1 to 4 channels, each in an own basic type component)
		X8 = 0,
		XY8,
		XYZ8,
		XYZW8,
		X16,
		XY16,
		XYZ16,
		XYZW16,
		X32,
		XY32,
		XYZ32,
		XYZW32,
	
		// combined layouts (1 to 4 channels packed into one basic type component)
		X4Y4Z4W4,
		X5Y5Z5W1,
		X5Y6Z5,
		X8Y8Z8W8,
		X9Y9Z9W5, // layout for shared exponent
		X10Y10Z10W2,
		X11Y11Z10,
		
		// opaque layouts for compressed formats
		BLOCK8, // data block of 8 bytes
		BLOCK16 // data block of 16 bytes
	};
	
	/// type of the components
	enum Type
	{
		INVALID_TYPE = -1,
		
		// signed normalized integer
		NORM = 0,

		// unsigned normalized integer
		UNORM,
		
		// signed integer
		INT,
		
		// unsigned integer
		UINT,
		
		// signed float. 16 bit: s1 e5 m10 bias=15, 32 bit: s1 e8 m23 bias=127
		FLOAT, 
	
		// unsigned float. 10 bit: e5 m5 bias=15, 11 bit: e5 m6 bias=15
		UFLOAT,
				
		// shared exponent
		SHARED_EXPONENT,
	};

	Layout layout;
	Type type;
	
	BufferFormat()
		: layout(INVALID_LAYOUT), type(INVALID_TYPE) {}

	BufferFormat(Layout layout, Type type)
		: layout(layout), type(type) {}

	BufferFormat(Layout baseLayout, int numChannels, Type type)
		: layout(Layout(baseLayout + numChannels - 1)), type(type) {}


	int getID() const
	{
		return int(this->layout) | (int(this->type) << 8);
	}
	

	// return true if type is normalized (either NORM or UNORM)
	bool isNormalized() const {return this->type == NORM || this->type == UNORM;}
	
	// return true if type is integer (either INT or UINT)
	bool isInteger() const {return this->type == INT || this->type == UINT;}

	// return true if type is floating point (either FLOAT or UFLOAT)
	bool isFloating() const {return this->type == FLOAT || this->type == UFLOAT;}

	// return true if type is signed (either NORM, INT or FLOAT)
	bool isSigned() const {return this->type == NORM || this->type == INT || this->type == FLOAT;}

	// return true if this is a standard layout of 8 bit components
	bool is8Bit() const {return this->layout >= BufferFormat::X8 && this->layout <= BufferFormat::XYZW8;}

	// return true if this is a standard layout of 16 bit components
	bool is16Bit() const {return this->layout >= BufferFormat::X16 && this->layout <= BufferFormat::XYZW16;}

	// return true if this is a standard layout of 32 bit components
	bool is32Bit() const {return this->layout >= BufferFormat::X32 && this->layout <= BufferFormat::XYZW32;}

	// return true if this is a standard layout of 32 bit int components
	bool isInt32() const {return this->type == BufferFormat::INT && this->is32Bit();}

	// return true if this is a standard layout of 16 bit float components
	bool isFloat16() const {return this->type == BufferFormat::FLOAT && this->is16Bit();}

	// return true if this is a standard layout of 32 bit float components
	bool isFloat32() const {return this->type == BufferFormat::FLOAT && this->is32Bit();}



	// layout type
	enum LayoutType
	{
		STANDARD,        // one basic type element per channel, e.g. XYZW32
		COMBINED,        // bits are combined into one element, e.g. X10Y10Z10W2
		DATA_BLOCK,      // opaque block of data, e.g. DXT1 or DXT5
	};

	struct LayoutInfo
	{
		// type of layout
		LayoutType type;

		// size of one basic type component in bytes: 1, 2 or 4
		int componentSize;

		// number of basic type components: 1 to 4
		int numComponents;
		
		// number of channels: 1 to 4
		int numChannels;

		// number of bits for all 4 channels
		int4 numBits;
	};

	// get layout info
	const LayoutInfo& getLayoutInfo() const {return BufferFormat::layoutInfos[this->layout];}
	
	// get type of layout
	LayoutType getLayoutType() const {return BufferFormat::layoutInfos[this->layout].type;}

	// get size of one basic type component in bytes (1 to 8)
	int getComponentSize() const {return BufferFormat::layoutInfos[this->layout].componentSize;}

	// get number of basic type components (1 to 4)
	int getNumComponents() const {return BufferFormat::layoutInfos[this->layout].numComponents;}

	// get number of elements (1 to 4) stored in the components. returns 0 for opaque data blocks
	int getNumChannels() const {return BufferFormat::layoutInfos[this->layout].numChannels;}

	// get number of bits for all 4 channels
	int4 getNumBits() const {return BufferFormat::layoutInfos[this->layout].numBits;}

	// get bit positions of all 4 channels
	int4 getBitPositions() const
	{
		int4 numBits = BufferFormat::layoutInfos[this->layout].numBits;
		return vector4(0, numBits.x, numBits.x + numBits.y, numBits.x + numBits.y + numBits.z);
	}

	// get number of bits for all 4 channels
	static int4 getNumBits(Layout layout) {return BufferFormat::layoutInfos[layout].numBits;}

	// get size in bytes
	size_t getMemorySize() const {const LayoutInfo& l = BufferFormat::layoutInfos[this->layout]; return l.numComponents * l.componentSize;}
	
	static const LayoutInfo layoutInfos[];
};


static inline bool operator ==(const BufferFormat& a, const BufferFormat& b)
{
	return a.layout == b.layout && a.type == b.type;
};

static inline bool operator !=(const BufferFormat& a, const BufferFormat& b)
{
	return a.layout != b.layout || a.type != b.type;
};

static inline bool operator <(const BufferFormat& a, const BufferFormat& b)
{
	return a.layout < b.layout || (a.layout == b.layout && a.type < b.type);
};


/// @}
	
} // namespace digi

#endif
