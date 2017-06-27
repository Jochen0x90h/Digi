#include <windows.h>
#include <Mmreg.h>

#include <digi/Base/Platform.h>
#include <digi/System/Log.h>
#include "../AudioException.h"
#include "../LineOut.h"


namespace digi {

// win32 waveOut implementation of AudioOut
class Win32WaveOut : public LineOut
{
public:

	Win32WaveOut(HWAVEOUT handle, HANDLE eventHandle, AudioFormat format,
		int numBuffers, int numSamplesPerBuffer, int sampleSize)
		: handle(handle), eventHandle(eventHandle), format(format),
		numBuffers(numBuffers), numSamplesPerBuffer(numSamplesPerBuffer), sampleSize(sampleSize), 
		currentBuffer(0), bufferPos(0), winReferencePosition(0), referencePosition(0), numWritten(0)
	{
		int bufferSize = numSamplesPerBuffer * sampleSize;
	
		// allocate sample buffer data
		this->bufferData = (uint8_t*)malloc(numBuffers * bufferSize);
		
		// allocate wave headers
		this->waveHeaders = new WAVEHDR[numBuffers];

		// prepare buffer headers
		for (int i = 0; i < numBuffers; ++i)
		{
			WAVEHDR& waveHdr = this->waveHeaders[i];
			waveHdr.lpData = (LPSTR)(this->bufferData + i * bufferSize);
			waveHdr.dwBufferLength = bufferSize;
			waveHdr.dwBytesRecorded = 0;
			waveHdr.dwUser = 0;
			waveHdr.dwFlags = 0;
			waveHdr.dwLoops = 0;
			waveHdr.lpNext = NULL;
			waveHdr.reserved = 0;

			MMRESULT result = waveOutPrepareHeader(
				handle,
				&waveHdr,
				sizeof(WAVEHDR));

			waveHdr.dwFlags |= WHDR_DONE;
		}
	}
		
	virtual ~Win32WaveOut()
	{
		if (this->handle != NULL)
			this->close();
	}

	virtual AudioFormat getFormat()
	{
		return this->format;
	}

	virtual void close()
	{
		if (this->handle == NULL)
			return;

		// stop playback. from windows doku:
		// All pending playback buffers are marked as done (WHDR_DONE) and returned to the application
		waveOutReset(this->handle);
		
		// unprepare buffer headers
		for (int i = 0; i < this->numBuffers; ++i)
		{
			WAVEHDR* waveHdr = &this->waveHeaders[i];
			waveOutUnprepareHeader(
				this->handle,
				waveHdr,
				sizeof(WAVEHDR));
		}

		// close device
		waveOutClose(this->handle);
		
		// close eventHandle
		CloseHandle(this->eventHandle);

		// free sample buffers
		free(this->bufferData);
		this->bufferData = NULL;
		
		// free wave headers
		delete [] this->waveHeaders;
		this->waveHeaders = NULL;
				
		this->handle = NULL;
		this->eventHandle = NULL;
	}
/*
	virtual int getBufferSize()
	{
		return this->bufferSize;
	}
*/
	virtual State getState()
	{
		State state = STOPPED;
	
		// check if last buffer is not done yet
		if ((this->waveHeaders[(this->currentBuffer + this->numBuffers - 1) % this->numBuffers].dwFlags & WHDR_DONE) == 0)
			state = PLAYING;

		return state;
	}

	virtual void play()
	{
	}

	virtual void pause()
	{
	}
	
	virtual void stop()
	{
	}

	virtual int getNumAvailableSamples()
	{
		int numFreeBuffers = 0;
		for (int i = 0; i < this->numBuffers; ++i)
		{	
			WAVEHDR& waveHdr = this->waveHeaders[(this->currentBuffer + numFreeBuffers) % this->numBuffers];
			if ((waveHdr.dwFlags & WHDR_DONE) == 0)
				break;
			++numFreeBuffers;
		}
		return numFreeBuffers * this->numSamplesPerBuffer - this->bufferPos;
	}

