#ifndef digi_CodeGenerator_CodeWriter_h
#define digi_CodeGenerator_CodeWriter_h

#include <string>
#include <fstream>

#include <boost/format.hpp>

#include <digi/Utility/StringUtility.h>
#include <digi/Utility/Convert.h>
#include <digi/System/File.h>
#include <digi/Math/All.h>

#include "Language.h"


namespace digi {

/// @addtogroup CodeGenerator
/// @{

template <typename Type>
struct TypeName;


// proxy to distinguish between int and identifier
inline int ident(int x)
{
	return x;
}

inline int ident(size_t x)
{
	return int(x);
}

class CodeStream
{
public:

	CodeStream(Language language = Language::CPP)
		: language(language) {}
	virtual ~CodeStream();

	void setLanguage(Language language) {this->language = language;}
	Language getLanguage() const {return this->language;}
	
	CodeStream& operator <<(bool v)
	{
		if (v)
			this->writeString("true", 4);
		else
			this->writeString("false", 5);
		return *this;
	}

	CodeStream& operator <<(char ch)
	{
		this->writeString(&ch, 1);
		return *this;
	}

	CodeStream& operator <<(signed char v)
	{
		return *this << (long long)v;
	}

	CodeStream& operator <<(unsigned char v)
	{
		return *this << (unsigned long long)v;
	}

	CodeStream& operator <<(short v)
	{
		return *this << (long long)v;
	}

	CodeStream& operator <<(unsigned short v)
	{
		return *this << (unsigned long long)v;
	}

	CodeStream& operator <<(int v)
	{
		return *this << (long long)v;
	}

	CodeStream& operator <<(unsigned int v)
	{
		return *this << (unsigned long long)v;
	}

	CodeStream& operator <<(long v)
	{
		return *this << (long long)v;
	}

	CodeStream& operator <<(unsigned long v)
	{
		return *this << (unsigned long long)v;
	}

	CodeStream& operator <<(long long v)
	{
		//! idea: if actual value exceeds 32 bits then use suffix (LL) or macro (INT64_C)
		
		char buf[24];
		char* end = toString(buf, v);
		/*if (this->language == CPP)
		{
			end[0] = 'L';
			end[1] = 'L';
			end += 2;
		}*/
		this->writeString(buf, end - buf);
		return *this;
	}

	CodeStream& operator <<(unsigned long long v)
	{
		char buf[24];
		char* end = toString(buf, v);
		if (this->language.isCPP())
		{
			end[0] = 'u';		
			//end[1] = 'L';
			//end[2] = 'L';
			end += 1;
		}
		this->writeString(buf, end - buf);
		return *this;
	}
	
	CodeStream& operator <<(float v)
	{
		char buf[20];
		
		// we need precise representation of 255*256*256=16711680 for storing a float into 4 bytes (depth to rgba buffer)
		char* end = toString(buf, v, 7);
		if (this->language == Language::CPP)
		{
			*end = 'f';
			++end;
		}
		this->writeString(buf, end - buf);
		return *this;
	}

	CodeStream& operator <<(double v)
	{
		char buf[32];
		char* end = toString(buf, v, 12);
		this->writeString(buf, end - buf);
		return *this;
	}

	CodeStream& operator <<(const char* v)
	{
		this->writeString(v, length(v));
		return *this;
	}

	CodeStream& operator <<(const std::string& v)
	{
		this->writeString(v.data(), v.length());
		return *this;
	}

	CodeStream& operator <<(const boost::format& f)
	{
		std::string v = f.str();
		this->writeString(v.data(), v.length());
		return *this;
	}
	
	// write argument list (e.g. "(1, 2, 3)")
	template <typename Iterator>
	void writeArgumentList(Iterator values, int numValues)
	{
		this->writeString("(", 1);
		for (int i = 0; i < numValues; ++i, ++values)
		{
			if (i != 0)
				*this << ", ";
			*this << *values;
		}
		this->writeString(")", 1);
	}
	
	virtual void writeString(const char* str, size_t len) = 0;

protected:
		
	Language language;
};

class StringCodeStream : public CodeStream
{
public:

	StringCodeStream(Language language = Language::CPP)
		: CodeStream(language) {}
	~StringCodeStream();
	
	virtual void writeString(const char* str, size_t len);

	const std::string& getString() {return this->str;}
	
	std::string str;
};

class StringRefCodeStream : public CodeStream
{
public:

	StringRefCodeStream(std::string& str, Language language = Language::CPP)
		: CodeStream(language), str(str) {}
	~StringRefCodeStream();
	
	virtual void writeString(const char* str, size_t len);
	
	std::string& str;
};


/**
	CodeWriter is for writing code of various C dialects to a text file.
	objects of this class should be created on stack
*/
class CodeWriter : public CodeStream
{
	friend class NestedInitializer;
	
public:


	CodeWriter(Pointer<IODevice> dev = null, Language language = Language::CPP)
		: CodeStream(language), dev(dev), deferredComma(false), numNewLines(0), indent(0) {}

