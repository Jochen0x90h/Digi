#include "GeometryNode.h"


namespace digi {

// GeometryNode

GeometryNode::GeometryNode(const Path& tangentSpaceNormal, bool outputTangent)
	: Node(2), hasTangentSpace(true)
{
	if (!tangentSpaceNormal.isNull())
		this->addInput("tangentSpaceNormal", "float3", tangentSpaceNormal);

	this->addOutput("surfacePosition", "float3");
	this->addOutput("viewVector", "float3"); // V
	
	if (outputTangent)
	{
		this->addOutput("tangent", "float3"); // T
		this->addOutput("bitangent", "float3"); // B
	}
	this->addOutput("normal", "float3"); // N	
}

GeometryNode::~GeometryNode()
{
}

std::string GeometryNode::getNodeType()
{
	return "GeometryNode";
}

void GeometryNode::writeUpdateCodeThis(NodeWriter& w)
{
	// calc world view matrix
	w << "float4x4 $worldViewMatrix = viewMatrix * worldMatrix;\n";
		
	// calc surface position in view space
	w << "float4 $position = vector4(transformPosition($worldViewMatrix, input.position), 1.0f);\n";
		
	// output view space position for user-defined clip planes
	w << "vPosition = $position;\n";

	// output projected position
	w << "pPosition = projectionMatrix * $position;\n";
	
	// output surface position for lighting calculations
	w << "$.surfacePosition = $position.xyz;\n";

	// view vector (V) in view space (points from surface to camera)
	w << "$.viewVector = -normalize($.surfacePosition);\n";
	
	if (!this->hasTangentSpace)
	{
		// input normal
		
		// calc normal matrix: inverse transpose of rotation component (omit division by determinant)
		w << "float3 $x = $worldViewMatrix.x.xyz;\n";
		w << "float3 $y = $worldViewMatrix.y.xyz;\n";
		w << "float3 $z = $worldViewMatrix.z.xyz;\n";
		w << "float3 $cx = cross($y, $z);\n";
		w << "float3 $cy = cross($z, $x);\n";
		w << "float3 $cz = cross($x, $y);\n";

		// calc normalized normal
		w << "float3 $n = input.normal;\n";
		w << "float3 $nn = normalize(vs($cx * $n.x + $cy * $n.y + $cz * $n.z));\n";
		
		//w << "float3x3 $m = inv(transpose(matrix3($worldViewMatrix.x.xyz, $worldViewMatrix.y.xyz, $worldViewMatrix.z.xyz)));\n";
	
		// calc normalized normal
		//w << "float3 $nn = normalize($m * input.normal);\n";
		
		// output normal (N) in view space
		w << "$.normal = $nn * (frontFacing ? 1.0f : -1.0f);\n";
	}
	else
	{
		// input tangent space
		bool outputTangent = this->hasAttribute("tangent");
		bool normalMapping = this->hasAttribute("tangentSpaceNormal");

		// tangent space in view space
		//! handle flipped/left handed tangent spaces
		w << "float3 $tangent = vs(transformDirection($worldViewMatrix, input.tangent));\n";
		w << "float3 $bitangent = vs(transformDirection($worldViewMatrix, input.bitangent));\n";
			
		// vs() forces calculation of normal in vertex shader. otherwise the auto-generated tangent space
		// gets interpolated erroneously
		w << "float3 $normal = vs(cross($tangent, $bitangent));\n";
				
		// calc normalized normal, tangent, bitangent
		w << "float3 $nn = normalize($normal);\n";
		if (outputTangent || normalMapping)
		{
			w << "float3 $nt = normalize($tangent);\n";
			w << "float3 $nb = cross($nn, $nt);\n";
		}
		
		if (outputTangent)
		{
			// output tangent (T) in view space
			w << "$.tangent = $nt;\n";

			// output biangent (B) in view space
			w << "$.bitangent = $nb;\n";					
		}

		if (!normalMapping)
		{
			// output normal (N) in view space
			w << "$.normal = $nn * (frontFacing ? 1.0f : -1.0f);\n";
		}
		else
		{		
			// transform sampled tangent space normal from [0, 1] to [-1, 1]
			w << "float3 $tn = $.tangentSpaceNormal * 2.0f - 1.0f;\n";
			
			// calc perturbed normal (N) in view space
			w << "$.normal = normalize($nn * (frontFacing ? 1.0f : -1.0f) * $tn.z + $nt * $tn.x + $nb * $tn.y);\n";
		}
	}
}

} // namespace digi
