#include <gtest/gtest.h>

#include <digi/Base/VersionInfo.h>
#include <digi/Utility/lexicalCast.h>
#include <digi/System/File.h>
#include <digi/System/IOException.h>
#include <digi/Data/DataException.h>
#include <digi/Data/DataWriter.h>
#include <digi/Data/DataReader.h>
#include <digi/Data/EbmlReader.h>
#include <digi/Data/EbmlWriter.h>
#include <digi/Data/JsonReader.h>
#include <digi/Data/JsonWriter.h>
#include <digi/Data/ReadHelper.h>
#include <digi/Data/TextReader.h>
#include <digi/Data/XmlReader.h>
#include <digi/Data/XmlWriter.h>

#include "InitLibraries.h"

using namespace digi;


class ThrowDevice : public IODevice
{
public:
	virtual int getState()
	{
		return 0;
	}

	virtual void close()
	{
	}
	
	virtual size_t read(void* data, size_t length)
	{
		throw IOException(this, IOException::IO_ERROR);
		return 0;
	}
	
	virtual size_t write(const void* data, size_t length)
	{
		throw IOException(this, IOException::IO_ERROR);
		return 0;
	}
};

class NullDevice : public IODevice
{
public:
	NullDevice()
		: state(OPEN) {}
	
	virtual int getState()
	{
		return this->state;
	}
	
	virtual void close()
	{
		this->state &= ~OPEN;
	}
	
	virtual size_t read(void* data, size_t length)
	{
		//this->state |= END_OF_INPUT;
		return 0;
	}
	
	virtual size_t write(const void* data, size_t length)
	{
		return length;
	}
	
	int state;
};



TEST(Data, InitLibraries)
{
	// installs utf-8 encoding for boost::filesystem
	initLibraries();
	std::string versionInfo = VersionInfo::get();
}

void helperTestDataReaderWriter(const char* path, bool bigEndian)
{
	{
		DataWriter w(path);

		w.write<int8_t>(-1);
		w.write<int16_t>(1000);
		w.write<int32_t>(-1000000);
		w.align(16);
		w.write<float>(1.0f);	
		w.write<double>(-999.0);	
		w.write<uint64_t>(UINT64_C(1000000000000));
			
		w.close();
	}

	{
		DataReader r(path);

		EXPECT_EQ(r.read<int8_t>(), -1);
		EXPECT_EQ(r.read<int16_t>(), 1000);
		EXPECT_EQ(r.read<int32_t>(), -1000000);
		r.align(16);
		EXPECT_EQ(r.read<float>(), 1.0f);
		EXPECT_EQ(r.read<double>(), -999.0);
		EXPECT_EQ(r.read<uint64_t>(), UINT64_C(1000000000000));

		r.close();
	}		
}
	
TEST(Data, DataReaderWriter)
{
	// test DataReader and DataWriter little and big endian
	helperTestDataReaderWriter("testLE.bin", false);
	helperTestDataReaderWriter("testBE.bin", true);
		
	// file does not exist
	try
	{
		DataReader r("doesnotexist.bin");
		EXPECT_TRUE(false) << "no exception was thrown";
	}
	catch (IOException& e)
	{
		EXPECT_EQ(e.getReason(), IOException::FILE_NOT_FOUND);
		EXPECT_STREQ(e.what(), "file not found");
	}

	// this device prodices read/write exceptions
	Pointer<IODevice> dev = new ThrowDevice();
	try
	{
		DataReader r(dev);
		r.read<uint8_t>();
		EXPECT_TRUE(false) << "no exception was thrown";
	}
	catch (IOException& e)
	{
		EXPECT_EQ(e.getReason(), IOException::IO_ERROR);
		EXPECT_STREQ(e.what(), "input/output error");
	}
	try
	{
		DataWriter w(dev);
		w.write<uint8_t>(0);
		w.close();
		EXPECT_TRUE(false) << "no exception was thrown";
	}
	catch (IOException& e)
	{
		EXPECT_EQ(e.getReason(), IOException::IO_ERROR);
		EXPECT_STREQ(e.what(), "input/output error");
	}
		
	// test end of file
	try
	{
		DataReader r(new NullDevice());
		r.read<uint8_t>();
		EXPECT_TRUE(false) << "no exception was thrown";
	}
	catch (DataException& e)
	{
		EXPECT_EQ(e.getReason(), DataException::UNEXPECTED_END_OF_DATA);
		EXPECT_STREQ(e.what(), "unexpected end of data");
	}
}

