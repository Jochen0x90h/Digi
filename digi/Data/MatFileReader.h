#ifndef digi_Data_MatlabReader_h
#define digi_Data_MatlabReader_h

#include "DataException.h"
#include "DataReader.h"


namespace digi {

/// @addtogroup Data
/// @{

/**
	Class for reading Matlab .mat files in version 6 format.
	
	Use "load <fileName>.mat" to load the file in Matlab.
	Use "save <fileName>.mat -V6" to save in old version 6 format.
*/
class MatFileReader
{
public:

	enum ArrayType
	{
		mxINVALID      = 0,  // invalid, returned when unexpected data encountered
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

	MatFileReader(Pointer<IODevice> dev)
		: reader(dev) {}
	MatFileReader(const fs::path& path)
		: reader(path) {}

	~MatFileReader();
	
	void close() {this->reader.close();}

	// read header. returns false on error.
	void readHeader(std::string& text);

	// read header of next variable in file. returns mxINVALID on error.
	ArrayType readVariableHeader(std::string& name, std::vector<int>& dimensions);
	
	/// read a string when readVariableHeader has returned mxCHAR_CLASS
	void readString(std::string& value);

	/// read a matrix when readVariableHeader has returned mxDOUBLE_CLASS or mxSINGLE_CLASS
	template <typename Iterator>
	void readMatrix(Iterator it)
	{
		// real part of matrix
		int dataType = this->reader.read<uint32_t>();
		int dataLength = this->reader.read<uint32_t>();
		if (dataType == miDOUBLE)
		{
			// the data is stored as double, cast to float
			this->reader.read<double>(it, dataLength / sizeof(double));
			//this->reader.skip(-dataLength & 7); // double is always 8-aligned
		}
		else if (dataType == miSINGLE)
		{
			// the data is stored as double, cast to float
			this->reader.read<float>(it, dataLength / sizeof(float));
			this->reader.skip(-dataLength & 7);
		}
		else
			throw DataException(this->reader.getDevice(), DataException::DATA_CORRUPT);
	}

	/// begin reading a struct when readVariableHeader has returned mxSTRUCT_CLASS. returns number of fields to read
	int beginStruct();
	
	/// call endStruct() after number of fields are read
	void endStruct();

protected:

	enum MATType
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

	struct StructInfo
	{
		std::list<std::string> names;
	};

	DataReader reader;
	std::vector<StructInfo> structStack;
};

/// @}

} // namespace digi

#endif
