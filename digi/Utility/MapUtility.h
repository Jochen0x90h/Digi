#ifndef digi_Utility_MapUtility_h
#define digi_Utility_MapUtility_h

#include <map>

#include <boost/optional.hpp>

#include "Pointer.h"


namespace digi {

/// @addtogroup Utility
/// @{

/// returns true if the map contains the key
template <typename K, typename V, typename C, typename A>
bool contains(const std::map<K, V, C, A>& map, const typename std::map<K, V, C, A>::value_type::first_type& key)
{
	return map.find(key) != map.end();
}

/// returns true if the map contains the key with given value
template <typename K, typename V, typename C, typename A>
bool contains(const std::map<K, V, C, A>& map,
	const typename std::map<K, V, C, A>::value_type::first_type& key,
	const typename std::map<K, V, C, A>::value_type::second_type& value)
{
	typename std::map<K, V, C, A>::const_iterator it = map.find(key);
	if (it != map.end())
		return it->second == value;
	return false;
}


template <typename V>
struct MapGetValueReturn
{
	typedef boost::optional<V> Type;
};

template <typename V>
struct MapGetValueReturn<V*>
{
	typedef V* Type;
};

template <typename V>
struct MapGetValueReturn<Pointer<V> >
{
	typedef Pointer<V> Type;
};

// get map value. returns the default value if not found
template <typename K, typename V, typename C, typename A>
V getValue(const std::map<K, V, C, A>& map,
	const typename std::map<K, V, C, A>::value_type::first_type& key, const V& defaultValue)
{
	typename std::map<K, V, C, A>::const_iterator it = map.find(key);
	
	// return default value if not found
	if (it == map.end())
		return defaultValue;
		
	return it->second;
}

// get map value. returns Nullable<Type> for types other than Pointer. returns null if not found
template <typename K, typename V, typename C, typename A>
typename MapGetValueReturn<V>::Type getValue(const std::map<K, V, C, A>& map,
	const typename std::map<K, V, C, A>::value_type::first_type& key)
{
	typename std::map<K, V, C, A>::const_iterator it = map.find(key);
	
	// return null or NULL if not found
	if (it == map.end())
		return typename MapGetValueReturn<V>::Type();
		
	return it->second;
}

} // namespace digi

#endif
