#include <pthread.h>

#include <vector>

#include <CoreAudio/CoreAudio.h>
#include <AudioToolbox/AudioToolbox.h>

#include "../LineOut.h"


namespace digi {

// apple core audio implementation of AudioOut
class CoreAudioOut : public LineOut
{
public:

	CoreAudioOut(AudioFormat format, int numBuffers, int numSamplesPerBuffer)
		: format(format), numBuffers(numBuffers), freeBuffers(numBuffers), numFreeBuffers(numBuffers), gotFree(false), currentBuffer(),
			numSamplesPerBuffer(numSamplesPerBuffer), bufferPos(0), offset(0.0), numWritten(0.0)
	{
		// core audio format
		AudioStreamBasicDescription f;
		f.mSampleRate = format.sampleRate;
		f.mFormatID = kAudioFormatLinearPCM;
		
		// type
		switch (format.type)
		{
		case AudioFormat::INT8:
			f.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger;
			f.mBitsPerChannel = 8;
			break;
		case AudioFormat::INT16:
			f.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger;
			f.mBitsPerChannel = 16;
			break;
		case AudioFormat::FLOAT32:
			f.mFormatFlags = kLinearPCMFormatFlagIsFloat;
			f.mBitsPerChannel = 32;
			break;
		}
		
		f.mChannelsPerFrame = format.numChannels;
		this->sampleSize = f.mBytesPerFrame = format.numChannels * (f.mBitsPerChannel >> 3);
		f.mFramesPerPacket = 1;
		f.mBytesPerPacket = f.mBytesPerFrame;
		f.mReserved = 0;

		// create audio queue
		OSStatus err = AudioQueueNewOutput(&f, CoreAudioOut::callback, this, NULL, NULL, 0, &this->audioQueue);
	
		// create buffers	
		int bufferSize = numSamplesPerBuffer * sampleSize;
		for (int i = 0; i < numBuffers; ++i)
		{
			err = AudioQueueAllocateBuffer(this->audioQueue, bufferSize, &this->freeBuffers[i]);
			this->freeBuffers[i]->mAudioDataByteSize = bufferSize;
		}
		
		// start
		err = AudioQueueStart(this->audioQueue, NULL);
		
		// create mutex and condition
		pthread_mutex_init(&this->mutex, NULL);
		pthread_cond_init(&this->condition, NULL);
	}
		
	virtual ~CoreAudioOut()
	{
		this->close();
	}

	static void callback(void* userData, AudioQueueRef audioQueue, AudioQueueBufferRef buffer)
	{
		CoreAudioOut* self = (CoreAudioOut*)userData;
		
		pthread_mutex_lock(&self->mutex);

		// put buffer that became free into list of free buffers
		self->freeBuffers[self->numFreeBuffers] = buffer;
		++self->numFreeBuffers;
		self->gotFree = true;

		pthread_mutex_unlock(&self->mutex);

		// signal to main thread
		pthread_cond_signal(&self->condition);
	}

	virtual AudioFormat getFormat()
	{
		return this->format;
	}

	virtual void close()
	{
		if (this->audioQueue == NULL)
			return;

		// close audio queue
		AudioQueueDispose(this->audioQueue, true);
		
		// close thread and condition
		pthread_mutex_destroy(&this->mutex);
		pthread_cond_destroy(&this->condition);
				
		this->audioQueue = NULL;
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
		pthread_mutex_lock(&this->mutex);
		if (this->numFreeBuffers < this->numBuffers)
			state = PLAYING;
		pthread_mutex_unlock(&this->mutex);
		
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
		int numFreeBuffers;
		pthread_mutex_lock(&this->mutex);
		numFreeBuffers = this->numFreeBuffers;
		pthread_mutex_unlock(&this->mutex);
		return numFreeBuffers * this->numSamplesPerBuffer - this->bufferPos;
	}

	virtual void wait()
	{
		pthread_mutex_lock(&this->mutex);
		//int numFreeBuffers = this->numFreeBuffers;
		//while (this->numFreeBuffers == numFreeBuffers)
		while (!this->gotFree)
		{
			pthread_cond_wait(&this->condition, &this->mutex);
		}
		this->gotFree = false;
		pthread_mutex_unlock(&this->mutex);
	}

	virtual double getPlayedTime()
	{
		AudioTimeStamp timeStamp;
		OSStatus result = AudioQueueGetCurrentTime(
			this->audioQueue,
			NULL,
			&timeStamp,
			NULL);
		double playedSamples = std::min(timeStamp.mSampleTime - this->offset, this->numWritten);

		return playedSamples / double(this->format.sampleRate);
	}

