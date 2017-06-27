#include <digi/Utility/VectorUtility.h>

#include "FogNode.h"


namespace digi {


// FogNode

FogNode::FogNode(const Path& lightColor, int flags)
	: Node(2), flags(flags)
{
	this->addInput("color", "float3");
	this->addInput("saturationDistance", "float");
	if (flags & USE_DISTANCE)
	{
		this->addInput("minDistance", "float");
		this->addInput("maxDistance", "float");		
	}
	if (flags & USE_HEIGHT)
	{
		this->addInput("minHeight", "float");
		this->addInput("maxHeight", "float");		
	}

	// color of light that illuminates the fog
	if (!lightColor.isNull())
		this->addInput("lightColor", "float3", lightColor);
	else
		this->addConstant("lightColor", splat3(1.0f));

	// input pixel color
	this->addInput("input", "float3");
	
	// output pixel color (with fog)
	this->addOutput("output", "float3");
}

FogNode::~FogNode()
{
}

std::string FogNode::getNodeType()
{
	return "FogNode";
}

void FogNode::writeUpdateCodeThis(NodeWriter& w)
{
	w << "float3 $color = $.color * $.lightColor;\n";
	w << "float $invSaturationDistance = 1.0f / $.saturationDistance;\n";
	
	if ((this->flags & USE_DISTANCE) == 0)
	{
		// distance in fog is -sufacePosition.z
		w << "float $distance = -position.z;\n";
	}
	else
	{
		// maya bug/feature: subtract minDistance from from distance. the effect is that we see
		// a layer of fog that is closer than minDistance if the distance is far.
		w << "float $distance = min(-position.z, $.maxDistance) - $.minDistance;\n";
	}
	
	if ((this->flags & USE_HEIGHT) == 0)
	{
		// prevent distance from being below zero
		w << "float $d = max($distance, 0.0f);\n";
	}
	else
	{
		// calc transform from view space into world space
		w << "float4x4 $matrix = inv(viewMatrix);\n";

		// height of camera in world space
		w << "float $cameraHeight = $matrix.w.y;\n";
		
		// relative height of surface position in world space
		w << "float $height = $matrix.x.y * position.x + $matrix.y.y * position.y + $matrix.z.y * position.z;\n";

		// calc slope of view ray
		w << "float $slope = -position.z / $height;\n";

		// calc crossing of ray with layer of fog
		w << "float $d1 = max(min(($.minHeight - $cameraHeight) * $slope, $distance), 0.0f);\n";
		w << "float $d2 = max(min(($.maxHeight - $cameraHeight) * $slope, $distance), 0.0f);\n";
		
		// calc the distance in fog layer (measured in view space z-direction)
		w << "float $d = abs($d2 - $d1);\n";
	}
		
	// calc exponential decay
	w << "float $decay = exp(-$d * $invSaturationDistance);\n";	
	
	// blend input with fog color
	if (this->flags & COLOR_BASED_TRANSPARENCY)
	{
		//w << "$.output = (1.0f - $color) * $.input + $color * ((1.0f - $decay) + $decay * $.input);\n";

		//(1 - color) * input + color * ((1 - decay) + decay * input)
		//input - color * input + (1 - decay) * color + decay * input * color	
		//input * (1 - color + decay * color) + (1 - decay) * color
		//input * (1 - (1 - decay) * color) + (1 - decay) * color
		//input + (1 - decay) * color + (decay - 1) * input * color
		//input + ((1 - decay) - (1 - decay) * input) * color
		//input + ((1 - decay) * (1 - input)) * color
		//input + (1 - input) * (1 - decay) * color

		w << "$.output = $.input + (1.0f - $.input) * (1.0f - $decay) * $color;\n";
	}
	else
	{	
		//w << "$.output = (1.0f - $decay) * $color + $decay * $.input;\n";
		w << "$.output = lerp($color, $.input, $decay);\n";
	}
}

} // namespace digi
