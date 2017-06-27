#ifndef digi_Data_BufferedReader_h
#define digi_Data_BufferedReader_h

#include <digi/System/IODevice.h>
#include <digi/System/FileSystem.h>


namespace digi {

/// @addtogroup Data
/// @{

/// provides a buffer for accelerating read of many small data blocks
class BufferedReader
{
public:

	BufferedReader(Pointer<IODevice> dev, int bufferSize = 1024);

	BufferedReader(const fs::path& path, int bufferSize = 1024);

	~BufferedReader();


	/// get underlying device
	Pointer<IODevice> getDevice() {return this->dev;}

	/// returns true if the device is open
	bool isOpen() {return this->dev->isOpen();}

	/// returns true if the device is at end of input (eof = end of file)
	//bool isEndOfInput() {return this->dev->isEndOfInput();}

	/// close underlying device
	void close() {this->dev->close();}


	/// read geven number of bytes
	void readData(void* data, size_t numBytes);

	/// skip given number of bytes by reading the data, no seek is used
	void skip(size_t numBytes);

	/// align current position by given size and return new position. Size must be power of 2
	int64_t align(size_t size);


	/// seek
	int64_t seek(int64_t position, IODevice::PositionMode mode);

	/// set position of file pointer
	void setPosition(int64_t position) {this->seek(position, IODevice::BEGIN);}
	
	/// get the current file position
	int64_t getPosition();

protected:

	template <typename DestValue, typename Convert>
	DestValue readInternal(Convert convert)
	{
		// check if at least one element is in the buffer
		if (int(this->end - this->begin) < Convert::SIZE)
			this->readBuffer(Convert::SIZE);

		uint8_t* bufferIt = this->begin;
		this->begin += Convert::SIZE;
		return DestValue(convert(bufferIt));
	}

	template <typename DestValue, typename Iterator, typename Convert>
	void readInternal(Iterator it, Iterator end, Convert convert)
	{
		while (it != end)
		{
			// check if at least one element is in the buffer
			if (int(this->end - this->begin) < Convert::SIZE)
				this->readBuffer(Convert::SIZE);

			uint8_t* bufferIt = this->begin;
			uint8_t* bufferEnd = this->end;
			while (it != end && bufferEnd - bufferIt >= Convert::SIZE)
			{
				*it = DestValue(convert(bufferIt));
				++it;
				bufferIt += Convert::SIZE;
			}
			this->begin = bufferIt;
		}
	}

	void readBuffer(size_t size);

	size_t readNextBuffer(bool noThrow);

public:

	/// parse a token using given parser. throws DataException::UNEXPECTED_END_OF_DATA if noThrow is false
	template <typename Parser>
	bool parse(Parser& parser, bool noThrow = false)
	{
		do
		{
			for (uint8_t* it = this->begin; it != this->end; ++it)
			{
				// pass byte to parser. -1 = continue, 0 = finished and parse *it again, 1 = finished and skip *it
				int result = parser.parse(*it);
				if (result >= 0)
				{
					it += result;
					parser.add((char*)this->begin, (char*)it);
					this->begin = it;
					
					// token was parsed
					return true;
				}
			}
			parser.add((char*)this->begin, (char*)this->end);
		} while (this->readNextBuffer(noThrow) > 0);
		
		// token was not parsed
		return false;
	}

protected:

	// input device
	Pointer<IODevice> dev;
	
	// preallocated buffer
	uint8_t* bufferBegin;
	uint8_t* bufferEnd;
	
	// range with valid data
	uint8_t* begin;
	uint8_t* end;
};

/// @}

} // namespace digi

#endif
