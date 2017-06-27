#ifndef digi_Utility_VectorUtility_h
#define digi_Utility_VectorUtility_h

#include <vector>
#include <algorithm>

#include <boost/assign/std/vector.hpp> // vector += element
#include <boost/concept_check.hpp>


namespace digi {

/// @addtogroup Utility
/// @{

/*
/// returns a pointer to the vector data
template <typename V, typename A>
V* getData(std::vector<V, A>& vector)
{
	if (vector.empty())
		return NULL;
	return &vector[0];
}

/// returns a pointer to the vector data
template <typename V, typename A>
const V* getData(const std::vector<V, A>& vector)
{
	if (vector.empty())
		return NULL;
	return &vector[0];
}
*/

/// add a new value to vector and return reference to value in vector
template <typename V, typename A>
V& add(std::vector<V, A>& vector)
{
	return *vector.insert(vector.end(), V());
}

/// add a value to a vector and return reference to the new value in vector
template <typename V, typename A>
V& add(std::vector<V, A>& vector, const V& value)
{
	return *vector.insert(vector.end(), value);
}

/// add a vector to a vector and return reference to first new value in vector
template <typename V, typename A>
void add(std::vector<V, A>& vector, const std::vector<V, A>& value)
{
	vector.insert(vector.end(), value.begin(), value.end());
}


// vector += element
using namespace boost::assign;
/*

// helper for operator += of containers
template <typename Container>
struct ContainerInserter
{
	typedef typename Container::value_type V;
	typedef typename Container::allocator_type A;

	Container& container;

	ContainerInserter(Container& container)
		: container(container)
	{}

	void operator()(const V& value) 
	{
		this->container.push_back(value);
	}

	void operator()(const std::vector<V, A>& vector) 
	{
		this->container.insert(this->container.end(), vector.begin(), vector.end());
	}

	void operator()(const std::list<V, A>& list) 
	{
		this->container.insert(this->container.end(), list.begin(), list.end());
	}
};

/// operator += for vector, see boost::assign.
/// can append to a vector using v += a, b, c; where a, b, c can be values, lists or vectors.
template <class V, class A, class Value2>
boost::assign::list_inserter<ContainerInserter<std::vector<V, A> >, V>
	operator +=(std::vector<V, A>& vector, const Value2& value)
{
	return boost::assign::make_list_inserter(ContainerInserter<std::vector<V, A> >(vector))(value);
}
*/

/// find a value in a constant vector. convenience for std::find
template <typename V, typename A>
typename std::vector<V, A>::const_iterator find(const std::vector<V, A>& vector,
	const typename std::vector<V, A>::value_type& value)
{
	return std::find(vector.begin(), vector.end(), value);
}

/// find a value in a vector. convenience for std::find
template <typename V, typename A>
typename std::vector<V, A>::iterator find(std::vector<V, A>& vector,
	const typename std::vector<V, A>::value_type& value)
{
	return std::find(vector.begin(), vector.end(), value);
}

/// checks if a vector contains a given value
template <typename V, typename A>
bool contains(const std::vector<V, A>& vector, const typename std::vector<V, A>::value_type& value)
{
	return std::find(vector.begin(), vector.end(), value) != vector.end();
}

/// fill the vector with an value
template <typename V, typename A>
void fill(std::vector<V, A>& vector, const typename std::vector<V, A>::value_type& value)
{
	typename std::vector<V, A>::iterator it = vector.begin();
	typename std::vector<V, A>::iterator end = vector.end();
	for (; it != end; ++it)
	{
		*it = value;
	}
}

/// sort the vector
template <typename V, typename A>
void sort(std::vector<V, A>& vector)
{
	std::sort(vector.begin(), vector.end());
}

/// sort the vector using the given compare functor
template <typename V, typename A, typename C>
void sort(std::vector<V, A>& vector, C compare)
{
	std::sort(vector.begin(), vector.end(), compare);
}

} // namespace digi

#endif
