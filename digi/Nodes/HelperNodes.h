#ifndef digi_Nodes_HelperNodes_h
#define digi_Nodes_HelperNodes_h

#include <digi/CodeGenerator/Node.h>


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Cross node.
	Calculates result = cross(a, b)
	
	inputs:
		a (float3)
		b (float3)
		
	outputs:
		result (float3)
*/	
class CrossNode : public Node
{
public:

	CrossNode() {}

	CrossNode(const Path& a, const Path& b);

	virtual ~CrossNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);
};


/**
	Cast node.
	Casts input to output type. Number of vector dimensions must be identical.
	
	inputs:
		input
		
	outputs:
		output
*/
class CastNode : public Node
{
public:

	CastNode(const std::string& inputType, const std::string& outputType);

	virtual ~CastNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);
};


class LinearCombination3x2Node : public Node
{
public:

	LinearCombination3x2Node() {}

	LinearCombination3x2Node(const Path& a, const Path& b, const Path& x);

	virtual ~LinearCombination3x2Node();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);
};

/// @}

} // namespace digi

#endif
