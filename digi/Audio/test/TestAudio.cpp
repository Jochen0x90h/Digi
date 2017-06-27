#include <gtest/gtest.h>

#include <digi/Utility/Convert.h>
#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/VectorUtility.h>
#include <digi/Utility/foreach.h>
#include <digi/Math/All.h>
#include <digi/System/File.h>
#include <digi/System/Timer.h>
#include <digi/Audio/LineOut.h>
#ifdef HAVE_OGG
#include <digi/Audio/OggVorbisDecoder.h>
#endif

#include "InitLibraries.h"

using namespace digi;


TEST(Audio, InitLibraries)
{
	initLibraries();
	std::string versionInfo = VersionInfo::get();
}

TEST(Audio, LineOut)
{
	// mono float, two devices playing simultaneously
	{
		const int bufferSize = 1024;

		Pointer<LineOut> out1 = LineOut::open(AudioFormat(AudioFormat::FLOAT32, AudioFormat::INTERLEAVED, 1, 44100), 4, bufferSize);
		Pointer<LineOut> out2 = LineOut::open(AudioFormat(AudioFormat::FLOAT32, AudioFormat::INTERLEAVED, 1, 44100), 4, bufferSize);
			
		std::vector<float> buffer(bufferSize);
		float x1 = 0;
		float x2 = 0;
		
		size_t numSamples = 0;
		while (numSamples < 44100 * 3)
		{
			int numFreeBuffers1 = out1->getNumAvailableSamples() / bufferSize;
			for (int bufferIndex = 0; bufferIndex < numFreeBuffers1; ++bufferIndex)
			{
				foreach (float& v, buffer)
				{
					v = sin(x1);
					x1 += 0.1f;
				}
					
				void* b = buffer.data();
				numSamples += out1->writeInterleaved(b, bufferSize);
			}

			int numFreeBuffers2 = out2->getNumAvailableSamples() / bufferSize;
			for (int bufferIndex = 0; bufferIndex < numFreeBuffers2; ++bufferIndex)
			{
				foreach (float& v, buffer)
				{
					// check if samples get clipped by the audio system
					v = sin(x2) * 1.1f;
					x2 += 0.05f;
				}
					
				void* b = buffer.data();
				out2->writeInterleaved(b, bufferSize);
			}

			std::cout << out1->getPlayedTime() << std::endl;
			out1->wait();
			out2->wait();
		}
		out1->close();
		out2->close();
	}		

	// stereo int16
	{
		const int bufferSize = 1024;

		Pointer<LineOut> out = LineOut::open(AudioFormat(AudioFormat::INT16, AudioFormat::INTERLEAVED, 2, 44100), 4, bufferSize);
			
		std::vector<short2> buffer(bufferSize);
		float2 x = {};
			
		size_t numSamples = 0;
		while (numSamples < 44100 * 3)
		{
			int numFreeBuffers = out->getNumAvailableSamples() / bufferSize;
			for (int bufferIndex = 0; bufferIndex < numFreeBuffers; ++bufferIndex)
			{
				foreach (short2& v, buffer)
				{
					v = convert_short2(sin(x) * 32000.0f);
					x += vector2(0.1f, 0.05f);
				}
					
				void* b = buffer.data();
				numSamples += out->writeInterleaved(b, bufferSize);
			}
			std::cout << out->getPlayedTime() << std::endl;
			out->wait();
		}
			
		// drain and wait
		out->drain();
		while (out->isPlaying())
		{
			std::cout << out->getPlayedTime() << std::endl;
			out->wait();
		}
		out->close();
	}		

	// play an ogg vorbis file
	#ifdef HAVE_OGG
	{
		const int bufferSize = 1024;
	
		// create decoder for test audio file
		Pointer<AudioDecoder> decoder = new OggVorbisDecoder("Gut so!.ogg");

		// get its format
		AudioFormat format = decoder->getFormat();

		// open line out devide with format of audio file and 4 buffers
		Pointer<LineOut> out = LineOut::open(format, 4, bufferSize);
		
		// copy data into device
		uint8_t buffer[8192];
		int numRead;
		do
		{
			// read 1024 samples
			numRead = int(decoder->readInterleaved(buffer, 1024));
				
			while (out->getNumAvailableSamples() < numRead)
			{
				// wait until samples for the whole buffer are available
				std::cout << out->getPlayedTime() << std::endl;
				//TS_TRACE(toString(time).c_str());
				out->wait();
			}
					
			out->writeInterleaved(buffer, numRead);
		} while (numRead != 0);//!decoder->isEndOfInput());
		
		// drain and wait
		out->drain();
		while (out->isPlaying())
		{
			std::cout << out->getPlayedTime() << std::endl;
			out->wait();
		}
		
		out->close();
		decoder->close();
	}
	#endif
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
