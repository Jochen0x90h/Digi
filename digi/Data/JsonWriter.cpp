#include "JsonWriter.h"


namespace digi {

void JsonWriter::beginStruct()
{
	this->handleState();
	this->writeData("{", 1);
	this->currentIndent += this->indent;
	this->state = STATE_INDENT;

	this->numElements = 0;
}

void JsonWriter::endStruct()
{
	int len = int(this->currentIndent.length() - this->indent.length());
	this->currentIndent.erase(std::max(len, 0));

	if (this->state == STATE_COMMA)
	{
		this->writeData("\n", 1);
		this->writeData(this->currentIndent.data(), this->currentIndent.size());
	}
	else
	{
		// struct is empty, write as "{}"
	}
	
	this->writeData("}", 1);
	this->state = STATE_COMMA;

	this->numElements = 0;
}

void JsonWriter::beginArray()
{
	this->handleState();
	this->writeData("[", 1);
	this->currentIndent += this->indent;
	this->state = STATE_INDENT;

	this->numElements = 0;
}

void JsonWriter::endArray()
{
	int len = int(this->currentIndent.length() - this->indent.length());
	this->currentIndent.erase(std::max(len, 0));

	if (this->state == STATE_COMMA)
	{
		if (this->numElements == 0)
		{
			// array contains a complex type
			this->writeData("\n", 1);
			this->writeData(this->currentIndent.data(), this->currentIndent.size());
		}		
	}
	else
	{
		// array is empty, write as "[]"
	}
	
	this->writeData("]", 1);
	this->state = STATE_COMMA;

	this->numElements = 0;
}

void JsonWriter::writeAttribute(StringRef name)
{
	if (this->state == STATE_COMMA)
		this->writeData(",\n", 2);
	else if (this->state == STATE_INDENT)
		this->writeData("\n", 1);
	this->writeData(this->currentIndent.data(), this->currentIndent.size());
	this->writeData("\"", 1);
	this->writeData(name.data(), name.size());
	this->writeData("\": ", 3);
	this->state = STATE_VALUE;

	this->numElements = 0;
}

void JsonWriter::writeNumber(StringRef value)
{
	this->handleState(10);
	this->writeData(value.data(), value.size());
	this->state = STATE_COMMA;
}

void JsonWriter::writeString(StringRef value)
{
	this->handleState(5);
	this->writeData("\"", 1);
	
	size_t len = value.size();
	size_t pos = 0;
	for (size_t i = 0; i < len; ++i)
	{
		char ch = value[i];
		if (ch == '\t' || ch == '\n' || ch == '\"' || ch == '\\')
		{
			this->writeData(value.data() + pos, i - pos);
			switch (ch)
			{
			case '\t':
				this->writeData("\\t", 2);
				break;
			case '\n':
				this->writeData("\\n", 2);
				break;
			case '\"':
				this->writeData("\\\"", 2);
				break;
			case '\\':
				this->writeData("\\\\", 2);
				break;
			}
			pos = i + 1;
		}
	}
	this->writeData(value.data() + pos, len - pos);
	
	//this->writeData(value.data(), value.size());
	this->writeData("\"", 1);
	this->state = STATE_COMMA;
}

void JsonWriter::writeBool(bool value)
{
	this->handleState(15);
	if (value)
		this->writeData("true", 4);
	else
		this->writeData("false", 5);
	this->state = STATE_COMMA;
}

void JsonWriter::writeNull()
{
	this->handleState(15);
	this->writeData("null", 4);
	this->state = STATE_COMMA;
}

void JsonWriter::writeComment(StringRef value)
{
	this->writeData(this->currentIndent.data(), this->currentIndent.size());
	this->writeData("// ", 3);
	this->writeData(value.data(), value.size());
	this->writeData("\n", 1);	
}

void JsonWriter::handleState(int maxNumElements)
{
	if (this->state == STATE_COMMA)
	{
		if (this->numElements < maxNumElements)
		{
			// comma, space
			this->writeData(", ", 2);
			++this->numElements;
		}
		else
		{
			// comma, new line, indent
			this->writeData(",\n", 2);
			this->writeData(this->currentIndent.data(), this->currentIndent.size());
			this->numElements = 1;
		}
	}
	else if (this->state == STATE_INDENT)
	{
		// indent
		if (maxNumElements == 0)
		{
			this->writeData("\n", 1);
			this->writeData(this->currentIndent.data(), this->currentIndent.size());
		}
		else
		{
			this->numElements = 1;
		}
	}		
}

} // namespace digi
