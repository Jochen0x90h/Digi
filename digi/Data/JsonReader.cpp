#include "DataException.h"
#include "JsonReader.h"


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

	// identifies the next json object
	struct TypeParser
	{
		enum Result
		{
			INVALID,
			STRUCT,
			ARRAY,
			NUMBER,
			STRING,
			VALUE
		};

		TypeParser()
			: result(INVALID) {}
		
		int parse(char ch)
		{
			if (ch == '{')
			{
				this->result = STRUCT;

				// consume '{'
				return 1;
			}
			if (ch == '[')
			{
				this->result = ARRAY;

				// consume '['
				return 1;
			}
			if (ch == '"')
			{
				this->result = STRING;
				
				// consume '"'
				return 1;
			}

			if (ch >= '0' && ch <= '9')
				this->result = NUMBER;
			else if (ch == 'f' || ch == 't' || ch == 'n')
				this->result = VALUE;

			// don't consume first character of number, false, true or null
			return 0;
		}

		void add(char* begin, char* end)
		{
		}

		Result result;
	};

	// parses a floating point number
	struct NumberParser
	{
		NumberParser(std::string& value)
			: value(value) {}
		
		int parse(char ch)
		{
			return (ch >= '0' && ch <= '9') || ch == '.' || ch == '+' || ch == '-' || ch == 'e' || ch == 'E' ? -1 : 0;
		}

		void add(char* begin, char* end)
		{
			this->value.append(begin, end);
		}

		std::string& value;
	};

	// parses a json value (false, true, null)
	struct ValueParser
	{
		ValueParser(std::string& value)
			: value(value) {}
		
		int parse(char ch)
		{
			return (ch >= 'a' && ch <= 'z') ? -1 : 0;
		}

		void add(char* begin, char* end)
		{
			this->value.append(begin, end);
		}

		std::string& value;
	};

	// parses a json quoted value
	struct QuotedValueParser
	{
		QuotedValueParser(char quoteChar, std::string& value)
			: quoteChar(quoteChar), lastChar(), value(value) {}
		
		int parse(char ch)
		{
			// check if ch is quote character (' or ")
			int result = ch == this->quoteChar ? 1 : -1;
			
			// check if ch is part of escape sequence
			if (this->lastChar == '\\')
			{
				// yes: do not match
				result = -1;
				this->lastChar = 0;
			}
			else
			{
				// no
				this->lastChar = ch;
			}
			return result;
		}

		void add(char* begin, char* end)
		{
			this->value.append(begin, end);
		}

		char quoteChar;
		char lastChar;
		std::string& value;
	};

} // anonymous namespace

