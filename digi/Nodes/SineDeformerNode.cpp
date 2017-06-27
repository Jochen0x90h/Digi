#include <digi/Utility/StringUtility.h>

#include "SineDeformerNode.h"


namespace digi {

// SineDeformerNode

SineDeformerNode::SineDeformerNode(const Path& worldMatrix)
{
	// world matrix of deformed object
	this->addInput("worldMatrix", "float4x4", worldMatrix);
	
	// world matrix of sine deformer handle
	this->addInput("handleMatrix", "float4x4");

	// parameters
	this->addInput("amplitude", "float");
	this->addInput("wavelength", "float");
	this->addInput("offset", "float");

	this->addInput("lowBound", "float");
	this->addInput("highBound", "float");
	
	// outputs for vertex node
	this->addOutput("projection", "float3");
	this->addOutput("projectionOffset", "float");
	this->addOutput("direction", "float3");
	this->addOutput("frequency", "float");
}

SineDeformerNode::~SineDeformerNode()
{
}

std::string SineDeformerNode::getNodeType()
{
	return "SineDeformerNode";
}

void SineDeformerNode::writeUpdateCodeThis(NodeWriter& w)
{
	// calc transform from object space into handle space (object -> world -> handle)
	w << "float4x4 $matrix = inv($.handleMatrix) * $.worldMatrix;\n";
	
	// projection onto y axis in handle space along which deformation takes place
	// (in the vertex node we calculate (matrix * position).y)
	w << "$.projection = vector3($matrix.x.y, $matrix.y.y, $matrix.z.y);\n";
	w << "$.projectionOffset = $matrix.w.y;\n";

	// calc x axis of handle scaled by amplitude in object space which is the displacement direction
	w << "$.direction = inv($matrix).x.xyz * $.amplitude;\n";

	// calc frequency of sine wave
	w << "$.frequency = 6.2831853f / $.wavelength;\n";
}


// SineDeformerBoundingBoxNode

SineDeformerBoundingBoxNode::SineDeformerBoundingBoxNode(const Path& inputPath, const Path& parameters)
	: BoundingBoxNode(inputPath)
{
	// add inputs for scene parameters
	this->addInput("direction", "float3", parameters + ".direction");
}

SineDeformerBoundingBoxNode::~SineDeformerBoundingBoxNode()
{
}

std::string SineDeformerBoundingBoxNode::getNodeType()
{
	return "SineDeformerBoundingBoxNode";
}

void SineDeformerBoundingBoxNode::writeUpdateCodeThis(NodeWriter& w)
{
	// size of bounding box is corner - center, i.e. half diameter in each dimension
	w << "$.output.center = $.input.center;\n";
	w << "$.output.size = $.input.size + abs($.direction);\n";
}


// SineDeformerVertexNode

SineDeformerVertexNode::SineDeformerVertexNode(const Path& inputPath, const Path& parameters)
	: VertexNode(inputPath)
{
	// add inputs for scene parameters
	this->addInput("projection", "float3", parameters + ".projection");
	this->addInput("projectionOffset", "float", parameters + ".projectionOffset");
	this->addInput("direction", "float3", parameters + ".direction");
	this->addInput("frequency", "float", parameters + ".frequency");
	this->addInput("offset", "float", parameters + ".offset");
	this->addInput("lowBound", "float", parameters + ".lowBound");
	this->addInput("highBound", "float", parameters + ".highBound");
}

SineDeformerVertexNode::~SineDeformerVertexNode()
{
}

std::string SineDeformerVertexNode::getNodeType()
{
	return "SineDeformerVertexNode";
}

void SineDeformerVertexNode::writeUpdateCodeThis(NodeWriter& w)
{
	/*
		deformer f(x, y, z) = (fx, fy, fz)
		
		h = projection.x * x + projection.y * y + projection.z * z + projectionOffset
		a = frequency * (h + offset)
		
		fx = x + direction.x * sin(a)
		fy = y + direction.y * sin(a)
		fz = z + direction.z * sin(a)

		dfx/dx = 1 + direction.x * cos(a) * frequency * projection.x
		dfy/dx = 0 + direction.y * cos(a) * frequency * projection.x
		dfz/dx = 0 + direction.z * cos(a) * frequency * projection.x
																								 								
		dfx/dy = 0 + direction.x * cos(a) * frequency * projection.y
		dfy/dy = 1 + direction.y * cos(a) * frequency * projection.y
		dfz/dy = 0 + direction.z * cos(a) * frequency * projection.y
																								 								
		dfx/dz = 0 + direction.x * cos(a) * frequency * projection.z
		dfy/dz = 0 + direction.y * cos(a) * frequency * projection.z
		dfz/dz = 1 + direction.z * cos(a) * frequency * projection.z
	*/

	// calc y in handle space
	w << "float $h = dot($.projection, $.input.position) + $.projectionOffset;\n";
	
	// calc angle of sine
	w << "float $a = $.frequency * (clamp($h, $.lowBound, $.highBound) + $.offset);\n";

	w << "float3 $dp = $.direction * sin($a);\n";
	w << "float3 $dt = $.direction * (step($.lowBound, $h) * step($h, $.highBound) * cos($a) * $.frequency);\n";

	w << "$.output.position = $.input.position + $dp;\n";
	w << "$.output.tangent = $.input.tangent + $dt * dot($.projection, $.input.tangent);\n";
	w << "$.output.bitangent = $.input.bitangent + $dt * dot($.projection, $.input.bitangent);\n";
}

} // namespace digi