TEST(Data, EBMLReaderWriter)
{
	EXPECT_EQ(EbmlWriter::calcSizeVarInt(0), 1);
	EXPECT_EQ(EbmlWriter::calcSizeVarInt(0x7e), 1);
	EXPECT_EQ(EbmlWriter::calcSizeVarInt(0x7f), 2);
	EXPECT_EQ(EbmlWriter::calcSizeVarInt(0x3ffe), 2);
	EXPECT_EQ(EbmlWriter::calcSizeVarInt(0x3fff), 3);

	// write ebml
	{
		EbmlWriter w("test.ebml");
	
		w.writeId(0x1A45DFA3);
		
		w.writeVarInt(0);
		w.writeVarInt(0x7e);
		w.writeVarInt(0x7f);
		w.writeVarInt(0x3ffe);
		w.writeVarInt(0x3fff);

		w.write(0x4286, -128);
		w.write(0x4287, 1000u);
		w.write(0x4288, true);
		w.write(0x2383E3, 1.0);
		w.write(0xB0, "ebml");
		
		w.close();
	}

	// read ebml
	{
		EbmlReader r("test.ebml");
		
		EXPECT_EQ(r.readId(), 0x1A45DFA3);
		EXPECT_EQ(r.getByteCount(), 4);

		EXPECT_EQ(r.readVarInt(), 0);
		EXPECT_EQ(r.getByteCount(), 5);
		EXPECT_EQ(r.readVarInt(), 0x7e);
		EXPECT_EQ(r.getByteCount(), 6);
		EXPECT_EQ(r.readVarInt(), 0x7f);
		EXPECT_EQ(r.getByteCount(), 8);
		EXPECT_EQ(r.readVarInt(), 0x3ffe);
		EXPECT_EQ(r.getByteCount(), 10);
		EXPECT_EQ(r.readVarInt(), 0x3fff);
		EXPECT_EQ(r.getByteCount(), 13);
		
		int i = 0;
		EXPECT_EQ(r.readId(), 0x4286);
		r.read(i);
		EXPECT_EQ(i, -128);

		digi::uint u = 0;
		EXPECT_EQ(r.readId(), 0x4287);
		r.read(u);
		EXPECT_EQ(u, 1000);

		bool b = false;
		EXPECT_EQ(r.readId(), 0x4288);
		r.read(b);
		EXPECT_EQ(b, true);

		double d = 0.0;
		EXPECT_EQ(r.readId(), 0x2383E3);
		r.read(d);
		EXPECT_EQ(d, 1.0);

		std::string s;
		EXPECT_EQ(r.readId(), 0xB0);
		r.read(s);
		EXPECT_EQ(s, "ebml");
		
		r.close();
	}
}

TEST(Data, JSONReaderWriter)
{
	// write json
	{
		JsonWriter w("test.json");
		w.beginStruct();
	
		w.writeAttribute("a");
		w.writeNumber("55");
	
		w.writeAttribute("b");
		w.writeString("abc\tdef\nxyz\"\\");
	
		w.writeAttribute("c");
		w.beginStruct();
		w.endStruct();

		w.writeAttribute("d");
		w.beginArray();
		w.writeString("xyz");
		w.endArray();

		w.writeAttribute("e");
		w.beginArray();
		w.beginStruct();
		w.endStruct();
		w.endArray();

		w.writeAttribute("f");
		w.beginStruct();
		w.writeAttribute("x");
		w.writeBool(false);
		w.writeAttribute("y");
		w.writeBool(true);
		w.writeAttribute("z");
		w.writeNull();
		w.endStruct();

		w.endStruct();
		w.close();
	}

	// read json
	{
		JsonReader r("test.json");
		EXPECT_EQ(r.read(), JsonReader::BEGIN_STRUCT);

		EXPECT_EQ(r.read(), JsonReader::ATTRIBUTE);
		EXPECT_EQ(r.getValue(), "a");
		EXPECT_EQ(r.read(), JsonReader::NUMBER);
		EXPECT_EQ(r.getValue(), "55");

		EXPECT_EQ(r.read(), JsonReader::ATTRIBUTE);
		EXPECT_EQ(r.getValue(), "b");
		EXPECT_EQ(r.read(), JsonReader::STRING);
		EXPECT_EQ(r.getValue(), "abc\tdef\nxyz\"\\");

		EXPECT_EQ(r.read(), JsonReader::ATTRIBUTE);
		EXPECT_EQ(r.getValue(), "c");
		EXPECT_EQ(r.read(), JsonReader::BEGIN_STRUCT);
		EXPECT_EQ(r.read(), JsonReader::END_STRUCT);

		EXPECT_EQ(r.read(), JsonReader::ATTRIBUTE);
		EXPECT_EQ(r.getValue(), "d");
		EXPECT_EQ(r.read(), JsonReader::BEGIN_ARRAY);
		EXPECT_EQ(r.read(), JsonReader::ELEMENT);
		EXPECT_EQ(r.read(), JsonReader::STRING);
		EXPECT_EQ(r.getValue(), "xyz");
		EXPECT_EQ(r.read(), JsonReader::END_ARRAY);

		EXPECT_EQ(r.read(), JsonReader::ATTRIBUTE);
		EXPECT_EQ(r.getValue(), "e");
		EXPECT_EQ(r.read(), JsonReader::BEGIN_ARRAY);
		EXPECT_EQ(r.read(), JsonReader::ELEMENT);
		EXPECT_EQ(r.read(), JsonReader::BEGIN_STRUCT);
		EXPECT_EQ(r.read(), JsonReader::END_STRUCT);
		EXPECT_EQ(r.read(), JsonReader::END_ARRAY);

		EXPECT_EQ(r.read(), JsonReader::ATTRIBUTE);
		EXPECT_EQ(r.getValue(), "f");
		EXPECT_EQ(r.read(), JsonReader::BEGIN_STRUCT);
		EXPECT_EQ(r.read(), JsonReader::ATTRIBUTE);
		EXPECT_EQ(r.getValue(), "x");
		EXPECT_EQ(r.read(), JsonReader::BOOLEAN);
		EXPECT_EQ(r.getBool(), false);
		EXPECT_EQ(r.read(), JsonReader::ATTRIBUTE);
		EXPECT_EQ(r.getValue(), "y");
		EXPECT_EQ(r.read(), JsonReader::BOOLEAN);
		EXPECT_EQ(r.getBool(), true);
		EXPECT_EQ(r.read(), JsonReader::ATTRIBUTE);
		EXPECT_EQ(r.getValue(), "z");
		EXPECT_EQ(r.read(), JsonReader::NULL_VALUE);
		EXPECT_EQ(r.read(), JsonReader::END_STRUCT);	

		EXPECT_EQ(r.read(), JsonReader::END_STRUCT);
	
		// test skipValue
		r.setPosition(0);
		r.skipValue();
		r.close();
	}
}

