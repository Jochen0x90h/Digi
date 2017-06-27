#include <gtest/gtest.h>

#include <digi/ImageIO/ImageIO.h>

#include "InitLibraries.h"

using namespace digi;


TEST(ImageIO, InitLibraries)
{
	initLibraries();
	std::string versionInfo = VersionInfo::get();
}

TEST(ImageIO, ImageIO)
{
	Pointer<Image> image = loadImage("brick.jpg");
	saveImage("brick.png", image);
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
