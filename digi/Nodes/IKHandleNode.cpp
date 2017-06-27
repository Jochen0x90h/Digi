#include <digi/Utility/StringUtility.h>

#include "IKHandleNode.h"


namespace digi {


IKHandleNode::IKHandleNode(int numJoints)
	: ikJoints(numJoints)
{
	// world matrix of ik handle transform node
	this->addInput("worldMatrix", "float4x4");

	// root joint
	this->addInput("rootParentMatrix", "float4x4");
	this->addInput("rootTranslate", "float3");
	
	// pole vector
	this->addInput("poleVector", "float3");
	
	// output rotate for joints
	for (int i = 0; i < numJoints; ++i)
	{
		this->addOutput(arg("jointRotate%0", i), "float3");
	}

	// internal 2D ik joints
	if (numJoints > 1)
		this->addState("ikJoints", arg("[%0]IK2D::Joint", numJoints));
}

IKHandleNode::~IKHandleNode()
{
}

std::string IKHandleNode::getNodeType()
{
	return "IKHandleNode";
}

void IKHandleNode::writeInitCode(NodeWriter& w, int scope)
{
	// call inherited method
	this->Node::writeInitCode(w, scope);

	// init ikJoints wich is a state
	if (scope == 0)
	{
		w.push(this);
		size_t numJoints = this->ikJoints.size();
		
		// set translations of ik joints (which are constant)
		if (numJoints > 1)
		{
			for (size_t i = 0; i < numJoints; ++i)
			{
				Joint& ikJoint = this->ikJoints[i];
				w << "$.ikJoints[" << i << "].translate = " << ikJoint.translate << ";\n";
			}
		}
		w.pop();
	}
}

void IKHandleNode::writeUpdateCodeThis(NodeWriter& w)
{
	// calc handle vector in world space (vector from root of chain to ik handle)
	w.writeLine("float3 $worldHandleVector = getPosition($.worldMatrix)\n\t- transformPosition($.rootParentMatrix, $.rootTranslate);");
	
	// calc transformation matrix from world space to root parent space
	w.writeLine("float4x4 $worldToRootParent = inv($.rootParentMatrix);");
	
	// calc handle vector in root space (root parent space and root space only differ by rootTranslate)
	w.writeLine("float3 $handleVector = transformDirection($worldToRootParent, $worldHandleVector);");
	
	// calc pole vector in root space
	w.writeLine("float3 $poleVector = transformDirection($worldToRootParent, $.poleVector);");
		
	// calc plane coordinate system in root space (rotate plane of ikRPsolver for 2d-projection of the chain)
	w.writeLine("float3 $planeX = normalize($handleVector);");
	w.writeLine("float3 $planeZ = normalize(cross($handleVector, $poleVector));");
	w.writeLine("float3 $planeY = cross($planeZ, $planeX);");
	
	w.writeLine("float3x3 $planeMatrix = matrix3($planeX, $planeY, $planeZ);");
	w << "float3x3 $restPlaneMatrix = " << transpose(this->restPlaneMatrix) << ";\n";

	w.writeLine("floatQuaternion $planeRotate = getRotation($planeMatrix * $restPlaneMatrix);");

	int numJoints = int(this->ikJoints.size());

	if (numJoints > 1)
	{
		// clear angles of ik joints and calc epsilon
		float ls = 0.0f;
		for (int i = 0; i < numJoints; ++i)
		{
			Joint& ikJoint = this->ikJoints[i];
			w << "$.ikJoints[" << i << "].rotate = 0.0f;\n";
			ls += lengthSquared(ikJoint.translate);
		}
		float epsilon = 1e-6f * ls;
		
		// calc target position
		w.writeLine("float2 $target = vector2(dot($handleVector, $planeX), 0.0f);");

		// run ik
		w << "IK2D::runCCD($.ikJoints, " << numJoints << ", $target, " << epsilon << ");\n";

		// apply rotation to root joint
		{ 
			// the calculated rotations have to be transformed into the joint orientation space 
			
			// plane rotation for root joint:
			// planeRotate * rootJointOrientation = rootJointOrientation * planeJointRotate
			// => planeJointRotate = rootJointOrientation^-1 * planeRotate * rootJointOrientation
			
			// ik rotation for all joints:
			// ikRotate * ikJointOrientation = ikJointOrientation * ikJointRotate
			// => ikJointRotate = ikJointOrientation^-1 * ikRotate * ikJointOrientation

			// total rotate for root joint
			// rootJointRotate = planeJointRotate * ikJointRotate =
			//   rootJointOrientation^-1 * planeRotate * rootJointOrientation
			// * ikJointOrientation^-1 * ikRotate * ikJointOrientation

			Joint& ikJoint = this->ikJoints[0];
		
			w << "$.jointRotate0 = getEuler" << MayaTransformNode::getEulerOrder(ikJoint.rotateMode) << "(\n\t";
			
			// build rootJointRotate
			if (getAngle(this->rootJointOrientation) > 1e-6f)
				w << conj(this->rootJointOrientation) << "\n\t* ";
			
			w << "$planeRotate\n\t* ";
			
			floatQuaternion q1 = this->rootJointOrientation * conj(ikJoint.orientation);
			if (getAngle(q1) > 1e-6f)
				w << q1 << "\n\t* ";
			
			w << "quaternionEulerZ($.ikJoints[0].rotate)";
			
			floatQuaternion q2 = ikJoint.orientation * ikJoint.restRotate;
			if (getAngle(q2) > 1e-6f)
				w << "\n\t* " << q2;

			w << ");\n";
		}
		
		// apply rotation to other joints
		for (int i = 1; i < numJoints; ++i)
		{
			// ik rotate is rotation about Z axis in plane space
			// ikRotate * ikJointOrientation = ikJointOrientation * ikJointRotate
			// => ikJointRotate = ikJointOrientation^-1 * ikRotate * ikJointOrientation

			Joint& ikJoint = this->ikJoints[i];

			w << "$.jointRotate" << ident(i) << " = getEuler" << MayaTransformNode::getEulerOrder(ikJoint.rotateMode) << "(\n\t";
			
			// build jointRotate
			if (getAngle(ikJoint.orientation) > 1e-6f)
				w << conj(ikJoint.orientation) << "\n\t* ";

			w << "quaternionEulerZ($.ikJoints[" << i << "].rotate)";

			floatQuaternion q1 = ikJoint.orientation * ikJoint.restRotate;
			if (getAngle(q1) > 1e-6f)
				w << "\n\t* " << q1;

			w << ");\n";
		}
	}
	else
	{
		// only one joint: ik is not necessary
		Joint& ikJoint = this->ikJoints[0];
		
		w << "$.jointRotate0 = getEuler" << MayaTransformNode::getEulerOrder(ikJoint.rotateMode) << "(";
		
		// build rootJointRotate
		if (getAngle(this->rootJointOrientation) > 1e-6f)
			w << conj(this->rootJointOrientation) << "\n\t* ";
		
		w << "$planeRotate";
		
		floatQuaternion q1 = this->rootJointOrientation * ikJoint.restRotate;
		if (getAngle(q1) > 1e-6f)
			w << "\n\t* " << q1;

		w << ");\n";
	}
}


} // namespace digi