JsonReader::Token JsonReader::read()
{
	this->value.clear();

	switch (this->state)
	{
	case STATE_ARRAY_END:
		// ',' or ']' is next
		{
			this->parse(this->whiteSpaceParser);
			Char2Parser parser(',', ']');
			this->parse(parser);
			if (!parser.success)
			{
				// parse error
				throw TextDataException(this->dev, DataException::DATA_CORRUPT, getLineIndex());
			}
		
			// check for end, pop state
			if (parser.success == ']')
			{
				this->state = this->states.back();
				this->states.pop_back();
				return END_ARRAY;
			}

			// parsed a ',': value or ']' is next
			this->state = STATE_ELEMENT;
		}
		// fall through: parse next value
	case STATE_ELEMENT:
		// value or ']' is next
		{
			this->parse(this->whiteSpaceParser);
			CharParser parser(']');
			this->parse(parser);
			if (parser.success)
			{
				this->state = this->states.back();
				this->states.pop_back();
				return END_ARRAY;
			}
			
			// return ELEMENT token
			state = STATE_ARRAY_VALUE;
			return ELEMENT;
		}
		break;
	
	case STATE_INITIAL:
		// check for utf-8 bom
		{
			CharParser parser(0xEF);
			this->parse(parser);
			if (parser.success)
			{
				CharParser parser2(0xBB);
				this->parse(parser2);
				CharParser parser3(0xBF);
				this->parse(parser3);
			
				if (!parser2.success || !parser3.success)
					throw TextDataException(this->dev, DataException::FORMAT_ERROR, getLineIndex());
			}
			this->state = STATE_STRUCT_VALUE;
		}
		// fall through
	case STATE_ARRAY_VALUE:
	case STATE_STRUCT_VALUE:
		// value (number, string, array or struct) is next
		// two states (STATE_ARRAY_VALUE and STATE_STRUCT_VALUE) to know if ']' or '}' has to come after the value
		{
			this->parse(this->whiteSpaceParser);

			// STATE_STRUCT_VALUE -> STATE_STRUCT_END
			// STATE_ARRAY_VALUE -> STATE_ARRAY_END
			this->state = State(this->state + 1);

			// determine next value type
			TypeParser typeParser;
			this->parse(typeParser);
			switch (typeParser.result)
			{
			case TypeParser::STRUCT:
				this->states.push_back(this->state);
				this->state = STATE_ATTRIBUTE;
				return BEGIN_STRUCT;
			case TypeParser::ARRAY:
				this->states.push_back(this->state);
				this->state = STATE_ELEMENT;
				return BEGIN_ARRAY;
			case TypeParser::NUMBER:
				{
					NumberParser parser(this->value);
					this->parse(parser);
					if (!value.empty())
						return NUMBER;
				}
				break;
			case TypeParser::STRING:
				{
					QuotedValueParser parser('"', this->value);
					this->parse(parser);
					
					// length of string (without trailing quote)
					size_t len = this->value.length() - 1;
					
					// replace escaped characters
					size_t i = 0;
					size_t j = 0;
					while (i < len)
					{
						if (this->value[i] == '\\')
						{
							// check next character (safe because trailing quote is behind last character)
							char next = this->value[i + 1];
							switch (next)
							{
							case 'n':
								this->value[j] = '\n';
								break;
							case 't':
								this->value[j] = '\t';
								break;
							default:
								this->value[j] = next;
							}
							i += 2;
						}
						else
						{
							this->value[j] = this->value[i];
							++i;
						}
						++j;
					}
					this->value.erase(j);
				}
				return STRING;
			case TypeParser::VALUE:
				{
					ValueParser parser(this->value);
					this->parse(parser);
					if (this->value == "false" || this->value == "true")
						return BOOLEAN;
					if (this->value == "null")
						return NULL_VALUE;
				}
				break;
			default:
				// parse error
				break;
			}

			// parse error
			throw TextDataException(this->dev, DataException::DATA_CORRUPT, getLineIndex());
		}
		break;

	case STATE_STRUCT_END:
		// ',' or '}' is next
		{
			this->parse(this->whiteSpaceParser);
			Char2Parser parser(',', '}');
			this->parse(parser);
			if (!parser.success)
			{
				// parse error
				throw TextDataException(this->dev, DataException::DATA_CORRUPT, getLineIndex());
			}
		
			// check for end, state stays the same
			if (parser.success == '}')
			{
				this->state = this->states.back();
				this->states.pop_back();
				return END_STRUCT;
			}

			// parsed a ',': next attribute follows
			this->state = STATE_ATTRIBUTE;
		}
		// fall through: parse next attribute
	case STATE_ATTRIBUTE:
		{
			// '}' or quoted attribute name is next
			this->parse(this->whiteSpaceParser);

			// check for '}'
			CharParser parser('}');
			this->parse(parser);
			if (parser.success)
			{
				this->state = this->states.back();
				this->states.pop_back();
				return END_STRUCT;
			}

			// parse attribute name
			CharParser parser1('"');
			this->parse(parser1);
			if (!parser1.success)
			{
				// parse error
				throw TextDataException(this->dev, DataException::DATA_CORRUPT, getLineIndex());
			}
			QuotedValueParser parser2('"', this->value);
			this->parse(parser2);
					
			// erase trailing quote
			this->value.erase(this->value.length() - 1);

			// parse ':'
			this->parse(whiteSpaceParser);
			CharParser parser3(':');
			this->parse(parser3);
			if (!parser3.success)
			{
				// parse error
				throw TextDataException(this->dev, DataException::DATA_CORRUPT, getLineIndex());
			}

			this->state = STATE_STRUCT_VALUE;
			return ATTRIBUTE;
		}
	}

	// should never be reached
	return COMMENT;
}

const std::string& JsonReader::readNumber()
{
	if (this->read() != JsonReader::NUMBER)
		throw TextDataException(this->dev, DataException::BAD_VALUE, getLineIndex());
	return this->value;
}

const std::string& JsonReader::readString()
{
	if (this->read() != JsonReader::STRING)
		throw TextDataException(this->dev, DataException::BAD_VALUE, getLineIndex());
	return this->value;
}

bool JsonReader::readBool()
{
	if (this->read() != JsonReader::BOOLEAN)
		throw TextDataException(this->dev, DataException::BAD_VALUE, getLineIndex());
	return this->value.length() == 4;
}

/*
bool JsonReader::skipValue()
{
	switch (this->read())
	{
	case BEGIN_STRUCT:
		while (this->read() == ATTRIBUTE)
			this->skipValue();
		return true;
	case BEGIN_ARRAY:
		while (this->skipValue());
		return true;
	case NUMBER:
	case STRING:
		// number or string
		return true;
	default:
		// other
		break;
	}
	return false;
}
*/
void JsonReader::skipValue(Token token)
{
	switch (token)
	{
	case BEGIN_STRUCT:
		while (this->read() == ATTRIBUTE)
			this->skipValue();
		break;
	case BEGIN_ARRAY:
		while ((token = this->read()) != END_ARRAY)
			this->skipValue(token);
		break;
	default:
		// other
		break;
	}
}

} // namespace digi
