#ifndef digi_Nodes_TextNode_h
#define digi_Nodes_TextNode_h

#include <digi/CodeGenerator/Node.h>


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Text node. uses a macro to generate symbols from a string. the macro has to
	be implemented by the backends (c++/opengl, js/webgl etc.)
	
	Inputs:
		scaleX (float)
		scaleY (float)
		spaceWidth (float)
		lineHeight (float)
		text (string)
		
	Outputs:
		center (float3, center of bounding box)
		size (float3, size of bounding box)
*/
class TextNode : public Node
{
public:

	TextNode();
	
	virtual ~TextNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);
	

	std::string name;
};

/// @}

} // namespace digi

#endif
