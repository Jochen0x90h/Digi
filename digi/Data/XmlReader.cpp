#include "DataException.h"
#include "XmlReader.h"


namespace digi {

namespace
{
	// parses two chars and sets success to the parsed char
	struct Char2Parser
	{
		Char2Parser(char ch1, char ch2)
			: ch1(ch1), ch2(ch2), success() {}

		int parse(char ch)
		{
			if (ch == this->ch1 || ch == this->ch2)
			{
				this->success = ch;
				return 1;
			}
			return 0;
		}

		void add(char* begin, char* end)
		{
		}

		char ch1;
		char ch2;
		char success;
	};

	// parses text until next tag ('<')
	struct TextParser
	{
		TextParser(std::string& text)
			: text(text) {}
		
		int parse(char ch)
		{
			return ch == '<' ? 1 : -1;
		}

		void add(char* begin, char* end)
		{
			this->text.append(begin, end);
		}

		std::string& text;
	};

	// parses an xml identifier
	struct IdentifierParser
	{
		IdentifierParser(std::string& identifier)
			: identifier(identifier) {}
		
		int parse(char ch)
		{
			return ch == ':' ||  (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || ch == '_' || (ch >= 'a' && ch <= 'z') || (ch & 0x80) != 0 ? -1 : 0;
		}

		void add(char* begin, char* end)
		{
			this->identifier.append(begin, end);
		}

		std::string& identifier;
	};

	// parses an xml unquoted value
	struct ValueParser
	{
		ValueParser(std::string& value)
			: value(value) {}
		
		int parse(char ch)
		{
			return uint8_t(ch) > 32 ? -1 : 0;
		}

		void add(char* begin, char* end)
		{
			this->value.append(begin, end);
		}

		std::string& value;
	};

	// parses an xml quoted value
	struct QuotedValueParser
	{
		QuotedValueParser(char ch, std::string& value)
			: ch(ch), lastChar(), value(value) {}
		
		int parse(char ch)
		{
			int result = this->lastChar != '\\' && ch == this->ch ? 1 : -1;
			this->lastChar = ch;
			return result;
		}

		void add(char* begin, char* end)
		{
			this->value.append(begin, end);
		}

		char ch;
		char lastChar;
		std::string& value;
	};

	// parses an xml comment until '--'
	struct CommentParser
	{
		CommentParser(std::string& value)
			: lastChar(), value(value) {}
		
		int parse(char ch)
		{
			int result = this->lastChar == '-' && ch == '-' ? 1 : -1;
			this->lastChar = ch;
			return result;
		}

		void add(char* begin, char* end)
		{
			this->value.append(begin, end);
		}

