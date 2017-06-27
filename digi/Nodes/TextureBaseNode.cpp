#include <digi/Utility/ArrayUtility.h>

#include "TextureBaseNode.h"


namespace digi {


// TextureBaseNode

static const char* components1[] = {""};
static const char* components2[] = {".x", ".y", ".z", ".w"};

TextureBaseNode::TextureBaseNode(int inputDimension, int outputDimension)
	: inputDimension(inputDimension), outputDimension(outputDimension)
{
	fill(this->addressModes, REPEAT);

	// input texture coordinates
	this->addInput("input", VectorInfo(MatrixInfo::FLOAT, inputDimension).toString());
	
	// output color
	this->addOutput("output", VectorInfo(MatrixInfo::FLOAT, outputDimension).toString());
}


TextureBaseNode::~TextureBaseNode()
{
}

void TextureBaseNode::writeUpdateCodeThis(NodeWriter& w)
{
	// get input components (e.g. ".x" and ".y" for 2D input)
	const char** components = this->getInputComponents();

	// get flags that indicate which address modifications are done automatically. e.g. hardware texturing does clamp and miror
	int addressFlags = this->getAddressFlags();

	// copy input to local variable
	w << "$@input $coord = $.input;\n";
	
	// check if the address modes for all components are the same
	bool sameAddressModes = true;
	for (int i = 1; i < this->inputDimension; ++i)
	{	
		sameAddressModes &= this->addressModes[0] == this->addressModes[i];
	}
	
	int inputDimension = this->inputDimension;
	if (sameAddressModes)
	{
		// can process address modes in one pass
		inputDimension = 1;
		
		// no components are needed
		components = components1;
	}

	// process all other address modes
	for (int i = 0; i < inputDimension; ++i)
	{	
		static const char* temporaries[] = {"$x", "$y", "$z", "$w"};
		
		AddressMode m = this->addressModes[i];

		std::string str = "$coord";
		str += components[i];
		size_t strLength = str.length();

		if ((addressFlags & AUTO_REPEAT) == 0)
		{
			// repeat
			if (m == REPEAT)
				str = "frac(" + str + ")";
			if (m == MIRROR_REPEAT)
				str = "frac(" + str + " * 0.5f) * 2.0f";
		}
		if ((addressFlags & AUTO_CLAMP) == 0)
		{
			// clamp
			if (m == CLAMP)
				str = "clamp(" + str + ", 0.0f, 1.0f)";
			if (m == MIRROR_CLAMP)
				str = "clamp(" + str + ", 0.0f, 2.0f)";
		}
		if ((addressFlags & AUTO_MIRROR) == 0)
		{
			// mirror
			if (m == MIRROR_REPEAT || m == MIRROR_CLAMP)
			{
				w << "float " << temporaries[i] << " = " << str << ";\n";
				str = std::string("min(") + temporaries[i] + ", 2.0f - " + temporaries[i] + ")";
			}
		}
		
		// overwrite $coord if necessary
		if (str.length() != strLength)
			w << "$coord" << components[i] << " = " << str << ";\n";
	}

	// call generateTexture of derived class that takes the texture coordinates from $coord
	this->generateTexture(w);
}

int TextureBaseNode::getAddressFlags()
{
	return 0;
}

const char** TextureBaseNode::getInputComponents()
{
	return this->inputDimension == 1 ? components1 : components2;
}

} // namespace digi
