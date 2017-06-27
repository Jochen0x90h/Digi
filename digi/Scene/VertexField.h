#ifndef digi_Scene_VertexField_h
#define digi_Scene_VertexField_h

#include <digi/Image/Buffer.h>

#include "ValueWrapper.h"
#include "ObjectReader.h"
#include "ObjectWriter.h"


namespace digi {

/// @addtogroup Scene
/// @{

class VertexField : public Object
{
public:
	
	virtual ~VertexField();

	static Pointer<VertexField> load(ObjectReader& r);
	virtual void save(ObjectWriter& w) = 0;		
};

// vertex field with vertex data stored in a constant buffer
class BufferVertexField : public VertexField
{
public:

	// semantic of vertex fields. only used for optimizations like reducing bit resolution
	enum Semantic
	{
		OTHER,
		POSITION,
		TANGENT, // tangent/bitangent/normal
		TEXCOORD,
		COLOR,
		WEIGHT,
		INDEX
	};

	BufferVertexField()
		: semantic() {}
	
	BufferVertexField(Pointer<Buffer> buffer, Semantic semantic)
		: buffer(buffer), semantic(semantic) {}

	virtual ~BufferVertexField();

	virtual void save(ObjectWriter& w);		


	// the vertex field data
	Pointer<Buffer> buffer;
	
	// semantic (only used for optimizations)
	Semantic semantic;
};

// vertex field that references another field by name, e.g. of generator or particle system
class RefVertexField : public VertexField
{
public:

	RefVertexField()
		{}
	
	RefVertexField(const std::string& fieldName)
		: fieldName(fieldName) {}

	virtual ~RefVertexField();

	virtual void save(ObjectWriter& w);		


	// filed name of generator (particle system or text mesh)
	std::string fieldName;
};

/// @}

} // namespace digi

#endif
