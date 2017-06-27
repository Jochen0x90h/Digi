#include "EbmlWriter.h"


namespace digi {

void EbmlWriter::writeHeader(const EbmlHeader& header)
{
	this->writeId(0x1A45DFA3);
	this->writeVarInt(26 + EbmlWriter::calcSize(header.docType));
	this->write(0x4286, header.ebmlVersion, 1u);
	this->write(0x42F7, header.ebmlReadVersion, 1u);
	this->write(0x42F2, header.ebmlMaxIDLength, 1u);
	this->write(0x42F3, header.ebmlMaxSizeLength, 1u);
	this->write(0x4282, header.docType);
	this->write(0x4287, header.docTypeVersion, 1u);
	this->write(0x4285, header.docTypeReadVersion, 1u);
}

void EbmlWriter::writeVarInt(uint64_t value, int size)
{
	uint8_t buf[8];
	uint8_t* end = buf + 8;
	uint8_t* begin = end;
	
	uint64_t maxValue = 0x80;
	for (int i = 1; i < size; ++i)
	{
		--begin;
		*begin = uint8_t(value);
		value >>= 8;
		maxValue >>= 1;
	}
	--begin;
	*begin = uint8_t(maxValue) | uint8_t(value);

	this->writeData(begin, end - begin);
}

void EbmlWriter::write(uint id, StringRef value)
{
	size_t size = value.size();

	// write id and size
	uint8_t buf[16];
	uint8_t* end = buf + 16;
	uint8_t* begin = EbmlWriter::writeVarInt(end, size);
	begin = EbmlWriter::writeId(begin, id);
	this->writeData(begin, end - begin);		

	// write data
	this->writeData(value.data(), size);
}

void EbmlWriter::write(uint id, StringRef value, size_t size)
{
	// write id and size
	uint8_t buf[16];
	uint8_t* end = buf + 16;
	uint8_t* begin = EbmlWriter::writeVarInt(end, size);
	begin = EbmlWriter::writeId(begin, id);
	this->writeData(begin, end - begin);		

	// write data
	size_t size2 = value.size();
	this->writeData(value.data(), std::min(size, size2));
	
	// pad with zeros if necessary
	if (size2 < size)
		this->skip(size - size2);
}

void EbmlWriter::write(uint id, const std::vector<uint8_t>& value)
{
	size_t size = value.size();

	// write id and size
	uint8_t buf[16];
	uint8_t* end = buf + 16;
	uint8_t* begin = EbmlWriter::writeVarInt(end, size);
	begin = EbmlWriter::writeId(begin, id);
	this->writeData(begin, end - begin);		

	// write data
	this->writeData(value.data(), size);
}

uint8_t* EbmlWriter::writeId(uint8_t* begin, uint id)
{
	while (id > 0)
	{
		--begin;
		*begin = uint8_t(id);
		id >>= 8;
	}
	return begin;
}

uint8_t* EbmlWriter::writeVarInt(uint8_t* begin, uint64_t value)
{
	// values with all data bits set are reserved (e.g. 0x7f, 0x3fff, 0x1fffff), therefore store one more byte
	uint64_t value1 = value + 1;
	uint64_t maxValue = 0x80;
	while (value1 >= maxValue)
	{
		--begin;
		*begin = uint8_t(value);
		value >>= 8;
		value1 >>= 8;
		maxValue >>= 1;
	}
	--begin;
	*begin = uint8_t(maxValue) | uint8_t(value);
	return begin;
}

} // namespace digi
