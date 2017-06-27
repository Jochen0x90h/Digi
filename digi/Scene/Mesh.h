#ifndef digi_Scene_Mesh_h
#define digi_Scene_Mesh_h

#include "Shape.h"


namespace digi {

/// @addtogroup Scene
/// @{

/// base class for mesh
class Mesh : public Shape
{
public:

	enum RenderMode
	{
		// render double sided
		DOUBLE_SIDED = 1,
		
		// render back side (render back side fist if double sided in two passes)
		BACK_SIDE = 2,
		
		// negative transform flips front and back
		FLIP = 4,
	};

	Mesh()
		: renderMode() {}
	Mesh(uint renderMode)
		: renderMode(renderMode) {}
	
	virtual ~Mesh();


	// cull mode (none, back, front)
	//RenderState::CullMode cullMode;
	
	// render mode
	uint renderMode;
};


/// constant triangle mesh
class ConstantMesh : public Mesh
{
public:

	ConstantMesh()
		{}
	ConstantMesh(uint renderMode)
		: Mesh(renderMode) {}
	
	virtual ~ConstantMesh();

	virtual void save(ObjectWriter& w);		

	/// get number of triangles
	int getNumTriangles() {return int(this->indices.size() / 3);}
	
	
	// list of vertices to use (number of triangles is length of indices divided by 3)
	std::vector<uint> indices;	
};


/// renders text using a string as input and a font texture
class TextMesh : public Mesh
{
public:

	TextMesh()
		{}
	TextMesh(uint renderMode)
		: Mesh(renderMode), maxNumSymbols() {}

	virtual ~TextMesh();

	virtual void save(ObjectWriter& w);


	// name of text mesh (must be unique, used for struct names and debugging)
	std::string name;

	// path of text mesh in instance.uniforms struct
	//std::string path;

	// maximum number of symbols, used to preallocate a vertex buffer
	uint maxNumSymbols;
};

/// @}

} // namespace digi

#endif
