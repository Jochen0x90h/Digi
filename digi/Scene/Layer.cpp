#include "Layer.h"


namespace digi {

// Layer

Layer::~Layer()
{
}

// serialize

template <typename Serializer>
void serialize(Serializer& s, Layer& layer)
{
	s & layer.name;
	//s & wrapUInt(layer.cullMode);
}

Pointer<Layer> Layer::load(ObjectReader& r)
{
	Pointer<Layer> layer = new Layer();
	VersionReader v(r);	
	serialize(r, *layer);
	return layer;
}

void Layer::save(ObjectWriter& w)
{
	// write version and members	
	VersionWriter v(w, 1);	
	serialize(w, *this);
}

} // namespace digi
