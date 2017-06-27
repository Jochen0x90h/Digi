#ifndef digi_Data_JsonWriter_h
#define digi_Data_JsonWriter_h

#include <sstream>

#include "BufferedWriter.h"


namespace digi {

/// @addtogroup Data
/// @{

/// JSON writer. see http://json.org
class JsonWriter : public BufferedWriter
{
public:
	JsonWriter(Pointer<IODevice> dev, const std::string& indent = "\t")
		: BufferedWriter(dev), state(STATE_VALUE), indent(indent) {}
	JsonWriter(const fs::path& path, const std::string& indent = "\t")
		: BufferedWriter(path), state(STATE_VALUE), indent(indent) {}

	
	/// begin structure ('{')
	void beginStruct();

	/// end structure ('}')
	void endStruct();

	/// begin array ('[')
	void beginArray();

	/// end array (']')
	void endArray();

	/// write attribute (e.g. '"foo": ')
	void writeAttribute(StringRef name);
	
	/// write number value (e.g. '55')
	void writeNumber(StringRef value);

	/// write string value (e.g. '"bar"')
	void writeString(StringRef value);			
	
	/// write boolean value ('false' or 'true')
	void writeBool(bool value);

	/// write null value ('null')
	void writeNull();

	/// write comment (e.g. '// comment'). note that comments are a JSON extension
	void writeComment(StringRef value);
	
protected:

	void handleState(int maxNumElements = 0);

	enum State
	{
		STATE_VALUE,
		STATE_INDENT,
		STATE_COMMA
	};
	State state;

	// indent character or string
	std::string indent;
	
	// current indentation (indent string concatenated n times)
	std::string currentIndent;

	// number of commas in a row
	int numElements;
};

/// @}

} // namespace digi

#endif
