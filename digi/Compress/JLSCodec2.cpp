#include <stddef.h>

#include <boost/range.hpp>

#define CHARLS_IMEXPORT
#include <CharLS.h>

#include <digi/Utility/ArrayUtility.h>
#include <digi/System/MemoryDevices.h>
#include <digi/Data/DataWriter.h>
#include <digi/Data/DataReader.h>

#include "JLSCodec2.h"


//---

typedef ContainerDevice<std::vector<uint8_t>&> ArrayRefIODevice;
/*
class ArrayRefIODevice : public File
{
	public:
	
		ArrayRefIODevice(std::vector<uint8_t>& data)
			: position(0), data(data), state(OPEN)
		{}

		virtual ~ArrayRefIODevice()
		{
		}

		virtual size_t readData(void* data, size_t length)
		{
			//size_t numRead = size_t(max(min(int64_t(length), int64_t(this->data.size()) - int64_t(this->position)), int64_t(0)));
			size_t numRead = min(length, size_t(this->data.size() - this->position));
			memcpy(data, &this->data[this->position], numRead);
			this->position += numRead;

			if (numRead == 0 && length > 0)
			{
				this->state |= END_OF_INPUT;
			}			

			return numRead;
		}
		
		virtual size_t writeData(const void* data, size_t length)
		{
			size_t size = this->data.size();
			if (this->position + length > size)
				this->data.resize(this->position + length);
			memcpy(&this->data[this->position], data, length);
			this->position += length;
			return length;
		}

		virtual void close()
		{
			this->state = 0;
		}

		virtual int getState()
		{
			return this->state;
		}

		virtual int64_t setPosition(int64_t position, PositionMode positionMode)
		{
			switch (positionMode)
			{
				case BEGIN:
					this->position = size_t(position);
					break;
				case CURRENT:
					this->position = size_t(this->position + position);
					break;
				case END:
					this->position = size_t(this->data.size() + position);
					break;
			}
			return this->position;
		}
		
	protected:
	
		size_t position;
		std::vector<uint8_t>& data;
		int state;
};
*/

//---

struct JLSBitWriter : public DataWriter
{
	JLSBitWriter(Pointer<IODevice> dev, bool bigEndian = false)
		: DataWriter(dev, bigEndian), bitPos(32), bitBuffer(0), insertZero(false), numWritten(0) {}
	
	void writeBit(bool bit)
	{
		this->writeBinaryValue(bit ? 1 : 0, 1);
	}
	
	void writeBinaryValue(uint value, int numBits)
	{
		// check if we want to write more bits than are available in the bit buffer
		while (numBits > this->bitPos)
		{
			// add upper bits to bit buffer
			int shift = numBits - this->bitPos;
			if (shift < 32)
				this->bitBuffer |= value >> shift;
			
			// reset bit buffer
			this->bitPos = 32;

			// store bit buffer (32 bits)
			uint8_t buffer[4];
			for (int i = 0; i < 4; ++i)
			{
				// check if we have to insert a zero bit after 0xFF has been written
				if (this->insertZero)
				{
					buffer[i] = this->bitBuffer >> 25;
					this->bitBuffer <<= 7;
					this->insertZero = false;
					
					// since we stored an additional zero, one bit will remain in the bit buffer
					--this->bitPos;
				}
				else
				{
					buffer[i] = this->bitBuffer >> 24;
					this->bitBuffer <<= 8;
					this->insertZero = buffer[i] == 0xff;
				}
			}
			this->writeData(buffer, 4);
			this->numWritten += 4;
			
			// set number of remaining bits
			numBits = shift;
		}
		
		this->bitPos -= numBits;
		this->bitBuffer |= value << this->bitPos;
	}
	
	void writeUnaryValue(int value)
	{
		this->writeBinaryValue(1, value + 1);
	}
	
	void flushBits()
	{
		uint8_t buffer[5];
		int i = 0;
		while (this->bitPos < 32 || this->insertZero)
		{
			if (this->insertZero)
			{
				buffer[i] = this->bitBuffer >> 25;
				this->bitBuffer <<= 7;
				this->insertZero = false;
				this->bitPos += 7;
			}
			else
			{
				buffer[i] = this->bitBuffer >> 24;
				this->bitBuffer <<= 8;
				this->insertZero = buffer[i] == 0xff;
				this->bitPos += 8;
			}
			++i;
		}
		this->writeData(buffer, i);
	}

	//Pointer<IODevice> dev;
	
	int bitPos;
	uint bitBuffer;
	bool insertZero;
	
	// debug
	int numWritten;
};

//---

struct JLSBitReader : public DataReader
{
	JLSBitReader(Pointer<IODevice> dev, bool bigEndian = false)
		: DataReader(dev, bigEndian), bitPos(0), bitBuffer(0), insertZero(false), numRead(0) {}
	
	bool readBit()
	{
		if (this->bitPos == 0)
		{
			// read next byte
			uint8_t b = this->read<uint8_t>();
			++this->numRead;
			
			// check if a zero bit follows after 0xFF has been read
			if (this->insertZero)
			{
				this->insertZero = false;
				this->bitBuffer = b;
				this->bitPos = 7;
			}
			else
			{
				this->insertZero = b == 0xff;
				this->bitBuffer = b;
				this->bitPos = 8;
			}
		}

		--this->bitPos;
		return ((this->bitBuffer >> this->bitPos) & 1) != 0;
	}

	uint readBinaryValue(int numBits)
	{
		while (this->bitPos - numBits < 0)
		{
			// read next byte
			uint8_t b = this->read<uint8_t>();
			++this->numRead;
		
			// check if a zero bit follows after 0xFF has been read
			if (this->insertZero)
			{
				this->insertZero = false;
				this->bitBuffer <<= 7;
				this->bitBuffer |= b;
				this->bitPos += 7;
			}
			else
			{
				this->insertZero = b == 0xff;
				this->bitBuffer <<= 8;
				this->bitBuffer |= b;
				this->bitPos += 8;
			}
		}

		this->bitPos -= numBits;
		uint mask = ~(uint(-1) << numBits);
		return (this->bitBuffer >> this->bitPos) & mask;
	}

