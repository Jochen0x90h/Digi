namespace webm
{
	void readStruct(EbmlReader& r, Seek& s, size_t maxSize)
	{
		s._size = r.readSize(maxSize);
		size_t start = r.getByteCount();
		while (r.getByteCount() - start < s._size)
		{
			uint32_t id = r.readId();
			switch (id)
			{
			case 0x53AB:
				s.seekID = r.read<uint32_t>();
				break;
			case 0x53AC:
				s.seekPosition = r.read<uint64_t>();
				break;
			default:
				r.skip(r.readSize(maxSize));
			};
		};
	}

	void readStruct(EbmlReader& r, SeekHead& s, size_t maxSize)
	{
		s._size = r.readSize(maxSize);
		size_t start = r.getByteCount();
		while (r.getByteCount() - start < s._size)
		{
			uint32_t id = r.readId();
			switch (id)
			{
			case 0x4DBB:
				{
					Seek& e = *s.seeks.insert(s.seeks.end(), Seek());
					readStruct(r, e, 1000u);
				}
				break;
			default:
				r.skip(r.readSize(maxSize));
			};
		};
	}

	namespace seekHead
	{
		void read(EbmlReader& r, SeekHead& s)
		{
			readStruct(r, s, 1000u);
		}
	}

	void readStruct(EbmlReader& r, Info& s, size_t maxSize)
	{
		s._size = r.readSize(maxSize);
		size_t start = r.getByteCount();
		while (r.getByteCount() - start < s._size)
		{
			uint32_t id = r.readId();
			switch (id)
			{
			case 0x2AD7B1:
				s.timeCodeScale = r.read<uint32_t>();
				break;
			case 0x4489:
				s.duration = r.read<double>();
				break;
			case 0x4D80:
				r.read(s.muxingApp, 100u);
				break;
			case 0x5741:
				r.read(s.writingApp, 100u);
				break;
			default:
				r.skip(r.readSize(maxSize));
			};
		};
	}

	namespace info
	{
		void read(EbmlReader& r, Info& s)
		{
			readStruct(r, s, 1000u);
		}
	}

	void readStruct(EbmlReader& r, Video& s, size_t maxSize)
	{
		s._size = r.readSize(maxSize);
		size_t start = r.getByteCount();
		while (r.getByteCount() - start < s._size)
		{
			uint32_t id = r.readId();
			switch (id)
			{
			case 0xB0:
				s.pixelWidth = r.read<uint32_t>();
				break;
			case 0xBA:
				s.pixelHeight = r.read<uint32_t>();
				break;
			case 0x53B8:
				s.stereoMode = r.read<uint8_t>();
				break;
			case 0x2383E3:
				s.frameRate = r.read<float>();
				break;
			default:
				r.skip(r.readSize(maxSize));
			};
		};
	}

	void readStruct(EbmlReader& r, Audio& s, size_t maxSize)
	{
		s._size = r.readSize(maxSize);
		size_t start = r.getByteCount();
		while (r.getByteCount() - start < s._size)
		{
			uint32_t id = r.readId();
			switch (id)
			{
			case 0x9F:
				s.channels = r.read<uint32_t>();
				break;
			case 0xB5:
				s.samplingFrequency = r.read<float>();
				break;
			default:
				r.skip(r.readSize(maxSize));
			};
		};
	}

	void readStruct(EbmlReader& r, TrackEntry& s, size_t maxSize)
	{
		s._size = r.readSize(maxSize);
		size_t start = r.getByteCount();
		while (r.getByteCount() - start < s._size)
		{
			uint32_t id = r.readId();
			switch (id)
			{
			case 0xD7:
				s.trackNumber = r.read<uint32_t>();
				break;
			case 0x73C5:
				s.trackUID = r.read<uint32_t>();
				break;
			case 0x83:
				s.trackType = r.read<uint8_t>();
				break;
			case 0x86:
				r.read(s.codecID, 100u);
				break;
			case 0x63A2:
				r.read(s.codecPrivate, 100000u);
				break;
			case 0xE0:
				readStruct(r, *(s.video = Video()), 10000u);
				break;
			case 0xE1:
				readStruct(r, *(s.audio = Audio()), 10000u);
				break;
			default:
				r.skip(r.readSize(maxSize));
			};
		};
	}

	void readStruct(EbmlReader& r, Tracks& s, size_t maxSize)
	{
		s._size = r.readSize(maxSize);
		size_t start = r.getByteCount();
		while (r.getByteCount() - start < s._size)
		{
			uint32_t id = r.readId();
			switch (id)
			{
			case 0xAE:
				{
					TrackEntry& e = *s.trackEntries.insert(s.trackEntries.end(), TrackEntry());
					readStruct(r, e, 100000u);
				}
				break;
			default:
				r.skip(r.readSize(maxSize));
			};
		};
	}

	namespace tracks
	{
		void read(EbmlReader& r, Tracks& s)
		{
			readStruct(r, s, 1000000u);
		}
	}

	void readStruct(EbmlReader& r, CueTrackPosition& s, size_t maxSize)
	{
		s._size = r.readSize(maxSize);
		size_t start = r.getByteCount();
		while (r.getByteCount() - start < s._size)
		{
			uint32_t id = r.readId();
			switch (id)
			{
			case 0xF7:
				s.trackNumber = r.read<uint32_t>();
				break;
			case 0xF1:
				s.clusterPosition = r.read<uint64_t>();
				break;
			default:
				r.skip(r.readSize(maxSize));
			};
		};
	}

	void readStruct(EbmlReader& r, CuePoint& s, size_t maxSize)
	{
		s._size = r.readSize(maxSize);
		size_t start = r.getByteCount();
		while (r.getByteCount() - start < s._size)
		{
			uint32_t id = r.readId();
			switch (id)
			{
			case 0xB3:
				s.timeCode = r.read<uint64_t>();
				break;
			case 0xB7:
				{
					CueTrackPosition& e = *s.cueTrackPositions.insert(s.cueTrackPositions.end(), CueTrackPosition());
					readStruct(r, e, 100u);
				}
				break;
			default:
				r.skip(r.readSize(maxSize));
			};
		};
	}

	void readStruct(EbmlReader& r, Cues& s, size_t maxSize)
	{
		s._size = r.readSize(maxSize);
		size_t start = r.getByteCount();
		while (r.getByteCount() - start < s._size)
		{
			uint32_t id = r.readId();
			switch (id)
			{
			case 0xBB:
				{
					CuePoint& e = *s.cuePoints.insert(s.cuePoints.end(), CuePoint());
					readStruct(r, e, 10000u);
				}
				break;
			default:
				r.skip(r.readSize(maxSize));
			};
		};
	}

	namespace cues
	{
		void read(EbmlReader& r, Cues& s)
		{
			readStruct(r, s, 100000000u);
		}
	}

	void readStruct(EbmlReader& r, Cluster& s, size_t maxSize)
	{
		s._size = r.readSize(maxSize);
		size_t start = r.getByteCount();
		while (r.getByteCount() - start < s._size)
		{
			uint32_t id = r.readId();
			switch (id)
			{
			case 0xE7:
				s.timeCode = r.read<uint64_t>();
				break;
			case 0xA3:
				{
					std::vector<uint8_t>& e = *s.simpleBlocks.insert(s.simpleBlocks.end(), std::vector<uint8_t>());
					r.read(e, 10000000u);
				}
				break;
			default:
				r.skip(r.readSize(maxSize));
			};
		};
	}

	namespace cluster
	{
		void read(EbmlReader& r, Cluster& s)
		{
			readStruct(r, s, 100000000u);
		}
	}

}
