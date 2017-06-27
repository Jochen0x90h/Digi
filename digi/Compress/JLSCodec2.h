#ifndef JLSCodec2_h
#define JLSCodec2_h

#include "Codec.h"

// same as JLSCodec but with all necessary code extracted from CharLS
class JLSCodec2 : public Codec
{
	
	public:
		
		virtual ~JLSCodec2();

	
		virtual bool compress(UByteArray& compressedData, const ushort* image, int width, int height, int depth);
		using Codec::compress;

		virtual bool decompress(const UByteArray& compressedData, ushort* image, int width, int height, int depth);
		using Codec::decompress;
			
		virtual std::string getName();
		virtual std::string getExtension();
		
		
		void getQImage(uint8_t* statImage, const ushort* image, int width, int height, int depth);
		void getkImage(uint8_t* statImage, const ushort* image, int width, int height, int depth);
};





// non causal ideal prediction that uses all 8 neighbors
class JLSCodecTest : public Codec
{
	
	public:
		
		virtual ~JLSCodecTest();

	
		virtual bool compress(UByteArray& compressedData, const ushort* image, int width, int height, int depth);
		using Codec::compress;

		virtual bool decompress(const UByteArray& compressedData, ushort* image, int width, int height, int depth);
		using Codec::decompress;
			
		virtual std::string getName();
		virtual std::string getExtension();
};



#endif