enum Enum {FOO, BAR};

TEST(Data, ReadHelper)
{
	const char* list[] = {"b", "d", "f"};
	EXPECT_EQ(findIndex(list, 3, StringRef("a")), 3);
	EXPECT_EQ(findIndex(list, 3, StringRef("b")), 0);
	EXPECT_EQ(findIndex(list, 3, StringRef("c")), 3);
	EXPECT_EQ(findIndex(list, 3, StringRef("d")), 1);
	EXPECT_EQ(findIndex(list, 3, StringRef("e")), 3);
	EXPECT_EQ(findIndex(list, 3, StringRef("f")), 2);
	EXPECT_EQ(findIndex(list, 3, StringRef("g")), 3);

	const EnumNameValue<const char*, Enum> enumList[] = {{"BAR", BAR}, {"FOO", FOO}};
	EXPECT_EQ(enumCast(enumList, std::string("FOO")), FOO);	
	EXPECT_EQ(enumCast(enumList, StringRef("BAR")), BAR);	
}

TEST(Data, TextReader)
{
	// write text
	Pointer<IODevice> file = File::create("test.txt");
	file->write("unix\n", 5);
	file->write("windows\r\n", 9);
	file->write("mac\r", 4);
	file->close();

	// read text
	TextReader r("test.txt");
	EXPECT_EQ(r.read(), "unix");
	EXPECT_EQ(r.read(), "windows");
	EXPECT_EQ(r.read(), "mac");
	r.close();
}

TEST(Data, XMLReaderWriter)
{
	// write xml
	{
		XmlWriter w("test.xml");
		
		XMLHeader header;
		header.version = "1.0";
		header.encoding = "UTF-8";
		w.writeHeader(header);

		w.beginStartTag("foo");
		w.endStartTag();
		{
			w.beginBlock();
			w.beginStartTag("bar");
			w.writeAttribute("a1", lexicalCast<std::string>(1));
			w.writeAttribute("a2", lexicalCast<std::string>(1.5));
			w.writeAttribute("a3", "s");
			w.endStartTag();
			w.endTag("bar");
			w.endBlock();
		}
		w.endTag("foo");
		w.close();
	}

	// read xml
	{
		XmlReader r("test.xml");
		
		XMLHeader header;
		r.readHeader(header);
		EXPECT_EQ(header.version, "1.0");
		EXPECT_EQ(header.encoding, "UTF-8");

		while (r.read() != XmlReader::BEGIN_START_TAG);
		EXPECT_EQ(r.getName(), "foo");
		while (r.read() != XmlReader::BEGIN_START_TAG);
		EXPECT_EQ(r.getName(), "bar");
		EXPECT_EQ(r.read(), XmlReader::ATTRIBUTE);
		EXPECT_EQ(r.getName(), "a1");
		EXPECT_EQ(lexicalCast<int>(r.getValue()), 1);
		EXPECT_EQ(r.read(), XmlReader::ATTRIBUTE);
		EXPECT_EQ(r.getName(), "a2");
		EXPECT_EQ(lexicalCast<float>(r.getValue()), 1.5f);
		EXPECT_EQ(r.read(), XmlReader::ATTRIBUTE);
		EXPECT_EQ(r.getName(), "a3");
		EXPECT_EQ(r.getValue(), "s");
		while (r.read() != XmlReader::END_TAG);
		EXPECT_EQ(r.getName(), "bar");
		while (r.read() != XmlReader::END_TAG);
		EXPECT_EQ(r.getName(), "foo");
		r.close();
	}
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
