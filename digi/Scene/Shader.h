#ifndef digi_Scene_Shader_h
#define digi_Scene_Shader_h

#include "Mesh.h"


namespace digi {

/// @addtogroup Scene
/// @{

class Shader : public Object
{
public:
	enum ShapeType
	{
		MESH,
		SPRITES,
	};
	
	enum Geometry
	{
		// geometry is described by position
		POSITION,

		// geometry is described by position and normal
		POSITION_NORMAL,

		// geometry is described by position and normal which is calculated from tangent/bitangent.
		// tangent/bitangent are not available to the shader (set to zero)
		POSITION_NORMAL_FROM_TANGENTS,

		// geometry is described by position and tangent/bitangent
		POSITION_TANGENTS
	};

	enum Semantic
	{
		// position, normal, tangent
		GEOMETRY,
	
		// texture coordinate
		TEXCOORD,
		
		// vertex color
		COLOR,
		
		// particle attribute
		PARTICLE,
	};


	Shader(Geometry geometry = POSITION_NORMAL)
		: geometry(geometry) {}
	virtual ~Shader();
	
	// name of shader (must be unique, used for struct names and debugging)
	std::string name;
	
	// path of shader in uniforms struct
	std::string path;
	
	// list of attributes to copy from scene into uniforms struct for this shader
	std::vector<std::string> assignments;


	// material input structure (uniform in opengl terminology)
	Pointer<Type> materialType;

	// geometry: position/normal or position/tangent space
	Geometry geometry;

	struct InputField
	{
		// name of input (e.g. "map1")
		std::string name;

		// type of vertex input (e.g. "float2")
		std::string type;

		// semantic of input (e.g. TEXCOORD)
		Semantic semantic;
		
		InputField() {}
		InputField(std::string name, std::string type, Semantic semantic)
			: name(name), type(type), semantic(semantic) {}
	};

	// vertex input structure (vertex attributes in opengl terminology)
	std::vector<InputField> inputFields;
	
	// unified shader code. globals: viewMatrix, projectionMatrix, material, transform, input, position, color
	std::string code;


	// get input fiels for given shape type (enum Shape::Type)
	void getInputFields(ShapeType shapeType, std::vector<InputField>& inputFields);

	static Pointer<Shader> load(ObjectReader& r);
	void save(ObjectWriter& w);
};

/// @}

} // namespace digi

#endif
