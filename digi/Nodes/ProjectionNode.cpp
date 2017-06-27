#include <digi/Utility/VectorFunctions.h>

#include "ProjectionNode.h"


namespace digi {


// ProjectionNode

ProjectionNode::ProjectionNode(Type type, int flags, const Path& input, const Path& matrix, const Path& parameters)
	: type(type), flags(flags)
{
	// input position or direction
	this->addInput("input", "float3", input);

	if ((flags & VIEW_SPACE) == 0)
	{
		if (matrix.node != null)
			this->addInput("matrix", "float4x4", matrix);
		else
			this->addConstant("matrix", "float4x4", matrix.path);
	}
	
	switch (type)
	{
	case PERSPECTIVE_PROJECTION:
		this->addInput("coneAngle", "float", parameters + ".coneAngle");
		break;
	default:
		;
	}
	
	// output uv
	this->addOutput("output", "float2");
}

ProjectionNode::~ProjectionNode()
{
}

std::string ProjectionNode::getNodeType()
{
	return "ProjectionNode";
}

void ProjectionNode::writeUpdateCodeThis(NodeWriter& w)
{
	if (this->flags & VIEW_SPACE)
	{
		w << "float3 $v = $.input;\n";
	}
	else
	{
		// calc transform from view to projector space
		w << "float4x4 $m = inv(viewMatrix * $.matrix);\n";
	
		// transform input position or direction into projector space
		if (this->flags & INPUT_IS_DIRECTION)
			w << "float3 $v = transformDirection($m, $.input);\n";
		else
			w << "float3 $v = transformPosition($m, $.input);\n";
	}
		
	switch (this->type)
	{
	case PARALLEL_PROJECTION:
		w << "$.output = $v.xy + 0.5f;\n";
		break;
	case PERSPECTIVE_PROJECTION:
		w << "$.output = $v.xy / ($v.z * (tan($.coneAngle * 0.5f) * 2.0f)) + 0.5f;\n";
		break;
	case SPHERICAL_PROJECTION:
		w << "$.output = vector2(0.5f - atan2(-$v.x, $v.z) * 0.15915f, atan2(length($v.xz), -$v.y) * 0.31831f);\n";
		break;
	case SPHERICAL_PROJECTION_YX:
		w << "$.output = vector2(atan2(length($v.xz), -$v.y) * 0.31831f, 0.5f - atan2(-$v.x, $v.z) * 0.15915f);\n";
		break;
	case SPHERICAL_PROJECTION2:
		w << "$.output = vector2(atan2($v.x, $v.y) * -0.31831f, atan2(length($v.xz), -$v.y) * 0.31831f);\n";
		break;
	case BALL_PROJECTION:
		w << "$.output = normalize(vector3(0.0f, 0.0f, 1.0f) + $v).xy * 0.5f + 0.5f;\n";
		break;
	default:
		;
	}
}


} // namespace digi
