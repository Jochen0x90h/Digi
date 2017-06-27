#ifndef DigiExport_RampNode_h
#define DigiExport_RampNode_h

#include <digi/Math/All.h>

#include "TextureBaseNode.h"


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Ramp with two input coordinates and a few keys, mainly for particles

	Inputs:
		input (float to float2)
	
	Outputs:
		outut (float to float4)
*/
class RampNode : public TextureBaseNode
{
public:
	
	enum Type
	{
		RAMP_V,
		RAMP_U,
		RAMP_DIAGONAL,
		RAMP_RADIAL,
		RAMP_CIRCULAR,
		RAMP_BOX,
	};
	
	enum Interpolation
	{
		NONE,
		LINEAR
	};

	RampNode(int inputDimension, int outputDimension, Type type, Interpolation interpolation);
	
	virtual ~RampNode();

	virtual std::string getNodeType();

	virtual void generateTexture(NodeWriter& w);

	// x and y values that define the ramp
	std::vector<float> xValues;
	std::vector<float4> yValues;

protected:

	void recurse(NodeWriter& w, int start, int numValues, int dim);

	Type type;
	Interpolation interpolation;
};

/// @}

} // namespace digi

#endif
