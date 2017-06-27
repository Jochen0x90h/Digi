#include <gtest/gtest.h>

#include <boost/bind.hpp>
#include <boost/format.hpp>

#include <digi/Utility/ListUtility.h>
#include <digi/Utility/foreach.h>
#include <digi/System/Log.h>
#include <digi/System/ConsoleLogChannel.h>
#include <digi/System/Timer.h>
#include <digi/Audio/AudioIn.h>
#include <digi/Audio/LineOut.h>
#include <digi/Video/VideoIn.h>
#include <digi/Video/VideoTexture.h>
#ifdef HAVE_OGG
	#include <digi/Video/VorbisEncoder.h>
	#include <digi/Video/VorbisDecoder.h>
#endif
#ifdef HAVE_VPX
	#include <digi/Video/VPXEncoder.h>
	#include <digi/Video/VPXDecoder.h>
#endif
#include <digi/Video/WebMDecoder.h>
#include <digi/Video/WebMEncoder.h>
#include <digi/Display/Display.h>

#include "InitLibraries.h"

using namespace digi;

TEST(Video, InitLibraries)
{
	initLibraries();
	std::string versionInfo = VersionInfo::get();
	Log::addChannel(new ConsoleLogChannel());
}

// test video input that generates sine wave
class TestVideoIn : public VideoIn
{
public:

	TestVideoIn(float frameRate)
		: frameRate(frameRate), phase()
	{
		this->y = new uint8_t[128*128];
		this->u = new uint8_t[64*64];
		this->v = new uint8_t[64*64];
	}
	
	virtual ~TestVideoIn()
	{
	}
		
	virtual VideoFormat getFormat()
	{
		return VideoFormat(VideoFormat::YV12, 128, 128, this->frameRate);
	}
	
	virtual void close()
	{
	}

	virtual bool read(Plane* planes)
	{
		planes[0].data = this->y;
		planes[0].width = 128;
		planes[0].height = 128;
		planes[1].data = this->u;
		planes[1].width = 64;
		planes[1].height = 64;
		planes[2].data = this->v;
		planes[2].width = 64;
		planes[2].height = 64;

		for (int j = 0; j < 128; ++j)
		{
			uint8_t* dst = (uint8_t*)planes[0].data + planes[0].width * j;
			for (int i = 0; i < 128; ++i)
			{
				*dst = uint8_t(127.5f + 60.0f * sin(float(i) * 0.2f + this->phase));
				++dst;
			}
		}
				
		memset(planes[1].data, 128, planes[1].width * planes[1].height);
		memset(planes[2].data, 128, planes[2].width * planes[2].height);
		
		this->phase += 0.1f;
		return true;
	}

	virtual void reset()
	{
		this->phase = 0.0f;
	}

protected:
	
	float frameRate;
	float phase;
	
	uint8_t* y;
	uint8_t* u;
	uint8_t* v;
};

// test audio input that generates sine wave
class TestAudioIn : public AudioIn
{
public:

	TestAudioIn(int sampleRate)
		: sampleRate(sampleRate), lPhase(), rPhase()
	{
	}
	
	virtual ~TestAudioIn()
	{
	}

	virtual AudioFormat getFormat()
	{
		return AudioFormat(AudioFormat::FLOAT32, AudioFormat::INTERLEAVED, 2, this->sampleRate);
	}
		
	virtual void close()
	{
	}
	
	virtual size_t read(Buffer* buffers, size_t numSamples)
	{
		float* samples = (float*)buffers[0];
		for (size_t i = 0; i < numSamples; ++i)
		{
			samples[0] = float(0.4 * sin(this->lPhase));
			samples[1] = float(0.2 * sin(this->rPhase));
			samples += 2;
			this->lPhase += 500.0 * 6.3 / 44100.0;
			this->rPhase += 1000.0 * 6.3 / 44100.0;
		}
		return numSamples;
	}

protected:
	
	int sampleRate;
	double lPhase;
	double rPhase;
};

// test text input for subtitles
class TestTextIn : public MediaEncoder
{
public:

	TestTextIn()
		: index()
	{
	
	}
	
	virtual void close()
	{
	}

	virtual void encode(Packet& packet)
	{
		this->subtitle = str(boost::format("Index%1%") % this->index);
		
		packet.time = double(this->index);
		packet.type = Packet::NORMAL;
		
		if ((this->index & 1) == 0)
		{
			packet.data = this->subtitle.data();
			packet.size = this->subtitle.size();
		}
		else
		{
			packet.data = " ";
			packet.size = 1;
		}
		
		++this->index;
	}
	
	
	int index;
	std::string subtitle;
};


