#ifndef digi_Utility_bitCast_h
#define digi_Utility_bitCast_h

#include <boost/static_assert.hpp>


namespace digi {

/// @addtogroup Utility
/// @{

/// bit cast. can cast float to int on a bit basis, i.e. without numeric interpretation.
/// e.g. as<int>(1.0f) is 0x3f800000
template <typename Target, typename Source>
Target as(const Source& source)
{
	BOOST_STATIC_ASSERT_MSG(sizeof(Source) == sizeof(Target), "as<Type>() can only cast to type with same size");
	return *(const Target*)&source;
}

/// @}

} // namespace digi

#endif
