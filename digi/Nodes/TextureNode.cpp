#include <digi/Utility/ArrayUtility.h>

#include "TextureNode.h"


namespace digi {


static const char* typeNames[] = {"Texture2D", "Texture3D", "TextureCube"};

// SamplerNode

SamplerNode::SamplerNode(TextureType textureType)
	: TextureBaseNode(textureType == TEXTURE_2D ? 2 : 3, 4), textureType(textureType)
{
	// input texture (in state scope as textures are set from the outside)
	this->addInput("texture", typeNames[textureType - 2], 0);
}

SamplerNode::~SamplerNode()
{
}

std::string SamplerNode::getNodeType()
{
	return "SamplerNode";
}

int SamplerNode::getAddressFlags()
{
	// address modification is done via sampler state
	return AUTO_REPEAT | AUTO_CLAMP | AUTO_MIRROR;
}

void SamplerNode::generateTexture(NodeWriter& w)
{
	// lookup texture
	w << "$.output = sample($.texture, $coord);\n";
}


// FileTextureSequenceNode

FileTextureSequenceNode::FileTextureSequenceNode(SamplerNode::TextureType textureType)
	: textureType(textureType), start(0), length(1)
{
	// input texture array (in state scope as textures are set from the outside)
	this->addInput("textures", Code() << "[1]" << typeNames[textureType - 2], 0);

	// texture index
	this->addInput("textureIndex", "int");

	// texture offset
	this->addInput("textureOffset", "int");

	// texture output
	this->addOutput("texture", typeNames[textureType - 2]);
}

FileTextureSequenceNode::~FileTextureSequenceNode()
{
}

std::string FileTextureSequenceNode::getNodeType()
{
	return "FileTextureSequenceNode";
}

void FileTextureSequenceNode::writeUpdateCodeThis(NodeWriter& w)
{
	w << "int $index = $.textureIndex + $.textureOffset";
	if (this->start != 0)
		w << " - " << this->start;
	w << ";\n";
	w << "$.texture = ($index >= 0 & $index < " << this->length << ") ? $.textures[$index] : $@texture();\n";
}

void FileTextureSequenceNode::setRange(int start, int length)
{
	this->start = start;
	this->length = length;
	this->setType("textures", Code() << '[' << length << ']' << typeNames[this->textureType - 2]);
}


// VertexInputNode

VertexInputNode::VertexInputNode(const std::string& inputName, const std::string& outputType,
	Shader::Semantic semantic)
	: Node(2), semantic(semantic), inputName(inputName)
{
	this->addOutput("output", outputType);
}

VertexInputNode::~VertexInputNode()
{
}

std::string VertexInputNode::getNodeType()
{
	return "VertexInputNode";
}

void VertexInputNode::writeUpdateCodeThis(NodeWriter& w)
{
	w << "$.output = input." << this->inputName << ";\n";
}


// ProjectionVisitor

ProjectionVisitor::ProjectionVisitor(NodeWriter& w, int maxScope)
	: WriteUpdateVisitor(w, maxScope)
{
	w.swapLine(this->projection);
}

ProjectionVisitor::~ProjectionVisitor()
{
}

void ProjectionVisitor::visitNode(Pointer<Node> node)
{
	if (Pointer<VertexInputNode> vertexInputNode = dynamicCast<VertexInputNode>(node))
	{
		if (vertexInputNode->semantic == Shader::TEXCOORD)
		{
			// print projection on first time
			if (!this->projection.empty())
			{
				this->w << this->projection << "\n";
				this->projection.clear();
			}
			
			this->w.push(node);
			this->w << "$.output = $projection;\n";
			this->w.pop();
			return;
		}		
	}

	this->WriteUpdateVisitor::visitNode(node);
}


// GetVertexInputsVisitor

GetVertexInputsVisitor::~GetVertexInputsVisitor()
{
}

void GetVertexInputsVisitor::visitNode(Pointer<Node> node)
{
	if (Pointer<VertexInputNode> vertexInputNode = dynamicCast<VertexInputNode>(node))
	{
		this->inputs += Shader::InputField(vertexInputNode->inputName,
			vertexInputNode->getType("output"),
			vertexInputNode->semantic);
	}
}

} // namespace digi
