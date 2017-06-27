#include <digi/Utility/StringUtility.h>

#include "MayaTransformNode.h"


namespace digi {


MayaTransformNode::MayaTransformNode(Type type)
	: type(type), rotateMode(ROTATE_EULER_XYZ), rotateOrientation(floatQuaternionIdentity()), jointOrientation(floatQuaternionIdentity())
{
	// visibility is added by constructor of MayaDagNode
	
	// position
	this->addInput("translate", "float3");
	
	// rotation
	this->addInput("rotate", "float3");

	// shear
	if (type != TYPE_JOINT)
		this->addInput("shear", "float3");

	// scale
	this->addInput("scale", "float3");

	// parent scale
	if (type == TYPE_JOINT)
	{
		this->addInput("parentScale", "float3");
		this->setInitializer("parentScale", splat3(1.0f));
	}
	
	this->addOutput("matrix", "float4x4");
}

MayaTransformNode::~MayaTransformNode()
{
}

std::string MayaTransformNode::getNodeType()
{
	return "MayaTransformNode";
}

void MayaTransformNode::optimize(OptimizePhase phase)
{
	// call inherited method
	this->MayaDagNode::optimize(phase);

	if (phase == OPTIMIZE_ATTRIBUTES)
	{
		// if matrix output is neither a data source for other attributes nor in the interface then we can remove it
		Path matrix = this->findAttribute("matrix", FIND_TYPED);
		if (!matrix.isNull() && !matrix.isDataSource() && (matrix.getFlags() & Attribute::INTERFACE) == 0)
			this->removeNode(matrix.node);
	}
}

void MayaTransformNode::writeUpdateCodeThis(NodeWriter& w)
{
	bool matrixDone = false;

	if (this->type == TYPE_TRANSFORM)
	{
		/*
			matrix = T * Rt * Rp * R * Ro * Rp^-1 * St * Sp * Sh * S * Sp^-1
			T: translate
			Rt: rotate pivot translation
			Rp: rotate pivot point
			R: rotate
			Ro: rotate orientation
			St: scale pivot translation
			Sp: scale pivot point
			Sh: shear in xy, zx, yz
			S: scale
		*/

		float3 translate;
		float3 rotate;
		float3 shear;
		float3 scale;
		bool isConstant =
			this->getInitializerValue("translate", translate)
			& this->getInitializerValue("rotate", rotate)
			& this->getInitializerValue("shear", shear)
			& this->getInitializerValue("scale", scale);
		if (isConstant)
		{
			// transform inputs are constant

			floatQuaternion rotateQuaternion;
			switch (this->rotateMode)
			{
			case ROTATE_EULER_XYZ:
				rotateQuaternion = quaternionEulerXYZ(rotate);
				break;
			case ROTATE_EULER_YZX:
				rotateQuaternion = quaternionEulerYZX(rotate);
				break;
			case ROTATE_EULER_ZXY:
				rotateQuaternion = quaternionEulerZXY(rotate);
				break;
			case ROTATE_EULER_XZY:
				rotateQuaternion = quaternionEulerXZY(rotate);
				break;
			case ROTATE_EULER_YXZ:
				rotateQuaternion = quaternionEulerYXZ(rotate);
				break;
			default:
				rotateQuaternion = quaternionEulerZYX(rotate);
			}

			float4x4 matrix = matrix4x4TranslateRotate(
				translate + this->rotatePivotTranslation + this->rotatePivotPoint,
				rotateQuaternion * this->rotateOrientation)
				* matrix4x4TranslateShearScale(
					-this->rotatePivotPoint + this->scalePivotTranslation + this->scalePivotPoint,
					shear,
					scale)
				* matrix4x4Translate(-this->scalePivotPoint);
			
			// check if matrix is identity
			if (max(max(abs(matrix - float4x4Identity()))) < 1e-6f)
			{
				if (this->hasAttribute("matrix"))
					w << "$.matrix = " << float4x4Identity() << ";\n";
	
				// iterate over instances and calculate world matrix and visibility
				foreach (Path path, this->getPathElementList("instance"))
				{
					w.push(path);	
					w << "$.worldMatrix = $.parentMatrix;\n";
					w.pop();
				}
				
				// mark that matrix is done
				matrixDone = true;
			}
			else
			{
				w << "float4x4 $matrix = " << matrix << ";\n";
			}
		}
		else
		{
			// translate (T)
			w << "float4x4 $matrix = matrix4x4TranslateRotate(\n\t$.translate";
		
			// translate offset (Rt * Rp)
			float3 to = this->rotatePivotTranslation + this->rotatePivotPoint;
			if (length(to) > 1e-6f)
				w << " + " << to;
			w << ",\n\t";
		
			// rotate
			w << "quaternionEuler" << MayaTransformNode::getEulerOrder(this->rotateMode) <<  "($.rotate)";
		
			// rotate orientation (Ro)
			if (getAngle(this->rotateOrientation) > 1e-6f)
				w << " * " << this->rotateOrientation;
				
			// inverse rotate pivot point, scale pivot translation, scale pivot point (Rp^-1 * St * Sp)
			w << ")\n\t* matrix4x4TranslateShearScale(\n\t\t" << (-this->rotatePivotPoint + this->scalePivotTranslation + this->scalePivotPoint);

			// shear, scale (Sh * S)
			w << ",\n\t\t$.shear,\n\t\t$.scale)";
		
			// inverse scale pivot point (Sp^-1)
			if (length(this->scalePivotPoint) > 1e-6f)
				w << "\n\t* matrix4x4Translate(" << (-this->scalePivotPoint) << ")";
			w << ";\n";
		}
	}
	if (this->type == TYPE_JOINT)
	{
		/*
			matrix = T * iS * Jo * R * Ro * S
			T: translate
			iS: inverse parent scale
			Jo: joint orientation
			R: rotate
			Ro: rotate orientation
			S: scale
		*/
		
		// translate (T)
		w << "float4x4 $matrix = matrix4x4TranslateMatrix3x3(\n\t$.translate,\n\t";
		
		// inverse parent scale (iS)
		w << "matrix3x3Scale(splat3(1.0f) / $.parentScale)\n\t";
		
		// total roate (Jo * R * Ro)
		w << "* matrix3x3Rotate(\n\t\t";
		
		// joint orientation (Jo)
		if (getAngle(this->jointOrientation) > 1e-10f)
			w << this->jointOrientation << "\n\t\t* ";

		// rotate (R)
		w << "quaternionEuler" << MayaTransformNode::getEulerOrder(this->rotateMode) <<  "($.rotate)";

		// rotate orientation (Ro)
		if (getAngle(this->rotateOrientation) > 1e-10f)
			w << "\n\t\t* " << this->rotateOrientation;

		w << ")\n\t";

		// scale (S)
		w << "* matrix3x3Scale($.scale));\n";
	}			
	
	if (!matrixDone)
	{
		if (this->hasAttribute("matrix"))
			w << "$.matrix = $matrix;\n";
	
		// iterate over instances and calculate world matrix
		foreach (Path path, this->getPathElementList("instance"))
		{
			w.push(path);
			w << "$.worldMatrix = $.parentMatrix * $matrix;\n";
			w.pop();
		}
	}

	// call inherited method to calculate visibility and optionally the parent inverse matrix
	this->MayaDagNode::writeUpdateCodeThis(w);
}

void MayaTransformNode::addInstance(StringRef instanceString, const Path& parentInstance, bool inheritsTransform)
{
	if (inheritsTransform)
		this->addInput(instanceString + ".parentMatrix", "float4x4", parentInstance + ".worldMatrix");
	else
		this->addAttributeWithInitializer(instanceString + ".parentMatrix", float4x4Identity(), Attribute::INPUT);
	this->addInput(instanceString + ".parentVisibility", "bool", parentInstance + ".worldVisibility");
	
	this->addOutput(instanceString + ".worldMatrix", "float4x4");
	this->addOutput(instanceString + ".worldVisibility", "bool");
	
	//this->addOutput(arg("instance.%0.parentInverseMatrix", instanceIndex), "float4x4");
}

const char* MayaTransformNode::getEulerOrder(RotateMode rotateMode)
{
	static const char* eulerOrders[] =
	{
		"XYZ",
		"YZX",
		"ZXY",
		"XZY",
		"YXZ",
		"ZYX"
	};
	
	int index = clamp(int(rotateMode - ROTATE_EULER_XYZ), 0, 5);
	return eulerOrders[index];
}


floatQuaternion MayaTransformNode::quaternionEuler(RotateMode rotateMode, float3 angles)
{
	switch (rotateMode)
	{
		case MayaTransformNode::ROTATE_EULER_XYZ:
			return quaternionEulerXYZ(angles);
			break;
		case MayaTransformNode::ROTATE_EULER_YZX:
			return quaternionEulerYZX(angles);
			break;
		case MayaTransformNode::ROTATE_EULER_ZXY:
			return quaternionEulerZXY(angles);
			break;
		case MayaTransformNode::ROTATE_EULER_XZY:
			return quaternionEulerXZY(angles);
			break;
		case MayaTransformNode::ROTATE_EULER_YXZ:
			return quaternionEulerYXZ(angles);
			break;
		case MayaTransformNode::ROTATE_EULER_ZYX:
			return quaternionEulerZYX(angles);
			break;
	}
	return floatQuaternionIdentity();
}


} // namespace digi
