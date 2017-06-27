#include "All.h"

// helpers for google test
// EXPECT_VECTOR_EQ: expect a vector value
// EXPECT_EPSILON_EQ: expect a value within an epsilon

#define EXPECT_VECTOR_EQ(expected, actual) \
	EXPECT_EQ(makeVectorEquals(expected), actual)

template <typename Type>
struct VectorEquals
{
	VectorEquals(const Type& value)
		: value(value) {}
	
	template <typename Type2>
	bool operator ==(const Type2& other) const
	{
		return digi::all(this->value == other);
	}
	
	Type value;
};

template <typename Type>
inline std::ostream& operator <<(std::ostream& w, const VectorEquals<Type>& v)
{
	w << v.value;
	return w;
}

template <typename Type>
VectorEquals<Type> makeVectorEquals(const Type& type) {return VectorEquals<Type>(type);}


#define EXPECT_EPSILON_EQ(expected, actual) \
	EXPECT_EQ(makeEpsilonEquals(expected), actual)

template <typename Type>
struct EpsilonEquals
{
	EpsilonEquals(const Type& value)
		: value(value) {}
	
	bool operator ==(const Type& other) const
	{
		return digi::epsilonEquals(this->value, other, 0.00001f);
	}
	
	Type value;
};

template <typename Type>
inline std::ostream& operator <<(std::ostream& w, const EpsilonEquals<Type>& v)
{
	w << v.value;
	return w;
}

template <typename Type>
EpsilonEquals<Type> makeEpsilonEquals(const Type& type) {return EpsilonEquals<Type>(type);}
