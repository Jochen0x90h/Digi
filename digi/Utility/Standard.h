#undef uint

#ifndef digi_Utility_Standard_h
#define digi_Utility_Standard_h

#include <boost/none.hpp>


/**
	this contains some standard stuff:
	
	ushort, uint
	null constant for boost::optional and Pointer
*/

namespace digi {

/// @addtogroup Utility
/// @{

typedef unsigned short ushort;
typedef unsigned int uint;

/// null
typedef boost::none_t NullType;
const NullType null = boost::none;

/// @}

} // namespace digi

#endif
