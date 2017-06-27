#ifndef digi_Utility_ArrayRef_h
#define digi_Utility_ArrayRef_h

#include <assert.h>
#include <vector>

#include "Standard.h"


namespace digi {

/// @addtogroup Utility
/// @{

template <typename Type>
class ArrayRef
{
public:
	typedef Type value_type;
	typedef Type* iterator;
	typedef Type* const_iterator;

	/// constructs empty ArrayRef
	ArrayRef()
		: d(NULL), s(0) {}

	/// constructs empty ArrayRef (e.g. ArrayRef(null))
	ArrayRef(NullType)
		: d(NULL), s(0) {}

	/// constructs ArrayRef from data and length. data must not be NULL
	ArrayRef (Type* data, size_t length)
		: d(data), s(length) {}

	/// constructs ArrayRef from c-array
	template <size_t length> 
	ArrayRef(Type (&data)[length])
		: d(data), s(length) {}

	/// constructs ArrayRef from std::vector
	ArrayRef(std::vector<Type>& data)
		: d(data.data()), s(data.size()) {}

	bool empty() const {return this->s == 0;}
	size_t size() const {return this->s;}
	iterator begin() const {return this->d;}
	iterator end() const {return this->d + this->s;}
	Type* data() const {return this->d;}
	
	Type& operator[](size_t index) const
	{
		assert(index < this->s && "invalid index");
		return this->d[index];
	}

protected:
	Type* d;
	size_t s;
};

template <typename Type>
class ArrayRef <const Type>
{
public:
	typedef Type value_type;
	typedef const Type* iterator;
	typedef const Type* const_iterator;

	/// constructs empty ArrayRef
	ArrayRef()
		: d(NULL), s(0) {}

	/// constructs empty ArrayRef (e.g. ArrayRef(null))
	ArrayRef(NullType)
		: d(NULL), s(0) {}

	/// constructs ArrayRef from data and length. data must not be NULL
	ArrayRef(const Type* data, size_t length)
		: d(data), s(length) {}

	/// constructs ArrayRef from constant c-array
	template <size_t length> 
	ArrayRef(const Type (&data)[length])
		: d(data), s(length) {}

	/// constructs ArrayRef from std::vector
	ArrayRef(const std::vector<Type>& data)
		: d(data.data()), s(data.size()) {}

	bool empty() const {return this->s == 0;}
	size_t size() const {return this->s;}
	iterator begin() const {return this->d;}
	iterator end() const {return this->d + this->s;}
	const Type* data() const {return this->d;}

	const Type& operator[](size_t index) const
	{
		assert(index < this->s && "invalid index");
		return this->d[index];
	}

protected:

	const Type* d;
	size_t s;
};

/// @}

} // namespace digi

#endif
