#ifndef digi_Nodes_EnvNode_h
#define digi_Nodes_EnvNode_h

#include <digi/CodeGenerator/Node.h>


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Environment map with ball or spherical projection

	Inputs:
		input (float3, is a color)
		worldMatrix (float4x4, world matrix of projector)		

	Outputs:
		output (float3, is a color)
*/
class EnvNode : public Node
{
public:

	enum Type
	{
		// zenith direction Z = (0, -1, 0), azimuth axis A = (0, 0, -1).
		// output.x is azimuth in range [0, 1), output.y is inclination in range [0, 1)
		SPHERICAL_PROJECTION,
		
		// same as SPHERICAL_PROJECTION but output.x and output.y are swapped
		SPHERICAL_PROJECTION_YX,
		
		// environment ball (normalize(vector3(0, 0, 1) + v))
		BALL_PROJECTION,
	};
		
	EnvNode(Type type, bool viewSpace, const Path& matrix);
	
	virtual ~EnvNode();

	virtual std::string getNodeType();

	virtual void writeUpdateCodeThis(NodeWriter& w);
	
protected:

	Type type;
	bool viewSpace;
};

/// @}

} // namespace digi

#endif
