#include "Shape.h"
#include "Instancer.h"


namespace digi {

// serializers for Instancer and Shape

// MultiInstancer
template <typename Serializer>
void serialize(Serializer& s, MultiInstancer& instancer)
{
	s & instancer.instancers;
}

// ShapeInstancer
template <typename Serializer>
void serialize(Serializer& s, ShapeInstancer& instancer)
{
	s & instancer.boundingBoxIndex;
	s & instancer.fields;
	s & instancer.deformer;
	s & instancer.shader;
	s & instancer.shape;
}

// ParticleInstancer
template <typename Serializer>
void serialize(Serializer& s, ParticleInstancer& instancer)
{
	s & instancer.particleSystem;
	s & instancer.name;
	s & instancer.path;
	s & instancer.assignments;
	
	s & instancer.uniformType;
	s & instancer.code;
	s & instancer.instances;
}


// Instancer

Instancer::~Instancer()
{
}

Pointer<Instancer> Instancer::load(ObjectReader& r)
{
	// read type id
	uint type;
	r & type;
	
	switch (type)
	{
	case 0:
		{
			// instancer that instances multiple other instancers
			MultiInstancer* instancer = new MultiInstancer();
			VersionReader v(r);
			serialize(r, *instancer);
			return instancer;
		}
	case 1:
		{
			// instancer that renders a shape (mesh or particle system)
			ShapeInstancer* instancer = new ShapeInstancer();
			VersionReader v(r);
			serialize(r, *instancer);
			return instancer;
		}
	case 2:
		{
			// instancer that instances other instances (e.g. meshes) per particle
			ParticleInstancer* instancer = new ParticleInstancer();
			VersionReader v(r);
			serialize(r, *instancer);
			return instancer;
		}
	}
	return null;
}


// MultiInstancer

MultiInstancer::~MultiInstancer()
{
}

void MultiInstancer::save(ObjectWriter& w)
{
	// write type id
	w & uint(0);

	// write version and members	
	VersionWriter v(w, 1);	
	serialize(w, *this);
}


// ShapeInstancer

ShapeInstancer::~ShapeInstancer()
{
}

void ShapeInstancer::save(ObjectWriter& w)
{
	// write type id
	w & uint(1);

	// write version and members	
	VersionWriter v(w, 1);	
	serialize(w, *this);
}


// ParticleInstancer

ParticleInstancer::~ParticleInstancer()
{
}

void ParticleInstancer::save(ObjectWriter& w)
{
	// write type id
	w & uint(2);

	// write version and members	
	VersionWriter v(w, 1);	
	serialize(w, *this);
}

} // namespace digi
