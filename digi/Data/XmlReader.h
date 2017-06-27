#ifndef digi_Data_XmlReader_h
#define digi_Data_XmlReader_h

#include "BufferedReader.h"
#include "Parser.h"
#include "XMLHeader.h"


namespace digi {

/// @addtogroup Data
/// @{

/// XML reader
class XmlReader : public BufferedReader
{
public:
	enum Token
	{
		//BEGIN_HEADER,
		END_HEADER,
		//NO_HEADER,
		
		BEGIN_START_TAG,
		END_START_TAG,		
		ATTRIBUTE,
		END_TAG,
		TEXT,
		COMMENT
	};

	XmlReader(Pointer<IODevice> dev)
		: BufferedReader(dev), state(STATE_TEXT) {}
	XmlReader(const fs::path& path)
		: BufferedReader(path), state(STATE_TEXT) {}

	// read XML header
	void readHeader(XMLHeader& header);

	/// read next element and return its type. then use getName() and getValue()
	Token read();
	
	/// get name of element that was last read
	const std::string& getName() {return this->name;}

	/// swap name into given string
	void swapName(std::string& name) {return name.swap(this->name);}

	/// get value of element that was last read
	const std::string& getValue() {return this->value;}
	
	/// skips the current tag past its end tag
	void skipTag();
	
	
	/// get current line index
	int getLineIndex() {return this->whiteSpaceParser.lineIndex;}
	
protected:

	enum State
	{
		STATE_TEXT,
		STATE_TAG_COMMENT,
		STATE_ATTRIBUTE
	};
	State state;
	std::string name;
	std::string value;
	
	WhiteSpaceParser whiteSpaceParser;
};

/// @}

} // namespace digi

#endif
