#ifndef digi_Data_MatlabWriter_h
#define digi_Data_MatlabWriter_h

#include <boost/range.hpp>

#include "NativeWriter.h"


namespace digi {

/// @addtogroup Data
/// @{

/**
	Class for writing Matlab .mat files in version 6 format.
	
	Use "load <fileName>.mat" to load the file in Matlab.
	Use "save <fileName>.mat -V6" to save in old version 6 format.
*/
class MatFileWriter
{
public:

	MatFileWriter(Pointer<IODevice> dev)
		: writer(dev) {}
	MatFileWriter(const fs::path& path)
		: writer(path) {}

	~MatFileWriter();

	void close() {this->writer.close();}

	// write header, e.g. "MATLAB 5.0 MAT-file, Created by ..."
	void writeHeader(const std::string& text);
	
	// write string (array of chars)
	void writeString(const std::string& name, const std::string& value);

	template <typename Iterator>
	void writeMatrix(const std::string& name, Iterator it, size_t numRows, size_t numCols)
	{
		//! todo: 64 bit support
		size_t dataLength = numRows * numCols * sizeof(double);
		int dimensions[2];
		dimensions[0] = int(numRows);
		dimensions[1] = int(numCols);
		this->writeVariableHeader(name, int(dataLength), mxDOUBLE_CLASS, dimensions, 2);

		// real part
		this->writer.write<uint32_t>(miDOUBLE);
		this->writer.write<uint32_t>(dataLength); // length
		this->writer.write<double>(it, numRows * numCols);
		//this->writer.skip(-dataLength & 7); // double is always 8-aligned
	}
	
	template <typename Iterator>
	void writeVector(const std::string& name, Iterator it, size_t length)
	{
		this->writeMatrix(name, it, length, 1);
	}

	template <typename Vector>
	void writeVector(const std::string& name, const Vector& vector)
	{
		this->writeMatrix(name, boost::begin(vector), boost::size(vector), 1);
	}


	/*
		begin writing a struct with numFields number of fields.
		use writeArray etc. to write the data for all fields.
		at last call EndWriteStruct.
	*/
	void beginStruct(const std::string& name, int numFields);
	void endStruct();
	
protected:

	enum Type
	{
		miINT8       = 1,  // 8 bit, signed
		miUINT8      = 2,  // 8 bit, unsigned
		miINT16      = 3,  // 16-bit, signed
		miUINT16     = 4,  // 16-bit, unsigned
		miINT32      = 5,  // 32-bit, signed
		miUINT32     = 6,  // 32-bit, unsigned
		miSINGLE     = 7,  // IEEE 754 single format
		miDOUBLE     = 9,  // IEEE 754 double format
		miINT64      = 12, // 64-bit, signed
		miUINT64     = 13, // 64-bit, unsigned
		miMATRIX     = 14, // MATLAB array
		miCOMPRESSED = 15, // Compressed Data
		miUTF8       = 16, // Unicode UTF-8 Encoded Character Data
		miUTF16      = 17, // Unicode UTF-16 Encoded Character Data
		miUTF32      = 18, // Unicode UTF-32 Encoded Character Data
	};

	enum ArrayType
	{
		mxCELL_CLASS   = 1,  // Cell array
		mxSTRUCT_CLASS = 2,  // Structure
		mxOBJECT_CLASS = 3,  // Object
		mxCHAR_CLASS   = 4,  // Character array
		mxSPARSE_CLASS = 5,  // Sparse array
		mxDOUBLE_CLASS = 6,  // Double precision array
		mxSINGLE_CLASS = 7,  // Single precision array
		mxINT8_CLASS   = 8,  // 8-bit, signed integer
		mxUINT8_CLASS  = 9,  // 8-bit, unsigned integer
		mxINT16_CLASS  = 10, // 16-bit, signed integer
		mxUINT16_CLASS = 11, // 16-bit, unsigned integer
		mxINT32_CLASS  = 12, // 32-bit, signed integer
		mxUINT32_CLASS = 13, // 32-bit unsigned, integer
	};

	void writeVariableHeader(const std::string& name, int dataLength, ArrayType arrayType,
		int* dimensions, int numDimensions);

	struct StructInfo
	{
		int64_t startPosition;
		int64_t fieldNamesPosition;
		
		StructInfo() : startPosition(0), fieldNamesPosition(0) {}
		StructInfo(int64_t startPosition) : startPosition(startPosition), fieldNamesPosition(0) {}
	};

	NativeWriter writer;
	std::vector<StructInfo> structStack;
};

/// @}

} // namespace digi

#endif