		char lastChar;
		std::string& value;
	};

} // anonymous namespace

void XmlReader::readHeader(XMLHeader& header)
{
	// read until '<'
	TextParser textParser(this->value);
	this->parse(textParser);
	
	// discard
	this->value.clear();
	
	// match '?'
	CharParser headerBegin('?');
	this->parse(headerBegin);
	if (!headerBegin.success)
	{
		// no header
		this->state = STATE_TAG_COMMENT;
		return;
	}

	// xml starts with header ('<?')

	// read identifier
	IdentifierParser identifierParser(this->name);
	this->parse(identifierParser);
	if (this->name != "xml")
	{
		// format error: is not xml
		throw DataException(this->dev, DataException::FORMAT_ERROR);
	}

	// skip white space
	this->parse(this->whiteSpaceParser);

	// attribute after tag
	this->state = STATE_ATTRIBUTE;

	// read attributes of header
	Token token;
	while ((token = this->read()) == XmlReader::ATTRIBUTE)
	{
		if (this->name == "version")
			header.version.swap(this->value);
		else if (this->name == "encoding")
			header.encoding.swap(this->value);
	}
	if (token != END_HEADER)
	{
		// format error: header does not end with '?>'
		throw DataException(this->dev, DataException::FORMAT_ERROR);
	}
	
	// check for '<!DOCTYPE'

}

XmlReader::Token XmlReader::read()
{
	this->name.clear();
	this->value.clear();
	
	switch (this->state)
	{
	case STATE_TEXT:
		{
			// skip white space
			this->parse(this->whiteSpaceParser);

			// read until '<'
			TextParser textParser(this->value);
			this->parse(textParser);

			// erase '<'
			this->value.erase(this->value.length() - 1);
			
			// tag or comment after text
			this->state = STATE_TAG_COMMENT;
			if (!this->value.empty())
				return TEXT;
		}
		// fall through

	case STATE_TAG_COMMENT:
		// '<' aleady parsed, identifier, '/' or '!' are next
		{
			// match '/' or '!'
			Char2Parser endTagOrComment('/', '!');
			this->parse(endTagOrComment);
			if (endTagOrComment.success == 0)
			{
				// begin tag matched ('<')
				
				// skip white space
				this->parse(this->whiteSpaceParser);

				// read identifier
				IdentifierParser identifierParser(this->name);
				this->parse(identifierParser);
				if (name.empty())
				{
					// parse error
					throw TextDataException(this->dev, DataException::DATA_CORRUPT, getLineIndex());
				}
				
				// skip white space
				this->parse(this->whiteSpaceParser);
				
				// attribute after tag
				this->state = STATE_ATTRIBUTE;
				return BEGIN_START_TAG;
			}
			if (endTagOrComment.success == '/')
			{
				// end tag matched ('</')
				
				// skip white space
				this->parse(this->whiteSpaceParser);

				// read identifier
				IdentifierParser identifierParser(this->name);
				this->parse(identifierParser);
				if (name.empty())
				{
					// parse error
					throw TextDataException(this->dev, DataException::DATA_CORRUPT, getLineIndex());
				}
				
				// skip white space
				this->parse(this->whiteSpaceParser);
				
				// match end of tag'>'
				CharParser tagEnd('>');
				this->parse(tagEnd);
				if (!tagEnd.success)
				{
					// parse error
					throw TextDataException(this->dev, DataException::DATA_CORRUPT, getLineIndex());
				}
				
				// text after end tag
				this->state = STATE_TEXT;
				return END_TAG;
			}

			// comment matched ('<!')
				
			CommentParser commentParser(this->value);

			// read until first '--' (must flollow immediately)
			this->parse(commentParser);
			bool error = this->value.length() != 2;
			this->value.clear();
			if (error)
			{
				// parse error
				throw TextDataException(this->dev, DataException::DATA_CORRUPT, getLineIndex());
			}

			// read until second '--'
			this->parse(commentParser);
			this->value.erase(this->value.length() - 2);

			// match end of tag '>' (must follow immediately)
			CharParser tagEnd('>');
			this->parse(tagEnd);
			if (!tagEnd.success)
			{
				// parse error
				throw TextDataException(this->dev, DataException::DATA_CORRUPT, getLineIndex());
			}
				
			// text after comment
			this->state = STATE_TEXT;
			return COMMENT;			
		}
		break;
	
	case STATE_ATTRIBUTE:
		// precondition: '>', attribute name or invalid character is next
		{
			Char2Parser tagType('?', '/');
			this->parse(tagType);

			CharParser tagEnd('>');
			this->parse(tagEnd);
			if (tagEnd.success)
			{
				// end of tag matched ('>')
				
				// text after end of tag
				this->state = STATE_TEXT;
				return tagType.success == 0 ? END_START_TAG : (tagType.success == '?' ? END_HEADER : END_TAG);
			}
			if (tagType.success != 0)
			{
				// parse error
				throw TextDataException(this->dev, DataException::DATA_CORRUPT, getLineIndex());
			}

			// read identifier
			IdentifierParser identifierParser(this->name);
			this->parse(identifierParser);
			if (name.empty())
			{
				// parse error
				throw TextDataException(this->dev, DataException::DATA_CORRUPT, getLineIndex());
			}

			// skip white space
			this->parse(this->whiteSpaceParser);
				
			// skip '='
			CharParser equals('=');
			this->parse(equals);
			if (!equals.success)
			{
				// parse error
				throw TextDataException(this->dev, DataException::DATA_CORRUPT, getLineIndex());
			}

			// skip white space
			this->parse(this->whiteSpaceParser);

			Char2Parser quoteParser('\'', '"');
			this->parse(quoteParser);
			if (quoteParser.success == 0)
			{
				ValueParser valueParser(this->value);
				this->parse(valueParser);
			}
			else
			{
				QuotedValueParser quotedValueParser(quoteParser.success, this->value);
				this->parse(quotedValueParser);

				// erase trailing quote
				this->value.erase(this->value.length() - 1);
			}
				
			// skip white space
			this->parse(this->whiteSpaceParser);
				
			// sate stays the same
			return ATTRIBUTE;
		}
		break;
	}

	// should never be reached
	return TEXT;
}

void XmlReader::skipTag()
{
	// skip attributes
	Token token;
	while ((token = this->read()) == ATTRIBUTE);
	if (token == END_START_TAG)
	{
		// skip children and text
		while ((token = this->read()) != END_TAG)
		{
			if (token == BEGIN_START_TAG)
			{
				std::string name;
				name.swap(this->name);
				
				this->skipTag();

				// check if end tag matches start tag
				if (this->name != name && !this->name.empty())
					throw TextDataException(this->dev, DataException::DATA_INCONSISTENT, getLineIndex());
			}
		}
	}
}

} // namespace digi
