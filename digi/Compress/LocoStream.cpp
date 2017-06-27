#include <digi/Utility/ArrayUtility.h>

#include "LocoStream.h"


namespace digi {


// LocoStreamCompressor

LocoStreamCompressor::LocoStreamCompressor(BufferedWriter& w)
	: w(w), bitBuffer(), freeCount(32)
{
}

void LocoStreamCompressor::flush()
{
	int byteCount = (32 - this->freeCount + 7) >> 3;
	uint8_t buffer[4];
	for (int i = 0; i < byteCount; ++i)
	{
		buffer[i] = this->bitBuffer >> 24;
		this->bitBuffer <<= 8;
	}
	this->w.writeData(buffer, byteCount);
}

void LocoStreamCompressor::writeBinaryValue(int value, int bitCount)
{
	// check if we want to write more bits than are free in the bit buffer
	while (bitCount > this->freeCount)
	{
		// add upper bits to bit buffer
		int shift = bitCount - this->freeCount;
		if (shift < 32)
			this->bitBuffer |= value >> shift;
		
		// store bit buffer (32 bits)
		uint8_t buffer[4];
		for (int i = 0; i < 4; ++i)
		{
			buffer[i] = this->bitBuffer >> 24;
			this->bitBuffer <<= 8;
		}
		this->w.writeData(buffer, 4);

		// reset bit buffer: all 32 bits are free
		this->freeCount = 32;
		
		// set number of remaining bits
		bitCount = shift;
	}
	
	// reduce number of free bits
	this->freeCount -= bitCount;
	
	// add value to bit buffer. if the value didn't fit, the remaining bits get placed into
	// the highest bits, therefore the leading bits are shifted out and are not a problem
	this->bitBuffer |= value << this->freeCount;
}

} // namespace digi