	int readUnaryValue()
	{
		int value = 0;
		
		while (!this->readBit())
			++value;
			
		return value;
/*		
		// count zero bits until the first one bit
		do
		{
			if (this->bitPos == 0)
			{
				// read next byte
				uint8_t b[1];
				this->dev->readData(b, 1);
				++this->numRead;
				
				// check if a zero bit follows after 0xFF has been read
				if (this->insertZero)
				{
					this->insertZero = false;
					this->bitBuffer = b[0];
					this->bitPos = 7;
				}
				else
				{
					this->insertZero = b[0] == 0xff;
					this->bitBuffer = b[0];
					this->bitPos = 8;
				}
			}

			while (this->bitPos > 0)
			{
				if (((this->bitBuffer >> this->bitPos) & 1) != 0)
					break;
				++value;
				--this->bitPos;
			}
		} while (this->bitPos == 0);
		
		// remove one bit
		--this->bitPos;
		
		return value;
*/				
	}

	//Pointer<IODevice> dev;
	
	int bitPos;
	uint bitBuffer;
	bool insertZero;
	
	// debug
	int numRead;
};

//---

#define JPEG_SOI   0xD8 // start of image
#define JPEG_EOI   0xD9 // end of image
#define JPEG_SOS   0xDA // start of scan

#define JPEG_SOF   0xF7 // start of frame (JPEG-LS)

#define JPEG_LSE   0xF8
#define JPEG_DNL   0xDC
#define JPEG_DRI   0xDD
#define JPEG_RSTm  0xD0
#define JPEG_COM   0xFE
#define JPEG_APP0  0xE0 // JFIF
#define JPEG_APP7  0xE7 // colorspace
#define JPEG_APP8  0xE8 // colorXForm


JLSCodec2::~JLSCodec2()
{
	
}
static const int BPP = 16;
static const int LIMIT = 64;
static const int RANGE = 65536;
static const int RESET = 64;
static const int NEAR = 0;

static const int BASIC_T1 = 3;
static const int BASIC_T2 = 7;
static const int BASIC_T3 = 21;

static int8_t QuantizeGradientOrg(int Di)
{
	const int FACTOR = (4095 + 128)/256;

	const int T1 = FACTOR * (BASIC_T1 - 2) + 2;
	const int T2 = FACTOR * (BASIC_T2 - 3) + 3;
	const int T3 = FACTOR * (BASIC_T3 - 4) + 4;

	if (Di <= -T3) return  -4;
	if (Di <= -T2) return  -3;
	if (Di <= -T1) return  -2;
	if (Di < -0)  return  -1;
	if (Di <=  0) return   0;
	if (Di < T1)   return   1;
	if (Di < T2)   return   2;
	if (Di < T3)   return   3;
	return  4;
}

#define QuantizeGratient(g) lut[65535 + (g)]

static inline int ComputeContextID(int Q1, int Q2, int Q3)
{
	return (Q1*9 + Q2)*9 + Q3;
}

static inline int Sign(int n)
{
	return (n >> 31) | 1;
}

static inline int BitWiseSign(int i)
{
	return i >> 31;
}

static inline int ApplySign(int i, int sign)
{
	return (sign ^ i) - sign;
}

static inline int GetPredictedValue(int Ra, int Rb, int Rc)
{
	if (Ra < Rb)
	{
		if (Rc < Ra)
			return Rb;

		if (Rc > Rb)
			return Ra;
	}
	else
	{
		if (Rc < Rb)
			return Ra;

		if (Rc > Ra)
			return Rb;
	}

	return Ra + Rb - Rc;
}

struct JlsContext
{
public:
	JlsContext() 
	{}

 	JlsContext(int a) :
		A(a),
		B(0),
		C(0),
		N(1)
	{
	}

	// accumulated absolutes of error values
	int A;
	
	// accumulated error values
	int B;
	
	// centering correction
	int C;
	
	// number of values
	int N;

	int GetErrorCorrection(int k) const
	{
		if (k != 0)
			return 0;

		return BitWiseSign(2 * B + N - 1);
	}
	

	void UpdateVariables(int errorValue)
	{
		//ASSERT(N != 0);

		// For performance work on copies of A,B,N (compiler will use registers).
		int b = B + errorValue;
		int a = A + abs(errorValue);
		int n = N;

		//ASSERT(a < 65536 * 256);
		//ASSERT(abs(b) < 65536 * 256);
		
		if (n == RESET) 
		{
			a = a >> 1;
			b = b >> 1;
			n = n >> 1;
		}

		n = n + 1;
		
		if (b + n <= 0) 
		{
			b = b + n;
			if (b <= -n)
			{
				b = -n + 1;
			}
			C = _tableC[C - 1];
		} 
		else  if (b > 0) 
		{
			b = b - n;				
			if (b > 0)
			{
				b = 0;
			}
			C = _tableC[C + 1];
		}
		A = a;
		B = b;
		N = n;
		//ASSERT(N != 0);
	}



	int GetGolomb() const
	{
		// k = ceil(log2(A / N))
		int Ntest = N;
		int Atest = A;
		int k = 0;
		for(; (Ntest << k) < Atest; k++) 
		{ 
			//ASSERT(k <= 32); 
		}
		return k;
	}

	static void CreateTableC()
	{
		static std::vector<signed char> rgtableC;
		
		rgtableC.reserve(256 + 2);

		rgtableC.push_back(-128);	
		for (int i = -128; i < 128; i++)
		{
			rgtableC.push_back(char(i));	
		}
		rgtableC.push_back(127);	
		
		signed char* pZero = &rgtableC[128 + 1];	
		//ASSERT(pZero[0] == 0);
		_tableC = pZero;
	}
private:

	static signed char* _tableC;
};

struct CContextRunMode 
{
	CContextRunMode(int a, int nRItype, int nReset) :
		A(a),
		N(1),	
		Nn(0),
		_nRItype(nRItype),
		_nReset(nReset)
	{
	}

	int A;
	int N;
	int Nn;
	int _nRItype;
	int _nReset;

	CContextRunMode()
	{}

	inline int GetGolomb() const
	{
		int Ntest	= N;
		int TEMP	= A + (N >> 1) * _nRItype;
		int k = 0;
		for(; Ntest < TEMP; k++) 
		{ 
			Ntest <<= 1;
			//ASSERT(k <= 32); 
		};
		return k;
	}

