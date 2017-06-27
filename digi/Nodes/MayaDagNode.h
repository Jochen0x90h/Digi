#ifndef digi_Nodes_MayaDagNode_h
#define digi_Nodes_MayaDagNode_h

#include <digi/CodeGenerator/Node.h>


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Maya dag node that contains an array of parent and world matrices and visibilities

	inputs:
		visibility (bool)
		instance
			0
				parentMatrix (float4x4)
				parentVisibility (bool)
			1
				...

	outputs:
		instance
			0
				worldMatrix (float4x4)
				worldVisibility (bool)
			1
				...
*/
class MayaDagNode : public Node
{
public:

	MayaDagNode();
	virtual ~MayaDagNode();

	virtual std::string getNodeType();
	
	virtual void writeUpdateCodeThis(NodeWriter& w);

	void addInstance(StringRef instanceString, const Path& parentInstance);
	void addInstance(int instanceIndex, const Path& parentInstance)
	{
		this->addInstance(arg("instance.%0", instanceIndex), parentInstance);
	}
};

/// @}

} // namespace digi

#endif
