#ifndef digi_Utility_lexicalCast_h
#define digi_Utility_lexicalCast_h

#ifndef HAVE_LOCALE
	#define BOOST_NO_STD_LOCALE
	#define BOOST_LEXICAL_CAST_ASSUME_C_LOCALE
#endif

#include <boost/lexical_cast.hpp>
#include <boost/range.hpp>
#include <boost/range/as_literal.hpp>

#include <digi/Base/Platform.h>


namespace digi {

/// @addtogroup Utility
/// @{

namespace detail
{
	template <typename Target, typename Source>
	struct LexicalCaster
	{
		static Target cast(const Source& source)
		{
			return boost::lexical_cast<Target>(source);
		}
	};
	
	// use "true" and "false" for bool
	template <typename Source>
	struct LexicalCaster<bool, Source>
	{
		template <typename InputIterator>
		static bool helper(boost::iterator_range<InputIterator> source)
		{
			bool isFalse = true;
			bool isTrue = true;
			int i = 0;
			InputIterator it = source.begin();
			InputIterator end = source.end();
			for (; i < 5 && it != end; ++i, ++it)
			{
				isFalse &= *it == "false"[i];
				isTrue &= *it == "true"[i];
			}
			if (it == end)
			{
				if (isFalse && i == 5)
					return false;
				if (isTrue && i == 4)
					return true;
			}
			return boost::lexical_cast<bool>(source);
		}
		
		static bool cast(const Source& source)
		{
			return helper(boost::as_literal(source));
		}
	};

	template <>
	struct LexicalCaster<std::string, bool>
	{
		static std::string cast(bool source)
		{
			return source ? "true" : "false";
		}
	};
	
	template <>
	struct LexicalCaster<std::wstring, bool>
	{
		static std::wstring cast(bool source)
		{
			return source ? L"true" : L"false";
		}
	};

	// treat int8_t as number
	template <typename Source>
	struct LexicalCaster<int8_t, Source>
	{
		static int8_t cast(const Source& source)
		{
			int v = boost::lexical_cast<int>(source);
			if (uint32_t(v + 128) > 255)
				throw boost::bad_lexical_cast(typeid(Source), typeid(int8_t));
			return int8_t(v);
		}
	};

	template <typename Target>
	struct LexicalCaster<Target, int8_t>
	{
		static Target cast(int8_t source)
		{			
			return boost::lexical_cast<Target>(int(source));
		}
	};

	// treat uint8_t as number
	template <typename Source>
	struct LexicalCaster<uint8_t, Source>
	{
		static uint8_t cast(const Source& source)
		{
			int v = boost::lexical_cast<int>(source);
			if (uint32_t(v) > 255)
				throw boost::bad_lexical_cast(typeid(Source), typeid(uint8_t));
			return int8_t(v);
		}
	};
	
	template <typename Target>
	struct LexicalCaster<Target, uint8_t>
	{
		static Target cast(uint8_t source)
		{
			return boost::lexical_cast<Target>(int(source));
		}
	};
}

// lexical cast. uses boost::lexical_cast with some modifications: uses "true" and "false" for bool
// and treats int8_t and uint8_t as numbers
template <typename Target, typename Source>
Target lexicalCast(const Source& source)
{
	return detail::LexicalCaster<Target, Source>::cast(source);
}

/// @}

} // namespace digi

#endif
