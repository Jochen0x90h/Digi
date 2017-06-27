#ifndef digi_Utility_ScopedAssign_h
#define digi_Utility_ScopedAssign_h

#include <digi/Base/Platform.h>


namespace digi {

/// @addtogroup Utility
/// @{


template <class Type>
class ScopedAssign
{
	public:

		ScopedAssign(Type& value, Type newValue) : value(value)
		{
			this->oldValue = value;
			value = newValue;
		}

		~ScopedAssign()
		{
			value = this->oldValue;
		}
		
	protected:
	
		Type& value;
		Type oldValue;
};

/// @}

} // namespace digi

#endif
