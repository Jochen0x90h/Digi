#include <gtest/gtest.h>

#include <boost/version.hpp>
#include <boost/format.hpp>
#include <boost/optional.hpp>

#include <digi/Base/VersionInfo.h>
#include <digi/Utility/ArrayRef.h>
#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/as.h>
#include <digi/Utility/Ascii.h>
#include <digi/Utility/Convert.h>
#include <digi/Utility/lexicalCast.h>
#include <digi/Utility/malloc16.h>
#include <digi/Utility/Object.h>
#include <digi/Utility/SetUtility.h>
#include <digi/Utility/StringUtility.h>
#include <digi/Utility/StringRef.h>
#include <digi/Utility/UtfTranscode.h>
#include <digi/Utility/VectorUtility.h>

#include "InitLibraries.h"

using namespace digi;


class DerivedObject : public Object
{
public:
	virtual ~DerivedObject() {}
};

TEST(Utility, initLibraries)
{
	initLibraries();
	std::string versionInfo = VersionInfo::get();
}

TEST(Utility, Compiler)
{
	// test some compiler functionality

	// first extend, then cast
	int i = -1;
	uint64_t j = i;
	EXPECT_EQ(~j, 0);
	
	// extend to int before calculation
	uint16_t a = 1;
	uint16_t b = 2;
	int d = int(a - b);
	EXPECT_EQ(d, -1);
	a = 50000;
	b = 50000;
	d = int(a + b);
	EXPECT_EQ(d, 100000);
}

// test struct
struct Foo {
	int i;
};

#if BOOST_VERSION / 100000 == 1 && BOOST_VERSION / 100 % 1000 == 56
	// workaround: function is missing in #include <boost/optional.hpp> of boost 1.56.0
	template <class CharType, class CharTrait, class T>
	std::basic_ostream<CharType, CharTrait>&
	boost::operator<<(std::basic_ostream<CharType, CharTrait>& out, optional<T> const& v)
	{
		if (v == null)
			return out << "(null)";
		return out << *v;
	}
#endif

TEST(Utility, boost)
{
	// test some used boost classes
	std::cout << "boost version: " << BOOST_VERSION << std::endl;

	// boost::Format
	EXPECT_EQ(str(boost::format("foo%1%") % 55), "foo55");

	// boost::optional
	{
		// test if optional of struct with no operator == is working (needs at least boost 1.56.0)
		boost::optional<Foo> foo;
		if (foo != null)
		{
		}

		boost::optional<int> a = 55;
		EXPECT_EQ(a, a);
		EXPECT_EQ(a, 55);
		EXPECT_EQ(55, a);
		a = null;
		EXPECT_EQ(a, null);
		EXPECT_EQ(null, a);

		boost::optional<size_t> b = 55;
		EXPECT_EQ(b, b);
		EXPECT_EQ(*b, 55);
		EXPECT_EQ(55, *b);
		b = null;
		EXPECT_EQ(b, null);
		EXPECT_EQ(null, b);
			
		// array access with boost::optional<size_t>
		b = 1;
		int ar[4];
		fill(ar, 0);
		ar[*b] = 33;
		EXPECT_EQ(ar[1], 33);
	}
}

TEST(Utility, ArrayRef)
{
	const int i[3] = {1, 2, 3};
	int j[3] = {1, 2, 3};

	EXPECT_EQ(ArrayRef<const char>("foo").size(), 4);
	EXPECT_EQ(ArrayRef<const int>(i).size(), 3);
	EXPECT_EQ(ArrayRef<int>(j).size(), 3);
}

TEST(Utility, ArrayUtility)
{
	int a[5];
	memset(a, 0, sizeof(a));
	fill(a, 33);

	for (size_t i = 0; i < boost::size(a); ++i)
	{
		EXPECT_EQ(a[i], 33);
	}
}

TEST(Utility, as)
{
	EXPECT_EQ(as<int>(1.0f), 0x3f800000);
}

TEST(Utility, Ascii)
{
	EXPECT_EQ(Ascii::toUpperCase("foo bar 5"), "FOO BAR 5");
	EXPECT_EQ(Ascii::toLowerCase("FOO BAR 5"), "foo bar 5");
	EXPECT_EQ(Ascii::capitalize("foo bar 5"), "Foo Bar 5");
}

