#include "LatticeNode.h"


namespace digi {


// FFDNode

FFDNode::FFDNode(const Path& worldMatrix)
{
	// world matrix of deformed object
	this->addInput("worldMatrix", "float4x4", worldMatrix);

	this->addInput("baseMatrix", "float4x4");
	this->addInput("deformedMatrix", "float4x4");
	//this->addState("lattice", arg("[%0]float3", prod(resolution)));

	// object space -> deformer space
	this->addOutput("o2dMatrix", "float4x4");

	// deformer space -> object space
	this->addOutput("d2oMatrix", "float4x4");
}

FFDNode::~FFDNode()
{
}

std::string FFDNode::getNodeType()
{
	return "FFDNode";
}

void FFDNode::writeUpdateCodeThis(NodeWriter& w)
{
	// calc transform from object space into deformer space (object -> world -> deformer)
	w << "$.o2dMatrix = inv($.baseMatrix) * $.worldMatrix;\n";
	
	// calc transform from deformer space into object space (deformer -> world -> object)
	w << "$.d2oMatrix = inv($.worldMatrix) * $.deformedMatrix;\n";
}


// LatticeBoundingBoxNode

LatticeBoundingBoxNode::LatticeBoundingBoxNode(const Path& inputPath, Pointer<Node> ffdNode, Pointer<Node> latticeNode,
	int3 resolution)
	: BoundingBoxNode(inputPath), resolution(resolution)
{
	this->addInput("o2dMatrix", "float4x4", Path(ffdNode, "o2dMatrix"));
	this->addInput("d2oMatrix", "float4x4", Path(ffdNode, "d2oMatrix"));
}

LatticeBoundingBoxNode::~LatticeBoundingBoxNode()
{
}

std::string LatticeBoundingBoxNode::getNodeType()
{
	return "LatticeBoundingBoxNode";
}

void LatticeBoundingBoxNode::writeUpdateCodeThis(NodeWriter& w)
{
	// size of bounding box is corner - center, i.e. half diameter in each dimension
	//!
	w << "$.output.center = $.input.center;\n";
	w << "$.output.size = $.input.size;\n";
}


// LatticeVertexNode

LatticeVertexNode::LatticeVertexNode(const Path& inputPath, Pointer<Node> ffdNode, Pointer<Node> latticeNode,
	int3 resolution)
	: VertexNode(inputPath), resolution(resolution)
{
	int numPoints = prod(resolution);

	this->addInput("o2dMatrix", "float4x4", Path(ffdNode, "o2dMatrix"));
	this->addInput("d2oMatrix", "float4x4", Path(ffdNode, "d2oMatrix"));

	this->addInput("lattice", arg("[%0]float3", numPoints));

	// connect individually as arrays can not be copied using operator =
	for (int i = 0; i < numPoints; ++i)
	{
		this->connect(arg("lattice[%0]", i), Path(latticeNode, arg("lattice[%0]", i)));	
	}
}

LatticeVertexNode::~LatticeVertexNode()
{
}

std::string LatticeVertexNode::getNodeType()
{
	return "LatticeVertexNode";
}

void LatticeVertexNode::writeUpdateCodeThis(NodeWriter& w)
{
	float3 r_1 = convert_float3(this->resolution - 1);

	w << "float4x4 $latticeMatrix = matrix4x4TranslateScale(" << r_1 * 0.5f << ", " << r_1 << ")"
		" * $.o2dMatrix;\n";

	w << "float3 $p = transformPosition($latticeMatrix, $.input.position)"
		/*" * " << r_1 << " + " << r_1 * 0.5f <<*/ ";\n";
	
	w << "float3 $pi = clamp(floor($p), splat3(0.0f), " << (r_1 - 1.0f) << ");\n";
	w << "float3 $pb = $p - $pi;\n";
	w << "float3 $pa = 1.0f - $pb;\n";
	w << "int3 $i = convert_int3($pi);\n";

	// get all 8 lattice points
	for (int i = 0; i < 8; ++i)
	{
		int ix = i & 1 ? 1 : 0;
		int iy = i & 2 ? 1 : 0;
		int iz = i & 4 ? 1 : 0;

		w << "float3 $l" << ident(i) << " = $.lattice[$i.x + " << ix <<
			" + ($i.y + " << iy << ") * " << this->resolution.x <<
			" + ($i.z + " << iz << ") * " << this->resolution.x * this->resolution.y <<
			"];\n";
	}
	
	// deform position
	w << "float3 $dp = ";
	for (int i = 0; i < 8; ++i)
	{
		if (i != 0)
			w << "\n\t+ ";
		int ix = i & 1 ? 1 : 0;
		int iy = i & 2 ? 1 : 0;
		int iz = i & 4 ? 1 : 0;
		
		w << "$l" << ident(i) <<
			" * $p" << "ab"[ix] << ".x"
			" * $p" << "ab"[iy] << ".y"
			" * $p" << "ab"[iz] << ".z";
	}
	w << ";\n";
	w << "$.output.position = transformPosition($.d2oMatrix, $dp);\n";
	
	// calc 12 differences
	w << "float3 $d10 = $l1 - $l0;\n";
	w << "float3 $d20 = $l2 - $l0;\n";

	w << "float3 $d31 = $l3 - $l1;\n";
	w << "float3 $d32 = $l3 - $l2;\n";
	
	w << "float3 $d40 = $l4 - $l0;\n";
	w << "float3 $d51 = $l5 - $l1;\n";
	w << "float3 $d62 = $l6 - $l2;\n";
	w << "float3 $d73 = $l7 - $l3;\n";

	w << "float3 $d54 = $l5 - $l4;\n";
	w << "float3 $d64 = $l6 - $l4;\n";

	w << "float3 $d75 = $l7 - $l5;\n";
	w << "float3 $d76 = $l7 - $l6;\n";
	
	// deform tangent and bitangent
	w << "float3x3 $m = ";
	for (int i = 0; i < 8; ++i)
	{
		if (i != 0)
			w << "\n\t+ ";
		int dx = i & ~1;
		int dy = i & ~2;
		int dz = i & ~4;

		int ix = i & 1 ? 1 : 0;
		int iy = i & 2 ? 1 : 0;
		int iz = i & 4 ? 1 : 0;		
		
		w << "matrix3("
			"$d" << ident(dx | 1) << ident(dx) << ","
			"$d" << ident(dy | 2) << ident(dy) << ","
			"$d" << ident(dz | 4) << ident(dz) << ")"
			" * $p" << "ab"[ix] << ".x"
			" * $p" << "ab"[iy] << ".y"
			" * $p" << "ab"[iz] << ".z";
	}
	w << ";\n";	
	w << "$.output.tangent = transformDirection($.d2oMatrix, $m * $.input.tangent);\n";
	w << "$.output.bitangent = transformDirection($.d2oMatrix, $m * $.input.bitangent);\n";
}

} // namespace digi
