#ifndef DigiExport_BlendPipeNode_h
#define DigiExport_BlendPipeNode_h

#include <boost/format.hpp>

#include <digi/CodeGenerator/Node.h>


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	blend pipeline. every pipeline stage blends the result
	of the previous stage (the current value) with its input.

	mode = COLOR3_ALPHA1, mode = COLOR3_TRANSPARENCY1:
		inputs:
			input0 (float4)
			input1 (float4)
			...
		
		outputs:
			output (float4)

	mode = COLOR3_ALPHA3:
		inputs:
			input0.color (float3)
			input0.alpha (float3)
			input1.color (float3)
			input1.alpha (float3)
			...

		outputs:
			output.color (float3)
			output.alpha (float3)

	mode = COLOR3_TRANSPARENCY3:
		inputs:
			input0.color (float3)
			input0.transparency (float3)
			input1.color (float3)
			input1.transparency (float3)
			...

		outputs:
			output.color (float3)
			output.transparency (float3)
*/
class BlendPipeNode : public Node
{
public:
	
	enum Mode
	{
		// color and scalar alpha packed into float4
		COLOR3_ALPHA1,
		
		// color and scalar transparency packed into float4
		COLOR3_TRANSPARENCY1,

		// 3-component color and alpha channels
		COLOR3_ALPHA3,

		// 3-component color and transparency channels
		COLOR3_TRANSPARENCY3,
	};

	// blend operation
	enum BlendOp
	{
		// alpha mode. initial color is zero, initial alpha is zero (Maya layered texture)
		BLEND_KEEP,       // color = color,                                               alpha = alpha
		BLEND_REPLACE,    // color = input.color,                                         alpha = input.alpha
		BLEND_OVER,       // color = lerp(color, input.color, input.alpha),               alpha = lerp(alpha, 1, input.alpha)
		BLEND_OVER2,      // color = color * (1 - input.alpha) + input.color,             alpha = lerp(alpha, 1, input.alpha)
		BLEND_IN,         // color = color * input.alpha,                                 alpha = alpha * input.alpha
		BLEND_OUT,        // color = color * (1 - input.alpha),                           alpha = alpha * (1 - input.alpha)
		BLEND_ADD,        // color = color + input.color * input.alpha,                   alpha = alpha
		BLEND_SUBTRACT,   // color = color - input.color * input.alpha,                   alpha = alpha
		BLEND_MULTIPLY,   // color = lerp(color, color * input.color, input.alpha),       alpha = alpha
		BLEND_DIFFERENCE, // color = lerp(color, abs(color - input.color), input.alpha),  alpha = alpha
		BLEND_LIGHTEN,    // color = lerp(color, max(color, input.color), input.alpha),   alpha = alpha
		BLEND_DARKEN,     // color = lerp(color, min(color, input.color), input.alpha),   alpha = alpha
		BLEND_SATURATE,   // color = lerp(color, color * (1 + input.color), input.alpha), alpha = alpha
		BLEND_DESATURATE, // color = lerp(color, color * (1 - input.color), input.alpha), alpha = alpha
		BLEND_ILLUMINATE, // color = lerp(color, color * input.color * 2, input.alpha),   alpha = alpha

		// transparency mode. initial color is zero, initial transparency is one (Maya layered shader)
		// BLEND_KEEP,       color = color,                                               transparency = transparency
		// BLEND_REPLACE,    color = input.color,                                         transparency = input.transparency
		// BLEND_OVER        color = lerp(input.color, color, input.transparency),        transparency = input.transparency * transparency
		// BLEND_OVER2       color = input.color + color * input.transparency,            transparency = input.transparency * transparency
	};
	
	BlendPipeNode(Mode mode = COLOR3_ALPHA1);
	
	virtual ~BlendPipeNode();

	virtual std::string getNodeType();

	// calculates the procedural texture inside kernel or pixel shader
	virtual void writeUpdateCodeThis(NodeWriter& w);	
	

	// add a blend stage and return the input name
	std::string addStage(int stageIndex, BlendOp blendOp);

	static std::string getInputName(int stageIndex) {return str(boost::format("input%1%") % stageIndex);}

protected:
	
	Mode mode;

	// the blend mode for every pipeline stage
	std::vector<BlendOp> blendOps;
};

/// @}

} // namespace digi

#endif
