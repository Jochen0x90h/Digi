#include <gtest/gtest.h>

#include <digi/Checksum/CRC32.h>

#include "InitLibraries.h"

using namespace digi;


TEST(Checksum, InitLibraries)
{
	initLibraries();
	std::string versionInfo = VersionInfo::get();
}

TEST(Checksum, CRC32)
{
		uint32_t crc = calcCRC32("foo", 3);
		EXPECT_EQ(crc, 0x8C736521);
}		

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
