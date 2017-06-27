#ifndef digi_Nodes_ShaderNode_h
#define digi_Nodes_ShaderNode_h

#include <digi/CodeGenerator/Node.h>

#include "LightNode.h"


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Shader node for Lambert, Phong or Blinn illumination model

	Diffuse light gets calculated as follows:
	diffuseLight = ambientLight + diffusity * SUM[light.n.color * max(0, dot(normal, light.n.vector))]

	Specular light gets calculated as follows (blinn formula):
	for phong: specularPower *= 4
	halfVector = normalize(light.n.vector + viewVector)
	specularLight = reflectivity * reflectingLight + specularity * SUM[light.n.color * pow(max(0, dot(normal, halfVector)), specularPower)]

	Output color gets calcluated as follows:
	output = diffuseColor * diffuseLight + specularColor * specularLight

	Note: incandescence is not handled by ShaderNode

	Inputs:
		duffuseColor (float3)
		diffusity (float, maya: diffuse)
		ambientLight (float3, maya: ambient color)
		
		only phong/blinn/schlick:
		specularColor (float3)
		specularPower (float3)
		specularity (float or float3, maya: only PhongE whiteness)
		reflectivity (float)
		reflectingLight (float3, maya: reflected color)

		viewVector (V) (float3, view space)
		normal (N) (float3, normalized, view space)

		light
			0
				vector (L) (float3, normalized)
				color (float3)
				shade (float, only ambient light)
			1
				vector
				color
			...

	Outputs:
		output.diffuse (float3)
		output.specular (float3)
*/
class ShaderNode : public Node
{
public:
	
	enum Type
	{
		// lambert illumination model
		LAMBERT,
		
		// phong illumination model ((R*V)^n)
		PHONG,
		
		// like phong, but with approximation x^n = x / (n - nx + x) (Maya: PhongE)
		SCHLICK,

		// blinn illumination model
		BLINN,
				
		// anisotropic
		ASHIKHMIN,
	};		

	// constructor. shaderParameters is data source for shader parameters, connections can be overridden later.
	// geometry is data source for position and normal.
	ShaderNode(Type type, bool specularityIsColor);

	virtual ~ShaderNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);

	Type getType() {return this->type;}
	bool hasSpecular() {return this->type != LAMBERT;}


	enum LightFlags
	{
		IS_AMBIENT = 4
	};

	struct Light
	{
		std::string name;
		int flags;
		
		Light()
			: flags() {}
		Light(std::string name, int flags)
			: name(name), flags(flags) {}
	};

	// add a light 
	void addLight(Pointer<LightNode> lightNode, const std::string& name);

	// set default light with vector L in camera space and color. use if no lights are added, call only once
	void setDefaultLight(float3 vector, float3 color);

	// get number of lights
	int getNumLights() {return int(this->lights.size());}

protected:

	Type type;
	std::vector<Light> lights;
};

/// @}

} // namespace digi

#endif
