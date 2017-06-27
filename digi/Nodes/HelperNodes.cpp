#include "HelperNodes.h"


namespace digi {


// CrossNode

CrossNode::CrossNode(const Path& a, const Path& b)
{
	this->addInput("a", "float3", a);
	this->addInput("b", "float3", b);
	
	this->addOutput("result", "float3");
}

CrossNode::~CrossNode()
{
}

std::string CrossNode::getNodeType()
{
  return "CrossNode";
}

void CrossNode::writeUpdateCodeThis(NodeWriter& w)
{
	w << "$.result = cross($.a, $.b);\n";
}


// CastNode

CastNode::CastNode(const std::string& inputType, const std::string& outputType)
{
	this->addInput("input", inputType);
	this->addOutput("output", outputType);
}

CastNode::~CastNode()
{
}

std::string CastNode::getNodeType()
{
  return "CastNode";
}

void CastNode::writeUpdateCodeThis(NodeWriter& w)
{
	std::string inputType = this->getType("input");
	MatrixInfo inputTypeInfo(inputType);
	
	std::string outputType = this->getType("output");
	MatrixInfo outputTypeInfo(outputType);
	
	if (outputType == inputType)
	{
		w << "$.output = $.input;\n";
	}
	else
	{
		w << "$.output = ";
		
		if (outputTypeInfo.type == MatrixInfo::BOOL)
		{
			if (inputTypeInfo.type == MatrixInfo::FLOAT)
			{
				// float -> bool (round to even)
				w << "abs($.input) > 0.5f;\n";
			}
			else
			{
				// int -> bool
				w << "$.input != 0;\n";			
			}
		}
		else if (inputTypeInfo.type == MatrixInfo::FLOAT && outputTypeInfo.type == MatrixInfo::INT)
		{
			// float -> int
			w << "convert_" << outputType << "_rte($.input);\n";			
		}
		else
		{
			// bool -> int, bool -> float, int -> float
			w << "convert_" << outputType << "($.input);\n";
		}
	}
}


// LinearCombination3x2Node

LinearCombination3x2Node::LinearCombination3x2Node(const Path& a, const Path& b,
	const Path& x)
{
	this->addInput("a", "float3", a);
	this->addInput("b", "float3", b);
	this->addInput("x", "float2", x);
	
	this->addOutput("result", "float3");
}

LinearCombination3x2Node::~LinearCombination3x2Node()
{
}

std::string LinearCombination3x2Node::getNodeType()
{
  return "LinearCombination3x2Node";
}

void LinearCombination3x2Node::writeUpdateCodeThis(NodeWriter& w)
{
	w << "$.result = $.a * $.x.x + $.b * $.x.y;\n";
}

} // namespace digi
