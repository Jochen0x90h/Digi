#ifndef digi_Nodes_AnimationNode_h
#define digi_Nodes_AnimationNode_h

#include <digi/Math/All.h>
#include <digi/CodeGenerator/Node.h>


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	animation curve for a scalar value

	inputs:
		input
	
	outputs:
		output
*/
class AnimationNode : public Node
{
public:
	
	enum InfinityType
	{
		CONSTANT,
		LINEAR,
		CYCLE,
		CYCLE_RELATIVE,
		OSCILLATE
	};

	struct Tangents
	{
		float inTangent;
		float outTangent;
		
		Tangents() : inTangent(), outTangent() {}
		Tangents(float inTangent, float outTangent) : inTangent(inTangent), outTangent(outTangent) {}
	};
	
	AnimationNode();
	
	virtual ~AnimationNode();

	virtual std::string getNodeType();

	virtual void generateBuffers(std::vector<NamedBuffer>& buffers);
	virtual void writeUpdateCodeThis(NodeWriter& w);

	std::vector<float> xValues;
	std::vector<Tangents> xTangents;
	std::vector<float> yValues;
	std::vector<Tangents> yTangents;
	std::vector<bool> stepFlags;

	InfinityType preType;
	InfinityType postType;
	
protected:
	
	void writeCyclicType(NodeWriter& w, float start, float end, InfinityType i);

	size_t firstBufferIndex;

	size_t numWrittenKeys;
	size_t numWrittenStepKeys;
	
	// scale/offset of x-axis for equidistant sampled tracks
	float xScale;
	float xOffset;
	//float yScale;
	//float yOffset;
};

/// @}

} // namespace digi

#endif
