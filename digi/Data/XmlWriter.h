#ifndef digi_Data_XmlWriter_h
#define digi_Data_XmlWriter_h

#include <sstream>

#include "BufferedWriter.h"
#include "XMLHeader.h"


namespace digi {

/// @addtogroup Data
/// @{

/// XML writer
class XmlWriter : public BufferedWriter
{
public:
	XmlWriter(Pointer<IODevice> dev, const std::string& indent = "\t")
		: BufferedWriter(dev), indent(indent) {}
	XmlWriter(const fs::path& path, const std::string& indent = "\t")
		: BufferedWriter(path), indent(indent) {}

	/// write XML header	
	void writeHeader(const XMLHeader& header);

	/// write begin tag with indentation and without closing bracket, e.g. '<name'
	void beginStartTag(StringRef name);
	
	/// write end of attribue list, always '>'
	void endStartTag() {this->writeData(">", 1);}

	/// write attribute, e.g. 'name="value"'
	void writeAttribute(StringRef name, StringRef value);
	
	/// write end of start tag and new line, always '/>'
	void endTag() {this->writeData("/>\n", 3);}

	/// write end tag and new line, e.g. '</tag>'
	void endTag(StringRef name);
	
	/// write text, e.g. 'text'
	void writeText(StringRef value);
	
	/// write tag with text, e.g. '<name>text</name>'
	void writeTextTag(StringRef name, StringRef value);

	/// write comment
	void writeComment(StringRef value);
	
	/// writes new line, increments indentation and writes indentation
	void beginBlock();
	
	/// decrements indentation and writes indentation
	void endBlock();

protected:

	// indent character or string
	std::string indent;
	
	// current indentation (indent string concatenated n times)
	std::string currentIndent;
};

/// @}

} // namespace digi

#endif
