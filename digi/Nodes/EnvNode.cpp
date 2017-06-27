#include "TextureNode.h"
#include "EnvNode.h"


namespace digi {


// EnvNode

EnvNode::EnvNode(Type type, bool viewSpace, const Path& matrix)
	: Node(2), type(type), viewSpace(viewSpace)
{
	// input color (stop graph traversal at input)
	this->addAttribute("input", "float3", Attribute::INPUT | Attribute::STOP);

	if (!viewSpace)
	{
		if (matrix.node != null)
			this->addInput("matrix", "float4x4", matrix);
		else
			this->addConstant("matrix", "float4x4", matrix.path);
	}
		
	// output color
	this->addOutput("output", "float3");
}

EnvNode::~EnvNode()
{
}

std::string EnvNode::getNodeType()
{
	return "EnvNode";
}

void EnvNode::writeUpdateCodeThis(NodeWriter& w)
{
	// evaluate input and apply projection to any upstream texture nodes
	{
		const int scope = 2;
	
		// generate local variable for inner evaluation
		TargetTypeVisitor targetTypeVisitor;
		this->visit(targetTypeVisitor, "input", scope, scope);
		std::string scope1Name = "inner";		
		targetTypeVisitor.type->writeVariable(w, scope1Name);
		
		// reflect view vector (V) about normal (N)
		w << "float3 $reflected = -reflect(viewVector, normal);\n";
		
		// transform reflected vector into projector space
		if (this->viewSpace)
		{
			w << "float3 $v = $reflected;\n";
		}
		else
		{
			// calc transform from view to projector space
			w << "float4x4 $m = inv(viewMatrix * $.matrix);\n";
		
			// transform input direction into projector space
			w << "float3 $v = transformDirection($m, $reflected);\n";
		}
		
		// transform surface position into projector space (the light is the projector)
		w << "float2 $projection = ";
		switch (this->type)
		{		
		case SPHERICAL_PROJECTION:
			w << "vector2(0.5f - atan2(-$v.x, $v.z) * 0.15915f, atan2(length($v.xz), -$v.y) * 0.31831f);";
			break;
		case SPHERICAL_PROJECTION_YX:
			w << "vector2(atan2(length($v.xz), -$v.y) * 0.31831f, 0.5f - atan2(-$v.x, $v.z) * 0.15915f);";
			break;
		case BALL_PROJECTION:
			w << "normalize(vector3(0.0f, 0.0f, 1.0f) + $v).xy * 0.5f + 0.5f;";
			break;
		}	
		
		// the projection visitor swaps the current code line out and prints it when first needed
		ProjectionVisitor projectionVisitor(w, scope);
		scope1Name.swap(w.scopes[scope]);
		this->visit(projectionVisitor, "input", scope, scope);
		w << "float3 $input = $.input;\n";
		scope1Name.swap(w.scopes[scope]);
	}
	
	w << "$.output = $input;\n";
}

} // namespace digi
