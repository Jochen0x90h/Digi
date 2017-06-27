#ifndef digi_Scene_Deformer_h
#define digi_Scene_Deformer_h

#include "ObjectReader.h"
#include "ObjectWriter.h"


namespace digi {

/// @addtogroup Scene
/// @{


/**
	Deformer node. A buffer can have a deformer that preprocesses the buffer before it goes
	into the vertex shader.
*/
class Deformer : public Object
{
public:

	// construct kernel
	Deformer();

	virtual ~Deformer();

	// name of shader (must be unique, used for struct names and debugging)
	std::string name;

	// path of deformer in instance.uniforms struct
	std::string path;

	// list of attributes to copy from scene into uniforms struct for this shader
	std::vector<std::string> assignments;
	
	
	// uniform input structure
	Pointer<Type> uniformType;

	// vertex input structure
	Pointer<Type> inputType;
	
	// vertex output structure
	Pointer<Type> outputType;
	
	// code of deformer
	std::string code;


	static Pointer<Deformer> load(ObjectReader& r);
	void save(ObjectWriter& w);
};



/// @}

} // namespace digi

#endif