	void UpdateVariables(int Errval, int EMErrval)
	{		
		if (Errval < 0)
		{
			Nn = Nn + 1;
		}
		A = A + ((EMErrval + 1 - _nRItype) >> 1);
		if (N == _nReset) 
		{
			A = A >> 1;
			N = N >> 1;
			Nn = Nn >> 1;
		}
		N = N + 1;
	}

	inline int ComputeErrVal(int temp, int k)
	{
		bool map = temp & 1;

		int errvalabs = (temp + map) / 2;

		if ((k != 0 || (2 * Nn >= N)) == map)
		{
			//ASSERT(map == ComputeMap(-errvalabs, k));
			return -errvalabs;
		}

		//ASSERT(map == ComputeMap(errvalabs, k));	
		return errvalabs;
	}

	bool ComputeMap(int Errval, int k) const
	{
		if ((k == 0) && (Errval > 0) && (2 * Nn < N))
			return 1;

		else if ((Errval < 0) && (2 * Nn >= N))
			return 1;		 

		else if ((Errval < 0) && (k != 0))
			return 1;

		return 0;
	}

	inline int ComputeMapNegativeE(int k) const
	{
		return  k != 0 || (2 * Nn >= N );
	}
};

static inline int CorrectPrediction(int Pxc)
{
	if ((Pxc & 65535) == Pxc)
		return Pxc;
	
	return (~(Pxc >> 31)) & 65535;		
}

static inline int GetMappedErrVal(int Errval)
{
	return (Errval >> 31) ^ (2 * Errval);
}

static inline int UnMapErrVal(int mappedError)
{
	//int sign = ~((mappedError & 1) - 1);
	int sign = (mappedError << 31) >> 31;
	return sign ^ (mappedError >> 1);
}


static inline void EncodeMappedValue(JLSBitWriter& w, int k, int mappederval, int limit)
{
	int highbits = mappederval >> k;

	if (highbits < limit - BPP - 1)
	{
		// unary encoding of high bits (...000001)
		w.writeUnaryValue(highbits);
		
		// binary encoding of k low bits
		w.writeBinaryValue(mappederval & ((1 << k) - 1), k);
		return;
	}
	
	w.writeUnaryValue(LIMIT - (BPP + 1));
	w.writeBinaryValue((mappederval - 1) & ((1 << BPP) - 1), BPP);
}

static inline int DecodeValue(JLSBitReader& r, int k, int limit, int qbpp)
{
	int highbits = r.readUnaryValue();

	if (highbits >= limit - (qbpp + 1))
		return r.readBinaryValue(qbpp) + 1;

	return (highbits << k) + r.readBinaryValue(k);
}

static inline uint16_t ComputeReconstructedSample(int Px, int ErrVal)
{
	return 65535 & (Px + ErrVal);
}

// used to determine how large runs should be encoded at a time. 
static const int J[32] = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 9, 10, 11, 12, 13, 14, 15};


static inline void EncodeRIError(JLSBitWriter& w, CContextRunMode& ctx, int Errval, int RUNindex)
{
	int k			= ctx.GetGolomb();
	bool map		= ctx.ComputeMap(Errval, k);
	int EMErrval	= 2 * abs(Errval) - ctx._nRItype - map;	

	//ASSERT(Errval == ctx.ComputeErrVal(EMErrval + ctx._nRItype, k));
	EncodeMappedValue(w, k, EMErrval, LIMIT - J[RUNindex] - 1);
	ctx.UpdateVariables(Errval, EMErrval);
}

static inline int DecodeRIError(JLSBitReader& r, CContextRunMode& ctx, int RUNindex)
{
	int k = ctx.GetGolomb();
	int EMErrval = DecodeValue(r, k, LIMIT - J[RUNindex]-1, BPP);	
	int Errval = ctx.ComputeErrVal(EMErrval + ctx._nRItype, k);
	ctx.UpdateVariables(Errval, EMErrval);
	return Errval;
}


