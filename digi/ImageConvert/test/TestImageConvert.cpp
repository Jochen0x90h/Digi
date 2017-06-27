#include <gtest/gtest.h>

#include <digi/Utility/Convert.h>
#include <digi/Utility/ArrayUtility.h>
#include <digi/Math/All.h>
#include <digi/Math/GTestHelpers.h>
#include <digi/System/MemoryDevices.h>
#include <digi/Image/BufferFormat.h>
#include <digi/Image/DDS.h>
#include <digi/Image/JPEGWrapper.h>
#include <digi/Image/PNGWrapper.h>
#include <digi/ImageConvert/BufferConverter.h>
#include <digi/ImageConvert/ImageConverter.h>

#include "InitLibraries.h"

using namespace digi;


TEST(ImageConvert, InitLibraries)
{
	initLibraries();
	std::string versionInfo = VersionInfo::get();
}

TEST(ImageConvert, Converter)
{
	Pointer<ConverterContext> context = new ConverterContext();
	
	BufferFormat srcFormat(BufferFormat::X8, BufferFormat::UNORM);
	BufferFormat dstFormat(BufferFormat::X8, BufferFormat::UNORM);

	std::string code;
	CodeWriter w(new StringRefDevice(code));
	ConverterWriter cw(w);
	
	cw.beginConverter();
	cw.load(srcFormat);
	cw.depackToFloat(srcFormat);
	cw.packFromFloat(dstFormat);
	cw.store(dstFormat, DataConverter::NATIVE);
	cw.endConverter();

	Pointer<DataConverter> converter = DataConverter::create(context, code);

/*		
	ConverterBuilder builder(context);
	builder.load(srcFormat);
	builder.depackToFloat(srcFormat);
	builder.packFromFloat(dstFormat);
	builder.store(dstFormat);
	Pointer<DataConverter> converter = builder.get();
*/	
	
	
	uint8_t srcData[4];
	uint8_t dstData[4];
	for (int i = 0; i < 4; ++i)
		srcData[i] = i;
	converter->convert(srcData, 1, NULL, dstData, 1, 4);
	
	EXPECT_EQ(dstData[0], 0);
	EXPECT_EQ(dstData[1], 1);
	EXPECT_EQ(dstData[2], 2);
	EXPECT_EQ(dstData[3], 3);
}

