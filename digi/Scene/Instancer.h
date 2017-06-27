#ifndef digi_Scene_Instancer_h
#define digi_Scene_Instancer_h

#include "Layer.h"
#include "VertexField.h"
#include "Deformer.h"
#include "Shader.h"
#include "Shape.h"
#include "ParticleSystem.h"


namespace digi {

/// @addtogroup Scene
/// @{

class Instancer;

// instance: transform and instancer (e.g. shape instancer). unnamed instances are used by particle instancers
struct Instance
{
	// index of transform (matrix and visibility) to use from array of transforms
	uint transformIndex;
	
	Pointer<Instancer> instancer;
	
	Instance()
		: transformIndex() {}
	Instance(uint transformIndex, Pointer<Instancer> instancer)
		: transformIndex(transformIndex), instancer(instancer) {}
};

// named instance. toplevel instances of shapes and particle systems in a scene are named
struct NamedInstance : public Instance
{
	std::string name;

	// set of render layers that this instance belongs to (instance is rendered if one of these layers is rendered)
	std::set<Pointer<Layer> > layers;

	NamedInstance()
		{}
	NamedInstance(uint transformIndex, Pointer<Instancer> instancer, const std::string& name)
		: Instance(transformIndex, instancer), name(name) {}
	NamedInstance(uint transformIndex, Pointer<Instancer> instancer, const std::string& name,
		const std::set<Pointer<Layer> >& layers)
		: Instance(transformIndex, instancer), name(name), layers(layers) {}
};

// serialize Instance
template <typename Serializer>
Serializer& operator &(Serializer& s, Instance& instance)
{
	s & instance.transformIndex;
	s & instance.instancer;
	return s;
}

// serialize NamedInstance
template <typename Serializer>
Serializer& operator &(Serializer& s, NamedInstance& instance)
{
	s & instance.transformIndex;
	s & instance.instancer;
	s & instance.name;
	s & instance.layers;
	return s;
}


/// base class for instancer. defines how pairs of shader and primitive are to be drawn
class Instancer : public Object
{
public:
	
	virtual ~Instancer();

	
	static Pointer<Instancer> load(ObjectReader& r);
	virtual void save(ObjectWriter& w) = 0;		
};


/// instancer that instances multiple other instancers
class MultiInstancer : public Instancer
{
public:

	virtual ~MultiInstancer();

	virtual void save(ObjectWriter& w);		


	std::vector<Pointer<Instancer> > instancers;
};


/// instancer that renders a shape (mesh or particle system).
/// consists of vertex fields, deformer, shader and shape
class ShapeInstancer : public Instancer
{
public:
/*
	enum Type
	{
		MESH,
		SPRITES,
	};
*/
	ShapeInstancer()
		: boundingBoxIndex() {}			
	ShapeInstancer(uint boundingBoxIndex, Pointer<Shader> shader, Pointer<Shape> shape)
		: boundingBoxIndex(boundingBoxIndex), shader(shader), shape(shape) {}
	ShapeInstancer(uint boundingBoxIndex, Pointer<Deformer> deformer, Pointer<Shader> shader, Pointer<Shape> shape)
		: boundingBoxIndex(boundingBoxIndex), deformer(deformer), shader(shader), shape(shape) {}

	virtual ~ShapeInstancer();

	virtual void save(ObjectWriter& w);		

	// set a vertex field with given buffer and semantic
	void setField(const std::string& name, Pointer<Buffer> buffer, BufferVertexField::Semantic semantic)
	{
		this->fields[name] = new BufferVertexField(buffer, semantic);
	}

	// set a vertex field with reference to given field of shape (e.g. position of particles)
	void setField(const std::string& name, const std::string& fieldName)
	{
		this->fields[name] = new RefVertexField(fieldName);
	}


	// map of field name -> vertex field. deformer and shader access vertex fields by field name
	typedef std::map<std::string, Pointer<VertexField> > FieldMap;
	typedef std::pair<const std::string, Pointer<VertexField> > FieldPair;

	// index of bounding box to use from array of bounding boxes
	uint boundingBoxIndex;
	
	// vertex fields
	FieldMap fields;	
	
	// vertex deformer (may be null if no deformer)
	Pointer<Deformer> deformer;
	
	// shader
	Pointer<Shader> shader;
	
	// shape (Mesh or ParticleSystem)
	Pointer<Shape> shape;
};


/// instancer that instances other instances (e.g. meshes) per particle
class ParticleInstancer : public Instancer
{
public:
	
	virtual ~ParticleInstancer();

	virtual void save(ObjectWriter& w);


	// the particle system
	Pointer<Shape> particleSystem;

	// name of particle instancer (must be unique, used for struct names and debugging)
	std::string name;

	// path of particle instancer in instance.uniforms struct
	std::string path;

	// list of attributes to copy from scene into uniforms struct for this particle instancer
	std::vector<std::string> assignments;


	// uniform input structure
	Pointer<Type> uniformType;

	// calculates matrix and index per particle
	std::string code;

	// each partice can select a shape by index
	std::vector<std::vector<Instance> > instances;
};

/// @}

} // namespace digi

#endif
