#include <digi/Utility/StringUtility.h>

#include "ReadFunctions.h"
#include "EbmlReader.h"


namespace digi {

void EbmlReader::readHeader(EbmlHeader& header)
{
	if (this->readId() != 0x1A45DFA3)
		throw digi::DataException(this->getDevice(), digi::DataException::FORMAT_ERROR);
	size_t maxSize = 1000u;

	size_t size = this->readSize(maxSize);
	size_t start = this->getByteCount();
	while (this->getByteCount() - start < size)
	{
		uint32_t id = this->readId();
		switch (id)
		{
		case 0x4286:
			this->read(header.ebmlVersion);
			break;
		case 0x42F7:
			this->read(header.ebmlReadVersion);
			break;
		case 0x42F2:
			this->read(header.ebmlMaxIDLength);
			break;
		case 0x42F3:
			this->read(header.ebmlMaxSizeLength);
			break;
		case 0x4282:
			this->read(header.docType, 100u);
			break;
		case 0x4287:
			this->read(header.docTypeVersion);
			break;
		case 0x4285:
			this->read(header.docTypeReadVersion);
			break;
		default:
			this->skip(this->readSize(maxSize));
		};
	};
}

uint EbmlReader::readId()
{
	uint8_t head;
	this->readData(&head, 1);
	if (head == 0)
		throw DataException(this->dev, DataException::DATA_CORRUPT);

	int numTails = 0;
	uint8_t mask = 0x80;
	while ((head & mask) == 0)
	{
		mask >>= 1;
		++numTails;
	}
	if (numTails > 3)
		throw DataException(this->dev, DataException::DATA_CORRUPT);

	uint32_t id = head;
	if (numTails > 0)
	{
		uint8_t buffer[4];
		this->readData(buffer, numTails);
		
		uint8_t* it = buffer;
		uint8_t* end = buffer + numTails;
		do
		{
			id <<= 8;
			id |= *it;
			++it;
		} while (it != end);
	}
	return id;
}

uint64_t EbmlReader::readVarInt()
{
	uint8_t head;
	this->readData(&head, 1);
	if (head == 0)
		throw DataException(this->dev, DataException::DATA_CORRUPT);
	int numTails = 0;
	uint8_t mask = 0x80;
	while ((head & mask) == 0)
	{
		mask >>= 1;
		++numTails;
	}
	uint64_t value = head & ~mask;
	
	if (numTails > 0)
	{
		uint8_t buf[8];
		this->readData(buf, numTails);
		
		uint8_t* it = buf;
		uint8_t* end = buf + numTails;
		do
		{
			value <<= 8;
			value |= *it;
			++it;
		} while (it != end);
	}
	return value;
}

size_t EbmlReader::readSize(size_t maxSize)
{
	uint64_t size = this->readVarInt();
	if (size > uint64_t(maxSize))
		throw DataException(this->dev, DataException::DATA_CORRUPT);
	return size_t(size);
}

void EbmlReader::read(bool& value)
{
	uint8_t length;
	this->readData(&length, 1);
	length ^= 0x80;
		
	// length must be in the range 0-8
	if (length > 8)
		throw DataException(this->dev, DataException::DATA_CORRUPT);
		
	value = false;
	if (length > 0)
	{
		uint8_t buffer[8];
		this->readData(buffer, length);
			
		uint8_t* it = buffer;
		uint8_t* end = buffer + length;
		do
		{
			value |= *it != 0;
			++it;
		} while (it != end);
	}
}

void EbmlReader::read(double& value)
{
	uint8_t length;
	this->readData(&length, 1);
	length ^= 0x80;
	
	if (length == 0)
	{
		value = 0.0;
	}
	else
	{
		// length must be 4, 8 or 10
		if (length != 4 && length != 8 && length != 10)
			throw DataException(this->dev, DataException::DATA_CORRUPT);

		uint8_t buffer[10];
		this->readData(buffer, length);
		if (length == 4)
		{
			value = ReadBE<float>()(buffer);
		}
		else if (length == 8)
		{
			value = ReadBE<double>()(buffer);
		}
		else if (length == 10)
		{
			value = ReadBE<long double>()(buffer);
		}
	}
}

void EbmlReader::read(std::string& value, size_t maxSize)
{
	size_t size = this->readSize(maxSize);

	value.resize(size);
	this->readData((void*)value.data(), size);

	// remove zero padding
	if (size > 0)
		value.resize(length(value.c_str()));
}

void EbmlReader::read(std::vector<uint8_t>& value, size_t maxSize)
{
	size_t size = this->readSize(maxSize);

	value.resize(size);
	this->readData(value.data(), size);
}

} // namespace digi
