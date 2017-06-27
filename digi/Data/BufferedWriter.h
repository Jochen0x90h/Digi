#ifndef digi_Data_BufferedWriter_h
#define digi_Data_BufferedWriter_h

#include <digi/System/IODevice.h>
#include <digi/System/FileSystem.h>


namespace digi {

/// @addtogroup Data
/// @{

class BufferedWriter
{
public:

	BufferedWriter(Pointer<IODevice> dev, int bufferSize = 1024);

	BufferedWriter(const fs::path& path, int bufferSize = 1024);

	/// destructor. note: you must call flush() or close() before the destructor is called, otherwise data loss occurs.
	/// the destructor does not flush since this could throw an exception.
	~BufferedWriter();


	/// get underlying device
	Pointer<IODevice> getDevice() {return this->dev;}

	/// returns true if the device is open
	bool isOpen() {return this->dev->isOpen();}

	/// flush buffer of BufferedWriter (not the underlying device)
	void flush() {this->writeBuffer();}

	/// write the buffer and close underlying device.
	void close() {this->writeBuffer(); this->dev->close();}


	/// write a byte
	void writeByte(uint8_t value)
	{
		// check if at least one byte is free
		if (int(this->bufferEnd - this->end) < 1)
			this->writeBuffer();

		*this->end = value;
		++this->end;
	}

	/// write numBytes of data
	void writeData(const void* data, size_t numBytes);

	/// skip numBytes of data (write zeros)
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

	template <typename DestValue, typename Value, typename Convert>
	void writeInternal(Value value, Convert convert)
	{
		// check if at least one element is free
		if (int(this->bufferEnd - this->end) < Convert::SIZE)
			this->writeBuffer();

		convert(DestValue(value), this->end);
		this->end += Convert::SIZE;
	}

	template <typename DestValue, typename Iterator, typename Convert>
	void writeInternal(Iterator it, Iterator end, Convert convert)
	{
		while (it != end)
		{
			// check if at least one element is free
			if (int(this->bufferEnd - this->end) < Convert::SIZE)
				this->writeBuffer();

			uint8_t* bufferIt = this->end;
			uint8_t* bufferEnd = this->bufferEnd;
			while (it != end && bufferEnd - bufferIt >= Convert::SIZE)
			{
				convert(DestValue(*it), bufferIt);
				++it;
				bufferIt += Convert::SIZE;
			}
			this->end = bufferIt;
		}
	}

	void writeBuffer();


	// output device
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
