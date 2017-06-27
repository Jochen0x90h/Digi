#include <digi/Utility/ListUtility.h>
#include <digi/Utility/MapUtility.h>
#include <digi/Utility/foreach.h>
#include <digi/System/Log.h>
#include <digi/Data/DataException.h>
#include <digi/Data/EbmlReader.h>

#include "WebMDecoder.h"


namespace digi {

#include "webm.ebml.id.h"
#include "webm.ebml.read.h"

namespace
{
	int getEBMLVarInt(uint8_t*& it)
	{
		uint8_t head = *it;
		++it;
		int numTails = 0;
		uint8_t mask = 0x80;
		while ((head & mask) == 0)
		{
			mask >>= 1;
			++numTails;
		}
		int value = head & ~mask;

		const uint8_t* end = it + numTails;
		while (it != end)
		{
			value <<= 8;
			value |= *it;
			++it;
		}
		return value;
	}

	typedef std::pair<const int, Pointer<MediaDecoder> > DecoderPair;
} // anonymous namespace


WebMDecoder::~WebMDecoder()
{
}

void WebMDecoder::close()
{
	this->r.close();
	foreach (const DecoderPair& p, this->decoders)
	{
		p.second->close();
	}
}

bool WebMDecoder::decode(double time)
{
	// update decoders. for example an audio decoder needs to output pending samples
	foreach (const DecoderPair& p, this->decoders)
	{
		p.second->update();
	}

	while (true)
	{
		while (this->blockIt != this->cluster.simpleBlocks.end())
		{
			// simple block
			// http://matroska.org/technical/specs/index.html#simpleblock_structure

			// get track number
			uint8_t* d = this->blockIt->data();
			uint8_t* it = d;
			int trackNumber = getEBMLVarInt(it);
			
			// calc timecode (time of cluster + time of block)
			uint64_t timeCode = this->cluster.timeCode + ((it[0] << 8) | it[1]);
			it += 2;
			
			// convert to seconds
			double blockTime = double(timeCode) * this->timeCode2Seconds;
			
			// return false if not at end of segment (this block is "in the future")
			if (blockTime > time)
				return false;
			
			// get flags
			uint8_t flags = *it;
			++it;
			
			// pass block to decoder
			Pointer<MediaDecoder> decoder = getValue(this->decoders, trackNumber);
			if (decoder != null)
				decoder->decode(blockTime, it, this->blockIt->size() - (it - d));
			
			++this->blockIt;
		}
		
		this->cluster.simpleBlocks.clear();

		// find next cluster
		while (true)
		{
			// return true if at end of segment
			if (this->r.getPosition() >= this->segmentEnd)
				return true;
		
			uint32_t id = this->r.readId();
			if (id == webm::cluster::id)
			{
				// found a cluster
				webm::cluster::read(this->r, this->cluster);
				this->blockIt = this->cluster.simpleBlocks.begin();
				break;
			}

			// skip unknown chunk
			this->r.skip(this->r.readSize(1000000));
		}
	}
}

double WebMDecoder::seek(int trackIndex, double time)
{
	// clear decoders, e.g. current subtitle
	foreach (const DecoderPair& p, this->decoders)
	{
		p.second->clear();
	}

	// clear current cluster
	this->cluster.simpleBlocks.clear();
	this->blockIt = this->cluster.simpleBlocks.begin();

	std::map<double, int64_t>& cues = this->cues[trackIndex];
	std::map<double, int64_t>::iterator it = cues.upper_bound(time);
	if (it != cues.begin())
		--it;
	if (it != cues.end())
	{
		// set position
		int64_t position = this->segmentStart + it->second;
		this->r.setPosition(position);

		// find next cluster
		while (true)
		{
			// check if at end of segment
			if (this->r.getPosition() >= this->segmentEnd)
				break;
		
			uint32_t id = this->r.readId();
			if (id == webm::cluster::id)
			{
				// found a cluster
				webm::cluster::read(this->r, this->cluster);
				this->blockIt = this->cluster.simpleBlocks.begin();
				
				// return start time of cluster
				return this->cluster.timeCode * this->timeCode2Seconds;
			}

			// skip unknown chunk
			this->r.skip(this->r.readSize(1000000));
		}
	}
	
	return this->duration;
}

void WebMDecoder::readHeader(webm::Info& info, webm::Tracks& tracks)
{
	// read ebml header
	EbmlHeader header;
	this->r.readHeader(header);
	if (header.ebmlReadVersion != 1 || header.docType != "webm" || header.docTypeReadVersion > 2)
		throw DataException(this->r.getDevice(), DataException::FORMAT_ERROR);
	
	// read until segment is found
	while (true)
	{
		uint32_t id = this->r.readId();
		if (id == 0x18538067)
		{
			// segment found
			int64_t segmentSize = this->r.readVarInt();
			this->segmentStart = r.getPosition();
			this->segmentEnd = this->segmentStart + segmentSize;
			
			break;
		}
		
		// skip unknown chunk
		this->r.skip(this->r.readSize(1000000));
	}
	
	// read header elements until first cluster
	bool haveSeek = false;
	bool haveInfo = false;
	bool haveTracks = false;
	bool haveCues = false;
	webm::SeekHead seekHead;
	webm::Cues cues;
	while (true)
	{
		uint32_t id = this->r.readId();
		if (id == webm::seekHead::id)
		{
			// read seek header
			haveSeek = true;
			webm::seekHead::read(this->r, seekHead);
		}
		else if (id == webm::info::id)
		{
			// read info
			haveInfo = true;
			webm::info::read(this->r, info);
		}
		else if (id == webm::tracks::id)
		{
			// read tracks
			haveTracks = true;
			webm::tracks::read(this->r, tracks);
		}
		else if (id == webm::cues::id)
		{
			// read cues
			haveCues = true;
			webm::cues::read(this->r, cues);
		}
		else if (id == webm::cluster::id)
		{
			// read first cluster
			webm::cluster::read(this->r, this->cluster);
			this->blockIt = this->cluster.simpleBlocks.begin();
			
			// stop
			break;
		}
		else
		{
			// skip unknown chunk
			this->r.skip(this->r.readSize(1000000));
		}
	}
	
	// if we have seek header and other info is still missing then read by seeking to it
	if (haveSeek && (!haveInfo || !haveTracks || !haveCues))
	{
		// save position
		int64_t position = this->r.getPosition();

		// iterate over seek infos
		foreach (webm::Seek& seek, seekHead.seeks)
		{
			this->r.setPosition(segmentStart + seek.seekPosition);
			switch (seek.seekID)
			{
			case webm::info::id:
				if (!haveInfo)
				{
					uint32_t id = this->r.readId();
					if (id != seek.seekID)
						throw DataException(this->r.getDevice(), DataException::DATA_CORRUPT);
					webm::info::read(this->r, info);
					haveInfo = true;
				}
				break;
			case webm::tracks::id:
				if (!haveTracks)
				{
					uint32_t id = this->r.readId();
					if (id != seek.seekID)
						throw DataException(this->r.getDevice(), DataException::DATA_CORRUPT);
					webm::tracks::read(this->r, tracks);
					haveTracks = true;
				}
				break;
			case webm::cues::id:
				if (!haveCues)
				{
					// read cues if possible
					try
					{
						uint32_t id = this->r.readId();
						if (id == seek.seekID)
						{
							webm::cues::read(this->r, cues);
							haveCues = true;
						}
					}
					catch (DataException&)
					{
						// something went wrong: we have no cues
					}
				}
				break;
			}
		}
		
		// restore position
		this->r.setPosition(position);
	}
	
	if (!haveInfo || !haveTracks)
	{
		// error: info or tracks missing
		throw DataException(this->r.getDevice(), DataException::DATA_INCOMPLETE);
	}

	// calc factor that converts webm time codes into seconds
	this->timeCode2Seconds = double(info.timeCodeScale) * 1e-9;

	// get duration
	this->duration = info.duration * this->timeCode2Seconds;
	
	// copy cues into map
	if (haveCues)
	{
		foreach (webm::CuePoint& cuePoint, cues.cuePoints)
		{
			double time = cuePoint.timeCode * this->timeCode2Seconds;
			foreach (webm::CueTrackPosition& trackPosition, cuePoint.cueTrackPositions)
			{
				//dNotify(time << " " << trackPosition.trackNumber);
				this->cues[trackPosition.trackNumber][time] = trackPosition.clusterPosition;
			}
		}
	}
}

} // namespace digi
