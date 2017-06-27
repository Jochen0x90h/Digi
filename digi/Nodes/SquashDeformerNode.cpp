#include <digi/Utility/StringUtility.h>

#include "SquashDeformerNode.h"


namespace digi {

// SquashDeformerNode

SquashDeformerNode::SquashDeformerNode(const Path& worldMatrix)
{
	// world matrix of deformed object
	this->addInput("worldMatrix", "float4x4", worldMatrix);
	
	// world matrix of sine deformer handle
	this->addInput("handleMatrix", "float4x4");

	// parameters
	this->addInput("factor", "float");
	this->addAttribute("expand", "float");
	this->addAttribute("startSmoothness", "float");
	this->addAttribute("endSmoothness", "float");
	this->addAttribute("lowBound", "float");
	this->addAttribute("highBound", "float");
	
	// outputs for vertex node
	this->addOutput("f", "float");
	this->addOutput("object2handle", "float4x4");
	this->addOutput("handle2object", "float4x4");
}

SquashDeformerNode::~SquashDeformerNode()
{
}

std::string SquashDeformerNode::getNodeType()
{
	return "SquashDeformerNode";
}

void SquashDeformerNode::writeUpdateCodeThis(NodeWriter& w)
{
	// factor > 0 is extend, factor < 0 is squash
	w << "$.f = $.factor >= 0.0f ? $.factor + 1.0f : 1.0f / (-$.factor + 1.0f);\n";

	// calc transform from object space into handle space (object -> world -> handle)
	w << "$.object2handle = inv($.handleMatrix) * $.worldMatrix;\n";
	
	// calc transform from handle to object space
	w << "$.handle2object = inv($.object2handle);\n";
}


// SquashDeformerBoundingBoxNode

SquashDeformerBoundingBoxNode::SquashDeformerBoundingBoxNode(const Path& inputPath, const Path& parameters)
	: BoundingBoxNode(inputPath)
{
	// add inputs for scene parameters
	//!
}

SquashDeformerBoundingBoxNode::~SquashDeformerBoundingBoxNode()
{
}

std::string SquashDeformerBoundingBoxNode::getNodeType()
{
	return "SquashDeformerBoundingBoxNode";
}

void SquashDeformerBoundingBoxNode::writeUpdateCodeThis(NodeWriter& w)
{
	//!
	w << "$.output.center = $.input.center;\n";
	w << "$.output.size = $.input.size;\n";
}


// SquashDeformerVertexNode

SquashDeformerVertexNode::SquashDeformerVertexNode(const Path& inputPath, const Path& parameters)
	: VertexNode(inputPath)
{
	// add inputs for scene parameters
	this->addInput("f", "float", parameters + ".f");
	this->addInput("expand", "float", parameters + ".f");
	this->addInput("startSmoothness", "float", parameters + ".startSmoothness");
	this->addInput("endSmoothness", "float", parameters + ".endSmoothness");
	this->addInput("lowBound", "float", parameters + ".lowBound");
	this->addInput("highBound", "float", parameters + ".highBound");
	this->addInput("object2handle", "float4x4", parameters + ".object2handle");
	this->addInput("handle2object", "float4x4", parameters + ".handle2object");
}

SquashDeformerVertexNode::~SquashDeformerVertexNode()
{
}

std::string SquashDeformerVertexNode::getNodeType()
{
	return "SquashDeformerVertexNode";
}

void SquashDeformerVertexNode::writeUpdateCodeThis(NodeWriter& w)
{
	/*
		// maya squash formula
		// libDeformSlice.dylib`TdeformSquashShape::deformPoint(T3dDblPoint&, TdoubleArray const&, T4dDblMatrix const&, T4dDblMatrix const&):
		double f = factor >= 0.0 ? factor + 1.0 : 1.0 / (-factor + 1.0);
		if (y <= lowBound)
		{
			$.input.position
			ox = x;
			oy = f * lowBound + y - lowBound;
		}
		else if (y >= highBound)
		{
			ox = x;
			oy = f * highBound + y - highBound;
		}
		else
		{
			double ny = (y - lowBound) / (highBound - lowBound);
			double ny2 = ny * 2.0 - 1.0;

			double sm = ny2 < 0.0 ? startSmoothness : endSmoothness;
			double sm1 = sm + 1.0;
			
			double e = sqrt(sqr(sm1) - 1.0);
			double d = sqrt(sqr(sm1) - sqr(ny2)) - e;
			double h = (1.0 - sm) / (sm1 - e) * d;
			double c = cos(ny2 * pi * 0.5);
			double i = (c * c * sm + h) * expand;
			double scale = (sqrt(f) - 1.0) * 2.0 * i + 1.0;
	 
			ox = x / scale;
			oy = f * y;
		}
	*/
	w << "float4 $p = $.object2handle * vector4($.input.position, 1.0f);\n";
	w << "if ($p.y <= $.lowBound)\n";
	w << "\t$p.y += $.f * $.lowBound - $.lowBound;\n";
	w << "else if ($p.y >= $.highBound)\n";
	w << "\t$p.y += $.f * $.highBound - $.highBound;\n";
	w << "else\n";
	w.beginScope();

	// calc normalized y which is 0 at lowBound and 1 ad highBound
	w << "float $ny = ($p.y - $.lowBound) / ($.highBound - $.lowBound);\n";
	w << "float $ny2 = $ny * 2.0f - 1.0f;\n";

	w << "float $sm = $ny2 < 0.0f ? $.startSmoothness : $.endSmoothness;\n";
	w << "float $sm1 = $sm + 1.0f;\n";

	w << "float $e = sqrt($sm1 * $sm1 - 1.0f);\n";
	w << "float $d = sqrt($sm1 * $sm1 - $ny2 * $ny2) - $e;\n";
	w << "float $h = (1.0f - $sm) / ($sm1 - $e) * $d;\n";
	w << "float $c = cos($ny2 * 1.570796f);\n";
	w << "float $i = ($c * $c * $sm + $h) * $.expand;\n";
	w << "float $scale = (sqrt($.f) - 1.0f) * 2.0f * $i + 1.0f;\n";
	
	w << "$p.x /= $scale;\n";
	w << "$p.y *= $.f;\n";
	w << "$p.z /= $scale;\n";
	w.endScope();
	
	w << "$.output.position = ($.handle2object * $p).xyz;\n";
	w << "$.output.tangent = $.input.tangent;\n"; //! tangent is not deformed
	w << "$.output.bitangent = $.input.bitangent;\n"; //!
}

} // namespace digi
