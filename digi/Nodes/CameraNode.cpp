#include "CameraNode.h"


namespace digi {

// CameraNode

CameraNode::CameraNode(Type type, Fit fit, float scaleUnit)
	: type(type), fit(fit)
{
	this->addInput("filmSize", "float2");
	this->addInput("filmOffset", "float2");

	this->addInput("nearClipPlane", "float");
	this->addInput("farClipPlane", "float");

	if (type == PERSPECTIVE || type == PERSPECTIVE_STEREO)
		this->addInput("focalLength", "float");
		
	this->addConstant("scaleUnit", scaleUnit);

	// the resulting camera projection (converted to matrix using matrix4x4Projection() in RenderTypes.h)
	this->addOutput("projection", "Projection");
	
	// separation of stereo cameras
	if (type == PERSPECTIVE_STEREO)
	{
		this->addAttribute("separation", "float");
	}
}

CameraNode::~CameraNode()
{
}

std::string CameraNode::getNodeType()
{
	return "CameraNode";
}

void CameraNode::writeUpdateCodeThis(NodeWriter& w)
{
	float mode = float(int(this->fit) + 1);
	if (this->type == ORTHOGRAPHIC)
		mode = -mode;
	w << "$.projection.mode = " << mode << ";\n";

	if (this->type == PERSPECTIVE || this->type == PERSPECTIVE_STEREO)
		w << "$.projection.scale = $.focalLength * $.scaleUnit;\n";
	else if (this->type == ORTHOGRAPHIC)
		w << "$.projection.scale = $.scaleUnit;\n";

	w << "$.projection.filmSize = $.filmSize;\n";
	w << "$.projection.filmOffset = $.filmOffset;\n";
	
	w << "$.projection.nearClipPlane = $.nearClipPlane;\n";
	w << "$.projection.farClipPlane = $.farClipPlane;\n";
}

} // namespace digi
