#include <digi/Utility/StringUtility.h>

#include "SkinClusterNode.h"



namespace digi {


// SkinClusterNode

SkinClusterNode::SkinClusterNode(int numBones)
	: invBoneBindMatrices(numBones)
{
	this->addOutput("matrix", arg("[%0]float4x3", numBones));
}

SkinClusterNode::~SkinClusterNode()
{
}

std::string SkinClusterNode::getNodeType()
{
	return "SkinClusterNode";
}

void SkinClusterNode::writeUpdateCodeThis(NodeWriter& w)
{
	// get number of bones
	int numBones = int(this->invBoneBindMatrices.size());
		
	// build bone matrix for every bone
	// ibG * B * ibB * bG
	// bG = geometry matrix at bind time
	// ibB = inverse bone matrix at bind time
	// B = current bone matrix
	// ibG = inverse geometry matrix at bind time
	w << "float3x4 $m;\n";
	for (int i = 0; i < numBones; ++i)
	{
		// calc inverse bind matrix
		float4x4 ibG = inv(this->geometryBindMatrix);
		
		// store only 3x4 bone matrices, therefore truncate the left-most matrix in the multiplication chain 
		float3x4 ibG_3x4 = matrix4(getXYZ(ibG.x), getXYZ(ibG.y), getXYZ(ibG.z), getXYZ(ibG.w));
		
		// calc ibB * bG
		float4x4 ibB_bG = this->invBoneBindMatrices[i] * this->geometryBindMatrix;

		//w << "$.matrix[" << i << "] = " << ibG_3x4 << "\n\t* $.bone" << ident(i) << "\n\t* " << ibB_bG << ";\n";
		w << "$m = " << ibG_3x4 << "\n\t* $.bone" << ident(i) << "\n\t* " << ibB_bG << ";\n";
		w << "$.matrix[" << i << "] = matrix3(vector4($m.x, $m.w.x), vector4($m.y, $m.w.y), vector4($m.z, $m.w.z));\n";
	}
}

void SkinClusterNode::connectBone(int index, const float4x4& invBoneBindMatrix, const Path& boneWorldMatrix)
{
	this->invBoneBindMatrices[index] = invBoneBindMatrix;
	this->addInput(arg("bone%0", index), "float4x4", boneWorldMatrix);
}


// SkinClusterBoundingBoxNode

SkinClusterBoundingBoxNode::SkinClusterBoundingBoxNode(const Path& inputPath, Pointer<SkinClusterNode> skinClusterNode,
	const std::vector<int>& boneSet)
	: BoundingBoxNode(inputPath), numBones(int(boneSet.size()))
{
	// create array input for bone matrices	
	this->addInput("matrix", arg("[%0]float4x3", this->numBones));

	// connect elements	
	for (int i = 0; i < this->numBones; ++i)
	{
		this->connect(arg("matrix[%0]", i), Path(skinClusterNode, arg("matrix[%0]", boneSet[i])));
	}	
}

SkinClusterBoundingBoxNode::~SkinClusterBoundingBoxNode()
{
}

std::string SkinClusterBoundingBoxNode::getNodeType()
{
	return "SkinClusterBoundingBoxNode";
}

void SkinClusterBoundingBoxNode::writeUpdateCodeThis(NodeWriter& w)
{
	// size of bounding box is corner - center, i.e. half diameter in each dimension
	w << "float3 $minP = {};\n";
	w << "float3 $maxP = {};\n";
	w << "float3 $bbs = $.input.size;\n";

	for (int i = 0; i < this->numBones; ++i)
	{
		w.beginScope();
		w << "float4x3& $m = $.matrix[" << i << "];\n";

		//w << "float3 $c = transformPosition($m, $.input.center);\n";
		//w << "float3 $s = abs($m.x) * $bbs.x + abs($m.y) * $bbs.y + abs($m.z) * $bbs.z;\n";
		w << "float3 $p = $.input.center;\n";
		w << "float3 $c = $m.x.xyz * $p.x + $m.y.xyz * $p.y + $m.z.xyz * $p.z + vector3($m.x.w, $m.y.w, $m.z.w);\n";
		w << "float3 $s = abs($m.x.xyz) * $bbs.x + abs($m.y.xyz) * $bbs.y + abs($m.z.xyz) * $bbs.z;\n";
		if (i == 0)
		{
			w << "$minP = $c - $s;\n";
			w << "$maxP = $c + $s;\n";
		}
		else
		{
			w << "$minP = min($minP, $c - $s);\n";
			w << "$maxP = max($maxP, $c + $s);\n";
		}	
		w.endScope();
	}

	w << "$.output.center = ($maxP + $minP) * 0.5f;\n";
	w << "$.output.size = ($maxP - $minP) * 0.5f;\n";
}


// SkinClusterVertexNode

SkinClusterVertexNode::SkinClusterVertexNode(const Path& inputPath, Pointer<SkinClusterNode> skinClusterNode,
	const std::vector<int>& boneSet)
	: VertexNode(inputPath), numWeights()
{
	int numBones = int(boneSet.size());

	// create array input for bone matrices	
	this->addInput("matrix", arg("[%0]float4x3", numBones));

	// connect elements	
	for (int i = 0; i < numBones; ++i)
	{
		this->connect(arg("matrix[%0]", i), Path(skinClusterNode, arg("matrix[%0]", boneSet[i])));
	}	
}

SkinClusterVertexNode::~SkinClusterVertexNode()
{
}

std::string SkinClusterVertexNode::getNodeType()
{
	return "SkinClusterVertexNode";
}

void SkinClusterVertexNode::writeUpdateCodeThis(NodeWriter& w)
{
	w << "float4x3 $m = ";
	if (this->numWeights > 1)
	{
		// calc blended bone matrix
		for (int i = 0; i < this->numWeights; ++i)
		{
			if (i > 0)
				w << "\n\t+ ";
			int index = i / 4;
			std::string swizzle;
			if (!((this->numWeights & 3) == 1 && i == this->numWeights - 1))
			{
				swizzle += '.';
				swizzle += "xyzw"[i & 3];
			}
			w << "$.weight" << ident(index) << swizzle << " * $.matrix[$.index" << ident(index) << swizzle << ']';
		}
		w << ";\n";		
		w.writeLine();
	}
	else
	{
		// only one bone influence per vertex
		w << "$.matrix[$.index0];\n";
	}

	// transform geometry
	//w << "$.output.position = transformPosition($m, $.input.position);\n";
	//w << "$.output.tangent = transformDirection($m, $.input.tangent);\n";
	//w << "$.output.bitangent = transformDirection($m, $.input.bitangent);\n";
	w << "float3 $p = $.input.position;\n";
	w << "float3 $t = $.input.tangent;\n";
	w << "float3 $b = $.input.bitangent;\n";
	w << "$.output.position = $m.x.xyz * $p.x + $m.y.xyz * $p.y + $m.z.xyz * $p.z + vector3($m.x.w, $m.y.w, $m.z.w);\n";
	w << "$.output.tangent = $m.x.xyz * $t.x + $m.y.xyz * $t.y + $m.z.xyz * $t.z;\n";
	w << "$.output.bitangent = $m.x.xyz * $b.x + $m.y.xyz * $b.y + $m.z.xyz * $b.z;\n";
}

} // namespace digi
