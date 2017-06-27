#include <digi/System/File.h>

#include "DataException.h"
#include "BufferedReader.h"


namespace digi {

BufferedReader::BufferedReader(Pointer<IODevice> dev, int bufferSize)
	: dev(dev), bufferBegin(new uint8_t[bufferSize]), bufferEnd(bufferBegin + bufferSize),
	begin(bufferBegin), end(bufferBegin)
{
}	

BufferedReader::BufferedReader(const fs::path& path, int bufferSize)
	: dev(File::open(path, File::READ)), bufferBegin(new uint8_t[bufferSize]), bufferEnd(bufferBegin + bufferSize),
	begin(bufferBegin), end(bufferBegin)
{
}

BufferedReader::~BufferedReader()
{
	delete [] this->bufferBegin;
}

void BufferedReader::readData(void* data, size_t numBytes)
{
	uint8_t* d = (uint8_t*)data;
	
	// copy data from buffer
	size_t toCopy = std::min(size_t(this->end - this->begin), numBytes);
	std::copy(this->begin, this->begin + toCopy, d);
	d += toCopy;
	this->begin += toCopy;
	numBytes -= toCopy;
	
	// check if done
	if (numBytes == 0)
		return;
	
	size_t bufferSize = this->bufferEnd - this->bufferBegin;
	if (numBytes > bufferSize)
	{
		// read directly if still more than one buffer size to read
		numBytes -= this->dev->read(d, numBytes);
	}
	else
	{
		// read buffer from device
		size_t numRead = this->dev->read(this->bufferBegin, bufferSize);
			
		// copy data from buffer
		size_t toCopy2 = std::min(numRead, numBytes);
		std::copy(this->bufferBegin, this->bufferBegin + toCopy2, d);
		this->begin = this->bufferBegin + toCopy2;
		this->end = this->bufferBegin + numRead;
		numBytes -= toCopy2;
	}
	
	// throw exception if not enough bytes could be read
	if (numBytes > 0)
		throw DataException(this->dev, DataException::UNEXPECTED_END_OF_DATA);
}

void BufferedReader::skip(size_t numBytes)
{
	while (true)
	{
		// skip data in buffer
		size_t toSkip = int(std::min(size_t(this->end - this->begin), numBytes));
		this->begin += toSkip;
		numBytes -= toSkip;
		
		// check if done
		if (numBytes == 0)
			return;

		// read buffer from device
		size_t numRead = this->dev->read(this->bufferBegin, this->bufferEnd - this->bufferBegin);
		if (numRead == 0)
			throw DataException(this->dev, DataException::UNEXPECTED_END_OF_DATA);
		this->begin = this->bufferBegin;
		this->end = this->bufferBegin + numRead;
	}
}

int64_t BufferedReader::seek(int64_t position, IODevice::PositionMode mode)
{
	// correct position if seek is relative to current position
	if (mode == IODevice::CURRENT)
		position -= this->end - this->begin;
		
	// clear buffer
	this->begin = this->end = this->bufferBegin;

	return this->dev->seek(position, mode);
}

int64_t BufferedReader::getPosition()
{
	return this->dev->getPosition() - int64_t(this->end - this->begin);
}

int64_t BufferedReader::align(size_t size)
{
	int64_t position = this->getPosition();
	size_t offset = size_t(-position) & (size - 1);
	this->skip(offset);
	return position + offset;
}

void BufferedReader::readBuffer(size_t size)
{
	// move tail of buffer to front
	size_t toCopy = this->end - this->begin;
	uint8_t* begin = this->bufferBegin;
	for (size_t i = 0; i < toCopy; ++i)
	{
		*(begin + i) = *(this->begin + i);
	}
	
	size_t numRead = this->dev->read(this->bufferBegin + toCopy, this->bufferEnd - this->bufferBegin - toCopy);
	this->begin = begin;
	this->end = begin + toCopy + numRead;
	
	// check if enough data was read
	if (toCopy + numRead < size)
		throw DataException(this->dev, DataException::UNEXPECTED_END_OF_DATA);
}

size_t BufferedReader::readNextBuffer(bool noThrow)
{
	size_t numRead = this->dev->read(this->bufferBegin, this->bufferEnd - this->bufferBegin);	
	this->begin = this->bufferBegin;
	this->end = this->bufferBegin + numRead;

	if (!noThrow && numRead == 0)
		throw DataException(this->dev, DataException::UNEXPECTED_END_OF_DATA);

	return numRead;
}

} // namespace digi
