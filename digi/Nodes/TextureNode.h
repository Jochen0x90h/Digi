#ifndef digi_Nodes_TextureNode_h
#define digi_Nodes_TextureNode_h

#include <digi/CodeGenerator/Node.h>
#include <digi/Scene/Shader.h>

#include "TextureBaseNode.h"


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Sampler node. inside a shader this is a texture lookup
	
	Inputs:
		input (float, float2 or float3)
		texture (Texture2D, Texture3D or TextureCube)
		
	Outputs:
		output (float4)
*/
class SamplerNode : public TextureBaseNode
{
public:

	enum TextureType
	{
		TEXTURE_2D = 2,
		TEXTURE_3D = 3,
		TEXTURE_CUBE = 4
	};

	SamplerNode(TextureType textureType);
	
	virtual ~SamplerNode();

	virtual std::string getNodeType();

	virtual int getAddressFlags();
	virtual void generateTexture(NodeWriter& w);
	
	
	TextureType textureType;
};


/**
	Scene node that contains a sequence of reference to textures

	Inputs:	
		textures (array of Texture2D, Texture3D or TextureCube)
		textureIndex (int)
		textureOffset (int)

	Outputs:
		texture (Texture2D, Texture3D or TextureCube)
*/
class FileTextureSequenceNode : public Node
{
public:
			
	FileTextureSequenceNode(SamplerNode::TextureType textureType);
	
	virtual ~FileTextureSequenceNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);
	
	void setRange(int start, int length);
	
protected:

	SamplerNode::TextureType textureType;
	int start;
	int length;
};


/**
	node that fetches a value from the vertex input for use in a shader.

	Outputs:
		output
*/
class VertexInputNode : public Node
{
public:

	/**
		constructs node in scope 2 to indicate it is in a shader. When the node graph is constructed the method
		propagateScopes() propagates this scope in data flow direction through texture nodes and other nodes until
		it reaches the shader node. This way everything is "lifted" to scope 2 that is needed to compute the shader.
	*/
	VertexInputNode(const std::string& inputName, const std::string& outputType, Shader::Semantic semantic);
	
	virtual ~VertexInputNode();
	
	virtual std::string getNodeType();
	
	virtual void writeUpdateCodeThis(NodeWriter& w);

	
	Shader::Semantic semantic;

	// name of vertex input (e.g. "position")
	std::string inputName;
};


class ProjectionVisitor : public WriteUpdateVisitor
{
public:

	ProjectionVisitor(NodeWriter& w, int maxScope);
	
	virtual ~ProjectionVisitor();
	
	virtual void visitNode(Pointer<Node> node);

protected:

	std::string projection;
};


class GetVertexInputsVisitor : public NodeVisitor
{
public:

	GetVertexInputsVisitor(std::vector<Shader::InputField>& inputs)
		: NodeVisitor(true), inputs(inputs) {}
	virtual ~GetVertexInputsVisitor();
	
	virtual void visitNode(Pointer<Node> node);

protected:

	// list of input fields (name, type, semantic) gets collected in visitNode()
	std::vector<Shader::InputField>& inputs;
};

/// @}

} // namespace digi

#endif
