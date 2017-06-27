#include <digi/Utility/VectorUtility.h>
#include <digi/System/Log.h>

#include "VorbisDecoder.h"


namespace digi {

namespace
{
	typedef std::vector<uint8_t>::const_iterator Iterator;

	int getXiphVarInt(Iterator& it)
	{
		int value = 0;
		uint8_t b;
		do
		{
			b = *it;
			++it;
			value += b;
		} while (b == 255);
		return value;
	}
} // anonymous namespace
	
VorbisDecoder::VorbisDecoder(const std::vector<uint8_t>& codecPrivate)
{
	// http://www.xiph.org/vorbis/doc/libvorbis/overview.html
	// http://matroska.org/technical/specs/codecid/index.html
	int result;

	// initialize vorbis
	vorbis_info_init(&this->info);
	vorbis_comment_init(&this->comment);
	
	Iterator d = codecPrivate.begin();
	Iterator it = d;
	
	// get number of packets in private data
	int numPackets = *it + 1;
	++it;
	//if (numPackets > 3)
	
	int sizes[3];
	int size = 0;
	int i;
	for (i = 0; i < numPackets - 1; ++i)
	{
		size += sizes[i] = getXiphVarInt(it);
	}
	sizes[i] = int(codecPrivate.size() - size - (it - d));
	
	// pass the three header packets to vorbis
	ogg_packet oggPacket;
	for (int i = 0; i < numPackets; ++i)
	{
		oggPacket.packet = (unsigned char*)&*it;
		oggPacket.bytes = sizes[i];
		oggPacket.b_o_s = i == 0;
		oggPacket.e_o_s = false;
		oggPacket.granulepos = 0;
		oggPacket.packetno = i;
		it += sizes[i];
		
		result = vorbis_synthesis_headerin(&this->info, &this->comment, &oggPacket);
		//if (result < 0)
	}
	
	result = vorbis_synthesis_init(&this->dspState, &this->info);
	//if (result < 0)

	// initialize a block as decoding buffer
	result = vorbis_block_init(&this->dspState, &this->block); 
	//if (result < 0)
	
	this->packetNumber = numPackets;
}

VorbisDecoder::~VorbisDecoder()
{
	vorbis_block_clear(&this->block);
	vorbis_dsp_clear(&this->dspState);
	vorbis_comment_clear(&this->comment);
	vorbis_info_clear(&this->info);
}

void VorbisDecoder::close()
{	
	this->output->close();
}

void VorbisDecoder::update()
{
	// get samples
	float** samples;
	int numSamples = vorbis_synthesis_pcmout(&this->dspState, &samples);
	
	// break if no samples are available
	if (numSamples == 0)
		return;

	// write samples to output
	size_t numWritten = this->output->write((void**)samples, numSamples);

	// tell vorbis how many samples were used
	vorbis_synthesis_read(&this->dspState, numWritten);
}

void VorbisDecoder::decode(double time, const uint8_t* data, size_t length)
{
	ogg_packet oggPacket;
	oggPacket.packet = (unsigned char*)data;
	oggPacket.bytes = length;
	oggPacket.b_o_s = false;
	oggPacket.e_o_s = false;
	oggPacket.granulepos = -1;
	oggPacket.packetno = this->packetNumber++;
	
	// decode packet
	if (vorbis_synthesis(&this->block, &oggPacket) == 0)
		vorbis_synthesis_blockin(&this->dspState, &this->block);
	
	// get samples
	float** samples;
	int numSamples = vorbis_synthesis_pcmout(&this->dspState, &samples);
	
	// write samples to output
	size_t numWritten = this->output->write((void**)samples, numSamples);

	// tell vorbis how many samples were used
	vorbis_synthesis_read(&this->dspState, numWritten);
}

} // namespace digi
