#ifndef digi_Scene_Shape_h
#define digi_Scene_Shape_h

#include "ObjectReader.h"
#include "ObjectWriter.h"


namespace digi {

/// @addtogroup Scene
/// @{

class Shape : public Object
{
public:

	virtual ~Shape();
	

	static Pointer<Shape> load(ObjectReader& r);
	virtual void save(ObjectWriter& w) = 0;
};

/// @}

} // namespace digi

#endif
