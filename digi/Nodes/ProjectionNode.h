#ifndef digi_Nodes_ProjectionNode_h
#define digi_Nodes_ProjectionNode_h

#include <digi/VectorMath/All.h>
#include <digi/CodeGenerator/Node.h>


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Projection node for projective textures

	Inputs:
		worldMatrix (float4x4, world matrix of projector)		
		coneAngle (float, only PERSPECTIVE_PROJECTION)
		surfacePosition (float3, position of surface in view space)

	Outputs:
		output (float2, uv coordinates)
*/
class ProjectionNode : public Node
{
public:

	enum Type
	{
		INVALID_PROJECTION = -1,
	
		// projects onto xy plane and adds 0.5
		PARALLEL_PROJECTION,
		
		// maya spot light
		PERSPECTIVE_PROJECTION,
		
		// zenith direction Z = (0, -1, 0), azimuth axis A = (0, 0, -1).
		// output.x is azimuth in range [0, 1), output.y is inclination in range [0, 1)
		SPHERICAL_PROJECTION,
		
		// same as SPHERICAL_PROJECTION but output.x and output.y are swapped
		SPHERICAL_PROJECTION_YX,
		
		// maya ambient light
		SPHERICAL_PROJECTION2,

		// environment ball (normalize(vector3(0, 0, 1) + v))
		BALL_PROJECTION,
	};
	
	enum Flags
	{
		// project in view space, i.e. no transform of the input vector takes place
		VIEW_SPACE = 1,
		
		// input is a direction vector
		INPUT_IS_DIRECTION = 2,
	};
	
	// construct ProjectionNode. parameters must provide .coneAngle (only PERSPECTIVE_PROJECTION)
	ProjectionNode(Type type, int flags, const Path& input,
		const Path& matrix, const Path& parameters);
	
	virtual ~ProjectionNode();

	virtual std::string getNodeType();

protected:

	virtual void writeUpdateCodeThis(NodeWriter& w);

	Type type;
	int flags;
};


/// @}

} // namespace digi

#endif
