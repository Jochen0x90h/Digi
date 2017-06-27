#include <digi/Utility/ArrayUtility.h>

#include "DLZSS.h"


namespace digi {


// DlzssCompressor

DlzssCompressor::DlzssCompressor(BufferedWriter& w)
	: w(w)
{
	// init range coder
	this->range = 0xffffffff;
	this->low = 0;
	this->cache = 0xff;
	this->cachesize = 0;

	// init weights
	this->typeWeight = 0x800;
	fill(this->lengthWeights, 0x800);
	fill(this->offsetWeights, 0x800);
	fill(this->literalWeights, 0x800);
}

void DlzssCompressor::flush()
{
	for (int i = 0; i < 5; ++i)
	{
		this->shiftOutput();
	}
	this->w.flush();
}

void DlzssCompressor::shiftOutput()
{
	if ((this->low & 0xffffffff) < 0xff000000 || (this->low >> 32) != 0)
	{
		uint8_t temp = this->cache;
		for (int i = 0; i < this->cachesize; ++i)
		{
			this->w.writeByte((temp + (this->low >> 32)) & 0xff);
			temp = 0xff;
		}
		this->cachesize = 0;
		this->cache = (this->low >> 24) & 0xff;
	}
	this->cachesize++;
	this->low = (this->low << 8) & 0xffffffff;
}

void DlzssCompressor::writeBitAndUpdateWeight(int bit, uint16_t& weight, int shift)
{
	uint32_t threshold = (this->range >> 12) * weight;

	if (!bit)
	{
		this->range = threshold;
		weight += (0x1000 - weight) >> shift;
	}
	else
	{
		this->range -= threshold;
		this->low += threshold;
		weight -= weight >> shift;
	}

	while (this->range < 0x1000000)
	{
		this->range <<= 8;
		this->shiftOutput();
	}		
}

void DlzssCompressor::writeUniversalCode(uint32_t value, uint16_t* weights, int shift1, int shift2)
{
	// write so many ones as the value needs bits followed by a zero
	uint32_t v = value;
	int numBits = 0;
	while (v > 0)
	{
		this->writeBitAndUpdateWeight(1, weights[numBits], shift1);
		v >>= 1;
		++numBits;
	}
	this->writeBitAndUpdateWeight(0, weights[numBits], shift1);

	// write value (msb first) without leading one
	for (int i = numBits - 2; i >= 0; --i)
		this->writeBitAndUpdateWeight((value >> i) & 1, weights[33 + i], shift2);
}

// DlzssDecompressor

DlzssDecompressor::DlzssDecompressor(uint8_t* compressed, size_t size)
	: compressed(compressed), size(size), index(4)
{
	// check if end of compressed data is reached
	if (size < 4)
		return;
		
	// init range decoder with first 4 bytes
	this->range = 0xffffffff;
	this->code = compressed[0] << 24 | compressed[1] << 16 | compressed[2] << 8 | compressed[3];
	
	// init weights
	this->typeWeight = 0x800;
	fill(this->lengthWeights, 0x800);
	fill(this->offsetWeights, 0x800);
	fill(this->literalWeights, 0x800);
}

int DlzssDecompressor::readBitAndUpdateWeight(uint16_t& weight, int shift)
{
	// normalize range decoder
	while (this->range < 0x1000000)
	{
		// check if end of compressed data is reached
		if (this->index >= this->size)
			return 0;
	
		this->code = (this->code << 8) | this->compressed[this->index];
		this->range <<= 8;
		
		++this->index;
	}

	uint32_t threshold = (this->range >> 12) * weight;
	if (this->code < threshold)
	{
		this->range = threshold;
		weight += (0x1000 - weight) >> shift;
		return 0;
	}
	this->range -= threshold;
	this->code -= threshold;
	weight -= weight >> shift;
	return 1;
}

uint32_t DlzssDecompressor::readUniversalCode(uint16_t* weights, int shift1, int shift2)
{
	// number of ones is number of bits
	int numBits = 0;
	while (this->readBitAndUpdateWeight(weights[numBits], shift1) == 1)
		++numBits;
	if (!numBits)
		return 0;

	// read value (msb first) without leading one
	uint32_t value = 1;
	for (int i = numBits - 2; i >= 0; --i)
		value = (value << 1) | this->readBitAndUpdateWeight(weights[33 + i], shift2);
	return value;
}

} // namespace digi
