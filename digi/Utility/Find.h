#ifndef digi_Utility_FindFunctions_h
#define digi_Utility_FindFunctions_h

#include <functional> // less

#include <digi/Base/Platform.h>


namespace digi {

/// @addtogroup Utility
/// @{

/**
	find using a custom compare function
*/
template <typename Iterator, typename Value, typename Compare>
Iterator find(Iterator begin, Iterator end, const Value& value, Compare compare)
{
	for (Iterator it = begin; it != end; ++it)
	{
		if (compare(*it, value))
			return it;
	}
	return end;
}


// binary search functions


/**
	does a binary search in the range [begin, end) and returns an iterator to
	the smallest value that is greater than or equal to \a value
	value = 4: [ 1  2  2 [4] 4  4  8  9  end ]
	value = 5: [ 1  2  2  4  4  4 [8] 9  end ]
*/
template <typename Iterator, typename Value, typename Compare>
Iterator binaryLowerBound(Iterator begin, Iterator end, const Value& value, Compare comp)
{
	int low = 0;
	int high = int(end - begin);

	while (true)
	{
		int mid = (low + high) >> 1;
		Iterator it = begin + mid;
		if (low >= high)
			return it;

		if (!comp(*it, value))
			high = mid;
		else
			low = mid + 1;
	}
}

/**
	overloaded binaryLowerBound using Less<Value> as compare function
*/
template <typename Iterator, typename Value>
inline Iterator binaryLowerBound(Iterator begin, Iterator end, const Value& value)
{
	return binaryLowerBound(begin, end, value, std::less<Value>());
}


/**
	does a binary search in the range [begin, end) and returns an iterator to
	the smallest value that is greater than \a value
	value = 4: [ 1  2  2  4  4  4 [8] 9  end ]
	value = 5: [ 1  2  2  4  4  4 [8] 9  end ]
*/
template <typename Iterator, typename Value, typename Compare>
Iterator binaryUpperBound(Iterator begin, Iterator end, const Value& value, Compare comp)
{
	int low = 0;
	int high = int(end - begin);

	while (true)
	{
		int mid = (low + high) >> 1;
		Iterator it = begin + mid;
		if (low >= high)
			return it;

		if (comp(value, *it))
			high = mid;
		else
			low = mid + 1;
	}
}

/**
	overloaded binaryUpperBound using Less<Value> as compare function
*/
template <typename Iterator, typename Value>
inline Iterator binaryUpperBound(Iterator begin, Iterator end, const Value& value)
{
	return binaryUpperBound(begin, end, value, std::less<Value>());
}


/**
	returns a pair of binaryLowerBound and binaryUpperBound
*/
template <typename Iterator, typename Value, typename Compare>
inline std::pair<Iterator, Iterator> binaryEqualRange(Iterator begin,
	Iterator end, const Value& value, Compare comp)
{
	return std::pair<Iterator, Iterator>(
		binaryLowerBound(begin, end, value, comp),
		binaryUpperBound(begin, end, value, comp));
}

/**
	returns a pair of binaryLowerBound and binaryUpperBound
*/
template <typename Iterator, typename Value>
inline std::pair<Iterator, Iterator> binaryEqualRange(Iterator begin,
	Iterator end, const Value& value)
{
	return binaryEqualRange(begin, end, value, std::less<Value>());
}


/**
	tries to find \a value or returns end if not found
*/
template <typename Iterator, typename Value, typename Compare>
inline Iterator binaryFind(Iterator begin, Iterator end,
	const Value& value, Compare comp)
{
	Iterator it = binaryLowerBound(begin, end, value, comp);
	if (it != end && comp(value, *it))
		it = end;
	return it;
}

/**
	tries to find \a value or returns end if not found
*/
template <typename Iterator, typename Value>
Iterator binaryFind(Iterator begin, Iterator end,
	const Value& value)
{
	return binaryFind(begin, end, value, std::less<Value>());
}


/**
	returns true if \a value is found
*/
template <typename Iterator, typename Value, typename Compare>
bool binaryContains(Iterator begin, Iterator end,
	const Value& value, Compare comp)
{
	Iterator it = binaryLowerBound(begin, end, value, comp);
	if (it != end && comp(value, *it))
		return false;
	return true;
}

/**
	returns true if \a value is found
*/
template <typename Iterator, typename Value>
bool binaryContains(Iterator begin, Iterator end,
	const Value& value)
{
	return binaryContains(begin, end, value, std::less<Value>());
}

/// @}

} // namespace digi

#endif
