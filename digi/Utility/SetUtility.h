#ifndef digi_Utility_SetUtility_h
#define digi_Utility_SetUtility_h

#include <set>

#include <digi/Base/Platform.h>


namespace digi {

/// @addtogroup Utility
/// @{

/// returns size of the set
template <typename V, typename C, typename A>
int getSize(const std::set<V, C, A>& set)
{
	return set.size();
}
	
/// returns true if the set is empty
template <typename V, typename C, typename A>
bool isEmpty(const std::set<V, C, A>& set)
{
	return set.empty();
}

/// returns true if the set contains the element
template <typename V, typename C, typename A>
bool contains(const std::set<V, C, A>& set, const typename std::set<V, C, A>::value_type& value)
{
	return set.find(value) != set.end();
}

} // namespace digi

#endif
