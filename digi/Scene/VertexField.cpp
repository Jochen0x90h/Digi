#include "VertexField.h"


namespace digi {

// VertexField

VertexField::~VertexField()
{
}

template <typename Serializer>
void serialize(Serializer& s, BufferVertexField& vertexField)
{
	s & vertexField.buffer;
	s & wrapUInt(vertexField.semantic);
}

template <typename Serializer>
void serialize(Serializer& s, RefVertexField& vertexField)
{
	s & vertexField.fieldName;
}

Pointer<VertexField> VertexField::load(ObjectReader& r)
{
	// read type id
	uint type;
	r & type;
	
	switch (type)
	{
	case 0:
		{
			BufferVertexField* vertexField = new BufferVertexField();
			VersionReader v(r);
			serialize(r, *vertexField);
			return vertexField;		
		}
	case 1:
		{
			RefVertexField* vertexField = new RefVertexField();
			VersionReader v(r);
			serialize(r, *vertexField);
			return vertexField;		
		}
	}
	return null;
}


// BufferVertexField

BufferVertexField::~BufferVertexField()
{
}

void BufferVertexField::save(ObjectWriter& w)
{
	// write type id
	w & uint(0);

	// write version and members	
	VersionWriter v(w, 1);	
	serialize(w, *this);
}


// RefVertexField

RefVertexField::~RefVertexField()
{
}

void RefVertexField::save(ObjectWriter& w)
{
	// write type id
	w & uint(1);

	// write version and members	
	VersionWriter v(w, 1);	
	serialize(w, *this);
}

} // namespace digi
