#ifndef digi_CodeGenerator_ScriptNode_h
#define digi_CodeGenerator_ScriptNode_h

#include "Node.h"


namespace digi {

/// @addtogroup CodeGenerator
/// @{

/**
	script node.
	use addAttibute(), addInput(), addOutput() etc. to add attributes.
	see NodeWriter.h how to access the attributes.
	
	e.g.
	scriptNode = new ScriptNode("$.foo = $.bar;");
	scriptNode->addInput("foo");
	scriptNode->addOutput("bar");
*/
class ScriptNode : public Node
{
public:
	
	ScriptNode() {}
	ScriptNode(StringRef script) : script(script) {}

	virtual ~ScriptNode();

	virtual std::string getNodeType();
		
	void setScript(const std::string& script) {this->script = script;}
	
	virtual void writeUpdateCodeThis(NodeWriter& w);

	
	std::string script;
};

/// @}

} // namespace digi

#endif
