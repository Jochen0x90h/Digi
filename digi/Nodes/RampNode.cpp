#include <digi/Utility/VectorUtility.h>

#include "RampNode.h"


namespace digi {

// RampNode

RampNode::RampNode(int inputDimension, int outputDimension,
	Type type, Interpolation interpolation)
	: TextureBaseNode(inputDimension, outputDimension),
	type(type), interpolation(interpolation)
{
}

RampNode::~RampNode()
{
}

std::string RampNode::getNodeType()
{
	return "RampNode";
}

void RampNode::generateTexture(NodeWriter& w)
{
	int numValues = int(this->xValues.size());

	if (numValues <= 1)
	{
		// 0 or 1 ramp key values
		float4 value = numValues == 1 ? this->yValues[0] : float4();
		w << "$.output = " << varVector(value, this->inputDimension) << ";\n";
	}
	else
	{	
		// 2 or more ramp key values
		
		// get input
		w << "float $u = $coord.x;\n";
		if (this->type != RAMP_U)
		{
			if (this->inputDimension >= 2)
				w << "float $v = $coord.y;\n";
			else
				w << "float $v = 0.0f;\n";
		}
				
		// map uv to ramp x value
		switch (this->type)
		{
		case RAMP_U:
			w << "float $x = $u;\n";
			break;
		case RAMP_V:
			w << "float $x = $v;\n";
			break;
		case RAMP_DIAGONAL:
			w << "float $x = ($u + $v) * 0.5f;\n";
			break;
		case RAMP_RADIAL:
			w << "float $x = atan2($u - 0.5f, $v - 0.5f) * 0.15915f + 0.5f;\n";
			break;
		case RAMP_CIRCULAR:
			w << "float $x = sqrt((sqr($u - 0.5f) + sqr($v - 0.5f)) * 2.0f);\n";
			break;
		case RAMP_BOX:
			w << "float $x = max(abs($u - 0.5f), abs($v - 0.5f)) * 2.0f;\n";
			break;
		}

		// calculate ramp output value
		/*w << "$@output $ramp;\n";
		this->recurse(w, 0, numValues, this->outputDimension);
		w << "$.output = $ramp;\n";*/

		w << "$.output = ";
		this->recurse(w, 0, numValues, this->outputDimension);
		w << ";\n";
	}	
}
/*
void RampNode::recurse(NodeWriter& w, int start, int numValues, int dim)
{
	if (numValues > 2)
	{
		// subdivide the ramp interval
		int m = numValues / 2;
		
		w << "if ($x < " << this->xValues[start + m] << ")\n";
		w.beginScope();

		this->recurse(w, start, m + 1, dim);

		w.endScope();
		w.writeLine("else");
		w.beginScope();

		this->recurse(w, start + m, numValues - m, dim);

		w.endScope();
	}
	else
	{
		// now we have only two values left: interpolate
		float* x = getData(this->xValues) + start;
		float4* y = getData(this->yValues) + start;
		if (this->interpolation == NONE)
		{
			// no interpolation
			w << "$ramp = " << varVector(y[0], dim) << ";\n";
		}
		else
		{
			// linear interpolation
			// ramp = y0 + (map - x0) / (x1 - x0) * (y1 - y0)
			// ramp = map * scale + offset
			// scale = (y1 - y0) / (x1 - x0)
			// offset = y0 - x0 * scale
			float4 scale = (y[1] - y[0]) / (x[1] - x[0]);
			float4 offset = y[0] - x[0] * scale;
			w << "$ramp = $x * " << varVector(scale, dim, true) << " + " << varVector(offset, dim, true) << ";\n";
		}
	}
}
*/
void RampNode::recurse(NodeWriter& w, int start, int numValues, int dim)
{
	if (numValues > 2)
	{
		// subdivide the ramp interval
		int m = numValues / 2;
		
		w << "$x < " << this->xValues[start + m] << " ? ";

		this->recurse(w, start, m + 1, dim);

		w << " : ";

		this->recurse(w, start + m, numValues - m, dim);
	}
	else
	{
		// now we have only two values left: interpolate
		float* x = this->xValues.data() + start;
		float4* y = this->yValues.data() + start;
		if (this->interpolation == NONE)
		{
			// no interpolation
			w << varVector(y[0], dim);
		}
		else
		{
			// linear interpolation
			// ramp = y0 + (x - x0) / (x1 - x0) * (y1 - y0)
			// ramp = x * scale + offset
			// scale = (y1 - y0) / (x1 - x0)
			// offset = y0 - x0 * scale
			float4 scale = (y[1] - y[0]) / (x[1] - x[0]);
			float4 offset = y[0] - x[0] * scale;
			w << "$x * " << varVector(scale, dim, true) << " + " << varVector(offset, dim, true);
		}
	}
}

} // namespace digi
