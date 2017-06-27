/*
	transcode between UTF-8, UTF-16 and UTF-32
*/

#ifndef digi_Utility_UtfTranscode_h
#define digi_Utility_UtfTranscode_h

#include <vector>

#include <boost/range.hpp>
#include <boost/range/as_literal.hpp>

#include <digi/Base/Platform.h>
#include <digi/Utility/StringUtility.h>
#include <digi/Utility/ArrayUtility.h>

/*
	UTF-8 encoding scheme:

	0x00000000 - 0x0000007F:
		0xxxxxxx

	0x00000080 - 0x000007FF:
		110xxxxx 10xxxxxx

	0x00000800 - 0x0000FFFF:
		1110xxxx 10xxxxxx 10xxxxxx

	0x00010000 - 0x001FFFFF:
		11110xxx 10xxxxxx 10xxxxxx 10xxxxxx

	0x00200000 - 0x03FFFFFF:
		111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx

	0x04000000 - 0x7FFFFFFF:
		1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx


	UTF-16 encoding scheme:

	0x00000000 - 0x0000FFFF:
	xxxxxxxx xxxxxxxx

	0x00010000 - 0x000FFFFF:
	110110xx xxxxxxxx  110111xx xxxxxxxx
*/


namespace digi {

/// @addtogroup InputOutput
/// @{

/// encode UTF-32 to UTF-x
template <size_t>
struct UtfEncoder
{
};

/// encode UTF-32 to UTF-32 (no-op)
template <>
struct UtfEncoder<4>
{
	template <typename OutputIterator>
	void operator ()(OutputIterator& it, int32_t ch)
	{
		*it++ = ch;
	}
};

/// encode UTF-32 to UTF-16
template <>
struct UtfEncoder<2>
{
	template <typename OutputIterator>
	void operator ()(OutputIterator& it, int32_t ch)
	{
		if (ch < 0x00010000)
		{
			*it++ = ch;
		}
		else
		{
			*it++ = 0xD800 | (ch >> 10 & 0x3Ff);
			*it++ = 0xDC00 | (ch & 0x3Ff);
		}
	}
};

/// encode UTF-32 to UTF-8
template <>
struct UtfEncoder<1>
{
	template <typename OutputIterator>
	void operator ()(OutputIterator& it, int32_t ch)
	{
		if (ch < 0x00000080)
		{
			*it++ = ch;
		}
		else
		{
			if (ch < 0x00000800)
			{
				*it++ = 0xC0 | ((ch >> 6) & 0x1F);
			}
			else
			{
				if (ch < 0x00010000)
				{
					*it++ = 0xE0 | ((ch >> 12) & 0x0F);
				}
				else
				{
					if (ch < 0x00200000)
					{
						*it++ = 0xF0 | ((ch >> 18) & 0x07);
					}
					else
					{
						if (ch < 0x04000000)
						{
							*it++ = 0xF8 | ((ch >> 24) & 0x03);
						}
						else
						{
							*it++ = 0xFC | ((ch >> 30) & 0x01);
							*it++ = 0x80 | ((ch >> 24) & 0x3F);
						}
						*it++ = 0x80 | ((ch >> 18) & 0x3F);
					}
					*it++ = 0x80 | ((ch >> 12) & 0x3F);
				}				
				*it++ = 0x80 | ((ch >> 6) & 0x3F);
			}
			*it++ = 0x80 | (ch & 0x3F);
		}
	}
};


/// decode UTF-x to UTF-32
template <size_t>
struct UtfDecoder
{
};

/// decode UTF-32 to UTF-32 (no-op)
template <>
struct UtfDecoder<4>
{
	template <typename InputIterator>
	int32_t operator ()(InputIterator& it)
	{
		return *it++;
	}
};

/// decode UTF-16 to UTF-32 
template <>
struct UtfDecoder<2>
{
	template <typename InputIterator>
	int32_t operator ()(InputIterator& it)
	{
		if ((*it & 0xFC00) == 0xD800)
		{
			int32_t ch = ((*it++ & 0x3FF) << 10);
			ch |= *it++ & 0x3FF;
			return ch;
		}
		return *it++;
	}
};

/// decode UTF-8 to UTF-32 
template <>
struct UtfDecoder<1>
{
	template <typename InputIterator>
	int32_t operator ()(InputIterator& it)
	{
		int32_t ch;
		if ((*it & 0x80) == 0)
		{
			ch = *it++ & 0x7F;
		}
		else
		{
			if ((*it & 0xE0) == 0xC0)
			{
				ch = (*it++ & 0x1F) << 6;
			}
			else
			{
				if ((*it & 0xF0) == 0xE0)
				{
					ch = (*it++ & 0x0F) << 12;
				}
				else
				{
					if ((*it & 0xF8) == 0xF0)
					{
						ch = (*it++ & 0x07) << 18;
					}
					else
					{
						if ((*it & 0xFC) == 0xF8)
						{
							ch = (*it++ & 0x03) << 24;
						}
						else
						{
							ch = (*it++ & 0x01) << 30;
							if ((*it & 0xC0) == 0x80)
								ch |= (*it++ & 0x3F) << 24;
						}
						if ((*it & 0xC0) == 0x80)
							ch |= (*it++ & 0x3F) << 18;
					}
					if ((*it & 0xC0) == 0x80)
						ch |= (*it++ & 0x3F) << 12;
				}				
				if ((*it & 0xC0) == 0x80)
					ch |= (*it++ & 0x3F) << 6;
			}
			if ((*it & 0xC0) == 0x80)
				ch |= *it++ & 0x3F;
		}
		return ch;
	}
};


/// transcode UTF-x iterators to UTF-x string
template <typename Output, typename InputIterator>
void utfTranscode(InputIterator inputIt, InputIterator inputEnd, Output& output)
{
	typedef typename Output::value_type OutputChar;
	
	const int bufferSize = 128;
	OutputChar buffer[bufferSize + 8];
	
	OutputChar* outputIt = buffer;
	OutputChar* outputEnd = buffer + bufferSize;

	UtfDecoder<sizeof(typename std::iterator_traits<InputIterator>::value_type)> decode;
	UtfEncoder<sizeof(OutputChar)> encode;
	
	while (inputIt < inputEnd)
	{
		// transcode one character
		int32_t ch = decode(inputIt);
		encode(outputIt, ch);
		
		// check if buffer is full
		if (outputIt >= outputEnd)
		{
			output.insert(output.end(), buffer, outputIt);
			outputIt = buffer;
		}		
	}

	output.insert(output.end(), buffer, outputIt);
}


/// transcode UTF-x string to UTF-x string in the form transcode(input, output)
template <typename Output, typename InputIterator>
void utfTranscode(boost::iterator_range<InputIterator> input, Output& output)
{
	utfTranscode(input.begin(), input.end(), output);
}

template <typename Output, typename Input>
void utfTranscode(const Input& input, Output& output)
{
	utfTranscode(boost::as_literal(input), output);
}


/// transcode UTF-x string to UTF-x string in the form output = utfString<OutputChar>(input)
template <typename OutputChar, typename Input>
std::basic_string<OutputChar, std::char_traits<OutputChar>, std::allocator<OutputChar> > utfString(const Input& input)
{
	std::basic_string<OutputChar, std::char_traits<OutputChar>, std::allocator<OutputChar> > output;
	utfTranscode(input, output);
	return output;
}

/// transcode UTF-x string to UTF-x vector in the form output = utfVector<OutputChar>(input)
template <typename OutputChar, typename Input>
std::vector<OutputChar, std::allocator<OutputChar> > utfVector(const Input& input)
{
	std::vector<OutputChar, std::allocator<OutputChar> > output;
	utfTranscode(input, output);
	return output;
}

/// @}

} // namespace digi

#endif
