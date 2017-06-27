#ifndef digi_Nodes_MayaPlaceTextureNode_h
#define digi_Nodes_MayaPlaceTextureNode_h

#include <digi/CodeGenerator/Node.h>


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Maya Place Texture node for shaders. Calculates a texture transformation matrix
	from various parameters.

	Inputs:
		translateFrame (float2)
		rotateFrame (float)
		repeatUV (float2)
		offset (float2)
		rotateUV (float)

		input (float2)
		
	Outputs:
		output (float2)
		borderCondition (bool)
*/
class MayaPlaceTextureNode : public Node
{
	public:
		
		MayaPlaceTextureNode();
		
		virtual ~MayaPlaceTextureNode();

		virtual std::string getNodeType();
	
		virtual void writeUpdateCodeThis(NodeWriter& w);

		
		bool mirrorU;
		bool mirrorV;
		bool stagger;
	
		bool borderU;
		bool borderV;
};


/// @}

} // namespace digi

#endif
