#include <digi/Utility/StringFunctions.h>

#include "VertexAnimationNode.h"


namespace digi {



// VertexAnimationNode

VertexAnimationNode::VertexAnimationNode()
	: numPositions(0), numTangents(0), numPositionFrames(0), numTangentFrames(0),
	positionFPS(0), tangentFPS(0)
{
	//this->addStructure("PositionVertex", positionVertexStructure);
	//this->addStructure("TangentVertex", tangentVertexStructure);
	
	this->addInput("time", "float");

	this->addState("positionFrame", "float");
	this->addState("tangentFrame", "float");

	this->addState("positionBuffer", "MemoryBuffer<float3>");
	this->addState("tangentBuffer", "MemoryBuffer<float3>");

	this->addState("positionState", "NibbleDecompressState<3>");
	this->addState("tangentState", "NibbleDecompressState<6>");

	// interpolation factors for interpolation in vertex processing kernel
	this->addOutput("positionAlpha", "float");
	this->addOutput("tangentAlpha", "float");
}

VertexAnimationNode::~VertexAnimationNode()
{
}

std::string VertexAnimationNode::getNodeType()
{
	return "VertexAnimationNode";
}

void VertexAnimationNode::generateStaticDataThis(NodeWriter& w)
{
	w.writeArray("$positionData", this->positionData, 20);
	w.writeArray("$tangentData", this->tangentData, 20);
}

void VertexAnimationNode::generateInitCodeThis(NodeWriter& w)
{
	// position
	w << "$.positionBuffer.init($@positionBuffer::SYSTEM, " << this->numPositions << " * 2);\n";
	w << "initAnimation($.positionBuffer, $.positionState, $positionData, " << this->numPositions << ");\n";

	// tangent and binormal
	w << "$.tangentBuffer.init($@tangentBuffer::SYSTEM, " << this->numTangents << " * 4);\n";
	w << "initAnimation($.tangentBuffer, $.tangentState, $tangentData, " << this->numTangents << ");\n";
}

void VertexAnimationNode::generateUpdateCodeThis(NodeWriter& w)
{
	// position decompression
	{
		// calc local time
		w.writeLine("float $positionTime = $.time * " + toCode(this->positionFPS) + ";");

		// calculate animation frame from local time
		w.writeLine("float $positionFrame = min(floor($positionTime), " + toCode(this->numPositionFrames - 2) + ");");

		// decompress to frame
		w.writeLine("while ($positionFrame > $.positionFrame)");
		w.writeLine("{");
		w.incIndent();
		w.writeLine("decompressAnimation($.positionBuffer, $.positionState, " + toCode(this->numPositions) + ");");
		w.writeLine("$.positionFrame += 1.0f;");
		w.decIndent();
		w.writeLine("}");
		
		// output interpolation factor
		w.writeLine("$.positionAlpha = clamp($positionTime - $positionFrame, 0.0f, 1.0f);");
	}

	// tangent and binormal decompression
	{
		// calc local time
		w.writeLine("float $tangentTime = $.time * " + toCode(this->tangentFPS) + ";");

		// calculate animation frame from local time
		w.writeLine("float $tangentFrame = min(floor($tangentTime), " + toCode(this->numTangentFrames - 2) + ");");

		// decompress to frame
		w.writeLine("while ($tangentFrame > $.tangentFrame)");
		w.writeLine("{");
		w.incIndent();
		w.writeLine("decompressAnimation($.tangentBuffer, $.tangentState, " + toCode(this->numTangents) + ");");
		w.writeLine("$.tangentFrame += 1.0f;");
		w.decIndent();
		w.writeLine("}");
		
		// output interpolation factor
		w.writeLine("$.tangentAlpha = clamp($tangentTime - $tangentFrame, 0.0f, 1.0f);");
	}
}



// VertexAnimationKernelNode

VertexAnimationKernelNode::VertexAnimationKernelNode(const std::string& name, Pointer<Node> sceneNode)
	: Node(name), sceneNode(sceneNode)
{
	this->addUniform("positionAlpha", "float");
	this->addUniform("tangentAlpha", "float");

	this->addOutput("position", "float3");
	this->addOutput("tangent", "float3");
	this->addOutput("binormal", "float3");
}

VertexAnimationKernelNode::~VertexAnimationKernelNode()
{
}

std::string VertexAnimationKernelNode::getNodeType()
{
	return "VertexAnimationKernelNode";
}

Pointer<Node> VertexAnimationKernelNode::getOuterScope()
{
	return this->sceneNode;
}

void VertexAnimationKernelNode::generateInitCodeThis(NodeWriter& w, int flags)
{
	w.writeLine("$.positionAlpha = $^positionAlpha;");
	w.writeLine("$.tangentAlpha = $^tangentAlpha;");
}

void VertexAnimationKernelNode::generateUpdateCodeThis(NodeWriter& w)
{
	// position
	w.writeLine("float $pr = $.positionAlpha;");
	w.writeLine("float $ps = 1.0f - $.positionAlpha;");
	
	w.writeLine("$.position = $ps * positionVertex[$.positionIndex * 2]\n"
		"\t + $pr * positionVertex[$.positionIndex * 2 + 1];");

	// tangent and binormal
	w.writeLine("float $tr = $.tangentAlpha;");
	w.writeLine("float $ts = 1.0f - $.tangentAlpha;");

	w.writeLine("$.tangent = $ts * tangentVertex[$.tangentIndex * 4]\n"
		"\t + $tr * tangentVertex[$.tangentIndex * 4 + 2];");
	
	w.writeLine("$.binormal = $ts * tangentVertex[$.tangentIndex * 4 + 1]\n"
		"\t + $tr * tangentVertex[$.tangentIndex * 4 + 3];");
}


} // namespace digi
