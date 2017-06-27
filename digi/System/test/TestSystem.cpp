#include <fstream>

#include <gtest/gtest.h>

#include <digi/Base/VersionInfo.h>
#include <digi/Utility/Convert.h>
#include <digi/System/File.h>
#include <digi/System/IOException.h>
#include <digi/System/SerialPort.h>
#include <digi/System/Timer.h>

#include "InitLibraries.h"

using namespace digi;


TEST(System, InitLibraries)
{
	// installs utf-8 encoding for boost::filesystem
	initLibraries();
	std::string versionInfo = VersionInfo::get();
}

#ifdef HAVE_LOCALE
TEST(System, File)
{
		fs::path dir = ".";

		// create a file using std::ofstream and check if it exists
		fs::path streamDir = dir / "stream.bin";
		fs::remove(streamDir);
		std::ofstream stream(streamDir.c_str(), std::ios::out);
		stream.close();
		EXPECT_TRUE(fs::exists(streamDir));
		
				
		// remove and then create a file for writing
		fs::path fileDir = dir / "file.bin";
		fs::remove(fileDir);
		Pointer<File> file = File::create(fileDir);
		
		// try to read on file which was opened for write which must fail
		try
		{
			uint8_t buf[4];
			file->read(buf, 4);
			EXPECT_TRUE(false) << "no exception was thrown";
		}
		catch (IOException& e)
		{
			// the error is os dependent
			#ifdef _WIN32
				EXPECT_EQ(e.getReason(), IOException::ACCESS_DENIED);
				EXPECT_STREQ(e.what(), "access denied");
			#else
				EXPECT_EQ(e.getReason(), IOException::INVALID_HANDLE);
				EXPECT_STREQ(e.what(), "invalid handle");
			#endif
		}
		
		// check if file size is 0
		EXPECT_EQ(file->getSize(), 0);
		
		// resize to 5 and check size again
		file->setSize(5);
		EXPECT_EQ(file->getSize(), 5);
		
		// check if position is still 0
		EXPECT_EQ(file->getPosition(), 0);

		// set position to end of file
		file->seek(0, File::END);
		EXPECT_EQ(file->getPosition(), 5);
		
		// close and check if file exists
		file->close();
		EXPECT_TRUE(fs::exists(fileDir));
		
		// try to write on closed file which must fail
		try
		{
			uint8_t buf[4];
			file->write(buf, 4);
			EXPECT_TRUE(false) << "no exception was thrown";
		}
		catch (IOException& e)
		{
			EXPECT_EQ(e.getReason(), IOException::CLOSED);
			EXPECT_STREQ(e.what(), "closed");
		}
}

TEST(System, FileSystem)
{
	// fs is alias to boost::filesystem

	// directory name with a umlaut and chinese character
	fs::path dir = L"./\u00E4\u6F22";
		
	// create directories
	fs::create_directory(dir);
	fs::create_directory(dir / "foo");
	fs::create_directory(dir + ".foo");
	
	// check if directories exist
	EXPECT_TRUE(fs::exists(dir));
	EXPECT_TRUE(fs::exists(dir / "foo"));
	EXPECT_TRUE(fs::exists(dir + ".foo"));

	// check while converting to 8 bit string and back (should always work as we use utf-8)
	EXPECT_TRUE(fs::exists(dir.string()));
	
	fs::directory_iterator end; // default construction yields past-the-end
	for (fs::directory_iterator it("."); it != end; ++it)
	{
		std::string str = it->path().string();
	}
}
#endif // HAVE_LOCALE

TEST(System, Path)
{
	fs::path foo = "foo";
	fs::path path = foo / "bar.ext";
	EXPECT_EQ(path, foo / L"bar.ext");
	EXPECT_EQ(path, foo / StringRef("bar.ext"));

	#ifdef _WIN32
		EXPECT_EQ(path, "foo\\bar.ext");
		EXPECT_EQ(path.string(), "foo\\bar.ext");
	#else
		EXPECT_EQ(path, "foo/bar.ext");
		EXPECT_EQ(path.string(), "foo/bar.ext");
	#endif

	EXPECT_EQ(path.filename(), "bar.ext");
	EXPECT_EQ(path.extension(), ".ext");

	// test to8BitPath which converts from wide unicode string to local 8 bit string
	// (problematic only on windows where 8 bit string is not utf-8 encoded)
	//EXPECT_EQ(to8BitPath(L"\u00E4"), "\xE4");
}

#ifdef TEST_SERIAL_DEVICE

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

TEST(System, SerialPort)
{
	Pointer<SerialPort> serialPort = SerialPort::open(TOSTRING(TEST_SERIAL_DEVICE), 115200, SerialPort::FORMAT_8N1, 1000);
	serialPort->write("hello world!\n", 13);
	serialPort->close();
}

#endif

TEST(System, Timer)
{
	int t1 = Timer::getMilliSeconds();
	Timer::milliSleep(500);
	int t2 = Timer::getMilliSeconds();
	
	int t = t2 - t1;
	EXPECT_TRUE(t > 480 && t < 550);
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
