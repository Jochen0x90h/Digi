#include <alsa/asoundlib.h>

#include "../WaveOut.h"


namespace digi {

// ALSA implementation of AudioDevice, see www.alsa-project.org
class ALSAWaveOut : public AudioDevice
{
public:

	ALSAWaveOut(snd_pcm_t* handle, AudioFormat format, int bufferSize)
	: handle(handle), format(format), numBuffers(numBuffers), bufferSize(bufferSize)
	{
		this->sampleSize = format.getSampleSize();
	}
	
	virtual ~ALSAWaveOut()
	{
		if (this->handle != NULL)
			this->close();
	}

	virtual AudioFormat getFormat()
	{
		return this->format;
	}
	
	virtual int getBufferSize()
	{
		return this->bufferSize;
	}
	
	virtual int getAvailableSamples()
	{
		return snd_pcm_avail(this->handle);
	}
	
	virtual size_t readData(void* data, size_t length)
	{
		return 0;
	}

	virtual size_t writeData(const void* data, size_t length)
	{
		int len = int(length) / this->sampleSize;

		// returns -1 on buffer underrun
		int numWritten;
		while ((numWritten = snd_pcm_writei(this->handle, buffer, this->bufferSize)) < 0)
			snd_pcm_prepare(this->handle);
		return numWritten * this->sampleSize;		
	}
	
	virtual double getTime()
	{
		snd_pcm_status_t* status;
		snd_pcm_status_alloca(&status);
		if (snd_pcm_status(handle, status) < 0)
		{
			// error
			return -1;
		}
		
		snd_timestamp_t timestamp;
		snd_pcm_status_get_tstamp(status, timestamp);	
		return double(timestamp.tv_sec) + double(timestamp.tv_usec) * 1e-6;
	}
	
	virtual void drain()
	{
		snd_pcm_drain(this->handle);
	}
	
	virtual void wait()
	{
		snd_pcm_wait(this->handle, 1000);
	}
	
	virtual void close()
	{
		if (this->handle == NULL)
			return;

		// close handle
		snd_pcm_close(this->handle);

		this->handle = NULL;
	}

	virtual int getState()
	{
		int state = 0;
		if (this->handle != NULL)
			state |= OPEN;
	
		snd_pcm_state_t state = snd_pcm_state(this->handle);
		if (state == SND_PCM_STATE_RUNNING || state == SND_PCM_STATE_DRAINING)
			state |= PLAYING;
		
		return state;
	}
	
	snd_pcm_t* handle;
	
	AudioFormat format;

	// size of one buffer
	int bufferSize;

	// size of one sample
	int sampleSize;	
};


Pointer<AudioDevice> WaveOut::open(AudioFormat format, int numBuffers, int bufferSize)
{
	// http://www.suse.de/~mana/alsa090_howto.html
	const char* deviceName = "plughw:0,0";
	
	snd_pcm_hw_params_t* hwParams;
	snd_pcm_hw_params_alloca(&hwParams);
	
	snd_pcm_t* handle;
	if (snd_pcm_open(&handle, deviceName, SND_PCM_STREAM_PLAYBACK, 0) < 0)
	{
		// error
		return null;
	}

	// init params
	snd_pcm_hw_params_any(handle, hwParams);
	
	// interleaved
	if (snd_pcm_hw_params_set_access(handle, hwParams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
		goto error;
	
	// type
	int alsaType;
	int sampleSize;
	switch (format.type)
	{
	case AudioFormat::INT8:
		alsaType = SND_PCM_FORMAT_S8 
		break;
	case AudioFormat::INT16:
		alsaType = SND_PCM_FORMAT_S16
		break;
	case AudioFormat::FLOAT32:
		alsaType = SND_PCM_FORMAT_FLOAT
		break;
	default:
		return null;
	}
	if (snd_pcm_hw_params_set_format(handle, hwParams, alsaType) < 0)
		goto error;

	// number of channels
	if (snd_pcm_hw_params_set_channels(handle, hwParams, format.numChannels) < 0)
		goto error;

	// rate
	int rate = format.samplesPerSecond;
	if (snd_pcm_hw_params_set_rate_near(handle, hwParams, &rate, 0) < 0)
		goto error;
	
	// number of buffers
	if (snd_pcm_hw_params_set_periods(handle, hwParams, numBuffers, 0) < 0)
		goto error;

	// buffer size
	if (snd_pcm_hw_params_set_buffer_size(handle, hwParams, numBuffers * bufferSize) < 0)
		goto error;

	// apply params
	if (snd_pcm_hw_params(handle, hwParams) < 0)
		goto error;
		
	return new ALSAWaveOut(handle, samplesPerSecond, numChannels, channelMask, numBuffers, bufferSize);
	
error:
	// close handle
	snd_pcm_close(handle);
	return null;
}

} // namespace digi
