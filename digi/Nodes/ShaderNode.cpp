#include <digi/Utility/VectorUtility.h>

#include "ShaderNode.h"


namespace digi {

// ShaderNode
ShaderNode::ShaderNode(Type type, bool specularityIsColor)
	: Node(2), type(type)
{
	this->addInput("diffuseColor", "float3");
	this->addInput("diffusity", "float");
	this->addInput("ambientLight", "float3");

	if (this->hasSpecular())
	{
		this->addInput("specularColor", "float3");
		if (type == PHONG)
		{
			this->addInput("specularPower", "float");
		}
		else if (type == SCHLICK)
		{
			this->addInput("roughness", "float");
			this->addInput("highlightSize", "float");
		}
		else if (type == BLINN)
		{
			this->addInput("eccentricity", "float");
		}
		else // ASHIKHMIN
		{
			this->addInput("angle", "float");
			this->addInput("spreadX", "float");
			this->addInput("spreadY", "float");
			this->addInput("roughness", "float");
			this->addInput("fresnelRefractiveIndex", "float");
		}
		
		this->addInput("specularity", specularityIsColor ? "float3" : "float");
		this->addInput("reflectivity", "float");
		this->addInput("reflectingLight", "float3");
	}

	this->addOutput("output.diffuse", "float3");
	this->addOutput("output.specular", "float3");
}

ShaderNode::~ShaderNode()
{
}

std::string ShaderNode::getNodeType()
{
	return "ShaderNode";
}

void ShaderNode::writeUpdateCodeThis(NodeWriter& w)
{
	bool hasSpecular = this->hasSpecular();

	// cache attributes since they are not available inside the foreach loops
	w << "float3 $N = normal;\n"; // normal (N)
	if (hasSpecular)
		w << "float3 $V = viewVector;\n"; // view vector (V)

	// diffuse light accumulator variable
	w << "float3 $dl = {};\n";

	// iterate over all lights
	foreach (const Light& light, this->lights)
	{
		w.push(Path(this, "light." + light.name));
		
		if ((light.flags & LightNode::EMITS_DIFFUSE) != 0)
		{
			// calc diffuse (N * L, light direction L is $.vector, light color is $.color)
			if ((light.flags & IS_AMBIENT) == 0)
				w << "$dl += $.color * max(dot($N, $.vector), 0.0f);\n";
			else
				w << "$dl += $.color * max(1.0f + $.shade * (dot($N, $.vector) - 1.0f), 0.0f);\n";
		}
		
		w.pop();
	}
	
	// calc final diffuse light
	w << "float3 $diffuseLight = $.ambientLight + $.diffusity * $dl;\n";

	// calc specular light (light direction L is $.vector, light color is $.color)
	if (hasSpecular)
	{
		// cache attributes
		if (this->type == PHONG)
		{
			w << "float $specularPower = $.specularPower;\n";
		}
		else if (this->type == SCHLICK)
		{
			w << "float $r = $.roughness * $.roughness;\n";
			w << "float $s = 1.0f / $.highlightSize;\n";
		}
		else if (this->type == BLINN)
		{
			w << "float $e = $.eccentricity;\n";
			
			w << "float $NV = dot($N, $V);\n";
		}
		else // ASHIKHMIN
		{
			w << "float $nx = sqr($.spreadX / $.roughness);\n";
			w << "float $ny = sqr($.spreadY / $.roughness);\n";
			w << "float $f = sqr($.fresnelRefractiveIndex - 1.0f) / sqr($.fresnelRefractiveIndex + 1.0f);\n";
			w << "float $A = sqrt(($nx + 1.0f) * ($ny + 1.0f)) * 0.039789f;\n"; // 1/8pi
			
			w << "float $x = $.angle * 0.0087266f;\n"; // 360 -> pi
			w << "float $c = cos($x);\n";
			w << "float $s = sin($x);\n";
			w << "float3 $T = $c * tangent + $s * bitangent;\n";
			w << "float3 $B = $c * bitangent - $s * tangent;\n";
		}

		// specular light accumulator variable
		w << "float3 $sl = {};\n";
		
		// iterate over all lights
		foreach (const Light& light, this->lights)
		{
			w.push(Path(this, "light." + light.name));
		
			if (hasSpecular && (light.flags & LightNode::EMITS_SPECULAR) != 0)
			{
				w.beginScope();
				
				// light vector (L)
				w << "float3 $L = $.vector;\n";					

				if (this->type == PHONG)
				{
					// calc specular
					// pow(R * V, specularPower)
					// where R = reflect(-L, N)					
					w << "$sl += $.color * pow(max(dot(reflect(-$L, $N), $V), 0.0f), $specularPower);\n";					
				}
				else if (this->type == SCHLICK)
				{
					// calc specular
					// x / (n - nx + x) (approximation of x^n)
					// = rx / (1 - x + rx)
					// where x = R * V, R = reflect(-L, N), n = specularPower, r = roughness
					w << "float $x = max(dot(reflect(-$L, $N), $V) * $s + (1.0f - $s), 0.0f);\n";
					w << "float $rx = $r * $x;\n";
					w << "$sl += $.color * $rx / (1.0f - $x + $rx);\n";
				}
				else if (this->type == BLINN)
				{
					w << "float3 $H = normalize($V + $L);\n";
					w << "float3 $R = reflect(-$L, $N);\n";

					w << "float $NH = dot($N, $H);\n";
					w << "float $RV = dot($R, $V);\n";
					w << "float $NL = max(dot($N, $L), 0.0f);\n";
					w << "float $HV = dot($H, $V);\n";
					
					// blinn model according to http://www.siggraph.org/education/materials/HyperGraph/illumin/specular_highlights/blinn_model_for_specular_reflect_1.htm
					
					// calc distribution function D
					w << "float $D = sqr($e / ($RV * $RV * ($e - 1.0f) + 1.0f));\n";

					// calc geometrical attenuation factor G
					// Ga = 1
					// Gb = 2 * NH * NV / HV
					// Gc = 2 * NH * NL / HV
					// G = min(Ga, Gb, Gc)
					w << "float $G = min(1.0f, 2.0f * $NH * min($NV, $NL) / $HV);\n";
					
					// Fresnel reflection F is only applied to reflection (e.g. env mapping)
					
					w << "$sl += $.color * $D * $G / $NV;\n";					
				}
				else // ASHIKHMIN
				{				
					w << "float3 $H = normalize($V + $L);\n";

					w << "float $NH = max(dot($N, $H), 0.0f);\n";
					w << "float $TH = dot($T, $H);\n";
					w << "float $BH = dot($B, $H);\n";
					w << "float $HL = dot($H, $L);\n"; // note: HL == HV
					w << "float $NL = max(dot($N, $L), 0.0f);\n";
					w << "float $NV = dot($N, $V);\n";
					
					w << "float $B = pow($NH, ($nx * $TH * $TH + $ny * $BH * $BH) / (1.0f - $NH * $NH)) / ($HL * max($NL, $NV));\n";
					
					// Fresnel term
					w << "float $F = $f + (1.0f - $f) * pow(1.0f - $HL, 5.0f);\n";
					
					w << "$sl += $.color * $A * $B * $F;\n";
				}
				
				w.endScope();
			}		
		
			w.pop();
		}
		if (this->type == BLINN)
		{
			// apply fresnel term to reflected light and fade out with specularity
			// schlick approximation of fresnel term:
			// ((n - 1)^2 + 4 * n * (1 - x)^5) / (n + 1)^2
			// where n = 1.5, x = dot(N, V)
			// maya: (1 - x)^3
			w << "float $f = 1.0f - $NV;\n";
			w << "float3 $specularLight = ((1.0f - $.specularity) * $f * $f * $f + $.specularity) * $.reflectivity * $.reflectingLight"
				" + $.specularity * $sl;\n";				
		}
		else
		{
			w << "float3 $specularLight = $.reflectivity * $.reflectingLight + $.specularity * $sl;\n";
		}
	}

	w << "$.output.diffuse = $.diffuseColor * $diffuseLight;\n";
	w << "$.output.specular = " << (hasSpecular ? "$.specularColor * $specularLight" : "0.0f") << ";\n";
}

void ShaderNode::addLight(Pointer<LightNode> lightNode, const std::string& name)
{
	Path path(lightNode, "output");
	
	this->addInput("light." + name + ".vector", "float3", path + ".vector");
	this->addInput("light." + name + ".color", "float3", path + ".color");

	int flags = lightNode->getFlags();
	if (lightNode->getType() == LightNode::AMBIENT_LIGHT)
	{
		this->addInput("light." + name + ".shade", "float", path + ".shade");
		flags = LightNode::EMITS_DIFFUSE | IS_AMBIENT;
	}
		
	this->lights += Light(name, flags);
}

void ShaderNode::setDefaultLight(float3 vector, float3 color)
{
	this->addConstant("light.default.vector", normalize(vector));
	this->addConstant("light.default.color", color);

	this->lights += Light("default", LightNode::EMITS_DIFFUSE | LightNode::EMITS_SPECULAR);
}

} // namespace digi
