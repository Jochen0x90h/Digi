#ifndef digi_Nodes_CameraNode_h
#define digi_Nodes_CameraNode_h

#include <digi/CodeGenerator/Node.h>


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Camera node
	
	inputs:
		filmSize (float2)
		filmOffset (float2)
		nearClipPlane (float)
		farClipPlane (float)
		focalLength (float, only PERSPECTIVE)
		separation(float, only PERSPECTIVE_STEREO)
		
	outputs:
		projection (Projection)
*/
class CameraNode : public Node
{
public:
	enum Type
	{
		PERSPECTIVE,
		PERSPECTIVE_STEREO,
		ORTHOGRAPHIC
	};
	
	// fit mode. used when window aspect and film aspect are different
	enum Fit
	{
		// camera view is fitted into film (film fills camera view)
		FIT_FILL,
		
		// horizontal film size is fitted to camera view
		FIT_HORIZONTAL,

		// vertical film size is fitted to camera view
		FIT_VERTICAL,
		
		// film is fitted into camera view (camera view esceeds film)
		FIT_OVERSCAN
	};
	
	CameraNode(Type type, Fit fit, float scaleUnit = 1.0f);
	
	virtual ~CameraNode();

	virtual std::string getNodeType();
		
	virtual void writeUpdateCodeThis(NodeWriter& w);

	Type getCameraType() {return this->type;}

protected:
	
	Type type;
	Fit fit;
};


/// @}

} // namespace digi

#endif
