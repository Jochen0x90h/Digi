#ifndef digi_Video_WebMDecoder_h
#define digi_Video_WebMDecoder_h

#include <map>

#include <boost/optional.hpp>

#include <digi/Data/EbmlReader.h>
#include "MediaDecoder.h"


namespace digi {

/// @addtogroup Video
/// @{

#include "webm.struct.h"

class WebMDecoder : public Object
{
public:
	
	/// decode webm from given device (must be seekable)
	WebMDecoder(Pointer<IODevice> dev, webm::Info& info, webm::Tracks& tracks)
		: r(dev), segmentStart(), segmentEnd(), timeCode2Seconds()
	{
		this->readHeader(info, tracks);
	}

	/// decode webm from given file path
	WebMDecoder(const fs::path& path, webm::Info& info, webm::Tracks& tracks)
		: r(path), segmentStart(), segmentEnd(), timeCode2Seconds()
	{
		this->readHeader(info, tracks);
	}

	virtual ~WebMDecoder();
	
	/// get duration of video
	double getDuration() {return this->duration;}

	void setDecoder(int trackIndex, Pointer<MediaDecoder> decoder) {this->decoders[trackIndex] = decoder;}
	void setDecoders(const std::map<int, Pointer<MediaDecoder> >& decoders) {this->decoders = decoders;}

	/// close input device and decoders
	void close();

	/// decode until given time
	bool decode(double time);

	/// seek to given time and return actual seeked time (start of next cluster). uses cues of track given by trackIndex
	double seek(int trackIndex, double time);

protected:

	void readHeader(webm::Info& info, webm::Tracks& tracks);

	// device where webm file is read from
	EbmlReader r;

	// segment
	int64_t segmentStart;
	int64_t segmentEnd;

	// scales time codes to seconds
	double timeCode2Seconds;

	// duration of video
	double duration;

	// cues: track id -> (time -> cluster position from segment start)
	std::map<int, std::map<double, int64_t> > cues;

	// current cluster
	webm::Cluster cluster;
	std::list<std::vector<uint8_t> >::iterator blockIt;
	
	// decoders for the tracks
	std::map<int, Pointer<MediaDecoder> > decoders;
};

/// @}

} // namespace digi

#endif
