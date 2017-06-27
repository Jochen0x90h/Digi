#ifndef digi_Data_findIndex_h
#define digi_Data_findIndex_h

#include <string>
#include <boost/lexical_cast.hpp>


namespace digi {

/// @addtogroup Data
/// @{

// find index of key in sorted list of strings. returns size if key is not found
template <typename Char, typename String>
size_t findIndex(const Char** list, size_t size, const String& key)
{
	size_t low = 0;
	size_t high = size - 1;
	while (true)
	{
		size_t mid = (low + high) >> 1;
		const Char** it = list + mid;
		if (low >= high)
			return *it == key ? mid : size;
		if (!(*it < key))
			high = mid;
		else
			low = mid + 1;
	}
}


template <typename Name, typename Value>
struct EnumNameValue
{
	Name name;
	Value value;
};


// convert enum from name (string/number) to enum value using a list that is sorted by name.
// throws boost::bad_lexical_cast if enum value not found
template <typename Name, typename Value, size_t size, typename Key>
Value enumCast(const EnumNameValue<Name, Value> (&list)[size], const Key& key)
{
	size_t low = 0;
	size_t high = size - 1;
	while (true)
	{
		size_t mid = (low + high) >> 1;
		const EnumNameValue<Name, Value>* it = list + mid;
		if (low >= high)
		{
			if (it->name == key)
				return it->value;

			// throw bad_lexical_cast from std::string to the enum type
			throw boost::bad_lexical_cast(typeid(Key), typeid(Value));
		}
		if (!(it->name < key))
			high = mid;
		else
			low = mid + 1;
	}
}

/// @}

} // namespace digi

#endif