	virtual void wait()
	{
		WaitForSingleObject(this->eventHandle, 1000);
	}

	virtual double getPlayedTime()
	{
		return double(this->getPlayedSamples()) / double(this->format.sampleRate);
	}

	virtual double getBufferedTime()
	{
		return double(this->numWritten + this->bufferPos - this->getPlayedSamples()) / double(this->format.sampleRate);
	}

	int64_t getPlayedSamples()
	{
		MMTIME mmTime;
		mmTime.wType = TIME_BYTES;
		
		MMRESULT result = waveOutGetPosition(this->handle, &mmTime, sizeof(mmTime));
		if (result != MMSYSERR_NOERROR)
		{
			// error
			return 0;
		}
		
		/*if (mmTime.wType == TIME_MS)
		{
			// milli seconds
			return double(mmTime.u.ms) / 1000.0;
		}
		else if (mmTime.wType == TIME_SAMPLES)
		{
			// samples

			// calc relative byte/sample position
			int bytePos = int(mmTime.u.sample * this->sampleSize - this->winReferencePosition);
			int64_t samplePos = int64_t(bytePos) / this->sampleSize;
			
			return double(samplePos + this->referencePosition) / double(this->format.samplesPerSecond);
		}
		else*/ if (mmTime.wType == TIME_BYTES)
		{
			// bytes

			// calc number of samples played from last reference position
			int numBytes = mmTime.u.cb - this->winReferencePosition;
			int numSamples = numBytes / this->sampleSize;
			
			// calc absolute number of samples played 
			return this->referencePosition + numSamples;
		}
		return 0;
	}

	virtual size_t write(const Buffer* buffers, size_t numSamples)
	{
		size_t l = numSamples;
		size_t samplePos = 0;
		while (l > 0)
		{
			WAVEHDR& waveHdr = this->waveHeaders[this->currentBuffer];
			if ((waveHdr.dwFlags & WHDR_DONE) == 0)
			{
				// no buffer is available
				break;
			}

			// calc number of samples to copy
			int toCopy = int(std::min(l, size_t(this->numSamplesPerBuffer - this->bufferPos)));
			
			// copy samples
			if (this->format.layout == AudioFormat::INTERLEAVED)
			{
				uint8_t* src = (uint8_t*)buffers[0] + samplePos * this->sampleSize;
				uint8_t* dst = (uint8_t*)waveHdr.lpData + this->bufferPos * this->sampleSize;
				memcpy(dst, src, toCopy * this->sampleSize);				
			}
			else
			{
				int numChannels = this->format.numChannels;
				for (int channelIndex = 0; channelIndex < numChannels; ++channelIndex)
				{
					switch (this->format.type)
					{
					case AudioFormat::INT8:
						{			
							int8_t* src = (int8_t*)buffers[channelIndex] + samplePos;
							int8_t* dst = (int8_t*)waveHdr.lpData + this->bufferPos * numChannels + channelIndex;
							for (int i = 0; i < toCopy; ++i)
							{
								*dst = *src;
								++src;
								dst += numChannels;
							}
						}
						break;
					case AudioFormat::INT16:
						{			
							int16_t* src = (int16_t*)buffers[channelIndex] + samplePos;
							int16_t* dst = (int16_t*)waveHdr.lpData + this->bufferPos * numChannels + channelIndex;
							for (int i = 0; i < toCopy; ++i)
							{
								*dst = *src;
								++src;
								dst += numChannels;
							}
						}
						break;
					case AudioFormat::FLOAT32:
						{			
							float* src = (float*)buffers[channelIndex] + samplePos;
							float* dst = (float*)waveHdr.lpData + this->bufferPos * numChannels + channelIndex;
							for (int i = 0; i < toCopy; ++i)
							{
								*dst = *src;
								++src;
								dst += numChannels;
							}
						}
						break;
					}
				}
			}			
			l -= toCopy;
			this->bufferPos += toCopy;
			samplePos += toCopy;
			
			// break if current buffer not full yet
			if (this->bufferPos < this->numSamplesPerBuffer)
				break;

			// output the buffer
			MMRESULT result = waveOutWrite(
				this->handle,
				&waveHdr,
				sizeof(WAVEHDR));
			this->numWritten += this->numSamplesPerBuffer;

			// next buffer
			this->currentBuffer = (this->currentBuffer + 1) % this->numBuffers;
			this->bufferPos = 0;
			
			// adjust reference position
			this->winReferencePosition += this->numSamplesPerBuffer * this->sampleSize;
			this->referencePosition += this->numSamplesPerBuffer;
		}
		return numSamples - l;
	}