TEST(Video, WebMEncoder)
{
	double duration = 10;
	
	// info
	webm::Info info;
	info.timeCodeScale = 1000000;
	info.muxingApp = "Inka3D";
	info.writingApp = "Inka3D";
	
	// tracks
	webm::Tracks tracks;
	std::map<int, Pointer<MediaEncoder> > encoders;

	// video track
#ifdef HAVE_VPX
	webm::TrackEntry& videoTrack = add(tracks.trackEntries);
	videoTrack.trackNumber = 1;
	videoTrack.trackType = 1; // video
	videoTrack.codecID = "V_VP8";
	videoTrack.video = webm::Video();
	webm::Video& video = *videoTrack.video;
	video.pixelWidth = 128;
	video.pixelHeight = 128;
	video.stereoMode = 0;
	video.frameRate = 25.0f;
	Pointer<TestVideoIn> videoIn = new TestVideoIn(video.frameRate);
	Pointer<VPXEncoder> vpxEncoder = new VPXEncoder(videoIn);
	vpxEncoder->doFirstPassAndInitForSecondPass(256, duration); // 256 kbit/s
	encoders[videoTrack.trackNumber] = vpxEncoder;
#endif
	
	// audio track
#ifdef HAVE_OGG
	webm::TrackEntry& audioTrack = add(tracks.trackEntries);
	audioTrack.trackNumber = 2;
	audioTrack.trackType = 2; // audio
	audioTrack.codecID = "A_VORBIS";
	audioTrack.audio = webm::Audio();
	webm::Audio& audio = *audioTrack.audio;
	audio.samplingFrequency = 44100;
	audio.channels = 2;
	Pointer<TestAudioIn> audioIn = new TestAudioIn(int(audio.samplingFrequency));
	Pointer<VorbisEncoder> vorbisEncoder = new VorbisEncoder(audioIn, audioTrack.codecPrivate);
	encoders[audioTrack.trackNumber] = vorbisEncoder;
#endif

	// subtitle track
	webm::TrackEntry& subtitleTrack = add(tracks.trackEntries);
	subtitleTrack.trackNumber = 3;
	subtitleTrack.trackType = 0x11; // subtitle
	subtitleTrack.codecID = "S_TEXT/UTF8";
	Pointer<TestTextIn> textEncoder = new TestTextIn();
	encoders[subtitleTrack.trackNumber] = textEncoder;

	// encode 10 seconds
	Pointer<WebMEncoder> encoder = new WebMEncoder("test.webm", info, tracks);
	encoder->encode(encoders, duration);
	encoder->finishSegment(info, tracks);
	encoder->close();
	
	// close encoders
	typedef std::pair<const int, Pointer<MediaEncoder> > EncoderPair;
	foreach (EncoderPair& p, encoders)
		p.second->close();	
}

// video player
Pointer<Display> display;
Pointer<VideoTexture> videoOut;

GLuint vb;
GLuint va;
GLuint program;

void draw()
{
	int2 size = display->getSize();
	glViewport(0, 0, size.x, size.y);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram(program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, videoOut == null ? 0 : videoOut->getTexture());
#if GL_MAJOR == 2
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);	
#else
	glBindVertexArray(va);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
#endif
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);

	// bring backbuffer to front
	display->update();
}