bool JLSCodec2::compress(UByteArray& compressedData, const uint16_t* image, int width, int height, int depth)
{
	height *= depth;
	
	Pointer<IODevice> dev = new ArrayRefIODevice(compressedData);
	JLSBitWriter w(dev, true);
	
	// start of image
	{
		w.write<uint8_t>(0xFF);
		w.write<uint8_t>(JPEG_SOI);
	}
	
	// start of frame
	{
		w.write<uint8_t>(0xFF);
		w.write<uint8_t>(JPEG_SOF);
		
		w.write<uint16_t>(2 + 9); // length of segment without header but including the length

		w.write<uint8_t>(16); // bpp
		w.write<uint16_t>(height);
		w.write<uint16_t>(width);

		w.write<uint8_t>(1); // number of components
		w.write<uint8_t>(1); // 1st component
		w.write<uint8_t>(0x11);
		w.write<uint8_t>(0);
	}

	// start of scan
	{
		w.write<uint8_t>(0xFF);
		w.write<uint8_t>(JPEG_SOS);
	
		w.write<uint16_t>(2 + 6); // length of segment without header but including the length
	
		w.write<uint8_t>(1); // number of components
		w.write<uint8_t>(1); // 1st component
		w.write<uint8_t>(0); // itable?

		w.write<uint8_t>(0); // allowed lossy error
		w.write<uint8_t>(0); // interleaved mode
		w.write<uint8_t>(0); // transform
	}	

	// quantization lookup table
	std::vector<int8_t> lut(65536 * 2 - 1);
	for (int diff = -65535; diff < 65536; diff++)
	{
		lut[65535 + diff] = QuantizeGradientOrg(diff);
	}

	// contexts
	JlsContext::CreateTableC();
	JlsContext _contexts[365];
	int A = max(2, (RANGE + 32)/64);
	for (size_t Q = 0; Q < boost::size(_contexts); ++Q)
	{
		_contexts[Q] = JlsContext(A);
	}

	CContextRunMode _contextRunmode[2];
	_contextRunmode[0] = CContextRunMode(max(2, (RANGE + 32)/64), 0, RESET);
	_contextRunmode[1] = CContextRunMode(max(2, (RANGE + 32)/64), 1, RESET);

	// compress data
	
	// DoScan()
	{
		//BitWriter w(dev);
		int pixelstride = width + 4;
	
		std::vector<uint16_t> vectmp;
		vectmp.resize(2 * pixelstride);
		
		int RUNindex = 0;

		for (int iline = 0; iline < height; ++iline)
		{
			uint16_t* ptypePrev = &vectmp[1];              // previous line ptr
			uint16_t* ptypeCur = &vectmp[1 + pixelstride]; // current line ptr
			if ((iline & 1) == 1)
			{
				std::swap(ptypePrev, ptypeCur);
			}

			// STRATEGY::OnLineBegin(_size.cx, ptypeCur, pixelstride)
			::memcpy(ptypeCur, image, width * sizeof(uint16_t));
			image += width;
			
			{
				//int RUNindex = rgRUNindex;

				// initialize edge pixels used for prediction
				ptypePrev[width]	= ptypePrev[width - 1];
				ptypeCur[-1]		= ptypePrev[0];
				
				// DoLine((PIXEL*) NULL)
				{
					int ipixel = 0;
					int Rb = ptypePrev[ipixel-1];
					int Rd = ptypePrev[ipixel];

					while (ipixel < width)
					{	
						int Ra = ptypeCur[ipixel -1];
						int Rc = Rb;
						Rb = Rd;
						Rd = ptypePrev[ipixel + 1];

						int Qs = ComputeContextID(QuantizeGratient(Rd - Rb), QuantizeGratient(Rb - Rc), QuantizeGratient(Rc - Ra));

						if (Qs != 0)
						{
							// ptypeCur[ipixel] = DoRegular(Qs, ptypeCur[ipixel], GetPredictedValue(Ra, Rb, Rc), (STRATEGY*)(NULL));
							{
								int x = ptypeCur[ipixel];
								int pred = GetPredictedValue(Ra, Rb, Rc);
								
								int sign = BitWiseSign(Qs);
								JlsContext& ctx	= _contexts[ApplySign(Qs, sign)];
								int k			= ctx.GetGolomb();
								int Px			= CorrectPrediction(pred + ApplySign(ctx.C, sign));	

								int ErrVal		= short(ApplySign(x - Px, sign));

								EncodeMappedValue(w, k, GetMappedErrVal(ctx.GetErrorCorrection(k) ^ ErrVal), LIMIT);
								ctx.UpdateVariables(ErrVal);
								ptypeCur[ipixel] = ComputeReconstructedSample(Px, ApplySign(ErrVal, sign));
							}
							
							ipixel++;
						}
						else
						{
							// ipixel += DoRunMode(ipixel, (STRATEGY*)(NULL));
							{
								int ctypeRem = width - ipixel;
								uint16_t* ptypeCurX = ptypeCur + ipixel;
								uint16_t* ptypePrevX = ptypePrev + ipixel;

								uint16_t Ra = ptypeCurX[-1];

								int runLength = 0;

								while (ptypeCurX[runLength] == Ra) 
								{
									ptypeCurX[runLength] = Ra;
									runLength++;

									if (runLength == ctypeRem)
										break;
								}

								//EncodeRunPixels(runLength, runLength == ctypeRem);
								int savedRunLength = runLength;
								{
									bool bEndofline = runLength == ctypeRem;
									
									while (runLength >= (1 << J[RUNindex])) 
									{
										//STRATEGY::AppendOnesToBitStream(1);
										w.writeBit(true);
										
										runLength = runLength - (1 << J[RUNindex]);
										//IncrementRunIndex();
										RUNindex = min(RUNindex + 1, 31);
									}

									if (bEndofline) 
									{
										if (runLength != 0) 
										{
											//STRATEGY::AppendOnesToBitStream(1);	
											w.writeBit(true);
										}
									}
									else
									{
										//STRATEGY::AppendToBitStream(runLength, J[RUNindex] + 1);	// leading 0 + actual remaining length
										w.writeBinaryValue(runLength, J[RUNindex] + 1);
									}
								}
								runLength = savedRunLength;

								if (runLength == ctypeRem)
									ipixel += runLength;
								else
								{
									//ptypeCurX[runLength] = EncodeRIPixel(ptypeCurX[runLength], Ra, ptypePrevX[runLength]);
									{
										int x = ptypeCurX[runLength];
										int Rb = ptypePrevX[runLength];
										
										if (abs(Ra - Rb) <= 0)
										{
											//int ErrVal = traits.ComputeErrVal(x - Ra);
											int ErrVal = short(x - Ra);
											EncodeRIError(w, _contextRunmode[1], ErrVal, RUNindex);
											ptypeCurX[runLength] = ComputeReconstructedSample(Ra, ErrVal);
										}
										else
										{
											//int ErrVal = traits.ComputeErrVal((x - Rb) * Sign(Rb - Ra));
											int ErrVal = short((x - Rb) * Sign(Rb - Ra));
											EncodeRIError(w, _contextRunmode[0], ErrVal, RUNindex);
											ptypeCurX[runLength] = ComputeReconstructedSample(Rb, ErrVal * Sign(Rb - Ra));
										}
									}
									//DecrementRunIndex();
									RUNindex = max(RUNindex - 1, 0);
									ipixel += runLength + 1;
								}
							} // ipixel += DoRunMode()
							
							Rb = ptypePrev[ipixel-1];
							Rd = ptypePrev[ipixel];
						}	
					}
				} // DoLine()
		
				//rgRUNindex = RUNindex;
				//ptypePrev += pixelstride;
				//ptypeCur += pixelstride;
			}
			
			// STRATEGY::OnLineEnd(_size.cx, ptypeCur - (components * pixelstride), pixelstride);
			// nothing to do			
		}

		// STRATEGY::FlushStreamEnd()
		w.flushBits();
	}

	// end of image
	{
		w.write<uint8_t>(0xFF);
		w.write<uint8_t>(JPEG_EOI);
	}

	return true;
}

