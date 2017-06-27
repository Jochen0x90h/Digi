#include <digi/Utility/IntUtility.h>
#include <digi/Utility/ListUtility.h>
#include <digi/Utility/VectorUtility.h>

#include "MatFileReader.h"


namespace digi {

// MatFileReader

/*
	note: the data alignment in the file is 8 bytes.
*/

MatFileReader::~MatFileReader()
{
}

void MatFileReader::readHeader(std::string& text)
{
	uint8_t buf[116];

	// descriptive text
	this->reader.readData(buf, sizeof(buf));
	buf[115] = 0;

	// subsys data offset
	this->reader.read<uint32_t>();
	this->reader.read<uint32_t>();

	// version
	uint16_t version = this->reader.read<uint16_t>();

	// endian indicator
	uint16_t endian = this->reader.read<uint16_t>();
	
	if (endian == 0x494D)
	{
		version = byteSwap(version);
		this->reader.setBigEndian(true);
		endian = 0x4D49;
	}
	if (endian != 0x4D49)
		throw DataException(this->reader.getDevice(), DataException::FORMAT_ERROR);
	if (version != 0x0100)
		throw DataException(this->reader.getDevice(), DataException::UNKNOWN_VERSION);

	text = (char*)buf;
}

MatFileReader::ArrayType MatFileReader::readVariableHeader(std::string& name, std::vector<int>& dimensions)
{
	ArrayType arrayType;
	
	// read type and length
	int type = this->reader.read<uint32_t>();
	this->reader.read<uint32_t>();
	if (type != miMATRIX)
		throw DataException(this->reader.getDevice(), DataException::DATA_CORRUPT);
	
	// array flags
	int flagsType = this->reader.read<uint32_t>();
	int flagsLength = this->reader.read<uint32_t>();
	if (flagsType != miUINT32 || flagsLength != 8)
		throw DataException(this->reader.getDevice(), DataException::DATA_CORRUPT);
	int flagsClass = this->reader.read<uint32_t>();
	this->reader.read<uint32_t>();
	arrayType = ArrayType(flagsClass & 0xff); // e.g. mxDOUBLE_CLASS
	
	// dimensions array
	int dimType = this->reader.read<uint32_t>();
	int dimLength = this->reader.read<uint32_t>();
	if (dimType != miINT32 || dimLength > 4096 || dimLength < 0 || (dimLength & 3) != 0)
		throw DataException(this->reader.getDevice(), DataException::DATA_CORRUPT);
	for (int i = 0; i < dimLength; i += 4)
	{
		int dim = this->reader.read<uint32_t>();
		dimensions += dim;
	}
	this->reader.skip(-dimLength & 7); // padding

	// name
	int nameType = this->reader.read<uint32_t>();
	int nameLength;
	int namePadding;
	if ((nameType >> 16) != 0)
	{
		// compressed name
		nameLength = nameType >> 16;
		nameType &= 0xffff;
		namePadding = -nameLength & 3;
	}
	else
	{
		nameLength = this->reader.read<uint32_t>();
		namePadding = -nameLength & 7;
	}
	if (nameType != miINT8 || nameLength > 4096 || nameLength < 0)
		throw DataException(this->reader.getDevice(), DataException::DATA_CORRUPT);
	name.resize(nameLength);
	this->reader.read<uint8_t>(name.begin(), nameLength);
	this->reader.skip(namePadding); // padding

	if (!this->structStack.empty())
	{
		StructInfo& info = this->structStack.back();

		if (info.names.empty())
			throw DataException(this->reader.getDevice(), DataException::DATA_CORRUPT);
			
		name = info.names.front();
		info.names.pop_front();
	}

	return arrayType;
}

void MatFileReader::readString(std::string& value)
{
	// string data
	int dataType = this->reader.read<uint32_t>();
	int dataLength = this->reader.read<uint32_t>();
	if (dataType != miUINT16)
		throw DataException(this->reader.getDevice(), DataException::DATA_CORRUPT);
	
	// the data is stored as int16, cast to char
	value.resize(dataLength >> 1);
	this->reader.read<int16_t>((char*)value.c_str(), dataLength >> 1);
	this->reader.skip(-dataLength & 7);
}

int MatFileReader::beginStruct()
{
	StructInfo& info = add(this->structStack);

	// field name length
	int typeAndLength = this->reader.read<uint32_t>();
	int nameLength = this->reader.read<uint32_t>();
	if (typeAndLength != (4 << 16 | miINT32) || nameLength > 4096)
		throw DataException(this->reader.getDevice(), DataException::DATA_CORRUPT);
		
	// header of field names (miINT8 and length)
	int namesType = this->reader.read<uint32_t>();
	int namesLength = this->reader.read<uint32_t>();
	if (namesType != miINT8 && namesLength % nameLength != 0)
		throw DataException(this->reader.getDevice(), DataException::DATA_CORRUPT);
		
	// calc number of fields
	int numFields = namesLength / nameLength;
	
	// read field names
	std::vector<char> buffer(nameLength);
	for (int i = 0; i < numFields; ++i)
	{
		this->reader.readData(buffer.data(), nameLength);
		buffer[nameLength - 1] = 0;
		info.names += buffer.data();
	}
	return numFields;
}

void MatFileReader::endStruct()
{
	// remove struct from stack
	size_t i = this->structStack.size() - 1;
	this->structStack.resize(i);
}

} // namespace digi
