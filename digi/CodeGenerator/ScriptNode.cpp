#include "NodeWriter.h"
#include "ScriptNode.h"


namespace digi {



// ScriptNode

ScriptNode::~ScriptNode()
{
}

std::string ScriptNode::getNodeType()
{
	return "ScriptNode";
}

void ScriptNode::writeUpdateCodeThis(NodeWriter& w)
{
	if (!this->script.empty())
	{
		w << this->script;
		if (*this->script.rbegin() != '\n')
			w << "\n";
	}	
}


} // namespace digi
