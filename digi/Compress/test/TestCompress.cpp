#include <gtest/gtest.h>

#include <digi/Base/VersionInfo.h>
#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/foreach.h>
#include <digi/System/File.h>
#include <digi/System/IOException.h>
#include <digi/System/MemoryDevices.h>
#include <digi/Data/BufferedWriter.h>
#include <digi/Compress/DLZSS.h>

#include "InitLibraries.h"

using namespace digi;



TEST(Compress, InitLibraries)
{
	// installs utf-8 encoding for boost::filesystem
	initLibraries();
	std::string versionInfo = VersionInfo::get();
}

// read a compressed .dat file
TEST(Compress, dat)
{
	Pointer<File> file = File::open("cube.dat");
	size_t size = size_t(file->getSize());
	std::vector<uint8_t> data(size);
	file->read(data.data(), data.size());
	
	DlzssDecompressor d(data.data(), data.size());
	
	std::vector<uint8_t> d8(72);
	d.decompress(d8);
}


uint32_t random(uint32_t& seed)
{
	static const uint32_t rand_a = 1103515245;
	static const uint32_t rand_c = 12345;
	return seed = (rand_a * seed + rand_c);
}

template <typename Value>
void compressHelper(Value* data, size_t size)
{
	std::vector<Value> data1(data, data + size);
	Pointer<MemoryDevice> dev = new MemoryDevice();
	BufferedWriter w(dev);
	DlzssCompressor c(w);
	c.compress(data, size);
	c.flush();

	std::vector<Value> data2(size);
	DlzssDecompressor d(dev->container.data(), dev->container.size());
	d.decompress(data2);
	for (int i = 0; i < size; ++i)
	{
		if (data1[i] != data2[i])
			EXPECT_EQ(data1[i], data2[i]);
	}
}

template <typename Value>
void compressRandom()
{
	std::vector<Value> data(10000000);

	// random values
	{
		uint32_t seed = 0;
		foreach (Value& value, data)
			value = random(seed);
		compressHelper(data.data(), data.size());
	}

	// random lengths
	{
		uint32_t seed = 0;
		typename std::vector<Value>::iterator it = data.begin();
		typename std::vector<Value>::iterator end = data.end();
		while (it != end)
		{
			uint8_t value = random(seed);
			int length = random(seed) & 0xffff;
			while (it != end && length > 0)
			{
				*it = value;
				++it;
				--length;
			}
		}
		//compressHelper(data.data(), data.size());
	}
}

TEST(Compress, DlzssCompress)
{
	{
		const int numValues = 256;
		uint8_t data[numValues];

		// increasing values
		for (int i = 0; i < numValues; ++i)
			data[i] = i * i >> 8;
		compressHelper(data, numValues);

		// difference of increasing values
		uint8_t last = 0;
		for (int i = 0; i < numValues; ++i)
		{
			uint8_t value = i * i >> 8;
			data[i] = value - last;
			last = value;
		}
		compressHelper(data, numValues);
	}
	
	compressRandom<uint8_t>();
	compressRandom<uint16_t>();
	compressRandom<uint32_t>();
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
