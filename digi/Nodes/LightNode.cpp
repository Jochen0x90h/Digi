#include <digi/Utility/VectorUtility.h>

#include "TextureNode.h"
#include "LightNode.h"


namespace digi {


// LightNode
LightNode::LightNode(Type type, Decay decay, int flags)
	: Node(2), type(type), decay(decay), flags(flags)
{
	this->addInput("worldMatrix", "float4x4");
	this->addInput("worldVisibility", "bool");

	this->addAttribute("color", "float3", Attribute::INPUT | Attribute::STOP);
	this->addAttribute("intensity", "float", Attribute::INPUT | Attribute::STOP);

	if (this->type == LightNode::AMBIENT_LIGHT)
	{
		this->addInput("shade", "float");
	}
	else if (this->type == LightNode::SPOT_LIGHT)
	{
		this->addInput("coneAngle", "float");
		this->addInput("penumbraAngle", "float");
	}

	this->addOutput("output.vector", "float3");
	this->addOutput("output.color", "float3");
	if (this->type == LightNode::AMBIENT_LIGHT)
		this->addOutput("output.shade", "float");
}

LightNode::~LightNode()
{
}

std::string LightNode::getNodeType()
{
	return "LightNode";
}

void LightNode::writeUpdateCodeThis(NodeWriter& w)
{
	// viewMatrix is globally available

	if (this->decay == LINEAR || this->decay == CUBIC || this->type != DIRECTIONAL_LIGHT)
	{
		// calc unnormalized light vector in view space
		w << "float3 $toLight = transformPosition(viewMatrix, $.worldMatrix.w.xyz) - position;\n";
		
		// calc inverse length of light vector
		w << "float $scale = 1.0f / length($toLight);\n";
	}
	
	// evaluate inputs and apply projection to any upstream texture nodes
	{
		const int scope = 2;
	
		// generate local variable for inner evaluation
		TargetTypeVisitor targetTypeVisitor;
		this->visit(targetTypeVisitor, "color", scope, scope);
		this->visit(targetTypeVisitor, "intensity", scope, scope);
		std::string scope1Name = "inner";		
		targetTypeVisitor.type->writeVariable(w, scope1Name);
		
		
		// transform surface position into projector space (the light is the projector)
		w << "float3 $v = transformPosition(inv(viewMatrix * $.worldMatrix), position);\n";
		w << "float2 $projection = ";
		switch (this->type)
		{		
		case AMBIENT_LIGHT:
			// spherical projection 2
			w << "vector2(atan2($v.x, $v.y) * -0.31831f, atan2(length($v.xz), -$v.y) * 0.31831f);";
			break;
		case DIRECTIONAL_LIGHT:
			// parallel projection
			w << "$v.xy + 0.5f;";
			break;
		case POINT_LIGHT:
			// spherical projection
			w << "vector2(0.5f - atan2(-$v.x, $v.z) * 0.15915f, atan2(length($v.xz), -$v.y) * 0.31831f);";
			break;
		case SPOT_LIGHT:
			// perspective projection
			w << "$v.xy / ($v.z * (tan($.coneAngle * 0.5f) * 2.0f)) + 0.5f;";
			break;
		}	
		
		// the projection visitor swaps the current code line out and prints it when first needed
		ProjectionVisitor projectionVisitor(w, scope);
		scope1Name.swap(w.scopes[scope]);
		this->visit(projectionVisitor, "color", scope, scope);
		this->visit(projectionVisitor, "intensity", scope, scope);
		w << "float3 $color = $.color;\n";
		w << "float3 $intensity = $.intensity;\n";		
		scope1Name.swap(w.scopes[scope]);
	}
	
	// calc color intensity
	w << "float3 $colorIntensity = $color * $intensity * convert_float($.worldVisibility)";
	switch (this->decay)
	{
	case NONE:
		break;
	case LINEAR:
		w << " * min($scale, 1.0f)";
		break;
	case QUADRATIC:
		w << " / max(dot($toLight, $toLight), 1.0f)";
		break;
	case CUBIC:
		w << " * min($scale / dot($toLight, $toLight), 1.0f)";
		break;
	};
	w << ";\n";
		
	switch (this->type)
	{		
	case AMBIENT_LIGHT:
		// calc light vector in view space
		w << "$.output.vector = $toLight * $scale;\n";
		w << "$.output.color = $colorIntensity;\n";
		w << "$.output.shade = $.shade;\n";
		break;

	case DIRECTIONAL_LIGHT:
		// directional light points in -z direction. therefore light vector is -(-z) = z
		// calc light vector in view space
		w << "$.output.vector = normalize(transformDirection(viewMatrix, $.worldMatrix.z.xyz));\n";
		w << "$.output.color = $colorIntensity;\n";
		break;
	
	case POINT_LIGHT:
		// calc light vector in view space
		w << "$.output.vector = $toLight * $scale;\n";
		w << "$.output.color = $colorIntensity;\n";
		break;
	
	case SPOT_LIGHT:
		w << "float3 $direction = -$.worldMatrix.z.xyz;\n";
		
		// dot(lightDirection, lightVector) yields -cos(coneAngle * 0.5) where lightDirection is the normalized
		// direction of the light and lightVector the normalized direction to the light from the surface.
		// therefore we calculate the cos of inner and outer cone angle. note: coneAngle goes from left border to
		// right border of cone while penumbraAngle is angle of penumbra on only one side of cone.
		
		// inner cone angle
		w << "float $inner = -cos($.coneAngle * 0.5f + min($.penumbraAngle, 0.0f));\n";
		
		// outer cone angle (prevent division by zero)
		w << "float $outer = -cos($.coneAngle * 0.5f + max($.penumbraAngle, 0.001f));\n";
		
		// cone scale factors
		w << "float $coneOffset = $outer;\n";
		w << "float $coneScale = 1.0f / ($outer - $inner);\n";

		// the following code will be executed in pixel shader
		
		// calc normalized light vector in view space and output it
		w << "float3 $lightVector = $toLight * $scale;\n";
		w << "$.output.vector = $lightVector;\n";
		
		// calc spot in view space (area lit by cone/penumbra)
		w << "float $spotValue = dot(normalize(transformDirection(viewMatrix, $direction)), $lightVector);\n"; 
		w << "float $spotIntensity = saturate(($coneOffset - $spotValue) * $coneScale);\n";
		
		// calc output color
		w << "$.output.color = $colorIntensity * $spotIntensity;\n";
		break;
	}	
}

} // namespace digi
