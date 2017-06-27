#ifndef digi_Utility_ListUtility_h
#define digi_Utility_ListUtility_h

#include <list>

#include <boost/assign/std/list.hpp> // vector += element


namespace digi {

/// @addtogroup Utility
/// @{

/// add a new value to list and return reference to value in list
template <typename V, typename A>
V& add(std::list<V, A>& list)
{
	return *list.insert(list.end(), V());
}

/// add a new value to list and return reference to value in list
template <typename V, typename A>
V& add(std::list<V, A>& list, const V& value)
{
	return *list.insert(list.end(), value);
}

// list += element
using namespace boost::assign;

/*
/// operator += for list, see boost::assign.
/// can append to a list using v += a, b, c; where a, b, c can be values, lists or vectors.
template <class V, class A, class Value2>
inline boost::assign::list_inserter<ContainerInserter<std::list<V, A> >, V>
	operator +=(std::list<V, A>& container, const Value2& value)
{
	return boost::assign::make_list_inserter(ContainerInserter<std::list<V, A> >(container))(value);
}
*/

} // namespace digi

#endif