bool JLSCodec2::decompress(const UByteArray& compressedData, uint16_t* image, int width, int height, int depth)
{
	height *= depth;

	Pointer<IODevice> dev = new ArrayRefIODevice((UByteArray&)compressedData);
	JLSBitReader r(dev, true);
	
	// start of image
	{
		int ff = r.read<uint8_t>();
		int soi = r.read<uint8_t>();
		if (ff != 0xff || soi != JPEG_SOI)
			return false;
	}
	
	// start of frame
	{
		int ff = r.read<uint8_t>();
		int sof = r.read<uint8_t>();
		
		int len = r.read<uint16_t>(); // length of segment without header but including the length

		if (ff != 0xff || sof != JPEG_SOF || len != 2 + 9)
			return false;

		int bpp = r.read<uint8_t>(); // bpp
		int rh = r.read<uint16_t>();
		int rw = r.read<uint16_t>();
		if (bpp != 16 || rw != width || rh != height)
			return false;

		r.read<uint8_t>(); // number of components
		r.read<uint8_t>(); // 1st component
		r.read<uint8_t>();
		r.read<uint8_t>();
	}

	// start of scan
	{
		r.read<uint8_t>();
		r.read<uint8_t>();
	
		r.read<uint16_t>(); // length of segment without header but including the length
	
		r.read<uint8_t>(); // number of components
		r.read<uint8_t>(); // 1st component
		r.read<uint8_t>(); // itable?

		r.read<uint8_t>(); // allowed lossy error
		r.read<uint8_t>(); // interleaved mode
		r.read<uint8_t>(); // transform
	}	


	// quantization lookup table
	std::vector<int8_t> lut(65536 * 2 - 1);
	for (int diff = -65535; diff < 65536; diff++)
	{
		lut[65535 + diff] = QuantizeGradientOrg(diff);
	}

	JlsContext::CreateTableC();

	JlsContext _contexts[365];
	int A = max(2, (RANGE + 32)/64);
	for (size_t Q = 0; Q < boost::size(_contexts); ++Q)
	{
		_contexts[Q] = JlsContext(A);
	}

	CContextRunMode _contextRunmode[2];
	_contextRunmode[0] = CContextRunMode(max(2, (RANGE + 32)/64), 0, RESET);
	_contextRunmode[1] = CContextRunMode(max(2, (RANGE + 32)/64), 1, RESET);

	// compress data
	
	// DoScan()
	{
		//BitReader r(dev);
		int pixelstride = width + 4;
	
		std::vector<uint16_t> vectmp;
		vectmp.resize(2 * pixelstride);
		
		int RUNindex = 0;

		for (int iline = 0; iline < height; ++iline)
		{
			uint16_t* ptypePrev = &vectmp[1];              // previous line ptr
			uint16_t* ptypeCur = &vectmp[1 + pixelstride]; // current line ptr
			if ((iline & 1) == 1)
			{
				std::swap(ptypePrev, ptypeCur);
			}

			// STRATEGY::OnLineBegin(_size.cx, ptypeCur, pixelstride)
			// nothing to do
			
			{
				//int RUNindex = rgRUNindex;

				// initialize edge pixels used for prediction
				ptypePrev[width]	= ptypePrev[width - 1];
				ptypeCur[-1]		= ptypePrev[0];
				
				// DoLine()
				{
					int ipixel = 0;
					int Rb = ptypePrev[ipixel-1];
					int Rd = ptypePrev[ipixel];

					while (ipixel < width)
					{	
						int Ra = ptypeCur[ipixel -1];
						int Rc = Rb;
						Rb = Rd;
						Rd = ptypePrev[ipixel + 1];

						int Qs = ComputeContextID(QuantizeGratient(Rd - Rb), QuantizeGratient(Rb - Rc), QuantizeGratient(Rc - Ra));

						if (Qs != 0)
						{
							// ptypeCur[ipixel] = DoRegular(Qs, ptypeCur[ipixel], GetPredictedValue(Ra, Rb, Rc), (STRATEGY*)(NULL));
							{
								int pred = GetPredictedValue(Ra, Rb, Rc);

								int sign = BitWiseSign(Qs);
								JlsContext& ctx	= _contexts[ApplySign(Qs, sign)];
								int k			= ctx.GetGolomb();
								int Px			= CorrectPrediction(pred + ApplySign(ctx.C, sign));	

								int ErrVal;
								/*const Code& code		= rgtableShared[k].Get(STRATEGY::PeekByte());
								if (code.GetLength() != 0)
								{
									STRATEGY::Skip(code.GetLength());
									ErrVal = code.GetValue(); 
									ASSERT(abs(ErrVal) < 65535);
								}
								else*/
								{
									ErrVal = UnMapErrVal(DecodeValue(r, k, LIMIT, BPP)); 
									//if (abs(ErrVal) > 65535)
									//	throw JlsException(InvalidCompressedData);
								}	
								ErrVal = ErrVal ^ ((NEAR == 0) ? ctx.GetErrorCorrection(k) : 0);
								ctx.UpdateVariables(ErrVal);
								ErrVal = ApplySign(ErrVal, sign);
								// return traits.ComputeReconstructedSample(Px, ErrVal); 
								ptypeCur[ipixel] = uint16_t(Px + ErrVal);
							}
							
							ipixel++;
						}
						else
						{
							// ipixel += DoRunMode(ipixel, (STRATEGY*)(NULL));
							{
								int ipixelStart = ipixel;
								
								uint16_t Ra = ptypeCur[ipixelStart-1];

								//LONG cpixelRun = DecodeRunPixels(Ra, ptypeCur + ipixelStart, _size.cx - ipixelStart);
								int cpixelRun;
								{
									uint16_t* ptype = ptypeCur + ipixelStart;
									int cpixelMac = width - ipixelStart;
									
									int ipixel = 0;
									while (r.readBit())
									{
										int cpixel = min(1 << J[RUNindex], int(cpixelMac - ipixel));
										ipixel += cpixel;
										//ASSERT(ipixel <= cpixelMac);

										if (cpixel == (1 << J[RUNindex]))
										{
											// IncrementRunIndex()
											RUNindex = min(RUNindex + 1, 31);
										}

										if (ipixel == cpixelMac)
											break;
									}
									
									if (ipixel != cpixelMac)
									{
										// incomplete run 	
										ipixel += r.readBinaryValue(J[RUNindex]);
									}

									for (int i = 0; i < ipixel; ++i)
									{
										ptype[i] = Ra;
									}	
									
									// return ipixel;
									cpixelRun = ipixel;
								}

								int ipixelEnd = ipixelStart + cpixelRun;

								if (ipixelEnd == width)
								{
									// return ipixelEnd - ipixelStart;
									ipixel += ipixelEnd - ipixelStart;
								}
								else
								{
									// run interruption
									uint16_t Rb = ptypePrev[ipixelEnd];
									//ptypeCur[ipixelEnd] = DecodeRIPixel(Ra, Rb);
									{
										if (abs(Ra - Rb) <= NEAR)
										{
											int ErrVal = DecodeRIError(r, _contextRunmode[1], RUNindex);
											//return static_cast<SAMPLE>(traits.ComputeReconstructedSample(Ra, ErrVal));
											ptypeCur[ipixelEnd] = uint16_t(Ra + ErrVal);
										}
										else
										{
											int ErrVal = DecodeRIError(r, _contextRunmode[0], RUNindex);
											//return static_cast<SAMPLE>(traits.ComputeReconstructedSample(Rb, ErrVal * Sign(Rb - Ra)));
											ptypeCur[ipixelEnd] = uint16_t(Rb + ErrVal * Sign(Rb - Ra));
										}
									}
									// DecrementRunIndex()
									RUNindex = max(RUNindex - 1, 0);
									
									// return ipixelEnd - ipixelStart + 1;
									ipixel += ipixelEnd - ipixelStart + 1;
								}

							} // ipixel += DoRunMode()
							
							Rb = ptypePrev[ipixel-1];
							Rd = ptypePrev[ipixel];
						}	
					}
				} // DoLine()
		
				//rgRUNindex = RUNindex;
				//ptypePrev += pixelstride;
				//ptypeCur += pixelstride;
			}
			
			// STRATEGY::OnLineEnd(_size.cx, ptypeCur - (components * pixelstride), pixelstride);
			::memcpy(image, ptypeCur, width * sizeof(uint16_t));
			image += width;
		}

		// STRATEGY::FlushStreamEnd()
		//w.flush();
	}

	// end of image

	

	return true;
}