	virtual void drain()
	{
		// check if there is an incomplete buffer
		if (this->bufferPos > 0)
		{
			WAVEHDR& waveHdr = this->waveHeaders[this->currentBuffer];
		
			// clear remains of buffer
			int toClear = (this->numSamplesPerBuffer - this->bufferPos) * this->sampleSize;
			memset((uint8_t*)waveHdr.lpData + this->bufferPos * this->sampleSize, 0, toClear);

			// output the buffer
			MMRESULT result = waveOutWrite(
				this->handle,
				&waveHdr,
				sizeof(WAVEHDR));
			this->numWritten += this->numSamplesPerBuffer;

			// next buffer
			this->currentBuffer = (this->currentBuffer + 1) % this->numBuffers;
			this->bufferPos = 0;
		}
	}

	
	HWAVEOUT handle;
	HANDLE eventHandle;
	
	AudioFormat format;
	
	// number of buffers
	int numBuffers;
	
	// size of one buffer in samples
	int numSamplesPerBuffer;
	
	// size of one sample
	int sampleSize;


	// data for all buffers
	uint8_t* bufferData;

	// header for each buffer
	WAVEHDR* waveHeaders;
	
	// index of current buffer
	int currentBuffer;
	
	// position in current buffer in samples
	int bufferPos;
	
	// current play position
	DWORD winReferencePosition;
	int64_t referencePosition;

	// number of samples written to audio api
	int64_t numWritten;
};


Pointer<LineOut> LineOut::open(AudioFormat format, int numBuffers, int numSamplesPerBuffer)
{
	WAVEFORMATEX f;
	f.cbSize = 0; // size of extra space behind WAVEFORMATEX

	// type
	switch (format.type)
	{
	case AudioFormat::INT8:
		f.wFormatTag = WAVE_FORMAT_PCM;
		f.wBitsPerSample = 8;
		break;
	case AudioFormat::INT16:
		f.wFormatTag = WAVE_FORMAT_PCM;
		f.wBitsPerSample = 16;
		break;
	case AudioFormat::FLOAT32:
		f.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
		f.wBitsPerSample = 32;
		break;
	}

	// number of channels
	f.nChannels = format.numChannels;

	// samples per second
	f.nSamplesPerSec = format.sampleRate;

	// calc derived values
	int sampleSize = format.numChannels * f.wBitsPerSample >> 3;
	f.nBlockAlign = sampleSize;
	f.nAvgBytesPerSec = f.nSamplesPerSec * f.nBlockAlign;

	// create eventHandle
	HANDLE eventHandle = CreateEvent(
		NULL,  // SD
		false, // reset type (false = auto)
		false, // initial state
		NULL); // object name
	
	// open device
	HWAVEOUT handle;
	MMRESULT result = waveOutOpen(
		&handle,                // reference to handle
		WAVE_MAPPER,            // devide ID
		&f,                     // waveform format
		(DWORD_PTR)eventHandle, // callback mechanism
		0,                      // callback instance
		CALLBACK_EVENT);        // open flags

	if (result != MMSYSERR_NOERROR)
	{
		CloseHandle(eventHandle);
		throw AudioException(result == WAVERR_BADFORMAT ? AudioException::FORMAT_NOT_SUPPORTED : AudioException::AUDIO_ERROR);
	}	

	return new Win32WaveOut(handle, eventHandle, format, numBuffers, numSamplesPerBuffer, sampleSize);
}

} // namespace digi