TEST(Utility, Convert)
{
	char buf[32];

	// toHexString
	int64_t v = 0x123456789abcdef0LL;
	EXPECT_EQ(toHexString(v), "123456789abcdef0");

	// arg
	EXPECT_EQ(arg("foo%0%0bar % ", 3), "foo33bar % ");

	// toString for double into string buffer
	{
		for (int i = 0; i < 150; ++i)
		{
			fill(buf, 0);
			//double d = -2.295555555 + 0.123 * i;
			double d = -2.29 + 0.12 * i;
			toString(buf, d, 6);
			//dNotify(buf << " " << d);
		}

		*toString(buf, 0.001, 6) = 0;
		EXPECT_STREQ(buf, "0.001");
		*toString(buf, 0.0001, 6) = 0;
		EXPECT_STREQ(buf, "1.0e-4");
		*toString(buf, 0.00001, 6) = 0;
		EXPECT_STREQ(buf, "1.0e-5");
		
		*toString(buf, 10000.0, 6) = 0;
		EXPECT_STREQ(buf, "10000.0");
		*toString(buf, 100000.0, 6) = 0;
		EXPECT_STREQ(buf, "1.0e5");
		*toString(buf, 1000000.0, 6) = 0;
		EXPECT_STREQ(buf, "1.0e6");
		*toString(buf, 10000000.0, 6) = 0;
		EXPECT_STREQ(buf, "1.0e7");

		*toString(buf, 100000.0, 7) = 0;
		EXPECT_STREQ(buf, "100000.0");
	}
}

TEST(Utility, lexicalCast)
{	
	// bool <-> string
	EXPECT_EQ(lexicalCast<bool>("false"), false);
	EXPECT_EQ(lexicalCast<bool>(std::string("true")), true);
	EXPECT_EQ(lexicalCast<bool>("0"), false);
	EXPECT_EQ(lexicalCast<bool>(StringRef("1")), true);
	EXPECT_EQ(lexicalCast<std::string>(false), "false");
	EXPECT_EQ(lexicalCast<std::string>(true), "true");

	// bool <-> wide string
	EXPECT_EQ(lexicalCast<bool>(L"false"), false);
	EXPECT_EQ(lexicalCast<bool>(std::wstring(L"true")), true);
	EXPECT_EQ(lexicalCast<bool>(L"0"), false);
	EXPECT_EQ(lexicalCast<bool>(L"1"), true);
	EXPECT_EQ(lexicalCast<std::wstring>(false), L"false");
	EXPECT_EQ(lexicalCast<std::wstring>(true), L"true");

	// 8 bit int <-> string
	EXPECT_EQ(lexicalCast<int8_t>("-128"), -128);
	EXPECT_EQ(lexicalCast<int8_t>(std::string("127")), 127);
	EXPECT_EQ(lexicalCast<uint8_t>("0"), 0);
	EXPECT_EQ(lexicalCast<uint8_t>(StringRef("255")), 255);
	EXPECT_EQ(lexicalCast<std::string>(255), "255");
	
	// 8 bit int <-> wide string
	EXPECT_EQ(lexicalCast<int8_t>(L"-128"), -128);
	EXPECT_EQ(lexicalCast<int8_t>(std::wstring(L"127")), 127);
	EXPECT_EQ(lexicalCast<uint8_t>(L"0"), 0);
	EXPECT_EQ(lexicalCast<uint8_t>(L"255"), 255);

	// float <-> string
	EXPECT_EQ(lexicalCast<float>("5.5"), 5.5f);
	EXPECT_EQ(lexicalCast<float>(std::string("1e10")), 1e10f);
	EXPECT_EQ(lexicalCast<std::string>(5.5f), "5.5");

	// float <-> wide string
	EXPECT_EQ(lexicalCast<float>(L"5.5"), 5.5f);
	EXPECT_EQ(lexicalCast<float>(std::wstring(L"1e10")), 1e10f);
#ifndef _WIN32_WCE
	EXPECT_EQ(lexicalCast<std::wstring>(5.5f), L"5.5");
#endif

	try
	{
		lexicalCast<uint8_t>("256");
		EXPECT_TRUE(false) << "no exception was thrown";
	}
	catch (boost::bad_lexical_cast& e)
	{
		//std::cout << e.what() << " src " << e.source_type().name() << " dst " << e.target_type().name() << std::endl;
		EXPECT_EQ(e.target_type(), typeid(uint8_t));
	}

	try
	{
		lexicalCast<uint16_t>("65536");
		EXPECT_TRUE(false) << "no exception was thrown";
	}
	catch (boost::bad_lexical_cast& e)
	{
		//std::cout << e.what() << " src " << e.source_type().name() << " dst " << e.target_type().name() << std::endl;
		EXPECT_EQ(e.target_type(), typeid(uint16_t));
	}

	try
	{
		lexicalCast<float>("");
		EXPECT_TRUE(false) << "no exception was thrown";
	}
	catch (boost::bad_lexical_cast& e)
	{
		EXPECT_EQ(e.target_type(), typeid(float));
	}
}