std::string JLSCodec2::getName()
{
	return "JPEG-LS";
}

std::string JLSCodec2::getExtension()
{
	return ".2.jls";
}


// test code

float3 hsvToRGB(float h, float s, float v)
{
	if (s == 0)
		return splat3(v);

	float var_h = h * 6.0f;
	float var_i = floor( var_h );
	float var_1 = v * ( 1.0f - s );
	float var_2 = v * ( 1.0f - s * ( var_h - var_i ) );
	float var_3 = v * ( 1.0f - s * ( 1 - ( var_h - var_i ) ) );

	float r;
	float g;
	float b;
	if      ( var_i == 0 ) { r = v     ; g = var_3 ; b = var_1; }
	else if ( var_i == 1 ) { r = var_2 ; g = v     ; b = var_1; }
	else if ( var_i == 2 ) { r = var_1 ; g = v     ; b = var_3; }
	else if ( var_i == 3 ) { r = var_1 ; g = var_2 ; b = v;     }
	else if ( var_i == 4 ) { r = var_3 ; g = var_1 ; b = v;     }
	else                   { r = v     ; g = var_1 ; b = var_2; }

	return vector3(r, g, b);
}

void JLSCodec2::getQImage(uint8_t* statImage, const uint16_t* image, int width, int height, int depth)
{
	height *= depth;
	
	// quantization lookup table
	std::vector<int8_t> lut(65536 * 2 - 1);
	for (int diff = -65535; diff < 65536; diff++)
	{
		lut[65535 + diff] = QuantizeGradientOrg(diff);
	}

	// compress data
	
	// DoScan()
	{
		int pixelstride = width + 4;
	
		std::vector<uint16_t> vectmp;
		vectmp.resize(2 * pixelstride);
		
		int RUNindex = 0;

		for (int iline = 0; iline < height; ++iline)
		{
			uint16_t* ptypePrev = &vectmp[1];              // previous line ptr
			uint16_t* ptypeCur = &vectmp[1 + pixelstride]; // current line ptr
			if ((iline & 1) == 1)
			{
				std::swap(ptypePrev, ptypeCur);
			}

			// STRATEGY::OnLineBegin(_size.cx, ptypeCur, pixelstride)
			::memcpy(ptypeCur, image, width * sizeof(uint16_t));
			image += width;
			
			{
				// initialize edge pixels used for prediction
				ptypePrev[width]	= ptypePrev[width - 1];
				ptypeCur[-1]		= ptypePrev[0];
				
				// DoLine((PIXEL*) NULL)
				{
					int ipixel = 0;
					int Rb = ptypePrev[ipixel-1];
					int Rd = ptypePrev[ipixel];

					while (ipixel < width)
					{	
						int Ra = ptypeCur[ipixel -1];
						int Rc = Rb;
						Rb = Rd;
						Rd = ptypePrev[ipixel + 1];

						int Qs = ComputeContextID(QuantizeGratient(Rd - Rb), QuantizeGratient(Rb - Rc), QuantizeGratient(Rc - Ra));
						
						float f = float(abs(Qs)) / 365.0f;
						/*
						float h = f;
						float s = 0.5f + 0.5f * f;
						float v = 0.5f + 0.5f * f;
						*/
						float h = 0.0f;
						float s = 0.0f;
						float v = f;
						
						float3 rgb = hsvToRGB(h, s, v);
						ubyte3 bytes = convert_ubyte3_sat_rte(rgb * 255.0f);
						statImage[0] = bytes.x;
						statImage[1] = bytes.y;
						statImage[2] = bytes.z;
						statImage += 3;
						++ipixel;
					}
				}
			}
		}
	}
}

