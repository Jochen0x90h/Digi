#include <digi/System/File.h>

#include "DataException.h"
#include "BufferedWriter.h"


namespace digi {

BufferedWriter::BufferedWriter(Pointer<IODevice> dev, int bufferSize)
	: dev(dev), bufferBegin(new uint8_t[bufferSize]), bufferEnd(bufferBegin + bufferSize),
	begin(bufferBegin), end(bufferBegin)
{
}

BufferedWriter::BufferedWriter(const fs::path& path, int bufferSize)
	: dev(File::create(path)), bufferBegin(new uint8_t[bufferSize]),
	bufferEnd(bufferBegin + bufferSize), begin(bufferBegin), end(bufferBegin)
{
}

BufferedWriter::~BufferedWriter()
{
	delete [] this->bufferBegin;
}

void BufferedWriter::writeData(const void* data, size_t numBytes)
{
	const uint8_t* d = (const uint8_t*)data;
	
	// copy data into buffer
	size_t toCopy = std::min(size_t(this->bufferEnd - this->end), numBytes);
	std::copy(d, d + toCopy, this->end);
	d += toCopy;
	this->end += toCopy;
	numBytes -= toCopy;
	
	// check if done
	if (numBytes == 0)
		return;
		
	// write buffer into device
	this->writeBuffer();
	
	size_t bufferSize = this->bufferEnd - this->bufferBegin;
	if (numBytes > bufferSize)
	{
		// write directly if still more than one buffer size to write
		size_t numWritten = this->dev->write(d, numBytes);
		if (numWritten < numBytes)
			throw DataException(this->dev, DataException::UNEXPECTED_END_OF_DATA);		
	}
	else
	{
		// copy tail into buffer
		std::copy(d, d + numBytes, this->bufferBegin);
		this->end += numBytes;
	}
}

void BufferedWriter::skip(size_t numBytes)
{
	while (true)
	{
		// skip data in buffer
		size_t toSkip = std::min(size_t(this->bufferEnd - this->end), numBytes);
		std::fill(this->end, this->end + toSkip, 0);
		this->end += toSkip;
		numBytes -= toSkip;
		
		// check if done
		if (numBytes == 0)
			return;

		// write buffer into device
		this->writeBuffer();
	}
}

int64_t BufferedWriter::seek(int64_t position, IODevice::PositionMode mode)
{
	// write buffer
	this->writeBuffer();
	
	return this->dev->seek(position, mode);
}

int64_t BufferedWriter::getPosition()
{
	return this->dev->getPosition() + int64_t(this->end - this->begin);
}

int64_t BufferedWriter::align(size_t size)
{
	int64_t position = this->getPosition();
	size_t offset = size_t(-position) & (size - 1);
	this->skip(offset);
	return position + offset;
}

void BufferedWriter::writeBuffer()
{
	size_t toWrite = this->end - this->begin;
	size_t numWritten = this->dev->write(&*this->begin, toWrite);
	this->begin += numWritten;
	if (toWrite < numWritten)
		throw DataException(this->dev, DataException::UNEXPECTED_END_OF_DATA);
	
	// reset buffer
	this->begin = this->end = this->bufferBegin;
}

} // namespace digi