	CodeWriter(const fs::path& path, Language language = Language::CPP)
		: CodeStream(language), dev(File::create(path)), deferredComma(false), numNewLines(0), indent(0) {}

	virtual ~CodeWriter();
		
	void close() {this->writeLineInternal(); this->dev->close();}
	
	/// write a string with given length and do substitutions
	virtual void writeString(const char* str, size_t len);

	/// write current line and new line character '\n'
	virtual void writeLine();
	
	/// append string to current line and write current line
	void writeLine(const std::string& line) {*this << line; this->writeLine();}
	void writeLine(const char* line) {*this << line; this->writeLine();}

	/// write current line with extra token such as ','
	void writeLine(char ch);

	// write comment
	void writeComment(const std::string& comment) {*this << "// " << comment << "\n";}


	/// add one tab to indentation string
	void incIndent() {++this->indent;}

	/// remove one tab from indentation string
	void decIndent() {this->indent = max(0, this->indent - 1);}


	/// begin a scope ("{")
	void beginScope();

	/// end a scope ("}")
	void endScope();
	
	/// end a scope ("}") with extra token such as ';' for struct or initializer or ',' for list
	void endScope(char ch);

	/// end a scope with statement (e.g. "while (true);")
	void endScope(const std::string& statement);


	/// begin an array ("[")
	void beginArray();
	
	/// end an array ("]") with extra token such as ';' for struct or initializer or ',' for list
	void endArray(char ch);


	/// begin namespace
	void beginNamespace(const std::string& name);

	/// end namespace
	void endNamespace() {this->endScope();}

	/// end namespace with name as comment
	void endNamespace(const std::string& name);

	
	// begin a function
	void beginFunction(const std::string& returnType, const std::string& name);
	
	// end a function
	void endFunction() {this->endScope();}
	

	/// begin anonymous struct
	void beginStruct();

	/// end anonymous struct
	void endStruct(const std::string& name);

	/// begin struct
	void beginStruct(const std::string& name);

	/// begin derived struct
	void beginStruct(const std::string& name, const std::string& parent);

	/// end struct
	void endStruct();


	// write array of values (e.g. "{1, 3, 3}")
	template <typename Iterator>
	void writeArray(Iterator values, size_t numValues)
	{
		bool curly = this->language.isCPP();
		*this << (curly ? '{' : '[');
		for (size_t i = 0; i < numValues; ++i)
		{
			if (i != 0)
				*this << ", ";
			*this << *values;
			++values;
		}
		*this << (curly ? '}' : ']');
	}

	template <typename Type>
	void writeArray(const std::vector<Type>& array)
	{
		this->writeArray(array.begin(), array.size());
	}

	// write array of values (e.g. "const int values[] = {1, 3, 3};")
	template <typename Iterator>
	void writeArray(const std::string& name, Iterator values, size_t numValues, size_t numValuesPerLine = 1)
	{
		*this << "const " << TypeName<typename std::iterator_traits<Iterator>::value_type>::get() << " " << name << "[] = \n";
		this->beginScope();

		while (numValues > 0)
		{
			size_t num = min(numValuesPerLine, numValues);
			for (size_t i = 0; i < num; ++i)
			{
				*this << *values << ", ";
				++values;
			}
			this->writeLine();
			numValues -= num;
		}

		this->endScope(';');
	}

	template <typename Type>
	void writeArray(const std::string& name, std::vector<Type>& array, int numValuesPerLine = 1)
	{
		this->writeArray(name, array.begin(), array.size(), numValuesPerLine);
	}


/*		
	CodeWriter& operator <<(void (*manipulator)(CodeWriter&))
	{
		manipulator(*this);
		return *this;
	}
*/

	void swapLine(std::string& line) {line.swap(this->line);}
	std::string takeLine() {std::string line; line.swap(this->line); return line;}

	int getIndent() {return this->indent;}
	void setIndent(int indent) {this->indent = indent;}
	
	void flush() {writeLineInternal();}

protected:
	
	/// write current line (including comma and new lines of previous lines and indent of current line)
	virtual void writeLineInternal();
	
	void finishLine(std::string& line)
	{
		// add deferred comma
		line.insert(0, this->deferredComma, ',');
		
		// add deferred new lines
		line.insert(this->deferredComma, this->numNewLines, '\n');

		// add indent
		line.insert(this->deferredComma + this->numNewLines, this->indent, '\t');
		
		this->deferredComma = 0;
		this->numNewLines = 1;
	}
		
	Pointer<IODevice> dev;
	
	// current line
	std::string line;
	
	// 1 if deferred comma. if endScope() or endArray() follows, the comma is omitted
	int deferredComma;
	
	// number of deferred new lines
	int numNewLines;

	// current indentation depth
	int indent;
};


/*
static inline void endl(CodeWriter& w)
{
	w.writeLine();
}
*/


/// @}

} // namespace digi

#endif
