#include "XmlWriter.h"


namespace digi {

void XmlWriter::writeHeader(const XMLHeader& header)
{
	// write header tag
	this->writeData("<?xml", 5);
	if (!header.version.empty())
		this->writeAttribute("version", header.version);
	if (!header.encoding.empty())
		this->writeAttribute("encoding", header.encoding);
	this->writeData("?>\n", 3);

	// write doctype
}

void XmlWriter::beginStartTag(StringRef name)
{
	this->writeData(this->currentIndent.data(), this->currentIndent.size());
	this->writeData("<", 1);
	this->writeData(name.data(), name.size());
}

void XmlWriter::writeAttribute(StringRef name, StringRef value)
{
	this->writeData(" ", 1);
	this->writeData(name.data(), name.size());
	this->writeData("=\"", 2);
	this->writeData(value.data(), value.size());	
	this->writeData("\"", 1);
}

void XmlWriter::endTag(StringRef name)
{
	this->writeData("</", 2);
	this->writeData(name.data(), name.size());
	this->writeData(">\n", 2);
}

void XmlWriter::writeText(StringRef value)
{
	this->writeData(value.data(), value.size());
}

void XmlWriter::writeTextTag(StringRef name, StringRef value)
{
	this->beginStartTag(name);
	this->endStartTag();
	this->writeText(value);
	this->endTag(name);
}

void XmlWriter::writeComment(StringRef value)
{
	this->writeData(this->currentIndent.data(), this->currentIndent.size());
	this->writeData("<!-- ", 5);
	this->writeData(value.data(), value.size());
	this->writeData(" -->\n", 5);	
}

void XmlWriter::beginBlock()
{
	this->writeData("\n", 1);
	this->currentIndent += this->indent;
}

void XmlWriter::endBlock()
{
	int len = int(this->currentIndent.length() - this->indent.length());
	this->currentIndent.erase(std::max(len, 0));
	this->writeData(this->currentIndent.data(), this->currentIndent.size());
}

} // namespace digi
