#include <digi/Utility/lexicalCast.h>
#include <digi/Utility/StringUtility.h>
#include <digi/Utility/VectorUtility.h>

#include "AnimationNode.h"


namespace digi {

namespace
{
	// key types

	float2 calcRange(const std::vector<float>& values)
	{
		float2 range = splat2(values[0]);
		foreach (float value, values)
		{
			range.x = min(range.x, value);
			range.y = max(range.y, value);
		}
		return range;
	}
}

// AnimationNode

AnimationNode::AnimationNode()
	: preType(CONSTANT), postType(CONSTANT),
	numWrittenKeys(), numWrittenStepKeys(), xScale(), xOffset()//, yScale(), yOffset()
{
	this->addInput("input", "float");
	this->addOutput("output", "float");
}

AnimationNode::~AnimationNode()
{
}

std::string AnimationNode::getNodeType()
{
	return "AnimationNode";
}

void AnimationNode::generateBuffers(std::vector<NamedBuffer>& buffers)
{
	bool bezier = true;
	
	this->firstBufferIndex = buffers.size();
	this->numWrittenKeys = 0;
	this->numWrittenStepKeys = 0;

	size_t numKeys = this->yValues.size();
	bool equidistant = this->yTangents.empty();
	bool weighted = !this->xTangents.empty();

	//std::string targetPath = makeTargetPath(this->getPath(1)).substr(1);
	
	if (numKeys >= 2)
	{
		if (equidistant)
		{
			// equidistant keys
			float xStart = this->xValues[0];  // start time of track
			float xOffset = this->xValues[1]; // position of first key relative to zero
			float xStep = this->xValues[2];   // sampling step
			float xEnd = this->xValues[3];	  // end time of track
				
			int startKey = -2 + int(floor((xStart - xOffset) / xStep + 0.9f));
			int endKey = 2 + int(floor((xEnd - xOffset) / xStep + 0.1f));
			
			Pointer<Buffer> keysBuffer = new Buffer(BufferFormat(BufferFormat::X32, BufferFormat::FLOAT),
				endKey - startKey + 1);
			float* data = keysBuffer->getData<float>();

			/*
			Pointer<Buffer> keysBuffer = new Buffer(BufferFormat(BufferFormat::X16, BufferFormat::UNORM),
				endKey - startKey + 1);
			uint16_t* data = keysBuffer->getData<uint16_t>();
			float2 range = calcRange(this->yValues);
			float scale = 65535.0f / (range.y - range.x + 1e-6f);
			float offset = -range.x * scale;
			
			*/
			
			for (int i = startKey; i <= endKey; ++i)
			{
				float y = this->yValues[clamp(i, 0, int(numKeys) - 1)];
				//data[i - startKey] = convert_short_rte(y * scale + offset);
				data[i - startKey] = y;
			}

			buffers += NamedBuffer("_" + lexicalCast<std::string>(buffers.size())/*targetPath + "_x"*/, keysBuffer);
			
			// calc x and y scale for sampling the track
			this->xScale = 1.0f / xStep;
			
			// -1 because catmull rom track samples value at x = 1 if x = 0 is given
			this->xOffset = -(xOffset / xStep + float(startKey)) - 1.0f;
			
			/*
			this->yScale = range.y - range.x;
			this->yOffset = range.x;
			*/
		}
		else
		{		
			// track
			std::vector<float> xValues;
			std::vector<float> yValues;

			// additional step track
			std::vector<float> stepXValues;
			std::vector<float> stepYValues;

			// add start key to step track
			stepXValues += this->xValues[0];
			stepYValues += 0.0f;

			// get step flags
			std::vector<bool> stepFlags(this->stepFlags);
			stepFlags.resize(numKeys);
			stepFlags[numKeys - 1] = false;

			// build keys and split step keys into an extra track
			bool lastStepFlag = false;
			double offset = 0.0;
			for (size_t i = 0; i < numKeys; ++i)
			{
				// get flag that indicates if this is a stepped key
				bool stepFlag = stepFlags[i];
				bool last = i == numKeys - 1;
				
				// check for stepped key
				if (lastStepFlag)
				{
					offset += double(this->yValues[i]) - double(this->yValues[i - 1]);

					// do not use last key, step on last key is handled with post infinity
					if (!last)
					{
						++this->numWrittenStepKeys;
						stepXValues += this->xValues[i];
						stepYValues += float(offset);
					}
				}
				
				// check for spline key, always use first key, use last key if we already have at least 2 keys to
				// complete the track
				if (!lastStepFlag || !stepFlag || (last && xValues.size() >= 2))
				{
					++this->numWrittenKeys;

					// add x values
					float x = this->xValues[i];
					if (weighted)
					{
						float xIn = !lastStepFlag ? this->xTangents[i].inTangent : 1.0f;
						if (bezier)
							xIn = x - xIn / 3.0f;
						xValues += xIn;
					}
					xValues += x;
					if (weighted)
					{
						float xOut = !stepFlag ? this->xTangents[i].outTangent : 1.0f;
						if (bezier)
							xOut = x + xOut / 3.0f;
						xValues += xOut;
					}
					
					// add y values
					float yIn = !lastStepFlag ? this->yTangents[i].inTangent : 0.0f;
					float y = this->yValues[i] - float(offset);
					float yOut = !stepFlag ? this->yTangents[i].outTangent : 0.0f;
					if (bezier)
					{
						yIn = y - yIn / 3.0f;
						yOut = y + yOut / 3.0f;
					}
					yValues += yIn;
					yValues += y;
					yValues += yOut;
				}

				lastStepFlag = stepFlag;
			}
			
			// generate named buffers for spline keys if there are at least 2 keys
			if (this->numWrittenKeys >= 2)
			{
				// use buffer index as name
				// leading and trailing tangents/control points are not used for x and y
				int removeCount = weighted ? 1 : 0;
				buffers += NamedBuffer("_" + lexicalCast<std::string>(buffers.size())/*targetPath + "_x"*/,
					createDataBuffer(xValues.data() + removeCount, xValues.size() - removeCount * 2));
				
				buffers += NamedBuffer("_" + lexicalCast<std::string>(buffers.size())/*targetPath + "_y"*/,
					createDataBuffer(yValues.data() + 1, yValues.size() - 2));
			}
				
			// generate named buffers for step keys if there are at least 2 keys
			if (this->numWrittenStepKeys >= 2)
			{
				buffers += NamedBuffer("_" + lexicalCast<std::string>(buffers.size())/*targetPath + "_stepX"*/,
					createDataBuffer(stepXValues.data(), stepXValues.size()));
				
				buffers += NamedBuffer("_" + lexicalCast<std::string>(buffers.size())/*targetPath + "_stepY"*/,
					createDataBuffer(stepYValues.data(), stepYValues.size()));
			}
		}
	}
}


/*
	the different cases for pre and post infinity handling:
	

	if (input < start)
		output = pre(input);
	else if (input > end)
		output = post(input);
	else
	{
		temp = input;
		output = anim(temp);
	}
	

	if (input < start)
		output = pre(input);
	else 
	{
		temp = input % range;
		output = anim(temp);
	}


	if (input > end)
		output = post(input);
	else
	{
		temp = input % range;
		output = anim(temp);
	}


	temp = input % range;
	output = anim(temp)

	
	if (input < (start + end) / 2)
		temp = input % range1;
	else 
		temp = input % range2;
	output = anim(temp);
*/
void AnimationNode::writeUpdateCodeThis(NodeWriter& w)
{
	// http://caad.arch.ethz.ch/info/maya/manual/UserGuide/Animation/KeyframeMoPath/04_graph_editor.doc1.html

	// note: generateBuffers must have been called before writeUpdateCodeThis

	size_t numKeys = this->yValues.size();
	bool equidistant = this->yTangents.empty();
	bool weighted = !this->xTangents.empty();

	if (numKeys == 0)
	{
		w << "$.output = 0.0f;\n";
	}
	else if (numKeys == 1)
	{
		w << "$.output = " << this->yValues[0] << ";\n";
	}
	else
	{
		float start = this->xValues.front();
		float end = this->xValues.back();

		bool preDone = false;
		bool postDone = false;
		bool else1 = false;
		bool else2 = false;
		
		// create temp variable for the input
		w << "float $input = $.input;\n";

		// check for pre infinity types where output can be calculated directly (without track data)
		if (this->preType == CONSTANT || this->preType == LINEAR)
		{
			w << "if ($input < " << start << ")\n";
			if (this->preType == CONSTANT || equidistant)
			{
				// constant
				w << "\t$.output = " << this->yValues.front() << ";\n";
			}
			else
			{
				// linear: use inTangent of first key
				float scale = this->yTangents[0].inTangent;
				if (!weighted)
					scale /= (this->xValues[1] - this->xValues[0]);
				else
					scale /= this->xTangents[0].inTangent;
				float offset = -this->xValues[0] * scale + this->yValues[0];
				w << "\t$.output = $input * " << scale << " + " << offset << ";\n";
			}
			preDone = true;
			else1 = true;
		}

		// check for post infinity types where output can be calculated directly (without track data)
		if (this->postType == CONSTANT || this->postType == LINEAR)
		{
			w << (else1 ? "else " : "") << "if ($input >= " << end << ")\n";
			if (this->postType == CONSTANT || equidistant)
			{
				// constant
				w << "\t$.output = " << this->yValues.back() << ";\n";
			}
			else
			{
				// linear: use outTangent of last key
				float scale = this->yTangents[numKeys - 1].outTangent;
				if (!weighted)
					scale /= (this->xValues[numKeys - 1] - this->xValues[numKeys - 2]);
				else
					scale /= this->xTangents[numKeys - 1].outTangent;
				float offset = -this->xValues[numKeys - 1] * scale + this->yValues[numKeys - 1];
				w << "\t$.output = $input * " << scale << " + " << offset << ";\n";
			}
			postDone = true;
			else1 = true;
		}	

		// generate else if if-statement was emitted
		if (else1)
		{
			w.writeLine("else");
			w.beginScope();
		}

		bool cycleRelative = this->preType == CYCLE_RELATIVE || this->postType == CYCLE_RELATIVE;
		if (cycleRelative)
		{
			// integer cycle index relative to the base cycle
			// init with zero in case only one end is CYCLE_RELATIVE
			w << "float $step = 0.0f;\n";
		}
		
		// if both infinity types only modify the input and are different, we need a case distinction
		if (this->preType != this->postType && !preDone && !postDone)
		{
			w << "if ($input < " << (start + end) * 0.5f << ")\n";
			w.writeLine("{");
			w.incIndent();
			else2 = true;
		}
		
		// pre infinity type
		this->writeCyclicType(w, start, end, this->preType);
		
		if (else2)
		{
			w.endScope();
			w.writeLine("else");
			w.beginScope();
		}
		
		// post infinity type
		if (this->postType != this->preType)
			this->writeCyclicType(w, start, end, this->postType);
		
		// closing bracket
		if (else2)
		{
			w.endScope();
		}

		// code for animation value calculation
		size_t bufferIndex = this->firstBufferIndex;
		w << "$.output = ";
		if (equidistant)
		{
			std::string y = lexicalCast<std::string>(bufferIndex++);
			w << "macro_" << y << "(";
			w << "evalCatmullRomTrack(buffers._" << y << ", $input * " << this->xScale << " + " << this->xOffset << ")";
				//" * " << this->yScale << " + " << this->yOffset;
			w << ")";
		}
		else
		{
			if (this->numWrittenKeys >= 2)
			{
				std::string x = lexicalCast<std::string>(bufferIndex++);
				std::string y = lexicalCast<std::string>(bufferIndex++);
				w << "macro_" << y << "(";
				if (!weighted)
					w << "eval";
				else
					w << "evalWeighted";
				w << "BezierTrack(buffers._" << x << ", buffers._" << y << ", " << this->numWrittenKeys << ", "
					"macro_" << x << "Inv($input))";
				w << ")";
			}
			else
			{
				// only one key
				w << this->yValues.front();
			}
			if (this->numWrittenStepKeys >= 2)
			{
				std::string x = lexicalCast<std::string>(bufferIndex++);
				std::string y = lexicalCast<std::string>(bufferIndex++);
				w << "\n\t+ ";
				w << "macro_" << y << "(";
				w << "evalStepTrack(buffers._" << x << ", buffers._" << y << ", " << this->numWrittenStepKeys << ", "
					"macro_" << x << "Inv($input))";
				w << ")";
			}
		}
		if (cycleRelative)
		{
			w << "\n\t+ $step * " << (this->yValues.back() - this->yValues.front());
		}
		w << ";\n";
		
		// closing bracket
		if (else1)
		{
			w.endScope();
		}
	}
}

void AnimationNode::writeCyclicType(NodeWriter& w, float start, float end, InfinityType i)
{
	switch (i)
	{
	case CONSTANT:
	case LINEAR:
		break;
	case CYCLE:
		{
			float scale1 = 1.0f / (end - start);
			float offset1 = -start * scale1;
			float scale2 = (end - start);
			float offset2 = start;
			w << "$input = frac($input * " << scale1 << " + " << offset1
				<< ") * " << scale2 << " + " << offset2 << ";\n";
		}
		break;
	case CYCLE_RELATIVE:
		{
			float scale1 = 1.0f / (end - start);
			float offset1 = -start * scale1;
			float scale2 = (end - start);
			float offset2 = start;
			w << "$input = $input * " << scale1 << " + " << offset1 << ";\n";
			w << "$step = floor($input);\n";
			w << "$input = frac($input) * " << scale2 << " + " << offset2 << ";\n";
		}
		break;
	case OSCILLATE:
		{
			float scale1 = 1.0f / (2.0f * (end - start));
			float offset1 = -start * scale1;
			float scale2 = 2.0f * (end - start);
			float offset2 = start;
			w << "$input = frac($input * " << scale1 << " + " << offset1 << ");\n";
			w << "$input = ($input < 0.5f ? $input : 1.0f - $input) * " << scale2 << " + " << offset2 << ";\n";
		}
		break;
	};
}

} // namespace digi
