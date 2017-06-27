#ifndef digi_Nodes_TextureBaseNode_h
#define digi_Nodes_TextureBaseNode_h

#include <digi/CodeGenerator/Node.h>


namespace digi {

/// @addtogroup Nodes
/// @{

/**
	Base class for texture nodes. Stores input and output dimension
	
	Inputs:
		borderValue (type same as output)

		input (float to float4, mapping coordinates)
	
	Outputs:
		outut (float to float4, e.g. color)
*/
class TextureBaseNode : public Node
{
	public:

		enum AddressMode
		{
			REPEAT,
			CLAMP,
			MIRROR_REPEAT,
			MIRROR_CLAMP,
		};
		
		// flags to indicate which address modifications are done automatically
		enum AddressFlags
		{
			AUTO_REPEAT = 1,
			AUTO_CLAMP = 2,
			AUTO_MIRROR = 4,
		};
		
		TextureBaseNode(int inputDimension, int outputDimension);
		
		virtual ~TextureBaseNode();

		virtual void writeUpdateCodeThis(NodeWriter& w);

		virtual int getAddressFlags();
		virtual void generateTexture(NodeWriter& w) = 0;

		const char** getInputComponents();

	
		// dimension of texture (1D to 4D)
		int inputDimension;

		// address modes for up to four dimensions
		AddressMode addressModes[4];
		
		// dimension of output
		int outputDimension;
};


/// @}

} // namespace digi

#endif
