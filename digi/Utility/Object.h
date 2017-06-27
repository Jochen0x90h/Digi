#ifndef digi_Utility_Object_h
#define digi_Utility_Object_h

#include <boost/detail/atomic_count.hpp>

#include "Pointer.h"


namespace digi {

/// @addtogroup Utility
/// @{


/// base class for reference counted objects
class Object
{
	friend inline void addReference(Object* object);
	friend void release(Object* object);

public:

	/// standard constructor
	Object() : referenceCount(0) {}

	/// destructor
	virtual ~Object();
	
private:
	
	Object(const Object&);
	Object& operator =(const Object&);
	
	boost::detail::atomic_count referenceCount;
};

inline void addReference(Object* object)
{
	if (object != NULL)
		++object->referenceCount;
}

void release(Object* object);

/// @}

} // namespace digi

#endif