TEST(ImageConvert, BufferConverter)
{
	Pointer<ConverterContext> context = new ConverterContext();
		
	Pointer<BufferConverter> converter = new BufferConverter(context);

	// half -> unsigned normalized
	{
		BufferFormat srcFormat(BufferFormat::XYZW16, BufferFormat::FLOAT);
		BufferFormat dstFormat(BufferFormat::XYZW8, BufferFormat::UNORM);

		half4 srcData[1];
		ubyte4 dstData[1];
		srcData[0] = make_half4(0.0f, 0.1f, 0.5f, 1.0f);

		converter->convert(srcFormat, srcData, dstFormat, dstData, 1); 
					
		EXPECT_VECTOR_EQ(dstData[0], make_ubyte4(0, 25, 128, 255));
	}

	// unsigned normalized -> float
	{
		BufferFormat srcFormat(BufferFormat::XYZW8, BufferFormat::UNORM);
		BufferFormat dstFormat(BufferFormat::XYZW32, BufferFormat::FLOAT);

		ubyte4 srcData[1];
		ALIGN(16) float4 dstData[1];
		srcData[0] = make_ubyte4(0, 10, 20, 30);

		converter->convert(srcFormat, srcData, dstFormat, dstData, 1); 
					
		EXPECT_EPSILON_EQ(dstData[0], make_float4(0, 10, 20, 30) / 255.0f);
	}
			
	// float2 that is not aligned to 8 byte
	{
		// clang: typedef float __attribute__((ext_vector_type(2), aligned(4))) packed_float2;
		BufferFormat srcFormat(BufferFormat::XY32, BufferFormat::FLOAT);
		BufferFormat dstFormat(BufferFormat::XY32, BufferFormat::FLOAT);
	
		float srcData[4];
		float dstData[5];
		fill(srcData, 0.0f);
		srcData[2] = 1.0f;
		srcData[3] = 2.0f;

		converter->convert(srcFormat, srcData, dstFormat, DataConverter::NATIVE, dstData, 12, 2);

		EXPECT_EPSILON_EQ(dstData[3], 1.0f);
		EXPECT_EPSILON_EQ(dstData[4], 2.0f);
	}
	
	// float3 that is not aligned to 16 byte
	{
		BufferFormat srcFormat(BufferFormat::XYZ32, BufferFormat::FLOAT);
		BufferFormat dstFormat(BufferFormat::XYZ32, BufferFormat::FLOAT);
	
		float srcData[6];
		float dstData[7];
		fill(srcData, 0.0f);
		srcData[3] = 1.0f;
		srcData[4] = 2.0f;
		srcData[5] = 3.0f;

		converter->convert(srcFormat, srcData, dstFormat, DataConverter::NATIVE, dstData, 16, 2);

		EXPECT_EPSILON_EQ(dstData[4], 1.0f);
		EXPECT_EPSILON_EQ(dstData[5], 2.0f);
		EXPECT_EPSILON_EQ(dstData[6], 3.0f);
	}
/*
	// float4 that is not aligned to 16 byte
	{
		// clang: typedef float __attribute__((ext_vector_type(4), aligned(4))) packed_float4;
		BufferFormat srcFormat(BufferFormat::XYZW32, BufferFormat::FLOAT);
		BufferFormat dstFormat(BufferFormat::XYZW32, BufferFormat::FLOAT);
	
		float srcData[8];
		float dstData[9];
		fill(srcData, 0.0f);
		srcData[4] = 1.0f;
		srcData[5] = 2.0f;
		srcData[6] = 3.0f;
		srcData[7] = 4.0f;

		converter->convert(srcFormat, srcData, dstFormat, DataConverter::NATIVE, dstData, 20, 2);

		EXPECT_EPSILON_EQ(dstData[5], 1.0f);
		EXPECT_EPSILON_EQ(dstData[6], 2.0f);
		EXPECT_EPSILON_EQ(dstData[7], 3.0f);
		EXPECT_EPSILON_EQ(dstData[8], 4.0f);
	}
*/

	// float -> byte buffer
	{
		BufferFormat srcFormat(BufferFormat::X32, BufferFormat::FLOAT);
		BufferFormat dstFormat(BufferFormat::X32, BufferFormat::FLOAT);

		float srcData[2];
		uint8_t buffer[10];
		srcData[0] = 1.0f;
		srcData[1] = -1.0f;

		// little endian
		converter->convert(srcFormat, srcData, dstFormat, DataConverter::LE, buffer, 6, 2);
		EXPECT_EQ(buffer[3], 0x3f);
		EXPECT_EQ(buffer[2], 0x80);
		EXPECT_EQ(buffer[1], 0x00);
		EXPECT_EQ(buffer[0], 0x00);
		EXPECT_EQ(buffer[9], 0xbf);
		EXPECT_EQ(buffer[8], 0x80);
		EXPECT_EQ(buffer[7], 0x00);
		EXPECT_EQ(buffer[6], 0x00);

		// big endian
		converter->convert(srcFormat, srcData, dstFormat, DataConverter::BE, buffer, 6, 2);
		EXPECT_EQ(buffer[0], 0x3f);
		EXPECT_EQ(buffer[1], 0x80);
		EXPECT_EQ(buffer[2], 0x00);
		EXPECT_EQ(buffer[3], 0x00);
		EXPECT_EQ(buffer[6], 0xbf);
		EXPECT_EQ(buffer[7], 0x80);
		EXPECT_EQ(buffer[8], 0x00);
		EXPECT_EQ(buffer[9], 0x00);
	}
		
	// unsigned combined formats
	{
		BufferFormat srcFormat(BufferFormat::X8Y8Z8W8, BufferFormat::UNORM);
		BufferFormat dstFormat(BufferFormat::X4Y4Z4W4, BufferFormat::UNORM);

		uint32_t srcData[2];
		uint16_t dstData[2];
		srcData[0] = 0x00102080;
		srcData[1] = 0x66778899;

		converter->convert(srcFormat, srcData, dstFormat, dstData, 2);

		EXPECT_EQ(dstData[0], 0x0128);
		EXPECT_EQ(dstData[1], 0x6789);
	}
	{
		BufferFormat srcFormat(BufferFormat::XYZ32, BufferFormat::FLOAT);
		BufferFormat dstFormat(BufferFormat::X5Y6Z5, BufferFormat::UNORM);

		packed_float3 srcData[2];
		ushort dstData[2];
		srcData[0] = vector3(0.0f, 0.501f, 1.0f);
		srcData[1] = vector3(-0.5f, 5.0f, -5.0f);

		converter->convert(srcFormat, srcData, dstFormat, dstData, 2);

		EXPECT_EQ(dstData[0], 0xfc00); // 11111 100000 00000
		EXPECT_EQ(dstData[1], 0x07e0); // 00000 111111 00000
	}

	// signed combined formats
	{
		BufferFormat srcFormat(BufferFormat::X8Y8Z8W8, BufferFormat::NORM);
		BufferFormat dstFormat(BufferFormat::X4Y4Z4W4, BufferFormat::NORM);

		uint32_t srcData[2];
		uint16_t dstData[2];
		srcData[0] = 0x00102080;
		srcData[1] = 0x66778899;

		converter->convert(srcFormat, srcData, dstFormat, dstData, 2);

		EXPECT_EQ(dstData[0], 0x0129);
		EXPECT_EQ(dstData[1], 0x679a);
	}
}

