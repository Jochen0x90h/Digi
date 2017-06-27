#include <stdexcept>

#include <digi/System/IOException.h>
#include <digi/Data/DataException.h>

#include "OggVorbisDecoder.h"


namespace digi {

namespace
{
	size_t vorbisRead(void *ptr, size_t size, size_t nmemb, void *datasource)
	{
		return static_cast<IOCatcher*>(datasource)->read(ptr, size * nmemb);
	}

	int vorbisSeek(void *datasource, ogg_int64_t offset, int whence)
	{
		return static_cast<IOCatcher*>(datasource)->seek(offset, File::PositionMode(whence)) == -1 ? -1 : 0;
	}

	int vorbisClose(void *datasource)
	{
		// the decoder is not responsible for closing the file
		return 0;
	}

	long vorbisTell(void *datasource)
	{
		return long(static_cast<IOCatcher*>(datasource)->getPosition());
	}

	void throwVorbisException(Pointer<IODevice> device, int result)
	{
		switch (result)
		{
		case 0:
			// ok
			break;
		case OV_HOLE:
			// interruption in the data (garbage between pages, loss of sync followed by recapture, or a corrupt page)
			throw DataException(device, DataException::DATA_CORRUPT);
			break;	
		case OV_EREAD:
			// A read from media returned an error.
			throw IOException(device, IOException::IO_ERROR);
			break;
		case OV_EFAULT:
			// Internal logic fault; indicates a bug or heap/stack corruption.
			throw std::logic_error("logic fault in vorbis decoder: indicates a bug or heap/stack corruption");
			break;
		case OV_ENOTVORBIS:
			// Bitstream is not Vorbis data.
			throw DataException(device, DataException::FORMAT_ERROR);
			break;
		case OV_EBADHEADER:
			// Invalid Vorbis bitstream header.
			throw DataException(device, DataException::DATA_CORRUPT);
			break;
		case OV_EVERSION:
			// Vorbis version mismatch.
			throw DataException(device, DataException::UNKNOWN_VERSION);
			break;
		case OV_ENOTAUDIO:
			// Bitstream is not audio data.
			throw DataException(device, DataException::FORMAT_NOT_SUPPORTED);
			break;
		}
	}
		
} // anonymous namespace


// OggVorbisDecoder

OggVorbisDecoder::OggVorbisDecoder(const fs::path& path)
	: ioCatcher(File::open(path, File::READ))
{
	this->open();
}

OggVorbisDecoder::OggVorbisDecoder(Pointer<File> file)
	: ioCatcher(file)
{
	this->open();
}

OggVorbisDecoder::~OggVorbisDecoder()
{
	ov_clear(&this->vorbisFile);
}
	
AudioFormat OggVorbisDecoder::getFormat()
{
	return this->format;
}

//int OggVorbisDecoder::getState()
//{
//	return this->state;
//}

void OggVorbisDecoder::close()
{
	//int result = ov_clear(&this->vorbisFile);
	//throwVorbisException(this->ioCatcher.dev, result);

	this->ioCatcher.dev->close();
}

size_t OggVorbisDecoder::read(Buffer* buffers, size_t length)
{
	uint8_t* data = (uint8_t*)buffers[0];
	
	//Lock lock(this->readWriteMutex);
	size_t sampleSize = this->format.getSampleSize();
	
	// vorbis often does not read the full length, therefore loop
	size_t l = length * sampleSize;
	size_t numRead = 0;
	while (numRead < l)
	{
		int bitstream;
		long result = ov_read(&this->vorbisFile, (char*)data, int(std::min(l - numRead, size_t(0x40000000))), 0, 2, 1,
			&bitstream);
		this->ioCatcher.checkState();
		throwVorbisException(this->ioCatcher.dev, result);
		if (result == 0)
		{
			// check for end of file
			//if (numRead == 0)
				//this->state |= END_OF_INPUT;
			break;
		}

		numRead += result;
		data += result;
	}
	return numRead / sampleSize;
}
/*
void OggVorbisDecoder::setRawPosition(int64_t position)
{
	// set position
	int result = ov_raw_seek(&this->vorbisFile, position);
	this->ioCatcher.checkState();
	throwVorbisException(result);

	// reset end of file flag
	this->state &= ~END_OF_INPUT;
}

int64_t OggVorbisDecoder::getRawPosition()
{
	// return current position in bytes
	return ov_raw_tell(&this->vorbisFile);
}
*/
void OggVorbisDecoder::setTime(double time)
{
	int result = ov_time_seek(&this->vorbisFile, time);
	this->ioCatcher.checkState();
	throwVorbisException(this->ioCatcher.dev, result);
	
	// reset end of file flag
	//this->state &= ~END_OF_INPUT;
}

double OggVorbisDecoder::getTime()
{
	return ov_time_tell(&this->vorbisFile);
}

double OggVorbisDecoder::getDuration()
{
	return ov_time_total(&this->vorbisFile, -1);	
}

void OggVorbisDecoder::open()
{
	// fill the callback structure
	ov_callbacks callbacks;
	callbacks.read_func = &vorbisRead;
	callbacks.seek_func = &vorbisSeek;
	callbacks.close_func = &vorbisClose;
	callbacks.tell_func = &vorbisTell;

	int result = ov_open_callbacks(static_cast<void*>(&this->ioCatcher), &this->vorbisFile, NULL, 0, callbacks);
	
	// check for file error
	this->ioCatcher.checkState();
	
	// check for vorbis error
	throwVorbisException(this->ioCatcher.dev, result);

	// set infomation about the audio file
	this->format = AudioFormat(AudioFormat::INT16, AudioFormat::INTERLEAVED, this->vorbisFile.vi->channels, this->vorbisFile.vi->rate);
	//this->numSamples = int(ov_pcm_total(&this->vorbisFile, -1));
}

} // namespace digi