	virtual double getBufferedTime()
	{
		AudioTimeStamp timeStamp;
		OSStatus result = AudioQueueGetCurrentTime(
			this->audioQueue,
			NULL,
			&timeStamp,
			NULL);
		double playedSamples = std::min(timeStamp.mSampleTime - this->offset, this->numWritten);

		return (this->numWritten + this->bufferPos - playedSamples) / double(this->format.sampleRate);
	}

	virtual size_t write(const Buffer* buffers, size_t numSamples)
	{
		size_t l = numSamples;
		size_t samplePos = 0;
		while (l > 0)
		{
			if (this->currentBuffer == NULL)
			{
				// try to get a buffer
				pthread_mutex_lock(&this->mutex);
				if (this->numFreeBuffers > 0)
				{
					--this->numFreeBuffers;
					this->currentBuffer = this->freeBuffers[this->numFreeBuffers];
				}
				pthread_mutex_unlock(&this->mutex);
				if (this->currentBuffer == NULL)
				{
					// no buffer is available
					break;
				}
			}

			// calc number of samples to copy
			size_t toCopy = std::min(l, size_t(this->numSamplesPerBuffer - this->bufferPos));
			
			// copy samples
			if (this->format.layout == AudioFormat::INTERLEAVED)
			{
				uint8_t* src = (uint8_t*)buffers[0] + samplePos * this->sampleSize;
				uint8_t* dst = (uint8_t*)this->currentBuffer->mAudioData + this->bufferPos * this->sampleSize;
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
							int8_t* dst = (int8_t*)this->currentBuffer->mAudioData + this->bufferPos * numChannels + channelIndex;
							for (size_t i = 0; i < toCopy; ++i)
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
							int16_t* dst = (int16_t*)this->currentBuffer->mAudioData + this->bufferPos * numChannels + channelIndex;
							for (size_t i = 0; i < toCopy; ++i)
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
							float* dst = (float*)this->currentBuffer->mAudioData + this->bufferPos * numChannels + channelIndex;
							for (size_t i = 0; i < toCopy; ++i)
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

			// enqueue the buffer
			this->enqueueBuffer();
		}
		return numSamples - l;
	}

	virtual void drain()
	{
		// check if there is an incomplete buffer
		if (this->currentBuffer != NULL)
		{
			// clear remains of buffer
			int toClear = (this->numSamplesPerBuffer - this->bufferPos) * this->sampleSize;
			memset((uint8_t*)this->currentBuffer->mAudioData + this->bufferPos * this->sampleSize, 0, toClear);

			// enqueue the buffer
			this->enqueueBuffer();
		}
	}

	void enqueueBuffer()
	{
		// get current time
		AudioTimeStamp currentTime;
		OSStatus result = AudioQueueGetCurrentTime(
			this->audioQueue,
			NULL,
			&currentTime,
			NULL);
			
		// theck if there is a gap between the last buffer and the new buffer
		AudioTimeStamp* startTime = (currentTime.mSampleTime > this->numWritten + this->offset)
			? &currentTime : NULL;
		
		// output the buffer
		AudioTimeStamp timeStamp;
		result = AudioQueueEnqueueBufferWithParameters(
			this->audioQueue,
			this->currentBuffer,
			0,          // inNumPacketDescs,
			NULL,       // *inPacketDescs,
			0,          // inTrimFramesAtStart,
			0,          // inTrimFramesAtEnd,
			0,          // inNumParamValues,
			NULL,       // *inParamValues,
			startTime,  // *inStartTime,
			&timeStamp  // *outActualStartTime
		);
		this->offset = timeStamp.mSampleTime - this->numWritten;

		this->numWritten += double(this->numSamplesPerBuffer);
		
		// next buffer
		this->currentBuffer = NULL;
		this->bufferPos = 0;
	}
	
	
	AudioFormat format;

	AudioQueueRef audioQueue;

	// buffers
	int numBuffers;
	std::vector<AudioQueueBufferRef> freeBuffers;
	volatile int numFreeBuffers;
	volatile bool gotFree;
	pthread_mutex_t mutex;
	pthread_cond_t condition;
	AudioQueueBufferRef currentBuffer;
	
	// size of one buffer in samples
	int numSamplesPerBuffer;
	
	// size of one sample in bytes
	int sampleSize;
	
	// position in current buffer in samples
	int bufferPos;
	
	// offset to compensate that sampleTime continues even if buffer queue is empty
	double offset;

	// number of samples written to audio api
	double numWritten;
};


Pointer<LineOut> LineOut::open(AudioFormat format, int numBuffers, int numSamplesPerBuffer)
{
	return new CoreAudioOut(format, numBuffers, numSamplesPerBuffer);
}

} // namespace digi