TEST(ImageConvert, ImageConverter)
{
	Pointer<ConverterContext> context = new ConverterContext();		
	Pointer<ImageConverter> converter = new ImageConverter(context);
		
	// RGBA -> BGRA
	{
		ImageFormat srcFormat(ImageFormat::XYZW8, ImageFormat::UNORM, ImageFormat::RGBA);
		ImageFormat dstFormat(ImageFormat::XYZW8, ImageFormat::UNORM, ImageFormat::BGRA);

		ubyte4 srcData[1];
		ubyte4 dstData[1];
		uint8_t buffer[4];
		srcData[0] = make_ubyte4(0, 10, 20, 30);

		converter->convertPixels(srcFormat, srcData, dstFormat, dstData, 1); 
		EXPECT_VECTOR_EQ(dstData[0], make_ubyte4(20, 10, 0, 30));
		
		converter->convertPixels(srcFormat, srcData, dstFormat, DataConverter::LE, buffer, 4, 1);
		EXPECT_EQ(buffer[0], 20);
		EXPECT_EQ(buffer[1], 10);
		EXPECT_EQ(buffer[2], 0);
		EXPECT_EQ(buffer[3], 30);

		converter->convertPixels(srcFormat, srcData, dstFormat, DataConverter::BE, buffer, 4, 1);
		EXPECT_EQ(buffer[0], 20);
		EXPECT_EQ(buffer[1], 10);
		EXPECT_EQ(buffer[2], 0);
		EXPECT_EQ(buffer[3], 30);
		
		dstFormat = ImageFormat(ImageFormat::X8Y8Z8W8, ImageFormat::UNORM, ImageFormat::BGRA);
		
		converter->convertPixels(srcFormat, srcData, dstFormat, DataConverter::LE, buffer, 4, 1);
		EXPECT_EQ(buffer[0], 20);
		EXPECT_EQ(buffer[1], 10);
		EXPECT_EQ(buffer[2], 0);
		EXPECT_EQ(buffer[3], 30);

		converter->convertPixels(srcFormat, srcData, dstFormat, DataConverter::BE, buffer, 4, 1);
		EXPECT_EQ(buffer[3], 20);
		EXPECT_EQ(buffer[2], 10);
		EXPECT_EQ(buffer[1], 0);
		EXPECT_EQ(buffer[0], 30);
	}
	
	// RGB -> Luminance
	{
		ImageFormat srcFormat(ImageFormat::XYZ32, ImageFormat::FLOAT, ImageFormat::RGB);
		ImageFormat dstFormat(ImageFormat::X32, ImageFormat::FLOAT, ImageFormat::Y);

		packed_float3 srcData[4];
		float dstData[4];
		srcData[0] = make_float3(1, 0, 0);
		srcData[1] = make_float3(0, 1, 0);
		srcData[2] = make_float3(0, 0, 1);
		srcData[3] = make_float3(1, 1, 1);

		converter->convertPixels(srcFormat, srcData, dstFormat, dstData, 4); 
					
		EXPECT_EPSILON_EQ(dstData[0], 0.299f);
		EXPECT_EPSILON_EQ(dstData[1], 0.587f);
		EXPECT_EPSILON_EQ(dstData[2], 0.114f);
		EXPECT_EPSILON_EQ(dstData[3], 1.0f);
	}
				
	// create mipmaps
	{
		Pointer<Image> image = loadJPEG("brick.jpg");
			
		Pointer<Image> converted = converter->convert(image,
			ImageFormat(ImageFormat::X8, ImageFormat::UNORM, ImageFormat::Y),
			vector3(128, 128, 1), true);
			
		int numMipmaps = converted->getNumMipmaps();
		for (int i = 0; i < numMipmaps; ++i)
		{
			savePNG(arg("brick.%0.png", i), converted, i);
		}
			
	}
}