TEST(Utility, malloc16)
{
	for (int i = 0; i < 20; ++i)
	{
		uint8_t* data = (uint8_t*)malloc16(20 + i * 11);		
		size_t p = (size_t)data;
		EXPECT_EQ(p & 15, 0);
		free16(data);
	}
}

TEST(Utility, SetUtility)
{
	{
		std::set<uint16_t> s;
		EXPECT_FALSE(contains(s, 55));
		s.insert(55);
		EXPECT_TRUE(contains(s, 55));
	}
	{
		std::set<int*> s;
		EXPECT_FALSE(contains(s, NULL));
		s.insert((int*)NULL);
		//s.insert(nullptr);
		EXPECT_TRUE(contains(s, NULL));
	}
}

TEST(Utility, StringRef)
{
	// empty constructors
	{
		StringRef r1;
		StringRef r2(null);
		StringRef r3 = StringRef(std::string());
		EXPECT_EQ(r1, "");
		EXPECT_EQ(r2, "");
		EXPECT_EQ(r3, "");

		EXPECT_TRUE(r1.data() != NULL);
		EXPECT_TRUE(r2.data() != NULL);
		EXPECT_TRUE(r3.data() != NULL);
	}
		
	// construct from constant c-string
	{
		StringRef r1("foo");
		EXPECT_EQ(r1, "foo");
		EXPECT_EQ(r1.length(), 3);

		const char* s = "foo";
		StringRef r2(s);
		EXPECT_EQ(r2, "foo");
		EXPECT_EQ(r2.length(), 3);
	}

	// getString
	{
		std::string s1;
		StringRef r(s1);
		std::string s2 = r.getString();
		EXPECT_EQ(s1, s2);
	}
		
	// operator std::string
	{
		std::string s1 = "foo";
		StringRef r(s1);
		std::string s2 = r;
		EXPECT_EQ(s1, s2);
	}		
		
	// operator +
	{
		StringRef a = "a";
		StringRef b = "b";
		EXPECT_EQ(a + b, "ab");
		EXPECT_EQ('a' + b, "ab");
		EXPECT_EQ(a + 'b', "ab");
	}

	// operator !=
	{
		StringRef a = "a";
		StringRef b = "b";
		EXPECT_TRUE(a != b);
	}
}
	
TEST(Utility, StringUtility)
{
	// length
	{
		char buf[10];
		buf[0] = 'x';
		buf[1] = 0;
		EXPECT_EQ(length(buf), 1);
		EXPECT_EQ(length(""), 0);
		EXPECT_EQ(length(L"abc"), 3);
	}
	
	// find
	EXPECT_EQ(find("abc", 'b'), 1);
	EXPECT_EQ(find("abc", 'x'), -1);
	EXPECT_EQ(find("abc", "bc"), 1);
	EXPECT_EQ(find("abc", "xx"), -1);
	
	// findOneOf
	EXPECT_EQ(findOneOf("abc", "cde"), 2);
	EXPECT_EQ(findOneOf("abc", "xyz", 0, -1), -1);
	EXPECT_EQ(findOneOf("abc", "xyz", 3, -1), -1);
		
	// findOneOfRev
	EXPECT_EQ(findOneOfRev("abc", "cde"), 2);
	EXPECT_EQ(findOneOfRev("abc", "xyz"), -1);

	// startsWith
	EXPECT_EQ(startsWith("abc", "ab"), true);
	EXPECT_EQ(startsWith("abc", "abcd"), false);

	// endsWith
	EXPECT_EQ(endsWith("abc", "bc"), true);
	EXPECT_EQ(endsWith("abc", "xabc"), false);

	// substring
	EXPECT_EQ(substring("abc", 1, 2), "b");
	EXPECT_EQ(substring("abc", 1), "bc");
	
	// trim
	EXPECT_EQ(trim(" abc\377 "), "abc\377");
	EXPECT_EQ(trim(" "), "");
	
	// wildcardMatch
	EXPECT_TRUE(wildcardMatch("foo", "foo"));
	EXPECT_TRUE(wildcardMatch("f?o", "foo"));

	EXPECT_TRUE(wildcardMatch("*oo", "oo"));
	EXPECT_TRUE(wildcardMatch("*oo", "foo"));
	EXPECT_TRUE(wildcardMatch("*oo", "fooo"));

	EXPECT_TRUE(wildcardMatch("f*o", "fo"));
	EXPECT_TRUE(wildcardMatch("f*o", "foo"));
	EXPECT_TRUE(wildcardMatch("f*o", "fooo"));
	EXPECT_TRUE(wildcardMatch("f*o", "fxyzo"));
	EXPECT_TRUE(wildcardMatch("f*o*", "fxyzoabc"));

	EXPECT_TRUE(wildcardMatch("fo*", "fo"));
	EXPECT_TRUE(wildcardMatch("fo*", "foo"));
	EXPECT_TRUE(wildcardMatch("fo*", "fooo"));

	EXPECT_TRUE(wildcardMatch("?*?", "abc"));
	EXPECT_TRUE(wildcardMatch("a?c", utfString<char>(L"a\u00E4c"))); // a umlaut

	EXPECT_FALSE(wildcardMatch("f*o", "fox"));
}
	
