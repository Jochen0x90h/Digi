#ifndef digi_Audio_OggVorbisDecoder_h
#define digi_Audio_OggVorbisDecoder_h

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include <digi/System/IOCatcher.h>
#include "AudioDecoder.h"


namespace digi {

/// @addtogroup Audio
/// @{

/// decoder for ogg vorbis
class OggVorbisDecoder : public AudioDecoder
{
public:

	/// constructor, reads the ogg virbis from the given file
	OggVorbisDecoder(const fs::path& path);

	/// constructor, reads the ogg virbis from the given device
	OggVorbisDecoder(Pointer<File> dev);
	
	/// destructor
	virtual ~OggVorbisDecoder();

	/// get format of samples
	virtual AudioFormat getFormat();

// state

	/// close decoder
	virtual void close();

// read

	/// read samples
	virtual size_t read(Buffer* buffers, size_t numSamples);

// time

	/// set the position of the audio file in seconds
	virtual void setTime(double time);

	/// set the position of the audio file in seconds
	virtual double getTime();

	/// get duration of file in seconds
	virtual double getDuration();

protected:
	
	void open();
	
	// passes exceptions through the vorbis lib
	IOCatcher ioCatcher;

	OggVorbis_File vorbisFile;
	AudioFormat format;
	//int state;
};

/// @}

} // namespace digi

#endif