void play(const fs::path& path)
{
	webm::Info info;
	webm::Tracks tracks;

	Pointer<WebMDecoder> decoder = new WebMDecoder(path, info, tracks);

	// set decoders for tracks
	int videoTrackNumber = -1;
	Pointer<LineOut> audioOut;
	foreach (webm::TrackEntry& track, tracks.trackEntries)
	{
	#ifdef HAVE_VPX
		if (videoOut == null && track.trackType == 1 && track.video != null)
		{
			webm::Video& video = *track.video;
			Pointer<VPXDecoder> vpxDecoder = new VPXDecoder(video.pixelWidth, video.pixelHeight, video.frameRate);
			videoOut = new VideoTexture(vpxDecoder->getFormat());
			vpxDecoder->setOutput(videoOut);
			decoder->setDecoder(track.trackNumber, vpxDecoder);
			videoTrackNumber = track.trackNumber;
		}
	#endif
	#ifdef HAVE_OGG
		if (audioOut == null && track.trackType == 2 && track.audio != null)
		{
			webm::Audio& audio = *track.audio;
			Pointer<VorbisDecoder> vorbisDecoder = new VorbisDecoder(track.codecPrivate);
			audioOut = LineOut::open(vorbisDecoder->getFormat(), 8, 0.2);//8, 1024);
			vorbisDecoder->setOutput(audioOut);
			decoder->setDecoder(track.trackNumber, vorbisDecoder);
		}
	#endif
	}

	// triangle for rendering video frame onto screen
	glGenBuffers(1, &vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	const float tri[] = {0,0,2,0,0,2};
	//float quad[] = {0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0};
	glBufferData(GL_ARRAY_BUFFER, sizeof(tri), tri, GL_STATIC_DRAW);

#if GL_MAJOR >= 3
	glGenVertexArrays(1, &va);
	glBindVertexArray(va);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
#endif

#if GL_MAJOR == 2
	GLuint vertexShader = createVertexShader(
		"attribute vec2 p;"
		"varying vec2 uv;"
		"void main(){"
			"gl_Position = vec4(p * 2.0 - 1.0, 0.0, 1.0);"
			"uv = p * vec2(1, -1) + vec2(0, 1);"
		"}", "copyVideo");
#else
	GLuint vertexShader = createVertexShader("#version 150\n"
		"in vec2 p;"
		"out vec2 uv;"
		"void main(){"
			"gl_Position = vec4(p * 2.0 - 1.0, 0.0, 1.0);"
			"uv = p * vec2(1, -1) + vec2(0, 1);"
		"}", "copyVideo");
#endif
	// shader for rendering video frame onto screen
	{
	#if GL_MAJOR == 2
		GLuint pixelShader = createPixelShader(
			"uniform sampler2D t;"
			"varying vec2 uv;"
			"void main (void){"
			"gl_FragColor = texture2D(t, uv);"
			"}", "copyVideo");
	#else
		GLuint pixelShader = createPixelShader("#version 150\n"
			"uniform sampler2D t;"
			"in vec2 uv;"
			"out vec4 color;"
			"void main (void){"
				"color = texture(t, uv);"
			"}", "copyVideo");
	#endif
		program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, pixelShader);
		glBindAttribLocation(program, 0, "p");
		glLinkProgram(program);
		glDeleteShader(pixelShader);
		glUseProgram(program);
		glUniform1i(glGetUniformLocation(program, "t"), 0);
	}
	glDeleteShader(vertexShader);

	// get key handles
	int esc = display->getHandle("key.escape");
	int fKey = display->getHandle("key.f");
	int keyLeft = display->getHandle("key.left");
	int keyRight = display->getHandle("key.right");
	std::map<int, double> seekFactors;
	seekFactors[display->getHandle("key.0")] = 0.0;
	seekFactors[display->getHandle("key.1")] = 0.1;
	seekFactors[display->getHandle("key.2")] = 0.2;
	seekFactors[display->getHandle("key.3")] = 0.3;
	seekFactors[display->getHandle("key.4")] = 0.4;
	seekFactors[display->getHandle("key.5")] = 0.5;
	seekFactors[display->getHandle("key.6")] = 0.6;
	seekFactors[display->getHandle("key.7")] = 0.7;
	seekFactors[display->getHandle("key.8")] = 0.8;
	seekFactors[display->getHandle("key.9")] = 0.9;
	int mouse = display->getHandle("mouse.left");
	
	// decode
	int startTime = Timer::getMilliSeconds();
	double time = 0;
	//bool slept = false;
	while (true)
	{
		if (audioOut == null)
		{
			int t = Timer::getMilliSeconds();
			time += (t - startTime) * 1e-3;
			startTime = t;
		}
		else
		{
			time += std::max(0.1 - audioOut->getBufferedTime(), 0.0);
		}
		
		// decode
		if (decoder->decode(time))
			break;

		// simulate a hang for 2 seconds
		/*if (time > 3 && !slept)
		{
			Timer::milliSleep(2000);
			slept = true;
		}*/
	
		// draw current frame
		draw();

		// event handling
		int event = display->getEvent();
		if (event > 0)
			dNotify(event);
			
		if (event == Display::EVENT_CLOSE || (event == esc && display->getBool(event)))
			break;
			
		if (event == fKey && display->getBool(fKey))
		{
			display->toggleFullscreen();
		}
		else if (event == keyLeft && display->getBool(keyLeft))
		{
			time = std::max(time - 10.0, 0.0);
			decoder->seek(videoTrackNumber, time);
		}
		else if (event == keyRight && display->getBool(keyRight))
		{
			time = std::min(time + 10.0, decoder->getDuration());
			decoder->seek(videoTrackNumber, time);
		}

		std::map<int, double>::iterator it = seekFactors.find(event);
		if (videoTrackNumber != -1 && it != seekFactors.end() && display->getBool(event))
		{
			time = decoder->getDuration() * it->second;
			decoder->seek(videoTrackNumber, time);
		}
		
		if (event == mouse && display->getBool(event))
		{
			double duration = decoder->getDuration();
			time = ceil(10.0 * time / duration) * duration / 10.0;
			if (time >= duration - 0.001)
				time = 0.0;
			decoder->seek(videoTrackNumber, time);
		}
	}

	decoder->close();
	
	// videoOut is global
	videoOut = null;
	//if (audioOut != null)
	//	audioOut->close();
	//audioOut = null;
	
	// delete gl resources
	glDeleteBuffers(1, &vb);
	glDeleteProgram(program);
	
	// close decoders
	//typedef std::pair<const int, Pointer<MediaDecoder> > DecoderPair;
	//foreach (DecoderPair& p, decoders)
	//	p.second->close();
}

TEST(Video, WebMDecoder)
{
	display = Display::open("TestVideo", 1024, 768);
	if (display == null)
		return;

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glDisable(GL_DEPTH_TEST);
		
	display->onpaint.connect(draw);
	
	if (fs::exists("big_buck_bunny_480p.webm"))
		play("big_buck_bunny_480p.webm");
	play("test.webm");
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