#ifdef HAVE_S3TC
TEST(ImageConvert, S3TC)
{
	Pointer<ConverterContext> context = new ConverterContext();		
	Pointer<ImageConverter> converter = new ImageConverter(context);

	Pointer<Image> image = loadJPEG("brick.jpg");
		
	saveDDS("brick.dds", image);
		
	// convert to dxt1 and mipmaps
	Pointer<Image> converted = converter->convert(image,
		ImageFormat(ImageFormat::BLOCK8, ImageFormat::UNORM, ImageFormat::DXT1),
		vector3(128, 128, 1), true);

	// save dds with mitmaps
	saveDDS("brick.dxt1.dds", converted);

	// save one dds for each mipmap
	int numMipmaps = converted->getNumMipmaps();
	for (int i = 0; i < numMipmaps; ++i)
	{
		saveDDS(arg("brick.dxt1.%0.dds", i), converted, i);
	}
}
#endif

#ifdef HAVE_PVRTC
TEST(ImageConvert, PVRTC)
{
	Pointer<ConverterContext> context = new ConverterContext();		
	Pointer<ImageConverter> converter = new ImageConverter(context);

	Pointer<Image> image = loadJPEG("brick.jpg");
			
	// convert to pvrtc with 4bpp and mipmaps
	Pointer<Image> converted = converter->convert(image,
		ImageFormat(ImageFormat::BLOCK8, ImageFormat::UNORM, ImageFormat::PVRTC4),
		vector3(128, 128, 1), true);

	// convert back
	Pointer<Image> converted2 = converter->convert(converted,
		ImageFormat(ImageFormat::XYZW8, ImageFormat::UNORM, ImageFormat::RGBA),
		vector3(128, 128, 1), true);

	// save one png for each mipmap
	int numMipmaps = converted2->getNumMipmaps();
	for (int i = 0; i < numMipmaps; ++i)
	{
		savePNG(arg("brick.pvrtc4.%0.png", i), converted2, i);
	}
}
#endif

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
