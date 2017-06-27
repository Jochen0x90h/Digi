#ifndef digi_Data_JsonReader_h
#define digi_Data_JsonReader_h

#include "BufferedReader.h"
#include "Parser.h"


namespace digi {

/// @addtogroup Data
/// @{

/// JSON reader. see http://json.org
class JsonReader : public BufferedReader
{
public:
	enum Token
	{
		BEGIN_STRUCT,
		END_STRUCT,
		BEGIN_ARRAY,
		END_ARRAY,
		
		// attribute was read, use getValue() to get the attribute name
		ATTRIBUTE,

		// array element was read
		ELEMENT,

		// a number was read, use getValue() and convert to number
		NUMBER,

		// a string was read, use getValue()
		STRING,

		// a boolean was read, use getBool()
		BOOLEAN,

		// a null value was read
		NULL_VALUE,

		COMMENT
	};

	JsonReader(Pointer<IODevice> dev)
		: BufferedReader(dev), state(STATE_INITIAL) {}
	JsonReader(const fs::path& path)
		: BufferedReader(path), state(STATE_INITIAL) {}


	/// read next element and return its type. then use getValue()
	Token read();
	
	/// read the next element and check if it is a number. if not DataException::FORMAT_ERROR is thrown
	const std::string& readNumber();

	/// read the next element and check if it is a string. if not DataException::FORMAT_ERROR is thrown
	const std::string& readString();

	/// read the next element and check if it is a bool. if not DataException::FORMAT_ERROR is thrown
	bool readBool();


	/// get value of element that was last read
	const std::string& getValue() {return this->value;}
	
	/// get bool value if the last read value was BOOLEAN
	bool getBool() {return this->value.length() == 4;}


	/// skips the next value past its end
	void skipValue() {this->skipValue(this->read());}

	/// skips the current value past its end
	void skipValue(Token token);


	/// get current line index
	int getLineIndex() {return this->whiteSpaceParser.lineIndex;}

	/// set position in file and reset parser state
	void setPosition(int64_t position)
	{
		BufferedReader::setPosition(position);
		this->state = STATE_STRUCT_VALUE;
		this->whiteSpaceParser.lineIndex = 1;
	}
	
protected:

	enum State
	{
		STATE_INITIAL,
	
		STATE_ATTRIBUTE,
		STATE_STRUCT_VALUE,
		STATE_STRUCT_END = STATE_STRUCT_VALUE + 1,

		STATE_ELEMENT,
		STATE_ARRAY_VALUE,
		STATE_ARRAY_END = STATE_ARRAY_VALUE + 1,
	};
	State state;
	std::vector<State> states;
	std::string value;
	
	WhiteSpaceParser whiteSpaceParser;
};

/// @}

} // namespace digi

#endif
