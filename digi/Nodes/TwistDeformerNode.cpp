#include <digi/Utility/StringUtility.h>

#include "TwistDeformerNode.h"


namespace digi {

// TwistDeformerNode

TwistDeformerNode::TwistDeformerNode(const Path& worldMatrix)
{
	// world matrix of deformed object
	this->addInput("worldMatrix", "float4x4", worldMatrix);
	
	// world matrix of sine deformer handle
	this->addInput("handleMatrix", "float4x4");

	// parameters
	this->addAttribute("startAngle", "float");
	this->addAttribute("endAngle", "float");
	this->addAttribute("lowBound", "float");
	this->addAttribute("highBound", "float");

	// outputs for vertex node
	this->addOutput("object2handle", "float4x4");
	this->addOutput("handle2object", "float4x4");
/*
	this->addInput("startAngle", "float");
	this->addInput("endAngle", "float");
	this->addInput("lowBound", "float");
	this->addInput("highBound", "float");
	
	// outputs for vertex node
	this->addOutput("projection", "float3");
	this->addOutput("projectionOffset", "float");
	this->addOutput("rotatePivot", "float3");
	this->addOutput("rotateAxis", "float3");
	this->addOutput("angleScale", "float");
	this->addOutput("angleOffset", "float");
*/
}

TwistDeformerNode::~TwistDeformerNode()
{
}

std::string TwistDeformerNode::getNodeType()
{
	return "TwistDeformerNode";
}

void TwistDeformerNode::writeUpdateCodeThis(NodeWriter& w)
{
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

	// handle to object transform
	w << "float4x4 $h2o = inv($matrix);\n";

	// calc rotate pivot in object space (is h2o * [0 0 0 1])
	w << "$.rotatePivot = $h2o.w.xyz;\n";
	
	// calc rotate axis in object space (is h2o * [0 1 0 0])
	w << "$.rotateAxis = normalize($h2o.y.xyz);\n";
	
	// scale and offset for angle calculation
	// angle = startAngle + (h - lowBound) / (highBound - lowBound) * (endAngle - startAngle)
	// angle = h * angleScale + angleOffset
	w << "$.angleScale = ($.endAngle - $.startAngle) / ($.highBound - $.lowBound);\n";
	w << "$.angleOffset = $.startAngle - $.lowBound * $.angleScale;\n";
*/
}


// TwistDeformerBoundingBoxNode

TwistDeformerBoundingBoxNode::TwistDeformerBoundingBoxNode(const Path& inputPath, const Path& parameters)
	: BoundingBoxNode(inputPath)
{
	// add inputs for scene parameters
/*
	this->addInput("rotatePivot", "float3", parameters + ".rotatePivot");
	this->addInput("rotateAxis", "float3", parameters + ".rotateAxis");
*/
}

TwistDeformerBoundingBoxNode::~TwistDeformerBoundingBoxNode()
{
}

std::string TwistDeformerBoundingBoxNode::getNodeType()
{
	return "TwistDeformerBoundingBoxNode";
}

void TwistDeformerBoundingBoxNode::writeUpdateCodeThis(NodeWriter& w)
{
/*
	// size of bounding box is corner - center, i.e. half diameter in each dimension

	// calc new center: project center onto rotation axis
	// calc intersection of auxiliary plane with rotation axis
	// g: x = rotatePivot + r * rotateAxis
	// E: dot(rotatieAxis, x - center) = 0
	// r = dot(rotateAxis, center - rotatePivot)
	w << "float3 $center = $.rotatePivot + dot($.rotateAxis, $.input.center - $.rotatePivot) * $.rotateAxis;\n";


	// calc radius of most distant bounding box corner

	// first calc vector from old center to new center
	w << "float3 $v = $center - $.input.center;\n";
	
	// then follow the three bounding box dimensions so that the vector increases
	w << "float3 $s = $.input.size;\n";
	w << "float3 $w = $v + select($s, -$s, $v < 0.0f);\n";
	
	// project onto rotation axis (like center) and calc distance which is radius
	w << "float3 $p = $.rotatePivot + dot($.rotateAxis, $w - $.rotatePivot) * $.rotateAxis;\n";
	w << "float $r = distance($w, $p);\n";

	
	// calc radius scale factors for each dimension
	// these tell us the width of a unit circle around the rotation axis
	w << "float $sx = length($.rotateAxis.yz);\n";
	w << "float $sy = length($.rotateAxis.zx);\n";
	w << "float $sz = length($.rotateAxis.xy);\n";

	
	// output center and size
	w << "$.output.center = $center;\n";
	w << "$.output.size = $s + $r * vector3($sx, $sy, $sz);\n";
*/
	//!
	w << "$.output.center = $.input.center;\n";
	w << "$.output.size = $.input.size;\n";

}


// TwistDeformerVertexNode

TwistDeformerVertexNode::TwistDeformerVertexNode(const Path& inputPath, const Path& parameters)
	: VertexNode(inputPath)
{
	// add inputs for scene parameters
	this->addInput("startAngle", "float", parameters + ".startAngle");
	this->addInput("endAngle", "float", parameters + ".endAngle");
	this->addInput("lowBound", "float", parameters + ".lowBound");
	this->addInput("highBound", "float", parameters + ".highBound");
	this->addInput("object2handle", "float4x4", parameters + ".object2handle");
	this->addInput("handle2object", "float4x4", parameters + ".handle2object");
/*
	this->addInput("projection", "float3", parameters + ".projection");
	this->addInput("projectionOffset", "float", parameters + ".projectionOffset");
	this->addInput("rotatePivot", "float3", parameters + ".rotatePivot");
	this->addInput("rotateAxis", "float3", parameters + ".rotateAxis");
	this->addInput("angleScale", "float", parameters + ".angleScale");
	this->addInput("angleOffset", "float", parameters + ".angleOffset");
	this->addInput("lowBound", "float", parameters + ".lowBound");
	this->addInput("highBound", "float", parameters + ".highBound");
*/
}

TwistDeformerVertexNode::~TwistDeformerVertexNode()
{
}

std::string TwistDeformerVertexNode::getNodeType()
{
	return "TwistDeformerVertexNode";
}

void TwistDeformerVertexNode::writeUpdateCodeThis(NodeWriter& w)
{
	w << "float4 $p = $.object2handle * vector4($.input.position, 1.0f);\n";
	w << "float4 $t = $.object2handle * vector4($.input.tangent, 0.0f);\n";
	w << "float4 $b = $.object2handle * vector4($.input.bitangent, 0.0f);\n";

	// calc normalized y which is 0 at lowBound and 1 ad highBound
	w << "float $ny = clamp(($p.y - $.lowBound) / ($.highBound - $.lowBound), 0.0f, 1.0f);\n";
	
	// calc y-dependent rotation angle
	w << "float $a = (1.0f - $ny) * $.startAngle + $ny * $.endAngle;\n";

	// calc cos and sin of rotation angle
	w << "float $c = cos($a);\n";
	w << "float $s = sin($a);\n";
	
	// 2d-rotate in x-z plane (counter-clockwise)
	w << "$p = vector4($c * $p.x - $s * $p.z, $p.y, $c * $p.z + $s * $p.x, 1.0f);\n";
	w << "$t = vector4($c * $t.x - $s * $t.z, $t.y, $c * $t.z + $s * $t.x, 0.0f);\n";
	w << "$b = vector4($c * $b.x - $s * $b.z, $b.y, $c * $b.z + $s * $b.x, 0.0f);\n";

	w << "$.output.position = ($.handle2object * $p).xyz;\n";
	w << "$.output.tangent = ($.handle2object * $t).xyz;\n";
	w << "$.output.bitangent = ($.handle2object * $b).xyz;\n";

	/*
		http://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToMatrix/index.htm
		
		deformer f(x, y, z) = (fx, fy, fz)
		
		h = projection.x * x + projection.y * y + projection.z * z + projectionOffset
		
		a = h * angleScale + angleOffset
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

		p.x = x - rotatePivot.x
		p.y = y - rotatePivot.y
		p.z = z - rotatePivot.z
	
		fx = m11 * p.x + m12 * p.y + m13 * p.z + rotatePivot.x
		fy = m21 * p.x + m22 * p.y + m23 * p.z + rotatePivot.y
		fz = m31 * p.x + m32 * p.y + m33 * p.z + rotatePivot.z

		dfx/dx = m11' * p.x + m11 * p.x'
		       = ((1 - c)' * rx * rx + c') * p.x + m11
		       = (sin(a) * projection.x * angleScale * rx * rx + sin(a) * projection.x * angleScale) * p.x + m11
		       = sin(a) * projection.x * angleScale * (rx * rx + 1) * p.x + m11
		dfy/dx = 
		dfz/dx = 
																								 								
		dfx/dy = 
		dfy/dy = 
		dfz/dy = 
																	 								
		dfx/dz = 
		dfy/dz = 
		dfz/dz = 
	*/
/*
	// calc y in handle space
	w << "float $h = dot($.projection, $.input.position) + $.projectionOffset;\n";
	
	// calc angle
	w << "float $a = clamp($h, $.lowBound, $.highBound) * $.angleScale + $.angleOffset;\n";

	// calc sin/cos
	w << "float $c = cos($a);\n";
	w << "float $s = sin($a);\n";
	w << "float $t = 1.0f - $c;\n";
	
	// get rotate axis
	w << "float $rx = $.rotateAxis.x;\n";
	w << "float $ry = $.rotateAxis.y;\n";
	w << "float $rz = $.rotateAxis.z;\n";
	
	// calc rotation matrix
	w << "float3x3 $m = matrix3x3(\n"
		"\t$t*$rx*$rx + $c, $t*$rx*$ry + $rz*$s, $t*$rx*$rz - $ry*$s,\n"
		"\t$t*$rx*$ry - $rz*$s, $t*$ry*$ry + $c, $t*$ry*$rz + $rx*$s,\n"
		"\t$t*$rx*$rz + $ry*$s, $t*$ry*$rz - $rx*$s, $t*$rz*$rz + $c);\n";

	w << "float3 $p = $.input.position - $.rotatePivot;\n";

	w << "$.output.position = $m * $p + $.rotatePivot;\n";

//!
	w << "$.output.tangent = $m * $.input.tangent;\n";
	w << "$.output.bitangent = $m * $.input.bitangent;\n";
*/
}

} // namespace digi
