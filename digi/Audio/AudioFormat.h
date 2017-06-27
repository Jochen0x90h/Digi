#ifndef digi_Audio_AudioFormat_h
#define digi_Audio_AudioFormat_h


namespace digi {

/// @addtogroup Audio
/// @{

/// audio format
struct AudioFormat
{
public:

	enum Type
	{
		INVALID_TYPE = -1,
		INT8,
		INT16,
		FLOAT32
	};

	enum Layout
	{
		INVALID_LAYOUT = -1,
		
		// interleaved write will be used
		INTERLEAVED,
		
		// write with separate channels will be used
		SEPARATE,
	};
	
	Type type;
	Layout layout;
	int numChannels;
	int sampleRate;
	
	AudioFormat()
		: type(INVALID_TYPE), layout(INVALID_LAYOUT), numChannels(), sampleRate() {}
		
	AudioFormat(Type type, Layout layout, int numChannels, int sampleRate)
		: type(type), layout(layout), numChannels(numChannels), sampleRate(sampleRate) {}	

	bool isInterleaved() {return this->layout == INTERLEAVED || this->numChannels == 1;}
	bool isSeparate() {return this->layout == SEPARATE || this->numChannels == 1;}

	// get size of a INT8, INT16 or FlOAT32 element
	int getElementSize();
	
	// get size of a sample. for separate layout this is the size of one channel.
	int getSampleSize()
	{
		return this->getElementSize() * (this->layout == INTERLEAVED ? this->numChannels : 1);
	}
	
	int getNumBuffers()
	{
		return this->layout == INTERLEAVED ? 1 : this->numChannels;
	}
};

/*
/// audio format
struct AudioFormat
{
public:

	enum Type
	{
		INVALID_TYPE = -1,
		NORM,
		FLOAT
	};
	
	// http://en.wikipedia.org/wiki/3D_sound
	enum ChannelMask
	{
		// device defined output order
		NO_MASK = 0,
		
		// front center
		MASK_1_0_MONO = 0x0004,
		
		// front left, front right
		MASK_2_0_STEREO = 0x0003,
		
		// front left, front right, bass
		MASK_2_1_STEREO = 0x000b,
		
		// front left, front right, back left, back right
		MASK_4_0_QUAD = 0x0033,
		
		// front left, front right, front center, bass, back center
		MASK_4_1_SURROUND = 0x010f,
		
		// front left, front right, bass, back left, back right
		MASK_4_1 = 0x003b,
		
		// front left, front right, front center, bass, back left, back right
		MASK_5_1 = 0x003f,
		
		// front left, front right, front center, bass, side left, side right
		MASK_5_1_SIDE = 0x060f,
		
		// front left, front right, front center, bass, back left, back right, back center
		MASK_6_1 = 0x013f,
		
		// front left, front right, front center, bass, back left, back right, front left of center, front right of center
		MASK_7_1_FRONT = 0x00ff,
		
		// front left, front right, front center, bass, back left, back right, side left, side right
		MASK_7_1_SURROUND = 0x063f,

		// front left, front right, front center, bass, back left, back right, side left, side right, left height, right height
		MASK_9_1_SURROUND = 0x1e3f,
	};

	int bitsPerChannel;
	int numChannels;
	Type type;
	int channelFlags;
	
	AudioFormat()
		: bitsPerChannel(0), numChannels(0), type(INVALID_TYPE), channelFlags(0)
	{
	}

	AudioFormat(int bitsPerChannel, int numChannels, Type type, int channelFlags)
		: bitsPerChannel(bitsPerChannel), numChannels(numChannels), type(type), channelFlags(channelFlags)
	{
	}
};
*/

/// @}

} // namespace digi

#endif
