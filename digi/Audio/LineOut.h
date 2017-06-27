#ifndef digi_Audio_LineOut_h
#define digi_Audio_LineOut_h

#include <digi/Utility/IntUtility.h>
#include "AudioOut.h"


namespace digi {

/// @addtogroup Audio
/// @{

/// audio output device for mono, stereo or multiple channels. typically drives the line level audio outputs.
/// works stream oriented on internal buffers
class LineOut : public AudioOut
{
public:

	/// open an audio output device. bufferSize is given in samples. latency is numBuffers * bufferSize / samplesPerSecond
	static Pointer<LineOut> open(AudioFormat format, int numBuffers, int numSamplesPerBuffer);

	/// open an audio ouput device. The duration in seconds that can be buffered in advance is given by bufferedLength
	static Pointer<LineOut> open(AudioFormat format, int numBuffers, double bufferedLength)
	{
		return open(format, numBuffers, int(format.sampleRate * bufferedLength / double(numBuffers)))	;
	}

	/// destructor
	virtual ~LineOut();

// state

	enum State
	{
		STOPPED,
		PLAYING,
		PAUSED
	};

	/// returns true if audio output is currently playing
	virtual State getState() = 0;
	bool isPlaying() {return getState() == PLAYING;}
	bool isPaused() {return getState() == PAUSED;}

	/// start playing
	virtual void play() = 0;

	/// pause playback. use play() to continue
	virtual void pause() = 0;
	
	/// stop playback
	virtual void stop() = 0;


	/// get number of available samples that are free and need to be written
	virtual int getNumAvailableSamples() = 0;

	/// wait until the next buffer becomes available
	virtual void wait() = 0;

// time

	/// returns the time already played in seconds
	virtual double getPlayedTime() = 0;
	
	/// returns the time that is buffered
	virtual double getBufferedTime() = 0;

// write

	/// stop after queued samples have been played
	virtual void drain() = 0;
};

/// @}

} // namespace digi

#endif