TEST(Utility, Pointer)
{
	Pointer<Object> o = new Object();
		
	// cast to derived object which must fail
	try
	{
		Pointer<DerivedObject> d = cast<DerivedObject>(o);
		EXPECT_TRUE(false) << "no exception was thrown";
	}
	catch (std::bad_cast& e)
	{
		const char* str = e.what();
		EXPECT_STREQ(str, "bad cast");
	}
}

TEST(Utility, UtfTranscode)
{
	UtfEncoder<1> encodeUtf8;
	UtfDecoder<1> decodeUtf8;
	for (int32_t ch = 10; ch <= 1000000000; ch *= 10)
	{
		char buffer[10];
		char* buf = buffer;
		encodeUtf8(buf, ch);
			
		buf = buffer;
		int32_t ch2 = decodeUtf8(buf);
			
		EXPECT_EQ(ch, ch2);
	}
		
	UtfEncoder<2> encodeUtf16;
	UtfDecoder<2> decodeUtf16;
	for (int32_t ch = 10; ch <= 1000000; ch *= 10)
	{
		uint16_t buffer[10];
		uint16_t* buf = buffer;
		encodeUtf16(buf, ch);
			
		buf = buffer;
		int32_t ch2 = decodeUtf16(buf);
			
		EXPECT_EQ(ch, ch2);
	}
	
	// transcode

	// umlaut A O U a o u eszet
	EXPECT_EQ(utfString<char>(utfVector<int>(L"\u00C4 \u00D6 \u00DC  \u00E4 \u00F6 \u00FC  \u00DF")),
		"\xC3\x84 \xC3\x96 \xC3\x9C  \xC3\xA4 \xC3\xB6 \xC3\xBC  \xC3\x9F");

	EXPECT_EQ(utfString<char>("fo\xc3\xb6"), "fo\xc3\xb6");
	EXPECT_EQ(utfString<char>(std::string("fo\xc3\xb6")), "fo\xc3\xb6");
	EXPECT_EQ(utfString<char>(StringRef("fo\xc3\xb6")), "fo\xc3\xb6");
	EXPECT_EQ(utfString<char>(L"fo\u00F6"), "fo\xc3\xb6");
	EXPECT_EQ(utfString<char>(std::wstring(L"fo\u00F6")), "fo\xc3\xb6");

	EXPECT_EQ(utfString<wchar_t>("fo\xc3\xb6"), L"fo\u00F6");
	EXPECT_EQ(utfString<wchar_t>(std::string("fo\xc3\xb6")), L"fo\u00F6");
	EXPECT_EQ(utfString<wchar_t>(StringRef("fo\xc3\xb6")), L"fo\u00F6");
	EXPECT_EQ(utfString<wchar_t>(L"fo\u00F6"), L"fo\u00F6");
	EXPECT_EQ(utfString<wchar_t>(std::wstring(L"fo\u00F6")), L"fo\u00F6");

	EXPECT_EQ(utfString<char>(utfVector<int>("fo\xc3\xb6")), "fo\xc3\xb6");
}


int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