void JLSCodec2::getkImage(uint8_t* statImage, const uint16_t* image, int width, int height, int depth)
{
	height *= depth;
	
	// quantization lookup table
	std::vector<int8_t> lut(65536 * 2 - 1);
	for (int diff = -65535; diff < 65536; diff++)
	{
		lut[65535 + diff] = QuantizeGradientOrg(diff);
	}

	// contexts
	JlsContext::CreateTableC();
	JlsContext _contexts[365];
	int A = max(2, (RANGE + 32)/64);
	for (size_t Q = 0; Q < boost::size(_contexts); ++Q)
	{
		_contexts[Q] = JlsContext(A);
	}

	// compress data
	
	// DoScan()
	{
		int pixelstride = width + 4;
	
		std::vector<uint16_t> vectmp;
		vectmp.resize(2 * pixelstride);
		
		int RUNindex = 0;

		for (int iline = 0; iline < height; ++iline)
		{
			uint16_t* ptypePrev = &vectmp[1];              // previous line ptr
			uint16_t* ptypeCur = &vectmp[1 + pixelstride]; // current line ptr
			if ((iline & 1) == 1)
			{
				std::swap(ptypePrev, ptypeCur);
			}

			// STRATEGY::OnLineBegin(_size.cx, ptypeCur, pixelstride)
			::memcpy(ptypeCur, image, width * sizeof(uint16_t));
			image += width;
			
			{
				// initialize edge pixels used for prediction
				ptypePrev[width]	= ptypePrev[width - 1];
				ptypeCur[-1]		= ptypePrev[0];
				
				// DoLine((PIXEL*) NULL)
				{
					int ipixel = 0;
					int Rb = ptypePrev[ipixel-1];
					int Rd = ptypePrev[ipixel];

					while (ipixel < width)
					{	
						int Ra = ptypeCur[ipixel -1];
						int Rc = Rb;
						Rb = Rd;
						Rd = ptypePrev[ipixel + 1];

						int Qs = ComputeContextID(QuantizeGratient(Rd - Rb), QuantizeGratient(Rb - Rc), QuantizeGratient(Rc - Ra));
						
						int x = ptypeCur[ipixel];
						int pred = GetPredictedValue(Ra, Rb, Rc);
						int sign = BitWiseSign(Qs);
						JlsContext& ctx	= _contexts[ApplySign(Qs, sign)];
						int k			= ctx.GetGolomb();
						int Px			= CorrectPrediction(pred + ApplySign(ctx.C, sign));	
						int ErrVal		= short(ApplySign(x - Px, sign));
						ctx.UpdateVariables(ErrVal);

						float f = float(k) / 16.0f;
						
						/*float h = f;
						float s = 1.0f;
						float v = 1.0f;*/
						float h = 0.0f;
						float s = 0.0f;
						float v = f;
						/*float h = float(ipixel)/float(width);
						float s = float(iline)/float(height);
						float v = 1.0f;*/
						
						float3 rgb = hsvToRGB(h, s, v);
						ubyte3 bytes = convert_ubyte3_sat_rte(rgb * 255.0f);
						statImage[0] = bytes.x;
						statImage[1] = bytes.y;
						statImage[2] = bytes.z;
						statImage += 3;
						++ipixel;
					}
				}
			}
		}
	}
}



// JLSCodecTest

JLSCodecTest::~JLSCodecTest()
{

}
	
