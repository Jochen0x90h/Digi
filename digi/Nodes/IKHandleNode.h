#ifndef digi_Nodes_IKHandleNode_h
#define digi_Nodes_IKHandleNode_h

#include <digi/Math/All.h>

#include "MayaTransformNode.h"


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	ik handle. defines the ik target and evaluates
	the ik chain
	
	inputs:
		float4x4 rootParentMatrix
		float3 rootTranslate
		float3 poleVector
		float3 jointRotate0,1,2...
		IK2D::Joint[numJoints] ikJoints
*/
class IKHandleNode : public Node
{
public:

	IKHandleNode() {}

	IKHandleNode(int numJoints);

	virtual ~IKHandleNode();

	virtual std::string getNodeType();
	
	virtual void writeInitCode(NodeWriter& w, int scope);

	virtual void writeUpdateCodeThis(NodeWriter& w);

	// plane coordinate system in rest position (must be orthonormal)
	float3x3 restPlaneMatrix;
	
	// orientation of root joint in root space
	floatQuaternion rootJointOrientation;
	
	// IK joint: rotate comes before translate
	struct Joint
	{
		// orientation of joint in plane space
		floatQuaternion orientation;
		
		// rotate of joint in rest position (maya: preferred angle)
		floatQuaternion restRotate;
		
		// rotate mode of joint
		MayaTransformNode::RotateMode rotateMode;
		
		// translate of 2D IK joint in plane space
		float2 translate;
		
		Joint() : rotateMode() {}
	};
	
	std::vector<Joint> ikJoints;
};

/// @}

} // namespace digi

#endif
