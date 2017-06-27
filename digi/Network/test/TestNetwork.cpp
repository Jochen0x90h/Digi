#include <gtest/gtest.h>

#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/StringUtility.h>
#include <digi/System/IOException.h>
#include <digi/Network/Socket.h>

#include "InitLibraries.h"

using namespace digi;


TEST(Network, InitLibraries)
{
	initLibraries();
	std::string versionInfo = VersionInfo::get();
}

TEST(Network, SocketIPClient)
{
	// connect to a webserver
	try
	{
		Pointer<Socket> socket = Socket::create(Socket::IP);
		socket->connect("de.wikipedia.org", 80);

		const char s[] = "GET /wiki/Http HTTP/1.1\nHost: de.wikipedia.org\n\n";
		size_t numWritten = socket->write(s, length(s));
		EXPECT_EQ(numWritten, length(s));

		char buf[257];
		size_t numRead = socket->read(buf, 256);
		buf[numRead] = 0;
		EXPECT_EQ(substring(buf, 0, 8), "HTTP/1.0");

		socket->close();
	}
	catch (IOException& e)
	{
		EXPECT_TRUE(false) << "failed to connect: " << e.what();
	}
		
	// connect to unknown host, must fail
	try
	{
		Pointer<Socket> socket = Socket::create(Socket::IP);
		socket->connect("unknown", 80);
		EXPECT_TRUE(false) << "no exception was thrown";
	}
	catch (IOException& e)
	{
		EXPECT_EQ(e.getReason(), IOException::UNKNOWN_HOST);
		EXPECT_STREQ(e.what(), "unknown host");
	}
}

TEST(Network, SocketIPServer)
{
	// server
	try
	{
		Pointer<Socket> server = Socket::create(Socket::IP);
		server->bind(8080);
		server->listen(5);
			
		Pointer<Socket> client = Socket::create(Socket::IP);
		client->connect("127.0.0.1", 8080);
			
		std::string address;
		int port;
		Pointer<IODevice> dev = server->accept(address, port);
		EXPECT_EQ(address, "127.0.0.1");
			
		char buffer[4];
		client->write("foo", 4);
		dev->read(buffer, 4);
		EXPECT_EQ((const char*)buffer, std::string("foo"));
			
		// shut down client send which must result in reading of zero bytes at server
		client->shutdown(Socket::SEND);
		size_t numRead = dev->read(buffer, 4);
		EXPECT_EQ(numRead, 0);
		//EXPECT_TRUE(dev->isEndOfInput());
			
		client->close();
		server->close();
		dev->close();
	}
	catch (IOException& e)
	{
		EXPECT_TRUE(false) << e.what();
	}
}

TEST(Network, SocketUDPTransfer)
{
	// transfer an udp packet
	try
	{	
		Pointer<Socket> socket1 = Socket::create(Socket::UDP);
		socket1->bind(8081);
		socket1->connect("127.0.0.1", 8082);
			
		Pointer<Socket> socket2 = Socket::create(Socket::UDP);
		socket2->bind(8082);
		socket2->connect("127.0.0.1", 8081);
			
		char buffer[4];
		socket1->write("foo", 4);
		socket2->read(buffer, 4);
		EXPECT_STREQ((const char*)buffer, "foo");
			
		socket2->write("bar", 4);
		socket1->read(buffer, 4);
		EXPECT_STREQ((const char*)buffer, "bar");

		socket1->close();
		socket2->close();
	}
	catch (IOException& e)
	{
		EXPECT_TRUE(false) << e.what();
	}
}

TEST(Network, SocketUDPBroadcast)
{
	// broadcast an udp packet
	try
	{	
		Pointer<Socket> socket1 = Socket::create(Socket::UDP);
		socket1->setBroadcast(8082);
			
		Pointer<Socket> socket2 = Socket::create(Socket::UDP);
		socket2->bind(8082);
			
		char buffer[4];
		socket1->write("foo", 4);
		socket2->read(buffer, 4);
		EXPECT_STREQ((const char*)buffer, "foo");
			
		socket1->close();
		socket2->close();
	}
	catch (IOException& e)
	{
		EXPECT_TRUE(false) << e.what();
	}		
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
