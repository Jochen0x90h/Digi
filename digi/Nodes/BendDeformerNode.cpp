#include <digi/Utility/StringUtility.h>

#include "BendDeformerNode.h"


namespace digi {

// BendDeformerNode

BendDeformerNode::BendDeformerNode(const Path& worldMatrix)
{
	// world matrix of deformed object
	this->addInput("worldMatrix", "float4x4", worldMatrix);
	
	// world matrix of sine deformer handle
	this->addInput("handleMatrix", "float4x4");

	// parameters
	this->addAttribute("lowBound", "float");
	this->addAttribute("highBound", "float");
	this->addAttribute("curvature", "float");
	this->addOutput("rp", "float");
	this->addOutput("lb", "float");
	this->addOutput("hb", "float");

	// outputs for vertex node
	this->addOutput("object2handle", "float4x4");
	this->addOutput("handle2object", "float4x4");
/*
	// parameters
	this->addInput("curvature", "float");

	this->addInput("lowBound", "float");
	this->addInput("highBound", "float");
	
	// outputs for vertex node
	this->addOutput("projection", "float3");
	this->addOutput("projectionOffset", "float");
	this->addOutput("xyProjection", "float3");
	this->addOutput("rotatePivot", "float3");
	this->addOutput("rotateAxis", "float3");
	this->addOutput("outLowBound", "float");
	this->addOutput("outHighBound", "float");
*/
}

BendDeformerNode::~BendDeformerNode()
{
}

std::string BendDeformerNode::getNodeType()
{
	return "BendDeformerNode";
}

void BendDeformerNode::writeUpdateCodeThis(NodeWriter& w)
{
	w << "float $c = $.curvature;\n";
	w << "if (abs($c) > 1e-4f)\n";
	w.beginScope();
	{
		// calc rotate pivot along x axis (1/circle radius)
		w << "$.rp = 1.0f / $c;\n";
		w << "$.lb = $.lowBound;\n";
		w << "$.hb = $.highBound;\n";
	}
	w.endScope();
	w << "else\n";
	w.beginScope();
	{
		w << "$.rp = 0.0f;\n";
		w << "$.lb = 0.0f;\n";
		w << "$.hb = 0.0f;\n";
	}
	w.endScope();


	// calc transform from object space into handle space (object -> world -> handle)
	w << "$.object2handle = inv($.handleMatrix) * $.worldMatrix;\n";
	
	// calc transform from handle to object space
	w << "$.handle2object = inv($.object2handle);\n";
/*
	// calc transform from object space into handle space (object -> world -> handle)
	w << "float4x4 $matrix = inv($.handleMatrix) * $.worldMatrix;\n";
	
	// projection of object space position onto y axis of handle space around which rotation takes place
	// (dot(projection, position) + projectionOffset replaces (matrix * vector4(position, 1)).y)
	w << "$.projection = vector3($matrix.x.y, $matrix.y.y, $matrix.z.y);\n";
	w << "$.projectionOffset = $matrix.w.y;\n";

	// calc vector to project point onto xz plane of handle in object space
	w << "$.xyProjection = $.projection / lengthSquared($.projection);\n";

	// handle to object transform
	w << "float4x4 $h2o = inv($matrix);\n";
		
	w << "float $c = $.curvature;\n";
	w << "if (abs($c) > 1e-4f)\n";
	w.beginScope();
	{
		// calc rotate pivot in object space (is position of handle + x-axis / radius in object space)
		w << "$.rotatePivot = $h2o.w.xyz + $h2o.x.xyz / $c;\n";	

		w << "$.outLowBound = $.lowBound;\n";
		w << "$.outHighBound = $.highBound;\n";
	}
	w.endScope();
	w << "else\n";
	w.beginScope();
	{
		w << "$.rotatePivot = 0.0f;\n";

		w << "$.outLowBound = 0.0f;\n";
		w << "$.outHighBound = 0.0f;\n";
	}
	w.endScope();
	
	// calc rotate axis in object space (is normalized negative z-axis)
	w << "$.rotateAxis = normalize(-$h2o.z.xyz);\n";
*/
}


// BendDeformerBoundingBoxNode

BendDeformerBoundingBoxNode::BendDeformerBoundingBoxNode(const Path& inputPath, const Path& parameters)
	: BoundingBoxNode(inputPath)
{
	// add inputs for scene parameters
	//!
}

BendDeformerBoundingBoxNode::~BendDeformerBoundingBoxNode()
{
}

std::string BendDeformerBoundingBoxNode::getNodeType()
{
	return "BendDeformerBoundingBoxNode";
}

void BendDeformerBoundingBoxNode::writeUpdateCodeThis(NodeWriter& w)
{
	//!
	w << "$.output.center = $.input.center;\n";
	w << "$.output.size = $.input.size;\n";
}


// BendDeformerVertexNode

BendDeformerVertexNode::BendDeformerVertexNode(const Path& inputPath, const Path& parameters)
	: VertexNode(inputPath)
{
	// add inputs for scene parameters
	this->addInput("curvature", "float", parameters + ".curvature");
	this->addInput("rp", "float", parameters + ".rp");
	this->addInput("lb", "float", parameters + ".lb");
	this->addInput("hb", "float", parameters + ".hb");
	this->addInput("object2handle", "float4x4", parameters + ".object2handle");
	this->addInput("handle2object", "float4x4", parameters + ".handle2object");
/*
	// add inputs for scene parameters
	this->addInput("projection", "float3", parameters + ".projection");
	this->addInput("projectionOffset", "float", parameters + ".projectionOffset");
	this->addInput("xyProjection", "float3", parameters + ".xyProjection");
	this->addInput("rotatePivot", "float3", parameters + ".rotatePivot");
	this->addInput("rotateAxis", "float3", parameters + ".rotateAxis");
	this->addInput("curvature", "float", parameters + ".curvature");
	this->addInput("lowBound", "float", parameters + ".outLowBound");
	this->addInput("highBound", "float", parameters + ".outHighBound");
*/
}

BendDeformerVertexNode::~BendDeformerVertexNode()
{
}

std::string BendDeformerVertexNode::getNodeType()
{
	return "BendDeformerVertexNode";
}

void BendDeformerVertexNode::writeUpdateCodeThis(NodeWriter& w)
{
	w << "float4 $p = $.object2handle * vector4($.input.position, 1.0f);\n";
	w << "float4 $t = $.object2handle * vector4($.input.tangent, 0.0f);\n";
	w << "float4 $b = $.object2handle * vector4($.input.bitangent, 0.0f);\n";

	// clamp y
	w << "float $yc = clamp($p.y, $.lb, $.hb);\n";

	// calc y-dependent rotation angle
	w << "float $a = $yc * $.curvature;\n";

	// calc cos and sin of rotation angle
	w << "float $c = cos($a);\n";
	w << "float $s = sin($a);\n";
	
	// calc x and y of point before rotate. x is relative to rotate pivot, y is zero when between lowBound and highBound
	w << "float $x = $p.x - $.rp;\n";
	w << "float $y = $p.y - $yc;\n";
	
	// 2d-rotate in x-y plane (clockwise)
	w << "$p = vector4($c * $x + $s * $y + $.rp, $c * $y - $s * $x, $p.z, 1.0f);\n";
	w << "$t = vector4($c * $t.x + $s * $t.y, $c * $t.y - $s * $t.x, $t.z, 0.0f);\n";
	w << "$b = vector4($c * $b.x + $s * $b.y, $c * $b.y - $s * $b.x, $b.z, 0.0f);\n";

	w << "$.output.position = ($.handle2object * $p).xyz;\n";
	w << "$.output.tangent = ($.handle2object * $t).xyz;\n";
	w << "$.output.bitangent = ($.handle2object * $b).xyz;\n";

	/*
		http://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToMatrix/index.htm
		
		deformer f(x, y, z) = (fx, fy, fz)
		
		h = projection.x * x + projection.y * y + projection.z * z + projectionOffset
		
		a = h * curvature
		c = cos(a)
		s = sin(a)
		t = 1 - c
	
		rx = rotateAxis.x
		ry = rotateAxis.y
		rz = rotateAxis.z
	
		m11 = t * rx * rx + c
		m12 = t * rx * ry - s * rz
		m13 = t * rx * rz + s * ry

		m21 = t * ry * rx - s * rz
		m22 = t * ry * ry + c
		m23 = t * ry * rz + s * rx

		m31 = t * rz * rx + s * ry
		m32 = t * rz * ry - s * rx
		m33 = t * rz * rz + c

		p.x = x - h * xyProjection.x - rotatePivot.x
		p.y = y - h * xyProjection.y - rotatePivot.y
		p.z = z - h * xyProjection.z - rotatePivot.z
	
		fx = m11 * p.x + m12 * p.y + m13 * p.z + rotatePivot.x
		fy = m21 * p.x + m22 * p.y + m23 * p.z + rotatePivot.y
		fz = m31 * p.x + m32 * p.y + m33 * p.z + rotatePivot.z
	*/
/*
	// calc height (y) in handle space
	w << "float $h = dot($.projection, $.input.position) + $.projectionOffset;\n";
	
	// clamp height
	w << "float $hc = clamp($h, $.lowBound, $.highBound);\n";

	// calc angle
	w << "float $a = $hc * $.curvature;\n";

	// calc sin/cos
	w << "float $c = cos($a);\n";
	w << "float $s = sin($a);\n";
	w << "float $t = 1.0f - $c;\n";
	
	// get rotate axis
	w << "float $rx = $.rotateAxis.x;\n";
	w << "float $ry = $.rotateAxis.y;\n";
	w << "float $rz = $.rotateAxis.z;\n";
	
	// calc rotation matrix
	w << "float3x3 $m = matrix3x3("
		"$t*$rx*$rx + $c, $t*$rx*$ry + $rz*$s, $t*$rx*$rz - $ry*$s, "
		"$t*$rx*$ry - $rz*$s, $t*$ry*$ry + $c, $t*$ry*$rz + $rx*$s, "
		"$t*$rx*$rz + $ry*$s, $t*$ry*$rz - $rx*$s, $t*$rz*$rz + $c);";

	w << "float3 $p = $.input.position - $hc * $.xyProjection - $.rotatePivot;\n";

	w << "$.output.position = $m * $p + $.rotatePivot;\n";

//!
	w << "$.output.tangent = $m * $.input.tangent;\n";
	w << "$.output.bitangent = $m * $.input.bitangent;\n";
*/
}

} // namespace digi
