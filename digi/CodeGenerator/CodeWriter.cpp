#include <digi/Utility/Ascii.h>
#include <digi/Utility/StringUtility.h>

#include "CodeWriter.h"


namespace digi {


// CodeStream

CodeStream::~CodeStream()
{
}


// StringCodeStream

StringCodeStream::~StringCodeStream()
{
}

void StringCodeStream::writeString(const char* str, size_t len)
{
	this->str.append(str, len);
}


// StringRefCodeStream

StringRefCodeStream::~StringRefCodeStream()
{
}

void StringRefCodeStream::writeString(const char* str, size_t len)
{
	this->str.append(str, len);
}


// CodeWriter

CodeWriter::~CodeWriter()
{
}

void CodeWriter::writeString(const char* str, size_t len)
{	
	size_t s = 0;
	while (s < len)
	{
		size_t e;
		for (e = s; e < len && str[e] != '\n'; ++e);
		
		this->line.append(str + s, e - s);
		if (e == len)
			break;
		s = e + 1;
			
		this->writeLine();
	}
}

void CodeWriter::writeLine()
{
	if (!this->line.empty())
	{
		// line not empty: write
		writeLineInternal();
		this->line.clear();
	}
	else
	{
		// line empty: defer new line
		++this->numNewLines;
	}
}

void CodeWriter::writeLine(char ch)
{
	if (ch == ',')
	{
		this->writeLine();
		this->deferredComma = 1;
	}
	else
	{
		*this << ch;
		this->writeLine();
	}
}

void CodeWriter::beginScope()
{
	*this << "{\n";
	this->incIndent();
}

void CodeWriter::endScope()
{
	this->deferredComma = 0;
	this->decIndent();
	*this << "}\n";
}

void CodeWriter::endScope(char ch)
{
	this->deferredComma = 0;
	this->decIndent();
	*this << '}';
	this->writeLine(ch);
}

void CodeWriter::endScope(const std::string& statement)
{
	this->decIndent();
	*this << "} " << statement << '\n';
}

void CodeWriter::beginArray()
{
	*this << "[\n";
	this->incIndent();
}

void CodeWriter::endArray(char ch)
{
	this->deferredComma = 0;
	this->decIndent();
	*this << ']';
	this->writeLine(ch);
}

void CodeWriter::beginNamespace(const std::string& name)
{
	*this << "namespace " << name << "\n{\n";
	this->incIndent();
}

void CodeWriter::endNamespace(const std::string& name)
{
	this->decIndent();
	*this << "} // " << name << "\n";
}

void CodeWriter::beginFunction(const std::string& returnType, const std::string& name)
{
	*this << returnType << " " << name << "()\n{\n";
	this->incIndent();
}

void CodeWriter::beginStruct()
{
	*this << "struct\n{\n";
	this->incIndent();
}

void CodeWriter::endStruct(const std::string& name)
{
	this->decIndent();
	*this <<"} " << name << ";\n";
}

void CodeWriter::beginStruct(const std::string& name)
{
	*this << "struct " << name << "\n{\n";
	this->incIndent();
}

void CodeWriter::beginStruct(const std::string& name, const std::string& parent)
{
	*this << "struct " << name << " : public " << parent << "\n{\n";
	this->incIndent();
}

void CodeWriter::endStruct()
{
	this->decIndent();
	this->writeLine("};");
}

void CodeWriter::writeLineInternal()
{
	// add deferred comma, new lines and indent
	this->finishLine(this->line);
	
	// write to device
	this->dev->write(this->line.data(), this->line.length());
}

} // namespace digi
