#ifndef digi_Scene_Layer_h
#define digi_Scene_Layer_h

#include "ObjectReader.h"
#include "ObjectWriter.h"


namespace digi {

/// @addtogroup Scene
/// @{

class Layer : public Object
{
public:

	Layer()
		/*: cullMode()*/ {}

	Layer(const std::string& name)
		: name(name)/*, cullMode()*/ {}

	virtual ~Layer();

	std::string name;

	// override culling mode
	//RenderState::CullMode cullMode;

	static Pointer<Layer> load(ObjectReader& r);
	void save(ObjectWriter& w);
};	

/// @}

} // namespace digi

#endif
