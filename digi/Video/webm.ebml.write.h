namespace webm
{
	size_t calcStructSize(Seek& s)
	{
		s._size = 17;
		return 18;
	}

	void writeStruct(EbmlWriter& w, Seek& s)
	{
		w.writeVarInt(s._size);
		w.write(0x53AB, s.seekID, 4u);
		w.write(0x53AC, s.seekPosition, 7u);
	}

	size_t calcStructSize(SeekHead& s)
	{
		s._size = 0;
		foreach (Seek& e, s.seeks)
		{
			s._size += 2 + calcStructSize(e);
		}
		return EbmlWriter::calcSizeVarInt(s._size) + s._size;
	}

	void writeStruct(EbmlWriter& w, SeekHead& s)
	{
		w.writeVarInt(s._size);
		foreach (Seek& e, s.seeks)
		{
			w.writeId(0x4DBB);
			writeStruct(w, e);
		}
	}

	namespace seekHead
	{
		size_t calcSize(SeekHead& s)
		{
			return 4 + calcStructSize(s);
		}

		void write(EbmlWriter& w, SeekHead& s)
		{
			calcStructSize(s);
			w.writeId(0x114D9B74);
			writeStruct(w, s);
		}
	}

	size_t calcStructSize(Info& s)
	{
		s._size = 18;
		s._size += 2 + EbmlWriter::calcSize(s.muxingApp);
		s._size += 2 + EbmlWriter::calcSize(s.writingApp);
		return EbmlWriter::calcSizeVarInt(s._size) + s._size;
	}

	void writeStruct(EbmlWriter& w, Info& s)
	{
		w.writeVarInt(s._size);
		w.write(0x2AD7B1, s.timeCodeScale, 3u);
		w.write(0x4489, s.duration, 8u);
		w.write(0x4D80, s.muxingApp);
		w.write(0x5741, s.writingApp);
	}

	namespace info
	{
		size_t calcSize(Info& s)
		{
			return 4 + calcStructSize(s);
		}

		void write(EbmlWriter& w, Info& s)
		{
			calcStructSize(s);
			w.writeId(0x1549A966);
			writeStruct(w, s);
		}
	}

	size_t calcStructSize(Video& s)
	{
		s._size = 12;
		s._size += 1 + EbmlWriter::calcSize(s.pixelWidth);
		s._size += 1 + EbmlWriter::calcSize(s.pixelHeight);
		return EbmlWriter::calcSizeVarInt(s._size) + s._size;
	}

	void writeStruct(EbmlWriter& w, Video& s)
	{
		w.writeVarInt(s._size);
		w.write(0xB0, s.pixelWidth);
		w.write(0xBA, s.pixelHeight);
		w.write(0x53B8, s.stereoMode, 1u);
		w.write(0x2383E3, s.frameRate, 4u);
	}

	size_t calcStructSize(Audio& s)
	{
		s._size = 9;
		return 10;
	}

	void writeStruct(EbmlWriter& w, Audio& s)
	{
		w.writeVarInt(s._size);
		w.write(0x9F, s.channels, 1u);
		w.write(0xB5, s.samplingFrequency, 4u);
	}

	size_t calcStructSize(TrackEntry& s)
	{
		s._size = 13;
		s._size += 1 + EbmlWriter::calcSize(s.codecID);
		if (!s.codecPrivate.empty())
		{
			s._size += 2 + EbmlWriter::calcSize(s.codecPrivate);
		}
		if (s.video)
		{
			s._size += 1 + calcStructSize(*s.video);
		}
		if (s.audio)
		{
			s._size += 1 + calcStructSize(*s.audio);
		}
		return EbmlWriter::calcSizeVarInt(s._size) + s._size;
	}

	void writeStruct(EbmlWriter& w, TrackEntry& s)
	{
		w.writeVarInt(s._size);
		w.write(0xD7, s.trackNumber, 1u);
		w.write(0x73C5, s.trackUID, 4u);
		w.write(0x83, s.trackType, 1u);
		w.write(0x86, s.codecID);
		if (!s.codecPrivate.empty())
		{
			w.write(0x63A2, s.codecPrivate);
		}
		if (s.video)
		{
			w.writeId(0xE0);
			writeStruct(w, *s.video);
		}
		if (s.audio)
		{
			w.writeId(0xE1);
			writeStruct(w, *s.audio);
		}
	}

	size_t calcStructSize(Tracks& s)
	{
		s._size = 0;
		foreach (TrackEntry& e, s.trackEntries)
		{
			s._size += 1 + calcStructSize(e);
		}
		return EbmlWriter::calcSizeVarInt(s._size) + s._size;
	}

	void writeStruct(EbmlWriter& w, Tracks& s)
	{
		w.writeVarInt(s._size);
		foreach (TrackEntry& e, s.trackEntries)
		{
			w.writeId(0xAE);
			writeStruct(w, e);
		}
	}

	namespace tracks
	{
		size_t calcSize(Tracks& s)
		{
			return 4 + calcStructSize(s);
		}

		void write(EbmlWriter& w, Tracks& s)
		{
			calcStructSize(s);
			w.writeId(0x1654AE6B);
			writeStruct(w, s);
		}
	}

	size_t calcStructSize(CueTrackPosition& s)
	{
		s._size = 3;
		s._size += 1 + EbmlWriter::calcSize(s.clusterPosition);
		return EbmlWriter::calcSizeVarInt(s._size) + s._size;
	}

	void writeStruct(EbmlWriter& w, CueTrackPosition& s)
	{
		w.writeVarInt(s._size);
		w.write(0xF7, s.trackNumber, 1u);
		w.write(0xF1, s.clusterPosition);
	}

	size_t calcStructSize(CuePoint& s)
	{
		s._size = 6;
		foreach (CueTrackPosition& e, s.cueTrackPositions)
		{
			s._size += 1 + calcStructSize(e);
		}
		return EbmlWriter::calcSizeVarInt(s._size) + s._size;
	}

	void writeStruct(EbmlWriter& w, CuePoint& s)
	{
		w.writeVarInt(s._size);
		w.write(0xB3, s.timeCode, 4u);
		foreach (CueTrackPosition& e, s.cueTrackPositions)
		{
			w.writeId(0xB7);
			writeStruct(w, e);
		}
	}

	size_t calcStructSize(Cues& s)
	{
		s._size = 0;
		foreach (CuePoint& e, s.cuePoints)
		{
			s._size += 1 + calcStructSize(e);
		}
		return EbmlWriter::calcSizeVarInt(s._size) + s._size;
	}

	void writeStruct(EbmlWriter& w, Cues& s)
	{
		w.writeVarInt(s._size);
		foreach (CuePoint& e, s.cuePoints)
		{
			w.writeId(0xBB);
			writeStruct(w, e);
		}
	}

	namespace cues
	{
		size_t calcSize(Cues& s)
		{
			return 4 + calcStructSize(s);
		}

		void write(EbmlWriter& w, Cues& s)
		{
			calcStructSize(s);
			w.writeId(0x1C53BB6B);
			writeStruct(w, s);
		}
	}

	size_t calcStructSize(Cluster& s)
	{
		s._size = 6;
		foreach (std::vector<uint8_t>& e, s.simpleBlocks)
		{
			s._size += 1 + EbmlWriter::calcSize(e);
		}
		return EbmlWriter::calcSizeVarInt(s._size) + s._size;
	}

	void writeStruct(EbmlWriter& w, Cluster& s)
	{
		w.writeVarInt(s._size);
		w.write(0xE7, s.timeCode, 4u);
		foreach (std::vector<uint8_t>& e, s.simpleBlocks)
		{
			w.write(0xA3, e);
		}
	}

	namespace cluster
	{
		size_t calcSize(Cluster& s)
		{
			return 4 + calcStructSize(s);
		}

		void write(EbmlWriter& w, Cluster& s)
		{
			calcStructSize(s);
			w.writeId(0x1F43B675);
			writeStruct(w, s);
		}
	}

}
