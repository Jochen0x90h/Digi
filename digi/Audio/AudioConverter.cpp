#include <algorithm>

#include "AudioConverter.h"


namespace digi {

AudioInConverter::AudioInConverter(Pointer<AudioIn> input, AudioFormat format, int numBufferSamples)
	: input(input), format(format), numBufferSamples(numBufferSamples)
{
	AudioFormat inFormat = input->getFormat();
		
	// allocate buffer(s)
	bool srcInterleaved = inFormat.layout == AudioFormat::INTERLEAVED;
	int srcSize = inFormat.getElementSize();	
	int bufferSize = numBufferSamples * inFormat.numChannels * srcSize;
	uint8_t* buffer = this->buffer = new uint8_t[bufferSize];
	for (int i = 0; i < inFormat.numChannels; ++i)
	{
		this->buffers[i] = buffer;
		buffer += srcInterleaved ? srcSize : bufferSize * srcSize;
	}
	
	// number of channels to convert
	this->numChannels = std::min(inFormat.numChannels, format.numChannels);
	
	// source info
	this->srcType = inFormat.type;
	this->srcStride = inFormat.getSampleSize(); 

	// destination info
	this->dstInterleaved = format.layout == AudioFormat::INTERLEAVED;
	this->dstSize = format.getElementSize();
	this->dstStride = format.getSampleSize();
}

AudioInConverter::~AudioInConverter()
{
	delete [] this->buffer;
}

AudioFormat AudioInConverter::getFormat()
{
	return this->format;
}

void AudioInConverter::close()
{
	return this->input->close();
}

template <typename Type>
void copy(uint8_t* src, int srcStride, uint8_t* dst, int dstStride, size_t numValues)
{
	for (size_t i = 0; i < numValues; ++i)
	{	
		*(Type*)dst = *(Type*)src;
		src += srcStride;
		dst += dstStride;
	}
}

template <typename SrcType, typename DstType>
void copyToFloat(uint8_t* src, int srcStride, DstType scale, uint8_t* dst, int dstStride, size_t numValues)
{
	for (size_t i = 0; i < numValues; ++i)
	{	
		*(DstType*)dst = DstType(*(SrcType*)src) * scale;
		src += srcStride;
		dst += dstStride;
	}
}

size_t AudioInConverter::read(Buffer* buffers, size_t numSamples)
{	
	size_t numRead = 0;
	while (numRead < numSamples)
	{
		// read
		size_t nr = this->input->read(this->buffers, std::min(numSamples, this->numBufferSamples));
		if (nr == 0)
			break;
		
		// convert
		for (int channelIndex = 0; channelIndex < this->numChannels; ++channelIndex)
		{
			uint8_t* src = (uint8_t*)this->buffers[channelIndex];
			uint8_t* dst = (uint8_t*)buffers[this->dstInterleaved ? 0 : channelIndex]
				+ numRead * this->dstStride + (this->dstInterleaved ? channelIndex * this->dstSize : 0);
			
			switch (this->srcType)
			{
			case AudioFormat::INT8:
				switch (this->format.type)
				{
				case AudioFormat::INT8:
					copy<int8_t>(src, this->srcStride, dst, this->dstStride, nr);
					break;				
				case AudioFormat::INT16:

					break;				
				case AudioFormat::FLOAT32:
					copyToFloat<int8_t, float>(src, this->srcStride, 1.0f / 127.0f, dst, this->dstStride, nr);
					break;				
				}
				break;
			
			case AudioFormat::INT16:
				switch (this->format.type)
				{
				case AudioFormat::INT8:

					break;				
				case AudioFormat::INT16:
					copy<int16_t>(src, this->srcStride, dst, this->dstStride, nr);
					break;				
				case AudioFormat::FLOAT32:
					copyToFloat<int16_t, float>(src, this->srcStride, 1.0f / 32767.0f, dst, this->dstStride, nr);
					break;				
				}
				break;
			
			case AudioFormat::FLOAT32:
				switch (this->format.type)
				{
				case AudioFormat::INT8:

					break;				
				case AudioFormat::INT16:

					break;				
				case AudioFormat::FLOAT32:
					copy<float>(src, this->srcStride, dst, this->dstStride, nr);
					break;				
				}
				break;				
			}
		}
		numRead += nr;
	}
	return numRead;
}

} // namespace digi
