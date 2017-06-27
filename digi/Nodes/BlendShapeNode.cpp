#include "BlendShapeNode.h"


namespace digi {


// BlendShapeNode

BlendShapeNode::BlendShapeNode()
{
	this->addOutput("baseWeight", "float");
}

BlendShapeNode::~BlendShapeNode()
{
}

std::string BlendShapeNode::getNodeType()
{
	return "BlendShapeNode";
}

void BlendShapeNode::writeUpdateCodeThis(NodeWriter& w)
{
	// calc base weight (1.0 - all absolute weights)
	w << "$.baseWeight = 1.0f";
	foreach (const Weight& weight, this->weights)
	{
		if (!weight.relative)
			w << " - $." << weight.name;
	}
	w << ";\n";
}

void BlendShapeNode::addWeight(const std::string& name, bool relative)
{
	this->addInput(name, "float");
	this->weights += Weight(name, relative);
}


// BlendShapeBoundingBoxNode

BlendShapeBoundingBoxNode::BlendShapeBoundingBoxNode(const Path& inputPath, Pointer<BlendShapeNode> sceneNode)
	: BoundingBoxNode(inputPath)
{
	// connect base weight and weights to outer node
	this->addInput("baseWeight", "float", Path(sceneNode, "baseWeight"));
	foreach (const BlendShapeNode::Weight& weight, sceneNode->weights)
	{
		this->addInput("target." + weight.name + ".weight", "float", Path(sceneNode, weight.name));
	}
}

BlendShapeBoundingBoxNode::~BlendShapeBoundingBoxNode()
{
}

std::string BlendShapeBoundingBoxNode::getNodeType()
{
	return "BlendShapeBoundingBoxNode";
}

void BlendShapeBoundingBoxNode::writeUpdateCodeThis(NodeWriter& w)
{
	std::stringstream center; center << "$.output.center = $.input.center * $.baseWeight";
	std::stringstream size; size << "$.output.size = $.input.size * abs($.baseWeight)";

	foreach (Path path, this->getPathElementList("target"))
	{
		center << "\n\t+ $" << path.path << ".center * $" << path.path << ".weight";
		size << "\n\t+ $" << path.path << ".size * abs($" << path.path << ".weight)";
	}

	w << center.str() << ";\n";
	w << size.str() << ";\n";
}


// BlendShapeVertexNode

BlendShapeVertexNode::BlendShapeVertexNode(const Path& inputPath, Pointer<BlendShapeNode> sceneNode)
	: VertexNode(inputPath)
{
	// connect base weight and weights to outer node
	this->addInput("baseWeight", "float", Path(sceneNode, "baseWeight"));
	foreach (const BlendShapeNode::Weight& weight, sceneNode->weights)
	{
		this->addInput("target." + weight.name + ".weight", "float", Path(sceneNode, weight.name));
	}
}

BlendShapeVertexNode::~BlendShapeVertexNode()
{
}

std::string BlendShapeVertexNode::getNodeType()
{
	return "BlendShapeVertexNode";
}

void BlendShapeVertexNode::writeUpdateCodeThis(NodeWriter& w)
{
	std::stringstream position; position << "$.output.position = $.input.position * $.baseWeight";
	std::stringstream tangent; tangent << "$.output.tangent = $.input.tangent * $.baseWeight";
	std::stringstream bitangent; bitangent << "$.output.bitangent = $.input.bitangent * $.baseWeight";

	foreach (Path path, this->getPathElementList("target"))
	{
		position << "\n\t+ $" << path.path << ".position * $" << path.path << ".weight";
		tangent << "\n\t+ $" << path.path << ".tangent * $" << path.path << ".weight";
		bitangent << "\n\t+ $" << path.path << ".bitangent * $" << path.path << ".weight";
	}

	w << position.str() << ";\n";
	w << tangent.str() << ";\n";
	w << bitangent.str() << ";\n";
}


} // namespace digi
