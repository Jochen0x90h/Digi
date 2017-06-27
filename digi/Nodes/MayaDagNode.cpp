#include <digi/Utility/StringUtility.h>

#include "MayaDagNode.h"


namespace digi {

MayaDagNode::MayaDagNode()
{
	this->addInput("visibility", "bool");
}

MayaDagNode::~MayaDagNode()
{
}

std::string MayaDagNode::getNodeType()
{
	return "MayaDagNode";
}

void MayaDagNode::writeUpdateCodeThis(NodeWriter& w)
{
	// calc world visibilities from own visibility and parent visibilities
	bool visibility;
	if (this->getInitializerValue("visibility", visibility))
	{
		// visibility is constant

		// iterate over instances and calculate world matrix and visibility
		foreach (Path path, this->getPathElementList("instance"))
		{
			w.push(path);
	
			if (visibility)
				w << "$.worldVisibility = $.parentVisibility;\n";
			else
				w << "$.worldVisibility = false;\n";

			if (path.hasAttribute(".worldInverseMatrix"))
				w << "$.worldInverseMatrix = inv($.worldMatrix);\n";

			w.pop();
		}
	}
	else
	{
		// cache attributes since they are not available inside the foreach loop
		w << "bool $visibility = $.visibility;\n";

		// iterate over instances and calculate world matrix and visibility
		foreach (Path path, this->getPathElementList("instance"))
		{
			w.push(path);
	
			w << "$.worldVisibility = $.parentVisibility & $visibility;\n";

			if (path.hasAttribute(".worldInverseMatrix"))
				w << "$.worldInverseMatrix = inv($.worldMatrix);\n";

			w.pop();
		}
	}
}

void MayaDagNode::addInstance(StringRef instanceString, const Path& parentInstance)
{
	this->addInput(instanceString + ".parentMatrix", "float4x4", parentInstance + ".worldMatrix");
	this->addInput(instanceString + ".parentVisibility", "bool", parentInstance + ".worldVisibility");
	
	// world matrix is equal to parent matrix
	this->addInput(instanceString + ".worldMatrix", "float4x4", parentInstance + ".worldMatrix");
	this->addOutput(instanceString + ".worldVisibility", "bool");
}

} // namespace digi
