#include "Shader.h"
#include "Instancer.h"


namespace digi {

// Shader

Shader::~Shader()
{
}

// serialize

template <typename Serializer>
Serializer& operator &(Serializer& s, Shader::InputField& inputField)
{
	s & inputField.name;
	s & inputField.type;
	s & wrapUInt(inputField.semantic);
	return s;
}

template <typename Serializer>
void serialize(Serializer& s, Shader& shader)
{
	s & shader.name;
	s & shader.path;
	s & shader.assignments;
	
	s & shader.materialType;
	s & wrapUInt(shader.geometry);
	s & shader.inputFields;
	s & shader.code;
}

void Shader::getInputFields(ShapeType shapeType, std::vector<InputField>& inputFields)
{
	inputFields += InputField("position", "float3", GEOMETRY);
	switch (shapeType)
	{
	case MESH:
		switch (this->geometry)
		{
		case Shader::POSITION:
			break;
		case Shader::POSITION_NORMAL:
			inputFields += InputField("normal", "float3", GEOMETRY);
			break;
		case Shader::POSITION_NORMAL_FROM_TANGENTS:
		case Shader::POSITION_TANGENTS:
			inputFields += InputField("tangent", "float3", GEOMETRY), InputField("bitangent", "float3", GEOMETRY);
			break;
		}		
		break;
	case SPRITES:
		inputFields += InputField("sprite", "float3", GEOMETRY);
		break;
	};

	add(inputFields, this->inputFields);
}

Pointer<Shader> Shader::load(ObjectReader& r)
{
	Pointer<Shader> shader = new Shader();
	VersionReader v(r);	
	serialize(r, *shader);
	return shader;
}

void Shader::save(ObjectWriter& w)
{
	VersionWriter v(w, 1);
	serialize(w, *this);
}

} // namespace digi
