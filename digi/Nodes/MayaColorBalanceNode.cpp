#include <digi/Utility/ArrayUtility.h>

#include "MayaColorBalanceNode.h"


namespace digi {


// MayaColorBalanceNode

MayaColorBalanceNode::MayaColorBalanceNode(const Path& parameters, const Path& input, const Path& enable)
	: alphaIsLuminance(false), invert(false)
{
	if (!enable.isNull())
		this->addInput("defaultColor", "float3", parameters + ".defaultColor");
	this->addInput("gain", "float4", parameters + ".gain");
	this->addInput("offset", "float4", parameters + ".offset");

	this->addInput("input", input.getType(), input);
	
	// add border enable flag
	if (!enable.isNull())
		this->addInput("enable", "float", enable);
	
	this->addOutput("output.color", "float4");
	this->addOutput("output.transparency", "float3");
}

MayaColorBalanceNode::MayaColorBalanceNode(const Path& input, const Path& enable)
	: alphaIsLuminance(false), invert(false)
{
	if (!enable.isNull())
		this->addInput("defaultColor", "float3");
	this->addInput("gain", "float4");
	this->addInput("offset", "float4");

	this->addInput("input", input.getType(), input);
	
	// add border enable flag
	if (!enable.isNull())
		this->addInput("enable", "float", enable);
	
	this->addOutput("output.color", "float4");
	this->addOutput("output.transparency", "float3");
}

MayaColorBalanceNode::~MayaColorBalanceNode()
{
}

std::string MayaColorBalanceNode::getNodeType()
{
	return "MayaColorBalanceNode";
}

void MayaColorBalanceNode::writeUpdateCodeThis(NodeWriter& w)
{
	// extend input color to float4 and apply gain/offset
	if (this->getType("input") == "float3")
		w << "float4 $color = vector4($.input, 1.0f);\n";
	else
		w << "float4 $color = $.input;\n";
		
	// replace alpha by luminance
	if (this->alphaIsLuminance)
		w << "$color.w = 0.30f * $color.x + 0.59f * $color.y + 0.11f * $color.z;\n";

	// invert input color
	if (this->invert)
		w << "$color = 1.0f - $color;\n";	
	
	// color/alpha gain/offset
	w << "$color = $color * $.gain + $.offset;\n";
	
	// default color
	if (this->hasTypedAttribute("enable"))
		w << "$color = $.enable * $color + (1.0f - $.enable) * vector4($.defaultColor,"
			"0.30f * $.defaultColor.x + 0.59f * $.defaultColor.y + 0.11f * $.defaultColor.z);\n";
			
	// output color and alpha
	w << "$.output.color = $color;\n";

	// output transparency
	w << "$.output.transparency = splat3(1.0f - $color.w);\n";	
	
}

} // namespace digi
