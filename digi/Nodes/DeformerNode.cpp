#include "DeformerNode.h"


namespace digi {


// BoundingBoxNode

BoundingBoxNode::BoundingBoxNode(const Path& inputPath)
{
	// add inputs
	this->addInput("input.center", "float3", inputPath + ".center");
	this->addInput("input.size", "float3", inputPath + ".size");

	// add outputs
	this->addOutput("output.center", "float3");
	this->addOutput("output.size", "float3");
}

BoundingBoxNode::~BoundingBoxNode()
{
}


// VertexNode

VertexNode::VertexNode(const Path& inputPath)
{
	// add inputs
	this->addInput("input.position", "float3", inputPath + ".position");
	this->addInput("input.tangent", "float3", inputPath + ".tangent");
	this->addInput("input.bitangent", "float3", inputPath + ".bitangent");

	// add outputs
	this->addOutput("output.position", "float3");
	this->addOutput("output.tangent", "float3");
	this->addOutput("output.bitangent", "float3");
}

VertexNode::~VertexNode()
{
}

} // namespace digi