bool JLSCodecTest::compress(UByteArray& compressedData, const uint16_t* image, int width, int height, int depth)
{
	height *= depth;
	
	Pointer<IODevice> dev = new ArrayRefIODevice(compressedData);
	JLSBitWriter w(dev, true);
	
	// start of image
	{
		w.write<uint8_t>(0xFF);
		w.write<uint8_t>(JPEG_SOI);
	}
	
	// start of frame
	{
		w.write<uint8_t>(0xFF);
		w.write<uint8_t>(JPEG_SOF);
		
		w.write<uint16_t>(2 + 9); // length of segment without header but including the length

		w.write<uint8_t>(16); // bpp
		w.write<uint16_t>(height);
		w.write<uint16_t>(width);

		w.write<uint8_t>(1); // number of components
		w.write<uint8_t>(1); // 1st component
		w.write<uint8_t>(0x11);
		w.write<uint8_t>(0);
	}

	// start of scan
	{
		w.write<uint8_t>(0xFF);
		w.write<uint8_t>(JPEG_SOS);
	
		w.write<uint16_t>(2 + 6); // length of segment without header but including the length
	
		w.write<uint8_t>(1); // number of components
		w.write<uint8_t>(1); // 1st component
		w.write<uint8_t>(0); // itable?

		w.write<uint8_t>(0); // allowed lossy error
		w.write<uint8_t>(0); // interleaved mode
		w.write<uint8_t>(0); // transform
	}	

	// quantization lookup table
	std::vector<int8_t> lut(65536 * 2 - 1);
	for (int diff = -65535; diff < 65536; diff++)
	{
		lut[65535 + diff] = QuantizeGradientOrg(diff);
	}

	JlsContext::CreateTableC();

	JlsContext _contexts[365];
	int A = max(2, (RANGE + 32)/64);
	for (size_t Q = 0; Q < boost::size(_contexts); ++Q)
	{
		_contexts[Q] = JlsContext(A);
	}

	CContextRunMode _contextRunmode[2];
	_contextRunmode[0] = CContextRunMode(max(2, (RANGE + 32)/64), 0, RESET);
	_contextRunmode[1] = CContextRunMode(max(2, (RANGE + 32)/64), 1, RESET);

	// compress data
	
	// DoScan()
	{
		//BitWriter w(dev);
		int pixelstride = width + 4;
	
		std::vector<uint16_t> vectmp;
		vectmp.resize(2 * pixelstride);
		
		int RUNindex = 0;

		const uint16_t* imageLine = image;
		for (int iline = 0; iline < height; ++iline)
		{
			uint16_t* ptypePrev = &vectmp[1];              // previous line ptr
			uint16_t* ptypeCur = &vectmp[1 + pixelstride]; // current line ptr
			if ((iline & 1) == 1)
			{
				std::swap(ptypePrev, ptypeCur);
			}

			// STRATEGY::OnLineBegin(_size.cx, ptypeCur, pixelstride)
			::memcpy(ptypeCur, imageLine, width * sizeof(uint16_t));
			imageLine += width;
			
			{
				//int RUNindex = rgRUNindex;

				// initialize edge pixels used for prediction
				ptypePrev[width]	= ptypePrev[width - 1];
				ptypeCur[-1]		= ptypePrev[0];
				
				// DoLine((PIXEL*) NULL)
				{
					int ipixel = 0;
					int Rb = ptypePrev[ipixel-1];
					int Rd = ptypePrev[ipixel];

					while (ipixel < width)
					{	
						int Ra = ptypeCur[ipixel -1];
						int Rc = Rb;
						Rb = Rd;
						Rd = ptypePrev[ipixel + 1];
/*
	http://en.wikipedia.org/wiki/Laplace_distribution#Parameter_estimation
	http://en.wikipedia.org/wiki/Geometric_distribution#Parameter_estimation
*/
						int Qs = ComputeContextID(QuantizeGratient(Rd - Rb), QuantizeGratient(Rb - Rc), QuantizeGratient(Rc - Ra));
						if (ipixel > 0 && ipixel < width - 1 && iline > 0 && iline < height - 1)
						{
							int c = image[ipixel + 0 + (iline + 1) * width];
							int f = image[ipixel - 1 + (iline + 1) * width];
							
							Qs = ComputeContextID(QuantizeGratient(Rd - Rb), QuantizeGratient(Rc - Ra), QuantizeGratient(f - c));
						}

						if (Qs != 0)
						{
							// ptypeCur[ipixel] = DoRegular(Qs, ptypeCur[ipixel], GetPredictedValue(Ra, Rb, Rc), (STRATEGY*)(NULL));
							{
								int x = ptypeCur[ipixel];
								int pred = GetPredictedValue(Ra, Rb, Rc);
								
								// non causal ideal prediction that uses all 8 neighbors
								if (ipixel > 0 && ipixel < width - 1 && iline > 0 && iline < height - 1)
								{
									int a = image[ipixel + 0 + (iline - 1) * width];
									int b = image[ipixel - 1 + (iline + 0) * width];
									int c = image[ipixel + 0 + (iline + 1) * width];
									int d = image[ipixel + 1 + (iline + 0) * width];
									
									int e = image[ipixel - 1 + (iline - 1) * width];
									int f = image[ipixel - 1 + (iline + 1) * width];
									int g = image[ipixel + 1 + (iline + 1) * width];
									int h = image[ipixel + 1 + (iline - 1) * width];
									
									pred = (2*a + 2*b + 2*c + 2*d - e - f - g - h) >> 2;
								}
								 
								int sign = BitWiseSign(Qs);
								JlsContext& ctx	= _contexts[ApplySign(Qs, sign)];
								int k			= ctx.GetGolomb();
								int Px			= CorrectPrediction(pred + ApplySign(ctx.C, sign));	

								int ErrVal		= short(ApplySign(x - Px, sign));

								EncodeMappedValue(w, k, GetMappedErrVal(ctx.GetErrorCorrection(k) ^ ErrVal), LIMIT);
								ctx.UpdateVariables(ErrVal);
								ptypeCur[ipixel] = ComputeReconstructedSample(Px, ApplySign(ErrVal, sign));
							}
							
							ipixel++;
						}
						else
						{
							// ipixel += DoRunMode(ipixel, (STRATEGY*)(NULL));
							{
								int ctypeRem = width - ipixel;
								uint16_t* ptypeCurX = ptypeCur + ipixel;
								uint16_t* ptypePrevX = ptypePrev + ipixel;

								uint16_t Ra = ptypeCurX[-1];

								int runLength = 0;

								while (ptypeCurX[runLength] == Ra) 
								{
									ptypeCurX[runLength] = Ra;
									runLength++;

									if (runLength == ctypeRem)
										break;
								}

								//EncodeRunPixels(runLength, runLength == ctypeRem);
								int savedRunLength = runLength;
								{
									bool bEndofline = runLength == ctypeRem;
									
									while (runLength >= (1 << J[RUNindex])) 
									{
										//STRATEGY::AppendOnesToBitStream(1);
										w.writeBit(true);
										
										runLength = runLength - (1 << J[RUNindex]);
										//IncrementRunIndex();
										RUNindex = min(RUNindex + 1, 31);
									}

									if (bEndofline) 
									{
										if (runLength != 0) 
										{
											//STRATEGY::AppendOnesToBitStream(1);	
											w.writeBit(true);
										}
									}
									else
									{
										//STRATEGY::AppendToBitStream(runLength, J[RUNindex] + 1);	// leading 0 + actual remaining length
										w.writeBinaryValue(runLength, J[RUNindex] + 1);
									}
								}
								runLength = savedRunLength;

								if (runLength == ctypeRem)
									ipixel += runLength;
								else
								{
									//ptypeCurX[runLength] = EncodeRIPixel(ptypeCurX[runLength], Ra, ptypePrevX[runLength]);
									{
										int x = ptypeCurX[runLength];
										int Rb = ptypePrevX[runLength];
										
										if (abs(Ra - Rb) <= 0)
										{
											//int ErrVal = traits.ComputeErrVal(x - Ra);
											int ErrVal = short(x - Ra);
											EncodeRIError(w, _contextRunmode[1], ErrVal, RUNindex);
											ptypeCurX[runLength] = ComputeReconstructedSample(Ra, ErrVal);
										}
										else
										{
											//int ErrVal = traits.ComputeErrVal((x - Rb) * Sign(Rb - Ra));
											int ErrVal = short((x - Rb) * Sign(Rb - Ra));
											EncodeRIError(w, _contextRunmode[0], ErrVal, RUNindex);
											ptypeCurX[runLength] = ComputeReconstructedSample(Rb, ErrVal * Sign(Rb - Ra));
										}
									}
									//DecrementRunIndex();
									RUNindex = max(RUNindex - 1, 0);
									ipixel += runLength + 1;
								}
							} // ipixel += DoRunMode()
							
							Rb = ptypePrev[ipixel-1];
							Rd = ptypePrev[ipixel];
						}	
					}
				} // DoLine()
		
				//rgRUNindex = RUNindex;
				//ptypePrev += pixelstride;
				//ptypeCur += pixelstride;
			}
			
			// STRATEGY::OnLineEnd(_size.cx, ptypeCur - (components * pixelstride), pixelstride);
			// nothing to do			
		}

		// STRATEGY::FlushStreamEnd()
		w.flushBits();
	}

	// end of image
	{
		w.write<uint8_t>(0xFF);
		w.write<uint8_t>(JPEG_EOI);
	}

	return true;
}

bool JLSCodecTest::decompress(const UByteArray& compressedData, uint16_t* image, int width, int height, int depth)
{
	return false;
}
			
std::string JLSCodecTest::getName()
{
	return "JPEG-LS";
}

std::string JLSCodecTest::getExtension()
{
	return ".jlsTest";
}
