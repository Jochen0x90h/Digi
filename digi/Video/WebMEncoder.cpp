#include <cmath>

#include <digi/Utility/foreach.h>
#include <digi/Utility/ListUtility.h>
#include <digi/Utility/MapUtility.h>
#include <digi/Utility/VectorUtility.h>
#include <digi/Data/DataException.h>
#include <digi/Data/EbmlWriter.h>

#include "WebMEncoder.h"


namespace digi {

#include "webm.ebml.id.h"
#include "webm.ebml.write.h"

namespace
{
	/// Murmur hash derived from public domain reference implementation at
	/// http://sites.google.com/site/murmurhash
	uint32_t murmur(const void* data, uint32_t len, uint32_t seed)
	{
		const uint8_t* d = (uint8_t*)data;
		const uint32_t m = 0x5bd1e995;
		const int r = 24;

		uint32_t h = seed ^ len;

		while (len >= 4)
		{
			uint32_t k;

			k  = d[0];
			k |= d[1] << 8;
			k |= d[2] << 16;
			k |= d[3] << 24;

			k *= m;
			k ^= k >> r;
			k *= m;

			h *= m;
			h ^= k;

			d += 4;
			len -= 4;
		}

		switch (len)
		{
		case 3:
			h ^= d[2] << 16;
		case 2:
			h ^= d[1] << 8;
		case 1:
			h ^= d[0];
			h *= m;
		};

		h ^= h >> 13;
		h *= m;
		h ^= h >> 15;

		return h;
	}
} // anonymous namespace


WebMEncoder::~WebMEncoder()
{
}

void WebMEncoder::encode(const std::map<int, Pointer<MediaEncoder> >& encoders, double duration)
{
	typedef std::pair<const int, Pointer<MediaEncoder> > EncoderPair;
	std::map<int, MediaEncoder::Packet> packets;
	webm::Cluster cluster;
	size_t size = 0;
	
	// encode first packet for every track
	foreach (const EncoderPair& p, encoders)
	{
		int trackNumber = p.first;
		MediaEncoder::Packet& packet = packets[trackNumber];
		
		// encode
		p.second->encode(packet);
	}
		
	while (true)
	{
		// find next packet (the one with lowest time)
		double lowestTime = 1e100;	
		int nextTrackNumber = 0;
		MediaEncoder::Packet* nextPacket = NULL;
		Pointer<MediaEncoder> encoder;
		foreach (const EncoderPair& p, encoders)
		{
			int trackNumber = p.first;
			MediaEncoder::Packet& packet = packets[trackNumber];
				
			// find lowest time code
			if (packet.time < lowestTime)
			{
				lowestTime = packet.time;
				nextTrackNumber = trackNumber;
				nextPacket = &packet;
				encoder = p.second;
			}
		}
		int trackNumber = nextTrackNumber;
		MediaEncoder::Packet& packet = *nextPacket;
		bool keyFrame = packet.type == MediaEncoder::Packet::KEY;
		
		// check if finished
		if (packet.time >= duration)
		{
			// finish: given duration reached
			this->duration = duration;
			break;
		}
		if (packet.type == MediaEncoder::Packet::END)
		{
			// finish: end packet encountered
			this->duration = packet.time;
			break;
		}		
		
		// add packet to cluster
		int64_t timeCode = int64_t(floor(packet.time * this->seconds2TimeCode + 0.5));
		int64_t blockTimeCode = timeCode - cluster.timeCode;
		
		// check if we have to start a new cluster
		bool sizeExceeded = size > 5000000;
		bool timeCodeExceeded = blockTimeCode > 32767;
		if (sizeExceeded || timeCodeExceeded || keyFrame)
		{
			if (!cluster.simpleBlocks.empty())
			{
				// write last cluster
				webm::cluster::write(w, cluster);
				
				// clear cluster
				cluster.simpleBlocks.clear();
				size = 0;
			}
			
			// set time code of cluster
			cluster.timeCode = timeCode;
			
			// block time code relative to new cluster is zero
			blockTimeCode = 0;
			
			// save a cue point if this is a keyframe
			if (keyFrame)
			{
				webm::CuePoint& cuePoint = add(cues.cuePoints);
				cuePoint.timeCode = timeCode;
				webm::CueTrackPosition& trackPosition = add(cuePoint.cueTrackPositions);
				trackPosition.trackNumber = trackNumber;
				trackPosition.clusterPosition = w.getPosition() - this->segmentStart;
			}
		}
		
		// write data into simple block
		std::vector<uint8_t>& simpleBlock = add(cluster.simpleBlocks);
		simpleBlock.resize(4 + packet.size);
		
		// track (ebml var int)
		simpleBlock[0] = 0x80 | trackNumber;
		
		// block timecode (int16 big endian)
		simpleBlock[1] = uint8_t(blockTimeCode >> 8);
		simpleBlock[2] = uint8_t(blockTimeCode);
		
		// flags
		uint8_t flags = 0;
		if (keyFrame)
				flags |= 0x80;
		//if (packet.invisible)
		//		flags |= 0x08;
		simpleBlock[3] = flags;
		
		// copy data
		uint8_t* data = (uint8_t*)packet.data;
		std::copy(data, data + packet.size, simpleBlock.begin() + 4);
		size += packet.size;
		
		// update hash
		uint32_t& hash = this->hashes[trackNumber];
		hash = murmur(packet.data, int(packet.size), hash);
		
		// encode next packet for track
		encoder->encode(packet);
	}
	
	// write last cluster
	if (!cluster.simpleBlocks.empty())
		webm::cluster::write(w, cluster);
}

void WebMEncoder::writeHeader(webm::Info& info, webm::Tracks& tracks)
{
	// write ebml header
	EbmlHeader header;
	header.docType = "webm";
	header.docTypeVersion = 2;
	header.docTypeReadVersion = 2;
	this->w.writeHeader(header);

	// start segment (reserve 8 byte for length)
	w.writeId(0x18538067);
	this->segmentStart = this->w.getPosition() + 8;

	// seconds to time code
	this->seconds2TimeCode = 1e9 / double(info.timeCodeScale);

	// seek info only to calculate size
	webm::SeekHead seekHead;
	seekHead.seeks.resize(3);

	// skip segment header to first cluster, write header later
	int64_t clustersStart = this->segmentStart
		+ webm::seekHead::calcSize(seekHead)
		+ webm::info::calcSize(info)
		+ webm::tracks::calcSize(tracks);
	w.setPosition(clustersStart);
}

void WebMEncoder::finishSegment(webm::Info& info, webm::Tracks& tracks)
{
	// set duration
	info.duration = this->duration * this->seconds2TimeCode;

	// set track unique identifiers
	foreach (webm::TrackEntry& track, tracks.trackEntries)
	{
		track.trackUID = this->hashes[track.trackNumber];
	}
	
	// write cues
	uint64_t cuesStart = w.getPosition();
	webm::cues::write(w, this->cues);

	
	// seek back to start of segment
	int64_t segmentEnd = w.getPosition();
	this->w.setPosition(this->segmentStart - 8);
	
	// write segment size
	w.writeVarInt(segmentEnd - this->segmentStart, 8);

	// write seekHead
	webm::SeekHead seekHead;
	seekHead.seeks.resize(3);
	webm::Seek& seekInfo = seekHead.seeks[0];
	webm::Seek& seekTracks = seekHead.seeks[1];
	webm::Seek& seekCues = seekHead.seeks[2];
	seekInfo.seekID = webm::info::id;
	seekTracks.seekID = webm::tracks::id;
	seekCues.seekID = webm::cues::id;
	seekInfo.seekPosition = webm::seekHead::calcSize(seekHead);
	seekTracks.seekPosition = seekInfo.seekPosition + webm::info::calcSize(info);
	seekCues.seekPosition = cuesStart - this->segmentStart;
	webm::seekHead::write(this->w, seekHead);

	// write info and tracks
	webm::info::write(this->w, info);
	webm::tracks::write(this->w, tracks);
	
	// seek to end
	w.setPosition(segmentEnd);
}

} // namespace digi
