#include "Object.h"


namespace digi {

// Object

Object::~Object()
{
}

void release(Object* object)
{
	if (object != NULL)
	{
		if (--object->referenceCount == 0)
			delete object;
	}
}

} // namespace digi
