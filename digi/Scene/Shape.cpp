#include "Mesh.h"
#include "ParticleSystem.h"
#include "Shape.h"


namespace digi {

// Mesh
template <typename Serializer>
void serialize(Serializer& s, Mesh& mesh)
{
	s & mesh.renderMode;
}

// ConstantMesh
template <typename Serializer>
void serialize(Serializer& s, ConstantMesh& constantMesh)
{
	serialize(s, (Mesh&)constantMesh);
	//s & wrapUInt(constantMesh.type);
	s & constantMesh.indices;
}

// TextMesh
template <typename Serializer>
void serialize(Serializer& s, TextMesh& textMesh)
{
	serialize(s, (Mesh&)textMesh);
	s & textMesh.name;
	//s & textMesh.path;
	s & textMesh.maxNumSymbols;	
}

// ParticleSystem::Field
template <typename Serializer>
Serializer& operator &(Serializer& s, ParticleSystem::Field& field)
{
	s & field.name;
	s & field.buffer;
	return s;
}

// ParticleSystem
template <typename Serializer>
void serialize(Serializer& s, ParticleSystem& particleSystem)
{
	s & particleSystem.name;
	s & particleSystem.path;
	s & particleSystem.emitterAssignments;
	s & particleSystem.assignments;
	
	s & particleSystem.emitterUniformType;
	s & particleSystem.emitterCode;	
	s & particleSystem.maxNumParticles;
	
	s & particleSystem.uniformType;
	s & particleSystem.particleType;
	s & particleSystem.initialState;
	s & particleSystem.createCode;
	s & particleSystem.updateCode;
}


// Shape

Shape::~Shape()
{
}

Pointer<Shape> Shape::load(ObjectReader& r)
{
	// read type id
	uint type;
	r & type;
	
	switch (type)
	{
	case 0:
		{
			Pointer<ConstantMesh> constantMesh = new ConstantMesh();
			VersionReader v(r);
			serialize(r, *constantMesh);
			return constantMesh;
		}
	case 1:
		{
			Pointer<ParticleSystem> particleSystem = new ParticleSystem();
			VersionReader v(r);
			serialize(r, *particleSystem);
			return particleSystem;
		}
	case 2:
		{
			Pointer<TextMesh> textMesh = new TextMesh();
			VersionReader v(r);
			serialize(r, *textMesh);
			return textMesh;
		}
	}
	return null;
}


// ConstantMesh

void ConstantMesh::save(ObjectWriter& w)
{
	// write type id
	w & uint(0);

	VersionWriter v(w, 1);
	serialize(w, *this);
}


// ParticleSystem

void ParticleSystem::save(ObjectWriter& w)
{
	// write type id
	w & uint(1);

	VersionWriter v(w, 1);
	serialize(w, *this);
}


// TextMesh

void TextMesh::save(ObjectWriter& w)
{
	// write type id
	w & uint(2);

	VersionWriter v(w, 1);
	serialize(w, *this);
}

} // namespace digi
