#ifndef digi_Video_WebMEncoder_h
#define digi_Video_WebMEncoder_h

#include <map>

#include <boost/optional.hpp>

#include <digi/Data/EbmlWriter.h>
#include "MediaEncoder.h"


namespace digi {

/// @addtogroup Video
/// @{

#include "webm.struct.h"

class WebMEncoder : public Object
{
public:
	
	/// encode webm to given device (must be seekable)
	WebMEncoder(Pointer<IODevice> dev, webm::Info& info, webm::Tracks& tracks)
		: w(dev)
	{
		this->writeHeader(info, tracks);
	}

	/// encode webm to given file path
	WebMEncoder(const fs::path& path, webm::Info& info, webm::Tracks& tracks)
		: w(path)
	{
		this->writeHeader(info, tracks);
	}

	virtual ~WebMEncoder();
	
	/// close device
	void close() {this->w.close();}

	/// encode
	void encode(const std::map<int, Pointer<MediaEncoder> >& encoders, double duration);

	void finishSegment(webm::Info& info, webm::Tracks& tracks);

protected:

	void writeHeader(webm::Info& info, webm::Tracks& tracks);

	// device where webm file is read from
	EbmlWriter w;

	// setment
	int64_t segmentStart;

	// scales seconds to time codes
	double seconds2TimeCode;

	std::map<uint32_t, uint32_t> hashes;
	webm::Cues cues;
	double duration;
};

/// @}

} // namespace digi

#endif
