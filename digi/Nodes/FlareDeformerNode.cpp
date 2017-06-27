#include <digi/Utility/StringUtility.h>

#include "FlareDeformerNode.h"


namespace digi {

// FlareDeformerNode

FlareDeformerNode::FlareDeformerNode(const Path& worldMatrix)
{
	// world matrix of deformed object
	this->addInput("worldMatrix", "float4x4", worldMatrix);
	
	// world matrix of sine deformer handle
	this->addInput("handleMatrix", "float4x4");

	// parameters
	this->addAttribute("lowBound", "float");
	this->addAttribute("highBound", "float");
	this->addAttribute("startFlareX", "float");
	this->addAttribute("startFlareZ", "float");
	this->addAttribute("endFlareX", "float");
	this->addAttribute("endFlareZ", "float");
	this->addAttribute("curve", "float");

	// outputs for vertex node
	this->addOutput("object2handle", "float4x4");
	this->addOutput("handle2object", "float4x4");
}

FlareDeformerNode::~FlareDeformerNode()
{
}

std::string FlareDeformerNode::getNodeType()
{
	return "FlareDeformerNode";
}

void FlareDeformerNode::writeUpdateCodeThis(NodeWriter& w)
{
	// calc transform from object space into handle space (object -> world -> handle)
	w << "$.object2handle = inv($.handleMatrix) * $.worldMatrix;\n";
	
	// calc transform from handle to object space
	w << "$.handle2object = inv($.object2handle);\n";
}


// FlareDeformerBoundingBoxNode

FlareDeformerBoundingBoxNode::FlareDeformerBoundingBoxNode(const Path& inputPath, const Path& parameters)
	: BoundingBoxNode(inputPath)
{
	// add inputs for scene parameters
}

FlareDeformerBoundingBoxNode::~FlareDeformerBoundingBoxNode()
{
}

std::string FlareDeformerBoundingBoxNode::getNodeType()
{
	return "FlareDeformerBoundingBoxNode";
}

void FlareDeformerBoundingBoxNode::writeUpdateCodeThis(NodeWriter& w)
{
	//!
	w << "$.output.center = $.input.center;\n";
	w << "$.output.size = $.input.size;\n";

}


// FlareDeformerVertexNode

FlareDeformerVertexNode::FlareDeformerVertexNode(const Path& inputPath, const Path& parameters)
	: VertexNode(inputPath)
{
	// add inputs for scene parameters
	this->addInput("lowBound", "float", parameters + ".lowBound");
	this->addInput("highBound", "float", parameters + ".highBound");
	this->addInput("startFlareX", "float", parameters + ".startFlareX");
	this->addInput("startFlareZ", "float", parameters + ".startFlareZ");
	this->addInput("endFlareX", "float", parameters + ".endFlareX");
	this->addInput("endFlareZ", "float", parameters + ".endFlareZ");
	this->addInput("curve", "float", parameters + ".curve");
	this->addInput("object2handle", "float4x4", parameters + ".object2handle");
	this->addInput("handle2object", "float4x4", parameters + ".handle2object");
}

FlareDeformerVertexNode::~FlareDeformerVertexNode()
{
}

std::string FlareDeformerVertexNode::getNodeType()
{
	return "FlareDeformerVertexNode";
}

void FlareDeformerVertexNode::writeUpdateCodeThis(NodeWriter& w)
{
	w << "float4 $p = $.object2handle * vector4($.input.position, 1.0f);\n";

	// calc normalized y which is 0 at lowBound and 1 ad highBound
	w << "float $ny = clamp(($p.y - $.lowBound) / ($.highBound - $.lowBound), 0.0f, 1.0f);\n";
	
	//w << "float $ny2 = $ny * 2.0f - 1.0f;\n";
	//w << "float $s = 1.0f + $.curve * (1.0f - $ny2) * ($ny2 + 1.0f);\n";
	w << "float $s = 1.0f + $.curve * (1.0f - $ny) * $ny * 4.0f;\n";
	w << "float $ny1 = 1.0f - $ny;\n";
	w << "$p.x *= ($ny1 * $.startFlareX + $ny * $.endFlareX) * $s;\n";
	w << "$p.z *= ($ny1 * $.startFlareZ + $ny * $.endFlareZ) * $s;\n";

	w << "$.output.position = ($.handle2object * $p).xyz;\n";
	w << "$.output.tangent = $.input.tangent;\n"; //! tangent is not deformed
	w << "$.output.bitangent = $.input.bitangent;\n"; //!
}

} // namespace digi
