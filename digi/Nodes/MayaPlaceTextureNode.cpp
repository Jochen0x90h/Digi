#include <digi/Utility/StringUtility.h>

#include "MayaPlaceTextureNode.h"


namespace digi {


// MayaPlaceTextureNode

MayaPlaceTextureNode::MayaPlaceTextureNode()
	: mirrorU(), mirrorV(), stagger(), borderU(), borderV()
{
	this->addInput("coverage", "float2");
	this->addInput("translateFrame", "float2");
	this->addInput("rotateFrame", "float");
	this->addInput("repeatUV", "float2");
	this->addInput("offset", "float2");
	this->addInput("rotateUV", "float");

	this->addInput("input", "float2");
	this->addOutput("output", "float2");
	this->addOutput("textureEnable", "float");
}

MayaPlaceTextureNode::~MayaPlaceTextureNode()
{
}

std::string MayaPlaceTextureNode::getNodeType()
{
	return "MayaPlaceTextureNode";
}

void MayaPlaceTextureNode::writeUpdateCodeThis(NodeWriter& w)
{
	/*
		TransformFrame = Co * Tf^-1 * Rp * Rf * Rp^-1
		TransformUV = Rp * R * Rp^-1 * O * S
		Co: Coverage (not implemented correctly for coverage < 1.0 and wrap on)
		Tf: Translate Frame
		Rf: Rotate Frame
		Rp: Rotate pivot point = (0.5, 0.5)
		S: Scale (Repeat UV)
		O: Offset
		R: Rotate UV
	*/

	// calc transformFrame
	w << "float3x3 $transformFrame = matrix3x3TranslateScale(splat2(0.0f), 1.0f / $.coverage) * matrix3x3TranslateRotate(-$.translateFrame + splat2(0.5f), $.rotateFrame)\n"
		"\t* matrix3x3Translate(splat2(-0.5f));\n";
	

	if (!this->mirrorU && !this->mirrorV && !this->stagger)
	{
		// no mirror: transform UV's in one step
		
		// calc transformUV
		w << "float3x3 $transformUV = matrix3x3TranslateRotate(splat2(0.5f), $.rotateUV)\n"
			"\t* matrix3x3TranslateScale(splat2(-0.5f) + $.offset, $.repeatUV);\n";

		// calc textureMatrix = transformUV * transformFrame
		w << "float2x3 $textureMatrix = getMatrix2x3($transformUV * $transformFrame);\n";

		// transform the uv's
		w << "$.output = $textureMatrix * vector3($.input, 1.0f);\n";

		// transform the uv's for border condition
		w << "float2 $coord = getMatrix2x3($transformFrame) * vector3($.input, 1.0f);\n";
	}
	else
	{		
		// mirror: transform UV's in two steps
		
		// (1) transform the uv's
		w << "float2 $coord = getMatrix2x3($transformFrame) * vector3($.input, 1.0f);\n";

		// apply repeatUV
		w << "float2 $scaledCoord = $coord * $.repeatUV;\n";
		
		// mirror and stagger
		w << "float $u = $scaledCoord.x;\n";
		w << "float $v = $scaledCoord.y;\n";
		
		if (this->mirrorV || this->stagger)
		{
			w.beginScope();
			w << "float $f = floor($v);\n";
			w << "float $mask = $f - floor($v * 0.5f) * 2.0f;\n";
			
			if (this->mirrorV)
				w << "$v += $mask * (1.0f - 2.0f * ($v - $f));\n";
			if (this->stagger)
				w << "$u += $mask * 0.5f;\n";
			w.endScope();
		}

		if (this->mirrorU)
		{
			w.beginScope();
			w << "float $f = floor($u);\n";
			w << "float $mask = $f - floor($u * 0.5f) * 2.0f;\n";
			w << "$u += $mask * (1.0f - 2.0f * ($u - $f));\n";
			w.endScope();
		}

		// calc transformUV (without repeatUV)
		w << "float3x3 $transformUV = matrix3x3TranslateRotate(splat2(0.5f), $.rotateUV)\n"
			"\t* matrix3x3Translate(splat2(-0.5f) + $.offset);\n";

		// (2) transform the uv's
		w << "$.output = getMatrix2x3($transformUV) * vector3($u, $v, 1.0f);\n";
	}
	
	// border condition

	// calc border condition
	w << "$.textureEnable = ";
	if (!this->borderU && !this->borderV)
		w << "1.0f";
	else
	{
		if (this->borderU)
			w << "step(0.0f, $coord.x) * step(-1.0f, -$coord.x)";
		if (this->borderU && this->borderV)
			w << "\n\t* ";
		if (this->borderV)
			w << "step(0.0f, $coord.y) * step(-1.0f, -$coord.y)";
	}
	w << ";\n";
}

} // namespace digi
