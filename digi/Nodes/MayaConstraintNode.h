#ifndef digi_Nodes_MayaConstraintNode_h
#define digi_Nodes_MayaConstraintNode_h

#include <digi/Math/All.h>

#include "MayaTransformNode.h"


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Maya parent constraint. Mixes the transformation of all targets

	inputs:
		target[]
			parentMatrix
			translate
			rotate
			weight
		constraintParentMatrix (parent matrix of constrained node)
	outputs:
		outputTranslate (float3, optional)
		outputRotate (float3, optional)
		outputScale (float3, optional)
*/
class MayaConstraintNode : public Node
{
public:

	enum Type
	{
		// point, orient and/or scale constraint, parent constraint
		P_O_S,

		// aim constraint
		AIM,
	};

	MayaConstraintNode(Type type, int numTargets);

	virtual ~MayaConstraintNode();

	virtual std::string getNodeType();
		

	virtual void writeUpdateCodeThis(NodeWriter& w);

	struct Target
	{
		float3 pivotTranslate;
		floatQuaternion jointOrientation;
		MayaTransformNode::RotateMode rotateMode;

		float3 translateOffset;
		floatQuaternion rotateOffset;
		
		Target()
			: rotateMode(MayaTransformNode::ROTATE_EULER_XYZ) {}
	};

	Type type;
	std::vector<Target> targets;
		
	floatQuaternion constraintJointOrientation;
	MayaTransformNode::RotateMode constraintRotateMode;

	float3 translateOffset;
	floatQuaternion rotateOffset;
	float3 scaleOffset;
};

/// @}

} // namespace digi

#endif
