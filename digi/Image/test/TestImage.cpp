#include <gtest/gtest.h>

#include <digi/Utility/foreach.h>
#include <digi/Math/GTestHelpers.h>
#include <digi/Image/JPEGWrapper.h>
#include <digi/Image/PNGWrapper.h>
#include <digi/Image/TIFFWrapper.h>
#include <digi/Image/WebPWrapper.h>

#include "InitLibraries.h"

using namespace digi;


TEST(Image, InitLibraries)
{
	initLibraries();
	std::string versionInfo = VersionInfo::get();
}

TEST(Image, ImageFormat)
{
	ImageFormat a(ImageFormat::XYZW8, ImageFormat::UNORM, ImageFormat::RGB);
	EXPECT_EQ(a.getComponentSize(), 1);
	EXPECT_EQ(a.getNumComponents(), 4);
	EXPECT_EQ(a.getNumChannels(), 4);
	EXPECT_VECTOR_EQ(a.getNumBits(), vector4(8, 8, 8, 8));
	EXPECT_VECTOR_EQ(a.getBitPositions(), vector4(0, 8, 16, 24));
	EXPECT_EQ(a.getMemorySize(), 4);
	EXPECT_EQ(a.getNumMappings(), 3);
	EXPECT_EQ(a.getMappingFlags(), ImageFormat::R_FLAG | ImageFormat::G_FLAG | ImageFormat::B_FLAG);

	ImageFormat b(ImageFormat::X8Y8Z8W8, ImageFormat::UNORM, ImageFormat::BGRA);
	EXPECT_EQ(b.getComponentSize(), 4);
	EXPECT_EQ(b.getNumComponents(), 1);
	EXPECT_EQ(b.getNumChannels(), 4);
	EXPECT_VECTOR_EQ(b.getNumBits(), vector4(8, 8, 8, 8));
	EXPECT_VECTOR_EQ(b.getBitPositions(), vector4(0, 8, 16, 24));
	EXPECT_EQ(b.getMemorySize(), 4);
	EXPECT_EQ(b.getNumMappings(), 4);
	EXPECT_EQ(b.getMappingFlags(), ImageFormat::R_FLAG | ImageFormat::G_FLAG | ImageFormat::B_FLAG | ImageFormat::A_FLAG);

	ImageFormat c(ImageFormat::XY32, ImageFormat::FLOAT, ImageFormat::YA);
	EXPECT_EQ(c.getComponentSize(), 4);
	EXPECT_EQ(c.getNumComponents(), 2);
	EXPECT_EQ(c.getNumChannels(), 2);
	EXPECT_VECTOR_EQ(c.getNumBits(), vector4(32, 32, 0, 0));
	EXPECT_VECTOR_EQ(c.getBitPositions(), vector4(0, 32, 64, 64));
	EXPECT_EQ(c.getMemorySize(), 8);
	EXPECT_EQ(c.getNumMappings(), 2);
	EXPECT_EQ(c.getMappingFlags(), ImageFormat::Y_FLAG | ImageFormat::A_FLAG);
}

