#ifndef digi_Compress_LocoStream_h
#define digi_Compress_LocoStream_h

#include <digi/Utility/ArrayUtility.h>
#include <digi/System/Log.h>
#include <digi/Data/BufferedWriter.h>


namespace digi {

/// @addtogroup Compress
/// @{

/// LOCO-I (LOw COmplexity LOssless COmpression for Images), the algorithm from JPEG-LS,
/// applied to a serial stream of data: LOCO-S
class LocoStreamCompressor
{
public:

	struct Context
	{
		// accumulated absolutes of error values
		int A;
		
		// accumulated error values
		int B;
		
		// centering correction
		int C;
		
		// number of values
		int N;		

		Context(int A)
			: A(A), B(0), C(0), N(1) {}
	};

	LocoStreamCompressor(BufferedWriter& w);
	
	template <typename Iterator>
	void compress(Iterator data, size_t size)
	{
		int bitCount = sizeof(data[0]) * 8;
		int signShift = 32 - bitCount;
		//int mask = (1 << bitCount) - 1;
		if (size == 0)
			return;
	
		std::vector<Context> contexts(365, Context(1 << (bitCount - 6)));
				
		// write first value directly
		writeBinaryValue(data[0], bitCount);

		int Rb = data[0];
		int Rc = Rb;
		int Rd = Rc;
		size_t pos = 1;
		while (pos < size)
		{
			// last values
			int Ra = data[pos - 1];
	
			// calc context index
			int shift = 0;//24 - bitCount;
			int g1 = quantizeGradient(((Rd - Rc) << signShift) >> shift);
			int g2 = quantizeGradient(((Rc - Rb) << signShift) >> shift);
			int g3 = quantizeGradient(((Rb - Ra) << signShift) >> shift);
			int Qs = (g1 * 9 + g2) * 9 + g3;
		
			if (Qs == 0)
			{
				// run mode
				size_t start = pos;
				while (pos < size && data[pos] == Ra)
					++pos;
				
				// write run length + 1 in elias gamma code
				// http://en.wikipedia.org/wiki/Elias_gamma_coding
				int length = int(pos - start) + 1;
				
				// number of bits to represent run length
				int count;
				for (count = 1; (1 << count) <= length; ++count);
				
				// write bit count as unary
				this->writeBinaryValue(0, count - 1);
				
				// write length as binary (first bit terminates unary count - 1)
				this->writeBinaryValue(length, count);
				
			}
			//if (Qs != 0)
			{
				// encode one value
				
				// get context
				Context& context = contexts[Qs >= 0 ? Qs : -Qs];
				int A = context.A;
				int B = context.B;
				int C = context.C;
				int N = context.N;
				
				// calc prediction
				int p = (Qs >= 0 ? context.C : -context.C) + Ra + Rb - Rc;
				int prediction = p;//std::min(std::max(p, 0), (1 << bitCount) - 1);
				
				// get value
				int value = data[pos];
				
				// calc prediction error
				int error = (Qs >= 0 ? value - prediction : prediction - value);
				
				// calc golomb parameter
				int k;
				for (k = 0; (N << k) < A; ++k);
				
				// correct error if k is zero (see paper)
				if (k == 0)
					error ^= (2 * B + N - 1) >> 31;
				
				// truncate to bitCount and sign extend
				error = (error << signShift) >> signShift;
				
				// map negative numbers to positive numbers (0->0, -1->1, 1->2, -2->3, 2->4, 127->254, -128->255)
				int mapped = (error >> 31) ^ (error << 1);
				
				int highBits = mapped >> k;
				int limit = 48;
				if (highBits < limit)
				{
					writeUnaryValue(highBits);
					writeBinaryValue(mapped & ((1 << k) - 1), k);	
				}
				else
				{
					// highBits is too large: write limit + number of needed bits 
					mapped -= limit;
					int i;
					for (i = 0; (1 << i) < mapped; ++i);
					writeUnaryValue(limit + i);
					writeBinaryValue(mapped, i);	
				}
			dNotify("value " << value << " error " << error << " " << mapped << " Qs " << Qs << " " << g1 << " " << g2 << " " << g3
				<< " " << " k " << k << " highBits " << highBits);
			
				// update context
				A += abs(error);
				B += error;
				++N;					
				if (N == 64) 
				{
					A >>= 1;
					B >>= 1;
					N >>= 1;
				}
				if (B + N <= 0) 
				{
					B = -N >> 1;
					--C;
				} 
				else if (B > 0) 
				{
					B = -N >> 1;
					++C;
				}
				
				// write back
				context.A = A;
				context.B = B;
				context.C = C;
				context.N = N;
			
				++pos;
			}
			
			Rd = Rc;
			Rc = Rb;
			Rb = Ra;
		}
	}

	template <typename Container>
	void compress(Container& container)
	{
		this->compress(container.data(), container.size());
	}

	// flush out compressed bytes that are stored in buffer
	void flush();

protected:

	// write value in binary format
	void writeBinaryValue(int value, int bitCount);

	// write value in unary format (value times zero followed by a one, e.g. 5 -> 000001)
	void writeUnaryValue(int value)
	{
		this->writeBinaryValue(1, value + 1);
	}

	static int quantizeGradient(int g)
	{
		const int T1 = 3 << 24;
		const int T2 = 7 << 24;
		const int T3 = 21 << 24;
	
		if (g <= -T3) return  -4;
		if (g <= -T2) return  -3;
		if (g <= -T1) return  -2;
		if (g < 0)    return  -1;
		if (g == 0)   return   0;
		if (g < T1)   return   1;
		if (g < T2)   return   2;
		if (g < T3)   return   3;
		return  4;
	}


	BufferedWriter& w;
	
	uint32_t bitBuffer;
	int freeCount;
};


/// @}

} // namespace digi

#endif
