#ifndef digi_Data_DataException_h
#define digi_Data_DataException_h

#include <digi/Utility/StringRef.h>
#include <digi/System/IOException.h>


namespace digi {

/// @addtogroup Data
/// @{

/// exception for data readers
class DataException : public std::exception
{
public:
		
	enum Reason
	{
		// unexpected end of data encountered (encountered end in the middle of a chunk or before xml end tag)
		UNEXPECTED_END_OF_DATA,

		// file format unknown or not supported for reading or writing
		FORMAT_NOT_SUPPORTED,

		// format error (magic number at beginning of file does not match, document type does not match)
		FORMAT_ERROR,		
		
		// unknown version of a known file format
		UNKNOWN_VERSION,
		
		// incomplete data encountered (some required information is missing)
		DATA_INCOMPLETE,

		// inconsistent data encountered (xml end tag does not match start tag)
		DATA_INCONSISTENT,

		// corrupt data encountered (chunk too long or wrong length, invalid token encountered)
		DATA_CORRUPT,

		// bad value (wrong type, lexical cast failed, out of range)
		BAD_VALUE
	};
	
	DataException(Pointer<IODevice> device, Reason reason)
		: device(device), reason(reason) {}
	
	virtual ~DataException() throw();
			
	/// get device where error occured. use getResource() on the device to find out the file, device, address etc.
	Pointer<IODevice> getDevice() {return this->device;}

	/// get exception reason
	Reason getReason() {return this->reason;}
	
	/// get exception reason as text
	virtual const char* what() const throw();

protected:
	
	Pointer<IODevice> device;
	Reason reason;
};


/// exception for text data readers with additional information where the error occured
class TextDataException : public DataException
{
public:

	TextDataException(Pointer<IODevice> device, Reason reason, int lineIndex)//, StringRef tag)
		: DataException(device, reason), lineIndex(lineIndex)/*, tag(tag)*/ {}

	virtual ~TextDataException() throw();


	int getLineIndex() {return this->lineIndex;}
	//const std::string& getTag() {return this->tag;}

protected:

	// line index (1 based) where the error occured
	int lineIndex;
	
	// tag where the error occured
	std::string tag;
};

/// @}

} // namespace digi

#endif