TEST(Image, ImageIO)
{
	// load jpeg
	Pointer<Image> image = loadJPEG("brick.jpg");
	EXPECT_EQ(image->getWidth(), 128);
	EXPECT_EQ(image->checkMipmaps(), false);

	// save jpeg
	saveJPEG("brick80.jpg", image, 80);
	saveJPEG("brick60.jpg", image, 60);
	saveJPEG("brick40.jpg", image, 40);

	// save png
	savePNG("brick.png", image, 100);
	savePNG("brick80.png", image, 80);
	savePNG("brick60.png", image, 60);
	savePNG("brick40.png", image, 40);

	// save tiff
	saveTIFF("brick.tif", image);

	// save webp
	saveWebP("brick.webp", image, 100);
	saveWebP("brick80.webp", image, 80);
	saveWebP("brick60.webp", image, 60);
	saveWebP("brick40.webp", image, 40);


	// set first pixel to red and save
	*image->getData<packed_ubyte3>() = make_ubyte3(255, 0, 0);
	saveJPEG("brickFirstRed.jpg", image, 80);

	// load png, set first pixel to red and save
	image = loadPNG("brick.png");
	*image->getData<packed_ubyte3>() = make_ubyte3(255, 0, 0);
	savePNG("brickFirstRed.png", image, 80);

	// load tiff, set first pixel to red and save
	image = loadTIFF("brick.tif");
	*image->getData<packed_ubyte3>() = make_ubyte3(255, 0, 0);				
	saveTIFF("brickFirstRed.tif", image);
	
	// save webp
	saveWebP("brickFirstRed.webp", image, 80.0f);
	
	
	// test gray
	image = new Image(Image::IMAGE, ImageFormat(ImageFormat::X8, ImageFormat::UNORM, ImageFormat::Y), 2, 2);
	uint8_t* grayData = image->getData<uint8_t>();
	grayData[0] = 20;
	grayData[1] = 200;
	grayData[2] = 200;
	grayData[3] = 20;
	savePNG("y.png", image, 100);
	savePNG("y80.png", image, 80);
	saveJPEG("y.jpg", image, 80);
	saveTIFF("y.tif", image);
	saveWebP("y.webp", image, 80.0f);

	// test gray alpha
	image = new Image(Image::IMAGE, ImageFormat(ImageFormat::XY8, ImageFormat::UNORM, ImageFormat::YA), 2, 2);
	ubyte2* grayAlphaData = image->getData<ubyte2>();
	grayAlphaData[0] = make_ubyte2(20, 20);
	grayAlphaData[1] = make_ubyte2(200, 200);
	grayAlphaData[2] = make_ubyte2(200, 200);
	grayAlphaData[3] = make_ubyte2(20, 20);
	savePNG("ya.png", image, 100);
	savePNG("ya80.png", image, 80);
	saveTIFF("ya.tif", image);
	saveWebP("ya.webp", image, 80.0f);

	// test rgb
	image = new Image(Image::IMAGE, ImageFormat(ImageFormat::XYZ8, ImageFormat::UNORM, ImageFormat::RGB), 2, 2);
	packed_ubyte3* rgbData = image->getData<packed_ubyte3>();
	rgbData[0] = make_ubyte3(20, 0, 0);
	rgbData[1] = make_ubyte3(0, 200, 0);
	rgbData[2] = make_ubyte3(0, 0, 200);
	rgbData[3] = make_ubyte3(20, 0, 0);
	savePNG("rgb.png", image, 100);
	savePNG("rgb80.png", image, 80);
	saveJPEG("rgb.jpg", image, 80);
	saveTIFF("rgb.tif", image);
	saveWebP("rgb.webp", image, 80.0f);

	// test rgb alpha
	image = new Image(Image::IMAGE, ImageFormat(ImageFormat::XYZW8, ImageFormat::UNORM, ImageFormat::RGBA), 2, 2);
	ubyte4* rgbAlphaData = image->getData<ubyte4>();
	rgbAlphaData[0] = make_ubyte4(20, 0, 0, 20);
	rgbAlphaData[1] = make_ubyte4(0, 200, 0, 200);
	rgbAlphaData[2] = make_ubyte4(0, 0, 200, 200);
	rgbAlphaData[3] = make_ubyte4(20, 0, 0, 20);
	savePNG("rgba.png", image, 100);
	savePNG("rgba80.png", image, 80);
	saveTIFF("rgba.tif", image);
	saveWebP("rgba.webp", image, 80.0f);
}

TEST(Image, Noise)
{
	Pointer<Image> image = new Image(Image::IMAGE, ImageFormat(ImageFormat::XYZ8, ImageFormat::UNORM, ImageFormat::RGB),
		256, 256);
		
	packed_ubyte3* data = image->getData<packed_ubyte3>();
		
	for (int j = 0; j < 256; ++j)
	{
		for (int i = 0; i < 256; ++i)
		{
			float x = i * 0.1f;
			float y = j * 0.1f;
				
			data[j * 256 + i] = convert_ubyte3_sat((noise3(vector3(x, y, 0.0f)) * 0.5f + 0.5f) * 256.0f);
		}	
	}

	savePNG("noise.png", image, 100);
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
