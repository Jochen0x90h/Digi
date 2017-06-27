#include <digi/Utility/ListUtility.h>
#include <digi/Utility/VectorUtility.h>

#include "MatFileWriter.h"


namespace digi {

// MatFileWriter

/*
	note: the data alignment in the file is 8 bytes.
*/

MatFileWriter::~MatFileWriter()
{
}

void MatFileWriter::writeHeader(const std::string& text)
{
	uint8_t buf[116];

	// descriptive text
	strncpy((char*)buf, text.c_str(), 115);
	buf[115] = 0;
	this->writer.writeData(buf, sizeof(buf));

	// subsys data offset
	this->writer.write<uint32_t>(0);
	this->writer.write<uint32_t>(0);

	// version
	this->writer.write<uint16_t>(0x0100);

	// endian indicator
	this->writer.write<uint16_t>(0x4D49);
}

void MatFileWriter::writeString(const std::string& name, const std::string& value)
{
	int length = int(value.length());
	int dataLength = length * 2;
	int dimensions[2];
	dimensions[0] = 1;
	dimensions[1] = length;
	this->writeVariableHeader(name, dataLength, mxCHAR_CLASS, dimensions, 2);

	// string
	this->writer.write<uint32_t>(miUINT16);
	this->writer.write<uint32_t>(dataLength); // length
	this->writer.write<uint16_t>((uint8_t*)value.c_str(), length);
	this->writer.skip(-dataLength & 7);
}

void MatFileWriter::beginStruct(const std::string& name, int numFields)
{
	int64_t structStart = this->writer.getPosition();

	int dimensions[2];
	dimensions[0] = 1;
	dimensions[1] = 1;
	this->writeVariableHeader(name, 0, mxSTRUCT_CLASS, dimensions, 2);

	// add struct info on stack and add starting position
	StructInfo& info = add(this->structStack, StructInfo(structStart));

	// field name length
	this->writer.write<uint32_t>(4 << 16 | miINT32);
	this->writer.write<uint32_t>(32);
	
	// header of field names (miINT8 and length)
	this->writer.write<uint32_t>(miINT8);
	this->writer.write<uint32_t>(numFields * 32);
	
	// get position of field names
	info.fieldNamesPosition = this->writer.getPosition();
	
	// skip space for fields names
	this->writer.skip(numFields * 32);
}

void MatFileWriter::endStruct()
{
	// get current position
	int64_t currentPos = this->writer.getPosition();

	// get position of struct length in file
	size_t i = this->structStack.size() - 1;
	int64_t lengthPos = this->structStack[i].startPosition + 4;

	// remove struct from stack
	this->structStack.resize(i);
	
	// write length
	this->writer.setPosition(lengthPos);
	this->writer.write<uint32_t>(currentPos - lengthPos - 4);
	
	// seek back to current position
	this->writer.setPosition(currentPos);
}


void MatFileWriter::writeVariableHeader(const std::string& name, int dataLength, ArrayType arrayType,
	int* dimensions, int numDimensions)
{
	int dimLength = numDimensions * 4;
	int nameLength = name.length();
	
	// check if we are inside a structure
	if (!this->structStack.empty())
	{
		// get current position
		int64_t currentPos = this->writer.getPosition();

		// get and update struct info
		StructInfo& info = this->structStack[this->structStack.size() - 1];
		int64_t fieldNamePos = info.fieldNamesPosition;
		info.fieldNamesPosition += 32;
		
		// write field name
		this->writer.setPosition(fieldNamePos);
		uint8_t buf[32];
		strncpy((char*)buf, name.c_str(), 31);
		buf[31] = 0;
		this->writer.writeData(buf, 32);
		
		// seek back to current position
		this->writer.setPosition(currentPos);

		// do not save name inside matrix
		nameLength = 0;
	}
	
	// write object type and overall length (header + data but without type and overall length)
	this->writer.write<uint32_t>(miMATRIX);
	this->writer.write<uint32_t>(
		+ 8 + 8 // array flags
		+ 8 + dimLength + (-dimLength & 7) // dimensions array
		+ 8 + (nameLength > 4 ? nameLength + (-nameLength & 7) : 0) // array name
		+ 8 + dataLength + (-dataLength & 7) // real part
	);
	
	// array flags
	this->writer.write<uint32_t>(miUINT32);
	this->writer.write<uint32_t>(8); // length
	this->writer.write<uint32_t>(0 << 8 | arrayType); // flags and array type (e.g. mxDOUBLE_CLASS)
	this->writer.write<uint32_t>(0);

	// dimensions array
	this->writer.write<uint32_t>(miINT32);
	this->writer.write<uint32_t>(dimLength); // length
	this->writer.write<uint32_t>(dimensions, numDimensions);
	this->writer.skip(-dimLength & 7); // align 8

	// matrix name
	if (nameLength <= 4)
	{
		// compressed name
		this->writer.write<uint32_t>((nameLength << 16) | miINT8);
		this->writer.writeData(name.c_str(), nameLength);
		this->writer.skip(-nameLength & 3); // align 4
	}
	else
	{
		this->writer.write<uint32_t>(miINT8);
		this->writer.write<uint32_t>(nameLength); // length
		this->writer.writeData(name.c_str(), nameLength);
		this->writer.skip(-nameLength & 7); // align 8
	}
}

} // namespace digi
