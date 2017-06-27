#include <digi/Utility/StringFunctions.h>

#include "CurveNode.h"


namespace digi {


// CurveNode

CurveNode::CurveNode(const std::string& name)
	: Node(name), numVertices(0)
{
	this->addOutput("curve", "Curve");
}

CurveNode::~CurveNode()
{
}

std::string CurveNode::getNodeType()
{
	return "CurveNode";
}

/*
void CurveNode::generateStruct(NodeWriter& w)
{
	// forward to vertex node
	w.pushNode(this);
	this->vertexNode->generateStruct(w);
	w.popNode();
}

void CurveNode::generateStaticDataThis(NodeWriter& w)
{
	// forward to vertex node
	w.pushNode(this);
	this->vertexNode->generateStaticDataThis(w);
	w.popNode();
}
*/
void CurveNode::generateInitCodeThis(NodeWriter& w)
{
/*	
	// forward to vertex node
	w.pushNode(this);
	this->vertexNode->generateInitCodeThis(w);
	w.popNode();
*/
	// init scale and offset
	w.writeLine("$.curve.range = float2(" + toCode(this->numVertices - 1) + ", 0);");
}

/*
void CurveNode::generateUpdateCodeThis(NodeWriter& w)
{
	// forward to vertex node
	w.pushNode(this);
	this->vertexNode->generateUpdateCodeThis(w);
	w.popNode();
}
*/

// SubCurveNode

SubCurveNode::SubCurveNode(const Path& inCurve)
{
//!	this->addInput("inCurve", inCurve.getType(), inCurve);
	this->addInput("minValue", "float");
	this->addInput("maxValue", "float");
	this->addOutput("curve", "Curve");
}

SubCurveNode::~SubCurveNode()
{
}

std::string SubCurveNode::getNodeType()
{
	return "SubCurveNode";
}

void SubCurveNode::generateUpdateCodeThis(NodeWriter& w)
{
	// copy vertex buffer
	w.writeLine("$.curve.buffer = $.inCurve.buffer;");
	
	// calc scale
	w.writeLine("float $scale = $.maxValue - $.minValue;");
	w.writeLine("$.curve.range.x = $.inCurve.range.x * scale;");
	
	// calc offset
	w.writeLine("$.curve.range.y = $.inCurve.range.x * $.minValue + $.inCurve.range.y;");
}


} // namespace digi
