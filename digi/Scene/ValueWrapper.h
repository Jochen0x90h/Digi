#ifndef digi_Scene_ValueWrapper_h
#define digi_Scene_ValueWrapper_h

#include <digi/Utility/Standard.h>


namespace digi {

/// @addtogroup Scene
/// @{
	

// treat unknown types (e.g. enum) as a given type
template <typename DataType, typename Type>
struct ValueWrapper
{
	ValueWrapper(Type& value) : value(value) {}
	
	Type& value;
};


template <typename Type>
ValueWrapper<int, Type> wrapInt(Type& x)
{
	return ValueWrapper<int, Type>(x);
}

template <typename Type>
ValueWrapper<uint, Type> wrapUInt(Type& x)
{
	return ValueWrapper<uint, Type>(x);
}


/// @}

} // namespace digi

#endif
