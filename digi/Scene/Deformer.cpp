#include <digi/Utility/StringUtility.h>
#include <digi/Utility/VectorUtility.h>

#include "Deformer.h"


namespace digi {


// Deformer

Deformer::Deformer()
{
}

Deformer::~Deformer()
{
}

// serialize

template <typename Serializer>
void serialize(Serializer& s, Deformer& deformer)
{
	s & deformer.name;
	s & deformer.path;
	s & deformer.assignments;

	s & deformer.uniformType;
	s & deformer.inputType;
	s & deformer.outputType;
	s & deformer.code;
}

Pointer<Deformer> Deformer::load(ObjectReader& r)
{
	Pointer<Deformer> deformer = new Deformer();
	VersionReader v(r);
	serialize(r, *deformer);
	return deformer;
}

void Deformer::save(ObjectWriter& w)
{
	VersionWriter v(w, 1);
	serialize(w, *this);
}


} // namespace digi
