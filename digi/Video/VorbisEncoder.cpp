#include <vorbis/vorbisenc.h>

#include <digi/Utility/VectorUtility.h>
#include <digi/Audio/AudioConverter.h>

#include "VorbisEncoder.h"


namespace digi {

namespace
{
	typedef std::vector<uint8_t>::iterator Iterator;

	int getXiphVarIntSize(int value)
	{
		int size = 1;
		while (value >= 255)
		{
			++size;
			value -= 255;
		}
		return size;
	}
	
	void setXiphVarInt(int value, Iterator& it)
	{
		while (value >= 255)
		{
			*it = 255;
			++it;
			value -= 255;
		}
		*it = value;
		++it;
	}
} // anonymous namespace
	
VorbisEncoder::VorbisEncoder(Pointer<AudioIn> input, std::vector<uint8_t>& codecPrivate)
	: input(input)
{
	// http://www.xiph.org/vorbis/doc/libvorbis/overview.html
	// http://matroska.org/technical/specs/codecid/index.html
	int result;

	AudioFormat format = input->getFormat();
	this->samples2Seconds = 1.0 / double(format.sampleRate);

	// initialize vorbis
	vorbis_info_init(&this->info);
	result = vorbis_encode_init_vbr(&this->info, format.numChannels, format.sampleRate, 0.4f);
	result = vorbis_analysis_init(&this->dspState, &this->info);
	vorbis_comment_init(&this->comment);
	ogg_packet p1, p2, p3;
	result = vorbis_analysis_headerout(&this->dspState, &this->comment, &p1, &p2, &p3);
	
	int size = 1 + getXiphVarIntSize(p1.bytes) + getXiphVarIntSize(p2.bytes)
		+ p1.bytes + p2.bytes + p3.bytes;
	
	codecPrivate.resize(size);
	Iterator it = codecPrivate.begin();
	
	// number of packets - 1
	*it = 3 - 1;
	++it;
	
	// length of first two packets as xiph var int
	setXiphVarInt(p1.bytes, it);
	setXiphVarInt(p2.bytes, it);
	
	// set the three packets
	it = std::copy(p1.packet, p1.packet + p1.bytes, it);
	it = std::copy(p2.packet, p2.packet + p2.bytes, it);
	it = std::copy(p3.packet, p3.packet + p3.bytes, it);

	// initialize a block as encoding buffer
	result = vorbis_block_init(&this->dspState, &this->block); 
	//if (result < 0)
		
	// convert input if necessary
	if (format.type != AudioFormat::FLOAT32 || format.layout != AudioFormat::SEPARATE)
	{
		this->input = new AudioInConverter(this->input, AudioFormat(AudioFormat::FLOAT32,
			AudioFormat::SEPARATE, format.numChannels, format.sampleRate));
	}
}

VorbisEncoder::~VorbisEncoder()
{
	vorbis_block_clear(&this->block);
	vorbis_dsp_clear(&this->dspState);
	vorbis_comment_clear(&this->comment);
	vorbis_info_clear(&this->info);
}

void VorbisEncoder::close()
{	
	this->input->close();
}

void VorbisEncoder::encode(Packet& packet)
{
	int result;

	// try to get a block
	while ((result = vorbis_analysis_blockout(&this->dspState, &this->block)) == 0)
	{
		// get buffers
		float** samples = vorbis_analysis_buffer(&this->dspState, 1024);

		// read samples from input
		int numRead = this->input->read((void**)samples, 1024);
		
		// tell vorbis how many samples we have written
		result = vorbis_analysis_wrote(&this->dspState, numRead);
	}

	vorbis_analysis(&this->block, NULL);
	vorbis_bitrate_addblock(&this->block);

	ogg_packet oggPacket;
	result = vorbis_bitrate_flushpacket(&this->dspState, &oggPacket);

	packet.time = this->samples2Seconds * oggPacket.granulepos;
	packet.type = Packet::NORMAL;
	packet.data = oggPacket.packet;
	packet.size = oggPacket.bytes;
}

} // namespace digi
