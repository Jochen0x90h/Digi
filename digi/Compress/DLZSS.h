#ifndef digi_Compress_DlzssCompressor_h
#define digi_Compress_DlzssCompressor_h

#include <digi/Utility/ArrayUtility.h>
#include <digi/Data/BufferedWriter.h>
#include <digi/System/Log.h>


namespace digi {

/// @addtogroup Compress
/// @{

/// differential LZSS. The input is assumed to be correlated so that calculating the difference leads to a series of
/// values where small values are more probable. http://michael.dipperstein.com/lzss
class DlzssCompressor
{
public:

	DlzssCompressor(BufferedWriter& w);
	
	template <typename Iterator>
	void compress(Iterator data, size_t size)
	{
		const int bitCount = sizeof(data[0]) * 8;
	
		const int typeShift = 4;
		const int literalShift1 = 4;
		const int literalShift2 = 4;
		const int offsetShift1 = 4;
		const int offsetShift2 = 4;
		const int lengthShift1 = 4;
		const int lengthShift2 = 4;

		const int minLength = 3;
		const int hashSize = 4096;
		const int windowSize = 4096;

		// sliding window dictionary
		size_t hash[hashSize];
		size_t window[windowSize];
		fill(hash, -windowSize);
		
		size_t pos = 0;
		uint32_t last = 0;
		int shift = 32 - bitCount;
		for (pos = 0; pos < size; ++pos)
		{
			uint32_t value = data[pos];

			// calc difference
			int32_t diff = int32_t((value - last) << shift) >> shift;
			
			// convert to signed magnitude representation (0->0, -1->1, 1->2, -2->3, 2->4, 127->254, -128->255)
			uint32_t d = (diff >> 31) ^ (diff << 1);
			data[pos] = d;
//std::cout << pos << " orig " << value << " diff " << diff << " d " << d << std::endl;
			last = value;
		}
		
		pos = 0;
		while (pos < size)
		{
			// find match
			size_t offset;
			uint32_t length = 0;

			// check if the minimum number of symbols required for a dictionary lookup is still following
			if (pos + minLength <= size)
			{
				// calc hash key from first values (modified Bernstein)
				// http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx
				uint32_t h = 0;
				for (int i = 0; i < minLength; ++i)
					h = 33 * (h ^ data[pos + i]);
				int key = (h ^ (h >> 16)) & (hashSize - 1);

				// prepend current position to hash bucket
				size_t o = hash[key];
				hash[key] = pos;
				window[pos & (windowSize - 1)] = o;
				
				// find match
				length = 0;
				while (o + windowSize > pos)
				{
					// match entry
					uint32_t l = 0;
					while (pos + l < size && data[o + l] == data[pos + l])
						++l;
					
					// check for longest match
					if (l > length)
					{
						offset = o;
						length = l;
					}
					
					o = window[o & (windowSize - 1)];
				}
			}
					
			if (length < minLength)
			{
				// literal: type = 0
				this->writeBitAndUpdateWeight(0, this->typeWeight, typeShift);

				// write value
				this->writeUniversalCode(data[pos], this->literalWeights, literalShift1, literalShift2);
				++pos;
			}
			else
			{
				//dNotify("pos " << pos - offset << " len " << length);
				
				// dictionary reference: type = 1
				this->writeBitAndUpdateWeight(1, this->typeWeight, typeShift);

				// write length and offset
				this->writeUniversalCode(pos - offset - 1, this->offsetWeights, offsetShift1, offsetShift2);
				this->writeUniversalCode(length - minLength, this->lengthWeights, lengthShift1, lengthShift2);
				
				// add hash entries for second to last symbol in dictionary reference
				if (pos + length + minLength <= size)
				{
					size_t end = pos + length;
					for (++pos; pos < end; ++pos)
					{
						// calc hash key from first values (modified Bernstein)
						uint32_t h = 0;
						for (int i = 0; i < minLength; ++i)
							h = 33 * (h ^ data[pos + i]);
						int key = (h ^ (h >> 16)) & (hashSize - 1);
					
						// prepend current position to hash bucket
						size_t o = hash[key];
						hash[key] = pos;
						window[pos & (windowSize - 1)] = o;
					}
					
				}
				else
				{
					pos += length;
				}
			}
		}
	}

	template <typename Container>
	void compress(Container& container)
	{
		this->compress(container.data(), container.size());
	}

	// flush out compressed bytes that are stored in range encoder
	void flush();

protected:

	void shiftOutput();
	void writeBitAndUpdateWeight(int bit, uint16_t& weight, int shift);
	void writeUniversalCode(uint32_t value, uint16_t* weights, int shift1, int shift2);


	BufferedWriter& w;
	
	// range encoder
	uint32_t range;
	uint64_t low;
	uint8_t cache;
	int cachesize;	

	// weights
	uint16_t typeWeight;
	uint16_t offsetWeights[64];
	uint16_t lengthWeights[64];
	uint16_t literalWeights[64];
};


/// differential LZSS. The input is assumed to be a series of difference values where small values are more probable
class DlzssDecompressor
{
public:

	DlzssDecompressor(uint8_t* compressed, size_t size);
	
	// decompress into array of 8 bit, 16 bit or 32 bit unisgned integer values
	template <typename Iterator>
	void decompress(Iterator data, size_t size)
	{
		const int typeShift = 4;
		const int literalShift1 = 4;
		const int literalShift2 = 4;
		const int offsetShift1 = 4;
		const int offsetShift2 = 4;
		const int lengthShift1 = 4;
		const int lengthShift2 = 4;

		const int minLength = 3;

		size_t pos = 0;
		while (pos < size)
		{
			if (this->readBitAndUpdateWeight(this->typeWeight, typeShift) == 0)
			{
				// literal
				data[pos++] = this->readUniversalCode(this->literalWeights, literalShift1, literalShift2);
			}
			else
			{
				// dictionary reference
				uint32_t offset = this->readUniversalCode(this->offsetWeights, offsetShift1, offsetShift2) + 1;
				uint32_t length = this->readUniversalCode(this->lengthWeights, lengthShift1, lengthShift2) + minLength;
				for (uint32_t i = 0; i < length; ++i)
				{
					data[pos] = data[pos - offset];
					++pos;
				}
			}
		}

		// regenerate original values
		uint32_t sum = 0;
		for (pos = 0; pos < size; ++pos)
		{				
			// convert from signed magnitude (0->0, 1->-1, 2->1, 3->-2, 4->2, 254->127, 255->-128) to two's complement
			uint32_t d = data[pos];
			int32_t diff = d & 1 ? (-(d >> 1) - 1) : d >> 1;
			data[pos] = sum += diff;
//std::cout << pos << " orig " << uint32_t(data[pos]) << " diff " << diff << " map " << d << std::endl;
		}
	}

	template <typename Container>
	void decompress(Container& container)
	{
		this->decompress(container.data(), container.size());
	}

protected:

	int readBitAndUpdateWeight(uint16_t& weight, int shift);
	uint32_t readUniversalCode(uint16_t* weights, int shift1, int shift2);

	uint8_t* compressed;
	size_t size;
	size_t index;
	
	// range decoder
	uint32_t range;
	uint32_t code;

	// weights
	uint16_t typeWeight;
	uint16_t offsetWeights[64];
	uint16_t lengthWeights[64];
	uint16_t literalWeights[64];
};

/// @}

} // namespace digi

#endif
