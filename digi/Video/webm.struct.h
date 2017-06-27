#ifndef GENERATED_webm_h
#define GENERATED_webm_h

namespace webm
{
	struct Seek
	{
		size_t _size;
		uint32_t seekID;
		uint64_t seekPosition;

		Seek() : _size(), seekID(), seekPosition() {}
	};

	struct SeekHead
	{
		size_t _size;
		std::vector<Seek> seeks;

		SeekHead() : _size() {}
	};

	struct Info
	{
		size_t _size;
		uint32_t timeCodeScale;
		double duration;
		std::string muxingApp;
		std::string writingApp;

		Info() : _size(), timeCodeScale(1000000), duration() {}
	};

	struct Video
	{
		size_t _size;
		uint32_t pixelWidth;
		uint32_t pixelHeight;
		uint8_t stereoMode;
		float frameRate;

		Video() : _size(), pixelWidth(), pixelHeight(), stereoMode(), frameRate() {}
	};

	struct Audio
	{
		size_t _size;
		uint32_t channels;
		float samplingFrequency;

		Audio() : _size(), channels(1), samplingFrequency(8000.0f) {}
	};

	struct TrackEntry
	{
		size_t _size;
		uint32_t trackNumber;
		uint32_t trackUID;
		uint8_t trackType;
		std::string codecID;
		std::vector<uint8_t> codecPrivate;
		boost::optional<Video> video;
		boost::optional<Audio> audio;

		TrackEntry() : _size(), trackNumber(), trackUID(), trackType(), codecPrivate() {}
	};

	struct Tracks
	{
		size_t _size;
		std::list<TrackEntry> trackEntries;

		Tracks() : _size() {}
	};

	struct CueTrackPosition
	{
		size_t _size;
		uint32_t trackNumber;
		uint64_t clusterPosition;

		CueTrackPosition() : _size(), trackNumber(), clusterPosition() {}
	};

	struct CuePoint
	{
		size_t _size;
		uint64_t timeCode;
		std::vector<CueTrackPosition> cueTrackPositions;

		CuePoint() : _size(), timeCode() {}
	};

	struct Cues
	{
		size_t _size;
		std::list<CuePoint> cuePoints;

		Cues() : _size() {}
	};

	struct Cluster
	{
		size_t _size;
		uint64_t timeCode;
		std::list<std::vector<uint8_t> > simpleBlocks;

		Cluster() : _size(), timeCode() {}
	};

} // webm
#endif
