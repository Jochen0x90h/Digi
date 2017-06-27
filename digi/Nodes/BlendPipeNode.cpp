#include <digi/Utility/VectorUtility.h>
#include <digi/Utility/foreach.h>

#include "BlendPipeNode.h"


namespace digi {

namespace
{
	struct ColorAlpha
	{
		std::string color;
		std::string alpha;

		ColorAlpha(const std::string& color, const std::string& alpha)
			: color(color), alpha(alpha)
		{
		}
	};

	struct ColorTransparency
	{
		std::string color;
		std::string transparency;

		ColorTransparency(const std::string& color, const std::string& transparency)
			: color(color), transparency(transparency)
		{
		}
	};
}

// BlendPipeNode

BlendPipeNode::BlendPipeNode(Mode mode)
	: mode(mode)
{
	// output
	switch (mode)
	{
	case COLOR3_ALPHA1:
	case COLOR3_TRANSPARENCY1:
		this->addOutput("output", "float4");
		break;		
	case COLOR3_ALPHA3:
		this->addOutput("output.color", "float3");
		this->addOutput("output.alpha", "float3");
		break;		
	case COLOR3_TRANSPARENCY3:
		this->addOutput("output.color", "float3");
		this->addOutput("output.transparency", "float3");
		break;		
	}
}

BlendPipeNode::~BlendPipeNode()
{
}

std::string BlendPipeNode::getNodeType()
{
	return "BlendPipeNode";
}

void BlendPipeNode::writeUpdateCodeThis(NodeWriter& w)
{
	size_t numStages = this->blendOps.size();

	// variables for current value
	w << "float3 $color = {};\n";
	const char* one;
	
	if (this->mode == COLOR3_ALPHA1 || this->mode == COLOR3_ALPHA3)
	{
		// alpha mode
		boost::format colorFormat;
		boost::format alphaFormat;
		if (this->mode == COLOR3_ALPHA1)
		{
			// scalar alpha
			w << "float $alpha = 0.0f;\n";
			one = "1.0f";
			colorFormat = boost::format("$.input%1%.xyz");
			alphaFormat = boost::format("$.input%1%.w");
		}
		else
		{
			// 3-component alpha
			w << "float3 $alpha = {};\n";
			one = "splat3(1.0f)";
			colorFormat = boost::format("$.input%1%.color");
			alphaFormat = boost::format("$.input%1%.alpha");
		}

		for (size_t stageIndex = 0; stageIndex < numStages; ++stageIndex)
		{
			// input of current stage
			ColorAlpha input(
				str(colorFormat % stageIndex),
				str(alphaFormat % stageIndex));
				
			// do blend op
			switch (this->blendOps[stageIndex])
			{
			case BLEND_KEEP:
				// do nothing
				break;
			case BLEND_REPLACE:
				w << "$color = " << input.color << ";\n";
				w << "$alpha = " << input.alpha << ";\n";
				break;
			case BLEND_OVER:
				w << "$color = lerp($color, " << input.color << ", " << input.alpha << ");\n";
				w << "$alpha = lerp($alpha, " << one << ", " << input.alpha << ");\n";
				break;
			case BLEND_OVER2:
				w << "$color = $color * (1.0f - " << input.alpha << ") + " << input.color << ");\n";
				w << "$alpha = lerp($alpha, " << one << ", " << input.alpha << ");\n";
				break;
			case BLEND_IN:
				w << "$color *= " << input.alpha << ";\n";
				w << "$alpha *= " << input.alpha << ";\n";
				break;
			case BLEND_OUT:
				w << "$color *= 1.0f - " << input.alpha << ";\n";
				w << "$alpha *= 1.0f - " << input.alpha << ";\n";
				break;
			case BLEND_ADD:
				w << "$color += " << input.color << " * " << input.alpha << ";\n";
				break;
			case BLEND_SUBTRACT:
				w << "$color -= " << input.color << " * " << input.alpha << ";\n";
				break;
			case BLEND_MULTIPLY:
				w << "$color = lerp($color, $color * " << input.color << ", " << input.alpha << ");\n";
				break;
			case BLEND_DIFFERENCE:
				w << "$color = lerp($color, abs($color - " << input.color << "), " << input.alpha << ");\n";
				break;
			case BLEND_LIGHTEN:
				w << "$color = lerp($color, max($color, " << input.color << "), " << input.alpha << ");\n";
				break;
			case BLEND_DARKEN:
				w << "$color = lerp($color, min($color, " << input.color << "), " << input.alpha << ");\n";
				break;
			case BLEND_SATURATE:
				w << "$color = lerp($color, $color * (1.0f + " << input.color << "), " << input.alpha << ");\n";
				break;
			case BLEND_DESATURATE:
				w << "$color = lerp($color, $color * (1.0f - " << input.color << "), " << input.alpha << ");\n";
				break;
			case BLEND_ILLUMINATE:
				w << "$color = lerp($color, $color * " << input.color << " * 2.0f, " << input.alpha << ");\n";
				break;		
			}
		}

		if (this->mode == COLOR3_ALPHA1)
		{
			w << "$.output = vector4($color, $alpha);\n";
		}
		else
		{
			w << "$.output.color = $color;\n";
			w << "$.output.alpha = $alpha;\n";
		}
	}
	else
	{
		// transparency mode
		boost::format colorFormat;
		boost::format transparencyFormat;
		if (this->mode == COLOR3_TRANSPARENCY1)
		{
			// scalar transparency
			w << "float $transparency = 1.0f;\n";
			one = "1.0f";
			colorFormat = boost::format("$.input%1%.xyz");
			transparencyFormat = boost::format("$.input%1%.w");
		}
		else
		{
			// 3-component transparency
			w << "float3 $transparency = splat3(1.0f);\n";
			one = "splat3(1.0f)";
			colorFormat = boost::format("$.input%1%.color");
			transparencyFormat = boost::format("$.input%1%.transparency");
		}

		for (size_t stageIndex = 0; stageIndex < numStages; ++stageIndex)
		{
			// input of current stage
			ColorTransparency input(
				str(colorFormat % stageIndex),
				str(transparencyFormat % stageIndex));
				
			// do blend op
			switch (this->blendOps[stageIndex])
			{
			case BLEND_KEEP:
				// do nothing
				break;
			case BLEND_REPLACE:
				w << "$color = " << input.color << ";\n";
				w << "$transparency = " << input.transparency << ";\n";
				break;
			case BLEND_OVER:
				w << "$color = lerp(" << input.color << ", $color, " << input.transparency << ");\n";
				w << "$transparency = " << input.transparency << " * $transparency;\n";
				break;
			case BLEND_OVER2:
				w << "$color = " << input.color << " + $color * " << input.transparency << ";\n";
				w << "$transparency = " << input.transparency << " * $transparency;\n";
				break;
			}
		}

		if (this->mode == COLOR3_TRANSPARENCY1)
		{
			w << "$.output = vector4($color, $transparency);\n";
		}
		else
		{
			w << "$.output.color = $color;\n";
			w << "$.output.transparency = $transparency;\n";
		}
	}
/*
	// variable for current value
	w << "float4 $current = {};\n";
	
	size_t numStages = this->blendOps.size();
	for (size_t stageIndex = 0; stageIndex < numStages; ++stageIndex)
	{
		// input of current stage
		std::string input = arg("$.input%0", stageIndex);
		
		// write into current value
		w << "$current = ";
		
		// do blend op
		switch (this->blendOps[stageIndex])
		{
		case BLEND_KEEP:
			w << "$current";
			break;
		case BLEND_REPLACE:
			w << input;
			break;
		case BLEND_OVER:
			w << "lerp($current, vector4(" << input << ".xyz, 1.0f), " << input << ".w)";
			break;
		case BLEND_IN:
			w << "$current * " << input << ".w";
			break;
		case BLEND_OUT:
			w << "$current * (1.0f - " << input << ".w)";
			break;
		case BLEND_ADD:
			w << "vector4($current.xyz + " << input << ".xyz * " << input << ".w, $current.w)";
			break;
		case BLEND_SUBTRACT:
			w << "vector4($current.xyz - " << input << ".xyz * " << input << ".w, $current.w)";
			break;
		case BLEND_MULTIPLY:
			w << "vector4(lerp($current.xyz, $current.xyz * " << input << ".xyz, " << input << ".w), $current.w)";
			break;
		case BLEND_DIFFERENCE:
			w << "vector4(lerp($current.xyz, abs($current.xyz - " << input << ".xyz), " << input << ".w), $current.w)";
			break;
		case BLEND_LIGHTEN:
			w << "vector4(lerp($current.xyz, max($current.xyz, " << input << ".xyz), " << input << ".w), $current.w)";
			break;
		case BLEND_DARKEN:
			w << "vector4(lerp($current.xyz, min($current.xyz, " << input << ".xyz), " << input << ".w), $current.w)";
			break;
		case BLEND_SATURATE:
			w << "vector4(lerp($current.xyz, $current.xyz * (1.0f + " << input << ".xyz), " << input << ".w), $current.w)";
			break;
		case BLEND_DESATURATE:
			w << "vector4(lerp($current.xyz, $current.xyz * (1.0f - " << input << ".xyz), " << input << ".w), $current.w)";
			break;
		case BLEND_ILLUMINATE:
			w << "vector4(lerp($current.xyz, $current.xyz * " << input << ".xyz * 2.0f, " << input << ".w), $current.w)";
			break;		
		}
		w << ";\n";
	}
	w << "$.output = $current;\n";
*/
}

std::string BlendPipeNode::addStage(int stageIndex, BlendOp blendOp)
{
	if (stageIndex >= this->blendOps.size())
		this->blendOps.resize(stageIndex + 1);
	this->blendOps[stageIndex] = blendOp;
	
	// get input name for stage index
	std::string inputName = BlendPipeNode::getInputName(stageIndex);
	
	// add input(s)
	if (this->mode == COLOR3_ALPHA1 || this->mode == COLOR3_TRANSPARENCY1)
	{
		this->addInput(inputName, "float4");
	}
	else
	{
		this->addInput(inputName + ".color", "float3");
		this->addInput(inputName + (this->mode == COLOR3_ALPHA3 ? ".alpha" : ".transparency"), "float3");
	}
	
	// return input name
	return inputName;
}

} // namespace digi
