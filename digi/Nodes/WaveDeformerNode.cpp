#include <digi/Utility/StringUtility.h>

#include "WaveDeformerNode.h"


namespace digi {

// WaveDeformerNode

WaveDeformerNode::WaveDeformerNode(const Path& worldMatrix)
{
	// world matrix of deformed object
	this->addInput("worldMatrix", "float4x4", worldMatrix);
	
	// world matrix of sine deformer handle
	this->addInput("handleMatrix", "float4x4");

	// parameters
	this->addInput("amplitude", "float");
	this->addInput("wavelength", "float");
	this->addInput("offset", "float");

	this->addInput("minRadius", "float");
	this->addInput("maxRadius", "float");
	
	// outputs for vertex node
	this->addOutput("frequency", "float");
	this->addOutput("outOffset", "float");
	this->addOutput("projection", "float2x3");
	this->addOutput("projectionOffset", "float2");
	this->addOutput("direction", "float3");
}

WaveDeformerNode::~WaveDeformerNode()
{
}

std::string WaveDeformerNode::getNodeType()
{
	return "WaveDeformerNode";
}

void WaveDeformerNode::writeUpdateCodeThis(NodeWriter& w)
{
	// calc transform from object space into handle space (object -> world -> handle)
	w << "float4x4 $matrix = inv($.handleMatrix) * $.worldMatrix;\n";
	
	// projection onto xz plane in handle space in which deformation takes place
	// (are used to calculate (matrix * position).xz in the vertex node)
	w << "$.projection = matrix3($matrix.x.xz, $matrix.y.xz, $matrix.z.xz);\n";
	w << "$.projectionOffset = $matrix.w.xz;\n";

	// calc y axis of handle scaled by amplitude in object space which is the displacement direction
	w << "$.direction = inv($matrix).y.xyz * $.amplitude;\n";

	// calc spatial frequency
	w << "$.frequency = 6.2831853f / $.wavelength;\n";

	// in maya the minRadius influences the offset
	w << "$.outOffset = $.offset - $.minRadius;\n";
}


// WaveDeformerBoundingBoxNode

WaveDeformerBoundingBoxNode::WaveDeformerBoundingBoxNode(const Path& inputPath, const Path& parameters)
	: BoundingBoxNode(inputPath)
{
	// add inputs for scene parameters
	this->addInput("direction", "float3", parameters + ".direction");
}

WaveDeformerBoundingBoxNode::~WaveDeformerBoundingBoxNode()
{
}

std::string WaveDeformerBoundingBoxNode::getNodeType()
{
	return "WaveDeformerBoundingBoxNode";
}

void WaveDeformerBoundingBoxNode::writeUpdateCodeThis(NodeWriter& w)
{
	// size of bounding box is corner - center, i.e. half diameter in each dimension
	w << "$.output.center = $.input.center;\n";
	w << "$.output.size = $.input.size + abs($.direction);\n";
}


// WaveDeformerVertexNode

WaveDeformerVertexNode::WaveDeformerVertexNode(const Path& inputPath, const Path& parameters)
	: VertexNode(inputPath)
{
	// add inputs for scene parameters
	this->addInput("projection", "float2x3", parameters + ".projection");
	this->addInput("projectionOffset", "float2", parameters + ".projectionOffset");
	this->addInput("direction", "float3", parameters + ".direction");
	this->addInput("frequency", "float", parameters + ".frequency");
	this->addInput("offset", "float", parameters + ".outOffset");
	this->addInput("minRadius", "float", parameters + ".minRadius");
	this->addInput("maxRadius", "float", parameters + ".maxRadius");
}

WaveDeformerVertexNode::~WaveDeformerVertexNode()
{
}

std::string WaveDeformerVertexNode::getNodeType()
{
	return "WaveDeformerVertexNode";
}

void WaveDeformerVertexNode::writeUpdateCodeThis(NodeWriter& w)
{
	/*
		deformer f(x, y, z) = (fx, fy, fz)
		
		h = projection.x.xy * x + projection.y.xy * y + projection.z.xy * z + projectionOffset.xy
		r = length(h)
		a = frequency * (r + offset)
		
		fx = x + direction.x * cos(a)
		fy = y + direction.y * cos(a)
		fz = z + direction.z * cos(a)

		dfx/dx = 1 + direction.x * -sin(a) * frequency / r * sum(h * projection.x.xy)
		dfy/dx = 0 + direction.y * -sin(a) * frequency / r * sum(h * projection.x.xy)
		dfz/dx = 0 + direction.z * -sin(a) * frequency / r * sum(h * projection.x.xy)
																							
		dfx/dy = 0 + direction.x * -sin(a) * frequency / r * sum(h * projection.y.xy)
		dfy/dy = 1 + direction.y * -sin(a) * frequency / r * sum(h * projection.y.xy)
		dfz/dy = 0 + direction.z * -sin(a) * frequency / r * sum(h * projection.y.xy)
																							
		dfx/dz = 0 + direction.x * -sin(a) * frequency / r * sum(h * projection.z.xy)
		dfy/dz = 0 + direction.y * -sin(a) * frequency / r * sum(h * projection.z.xy)
		dfz/dz = 1 + direction.z * -sin(a) * frequency / r * sum(h * projection.z.xy)
	*/

	w << "float3 $p = $.input.position;\n";
	
	// calc xz in handle space	
	w << "float2 $h = $.projection.x * $p.x + $.projection.y * $p.y + $.projection.z * $p.z + $.projectionOffset;\n";
	
	// calc radius
	w << "float $r = length($h);\n";
	
	// calc angle of sine	
	w << "float $a = $.frequency * (clamp($r, $.minRadius, $.maxRadius) + $.offset);\n";

	// calc all three sum terms
	w << "float3 $hp = vector3(sum($h * $.projection.x), sum($h * $.projection.y), sum($h * $.projection.z));\n";

	w << "float3 $dp = $.direction * cos($a);\n";
	w << "float3 $dt = $.direction * (step($.minRadius, $r) * step($r, $.maxRadius) * sin($a) * $.frequency);\n";

	w << "$.output.position = $.input.position + $dp;\n";
	w << "$.output.tangent = $.input.tangent - $dt * dot($hp, $.input.tangent);\n";
	w << "$.output.bitangent = $.input.bitangent - $dt * dot($hp, $.input.bitangent);\n";
}

} // namespace digi
