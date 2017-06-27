#ifndef digi_Utility_ArrayFunctions_h
#define digi_Utility_ArrayFunctions_h

#include <stddef.h>
#include <algorithm>


namespace digi {

/// @addtogroup Utility
/// @{

/// fills a c-array with given value (dependent type for value used to prevent template matching for value)
template <typename Type, size_t length> 
void fill(Type (& array)[length], const typename std::pair<Type, int>::first_type& value)
{ 
	for (size_t i = 0; i < length; ++i)
		array[i] = value;
}

/// sorts a c-array
template <typename Type, size_t length>
void sort(Type (& array)[length])
{
	std::sort(array, array + length);
}

/// sorts a c-array with given less operator
template <typename Type, size_t length, typename Less>
void sort(Type (& array)[length], Less& less)
{
	std::sort(array, array + length, less);
}

} // namespace digi

#endif
