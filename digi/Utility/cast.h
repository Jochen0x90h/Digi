#ifndef digi_Utility_cast_h
#define digi_Utility_cast_h

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
	struct Caster
	{
		static boost::optional<Target> cast(const Source& source)
		{
			return (Target)source;
		}
	};
	
	// string cast to bool, use "false" and "true"
	template <typename Source>
	struct Caster<bool, Source>
	{
		template <typename InputIterator>
		static bool helper(boost::iterator_range<InputIterator> source)
		{
			InputIterator it = source.begin();
			InputIterator end = source.end();
			if (it == end)
				return 
				
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
		
		static boost::optional<bool> cast(const Source& source)
		{
			return helper(boost::as_literal(source));
		}
	};

	// cast bool to string, use "false" and "true"
	template <>
	struct Caster<std::string, bool>
	{
		static boost::optional<std::string> cast(bool source)
		{
			return source ? "true" : "false";
		}
	};
	
	// cast bool to wstring
	template <>
	struct Caster<std::wstring, bool>
	{
		static boost::optional<std::wstring> cast(bool source)
		{
			return source ? L"true" : L"false";
		}
	};

	// cast to int8_t, treat as number
	template <typename Source>
	struct Caster<int8_t, Source>
	{
		static boost::optional<int8_t> cast(const Source& source)
		{
			int32_t v = cast<int32_t>(source);
			if (uint32_t(v + 128) > 255)
				throw boost::bad_lexical_cast(typeid(Source), typeid(int8_t));
			return int8_t(v);
		}
	};

	// cast from int8_t, treat as number
	template <typename Target>
	struct Caster<Target, int8_t>
	{
		static boost::optional<Target> cast(int8_t source)
		{			
			return cast<Target>(int(source));
		}
	};

	// cast to uint8_t, treat as number
	template <typename Source>
	struct Caster<uint8_t, Source>
	{
		static boost::optional<uint8_t> cast(const Source& source)
		{
			uint32_t v = cast<uint32_t>(source);
			if (v > 255)
				return null;
			return uint8_t(v);
		}
	};
	
	// cast from uint8_t, treat as number
	template <typename Target>
	struct Caster<Target, uint8_t>
	{
		static boost::optional<Target> cast(uint8_t source)
		{
			return cast<Target>(int(source));
		}
	};
}

// universal cast. Returns null if the cast fails. uses "true" and "false" for bool
// and treats int8_t and uint8_t as numbers instead of characters
template <typename Target, typename Source>
boost::optional<Target> cast(const Source& source)
{
	return detail::Caster<Target, Source>::cast(source);
}

template <typename Target, typename Source>
boost::optional<Target> cast(const boost::optional<Source>& source)
{
	if (source == null)
		return null;
	return detail::Caster<Target, Source>::cast(*source);
}

template <typename Target, typename Source>
Target as(const boost::optional<Source>& source)
{
	return *cast<Target>(source);
}

/// @}

} // namespace digi

#endif
