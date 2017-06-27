#include <digi/Utility/StringFunctions.h>

#include "JointNode.h"


namespace digi {


// JointNode

JointNode::JointNode(const Path& parentMatrix, const Path& parentScaleMatrix,
	TranslateMode translateMode, RotateMode rotateMode, ScaleMode scaleMode)
	: translateMode(translateMode), rotateMode(rotateMode), scaleMode(scaleMode), 
	jointOrientation(0, 0, 0, 1), rotateOrientation(0, 0, 0, 1)
{
	// parent matrix
	this->addInput("parentMatrix", "float4x4", parentMatrix);

	// parent visibility
	this->addInput("parentVisibility", "bool");

	// parent scale
	if (!parentScaleMatrix.isNull())
		this->addInput("parentScaleMatrix", "float3x3", parentScaleMatrix);
		
	// position
	this->addInput("translate", JointNode::getTranslateType(translateMode));
	
	// rotation
	this->addInput("rotate", JointNode::getRotateType(rotateMode));

	// scale
	this->addInput("scale", JointNode::getScaleType(scaleMode));

	// output scale matrix
	this->addOutput("scaleMatrix", "float3x3");
	
	// visibility
	this->addInput("visibility", "bool");

	// output transformation matrix
	this->addOutput("worldMatrix", "float4x4");

	// output visibility
	this->addOutput("worldVisibility", "bool");
}

JointNode::~JointNode()
{
}

std::string JointNode::getNodeType()
{
	return "JointNode";
}

void JointNode::generateUpdateCodeThis(NodeWriter& w)
{
	// P * T * iS * Jo * R * Ro * S
	
	// rotate (R)
	std::string rotate = this->getRotate();
	
	// joint orientation (Jo)
	if (getAngle(this->jointOrientation) > 1e-10f)
		rotate = toCode(this->jointOrientation) + "\n\t\t* " + rotate;

	// rotate orientation (Ro)
	if (getAngle(this->rotateOrientation) > 1e-10f)
		rotate += " * " + toCode(this->rotateOrientation);

	std::string m3x3 = "matrix3x3Rotate(\n\t\t" + rotate + ")";
	
	if (this->hasAttribute("parentScaleMatrix"))
	{
		m3x3 = "inv($.parentScaleMatrix) * " + m3x3;
	}
		
	if (this->scaleMode == SCALE_NONE)
	{
		w.writeLine("$.scaleMatrix = matrix3x3Scale(1.0f);");
		m3x3 += " * $.scaleMatrix";
	}
	else
	{
		w.writeLine("$.scaleMatrix = matrix3x3Scale($.scale);");
		m3x3 += " * $.scaleMatrix";
	}
	
	w.writeLine("$.worldMatrix = $.parentMatrix * matrix4x4TranslateMatrix3x3(\n\t"
		+ this->getTranslate() + ",\n\t"
		+ m3x3 + ");");

	// visibility
	w << "$.worldVisibility = $.parentVisibility & $.visibility;\n";
}

std::string JointNode::getTranslate()
{
	// return position component as string
	std::string translate;
	switch (this->translateMode)
	{
		case TRANSLATE_X:
			translate = "vector3($.translate, 0.0f, 0.0f)";
			break;
		case TRANSLATE_Y:
			translate = "vector3(0.0f, $.translate, 0.0f)";
			break;
		case TRANSLATE_Z:
			translate = "vector3(0.0f, 0.0f, $.translate)";
			break;
		case TRANSLATE_GENERAL:
			translate = "$.translate";
			break;
	}
	return translate;
}		

std::string JointNode::getRotate()
{
	// return rotation component as string
	std::string rotate;
	switch (this->rotateMode)
	{
		case ROTATE_HINGE_X:
		case ROTATE_HINGE_Y:
		case ROTATE_HINGE_Z:
			rotate = std::string("quaternionEuler")
				+ JointNode::getAxis(this->rotateMode - JointNode::ROTATE_HINGE_X)
				+ "($.rotate)";
			break;
		case ROTATE_SADDLE_YZ:
		case ROTATE_SADDLE_ZX:
		case ROTATE_SADDLE_XY:
			rotate = std::string("quaternionSaddle")
				+ JointNode::getAxis(this->rotateMode - JointNode::ROTATE_SADDLE_YZ + 1)
				+ JointNode::getAxis(this->rotateMode - JointNode::ROTATE_SADDLE_YZ + 2)
				+ "($.rotate)";
			break;
		case ROTATE_BALL_XYZ:
		case ROTATE_BALL_YZX:
		case ROTATE_BALL_ZXY:
			rotate = std::string("quaternionSaddle")
				+ JointNode::getAxis(this->rotateMode - JointNode::ROTATE_BALL_XYZ + 1)
				+ JointNode::getAxis(this->rotateMode - JointNode::ROTATE_BALL_XYZ + 2)
				+ "($.rotate$:yz)\n\t\t* quaternionEuler"
				+ JointNode::getAxis(this->rotateMode - JointNode::ROTATE_BALL_XYZ)
				+ "($.rotate.x)";
			break;
		case ROTATE_RQ:
			rotate = "quaternionRQ($.rotate)";
			break;
		case ROTATE_EULER_XYZ:
		case ROTATE_EULER_YZX:
		case ROTATE_EULER_ZXY:
		case ROTATE_EULER_XZY:
		case ROTATE_EULER_YXZ:
		case ROTATE_EULER_ZYX:
			rotate = std::string("quaternionEuler") + JointNode::getEulerOrder(this->rotateMode) + "($.rotate)";
			break;
		case ROTATE_GENERAL:
			rotate = "$.rotate";
			break;
	}
	return rotate;
}


int JointNode::getNumTranslateComponents(TranslateMode translateMode)
{
	static int numComponents[] =
	{
		0, // TRANSLATE_NONE
		1, // TRANSLATE_X
		1, // TRANSLATE_Y
		1, // TRANSLATE_Z
		3, // TRANSLATE_GENERAL
	};
	
	return numComponents[int(translateMode)];
}

std::string JointNode::getTranslateType(TranslateMode translateMode)
{
	static const char* types[] =
	{
		"",       // TRANSLATE_NONE
		"float",  // TRANSLATE_X
		"float",  // TRANSLATE_Y
		"float",  // TRANSLATE_Z
		"float3", // TRANSLATE_GENERAL
	};

	return types[int(translateMode)];
}

int JointNode::getNumRotateComponents(RotateMode rotateMode)
{
	static int numComponents[] =
	{
		0, // ROTATE_NONE
		1, // ROTATE_HINGE_X
		1, // ROTATE_HINGE_Y
		1, // ROTATE_HINGE_Z
		2, // ROTATE_SADDLE_YZ
		2, // ROTATE_SADDLE_ZX
		2, // ROTATE_SADDLE_XY
		3, // ROTATE_BALL_XYZ
		3, // ROTATE_BALL_YZX
		3, // ROTATE_BALL_ZXY
		3, // ROTATE_RQ
		3, // ROTATE_EULER_XYZ
		3, // ROTATE_EULER_YZX
		3, // ROTATE_EULER_ZXY
		3, // ROTATE_EULER_XZY
		3, // ROTATE_EULER_YXZ
		3, // ROTATE_EULER_ZYX
		4, // ROTATE_GENERAL
	};

	return numComponents[int(rotateMode)];
}

std::string JointNode::getRotateType(RotateMode rotateMode)
{
	static const char* types[] =
	{
		"",            // ROTATE_NONE
		"float",       // ROTATE_HINGE_X
		"float",       // ROTATE_HINGE_Y
		"float",       // ROTATE_HINGE_Z
		"float2",      // ROTATE_SADDLE_YZ
		"float2",      // ROTATE_SADDLE_ZX
		"float2",      // ROTATE_SADDLE_XY
		"float3",      // ROTATE_BALL_XYZ
		"float3",      // ROTATE_BALL_YZX
		"float3",      // ROTATE_BALL_ZXY
		"float3",      // ROTATE_RQ
		"float3",      // ROTATE_EULER_XYZ
		"float3",      // ROTATE_EULER_YZX
		"float3",      // ROTATE_EULER_ZXY
		"float3",      // ROTATE_EULER_XZY
		"float3",      // ROTATE_EULER_YXZ
		"float3",      // ROTATE_EULER_ZYX
		"floatQuaternion", // ROTATE_GENERAL
	};

	return types[int(rotateMode)];
}

char JointNode::getAxis(int axis)
{
	static const char axes[] = {'X', 'Y', 'Z'};
	return axes[(axis % 3)];
}

const char* JointNode::getEulerOrder(RotateMode rotateMode)
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

int JointNode::getNumScaleComponents(ScaleMode scaleMode)
{
	static int numComponents[] =
	{
		0, // SCALE_NONE
		1, // SCALE_UNIFORM
		3, // SCALE_AXIS
	};
	
	return numComponents[int(scaleMode)];
}

std::string JointNode::getScaleType(ScaleMode scaleMode)
{
	static const char* types[] =
	{
		"",         // SCALE_NONE
		"float",    // SCALE_UNIFORM
		"float3",   // SCALE_AXIS
	};

	return types[int(scaleMode)];
}



} // namespace digi
