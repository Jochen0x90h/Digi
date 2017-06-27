#ifndef digi_CodeGenerator_Language_h
#define digi_CodeGenerator_Language_h


namespace digi {

/// @addtogroup CodeGenerator
/// @{

struct Language
{
	enum Type
	{
		NONE,
		
		CPP,

		// shading languages
		GLSL_1_2, // up to 1.2 (texture2D)
		GLSL_1_5, // up to 1.5 (texture)
		ESSL, // glsl for opengl es (precision hint)
		HLSL,
		
		//CL,
		//CG,
		
		// languages with no vector and struct support
		JAVA,
		JS,
	};

	Type type;
	
	Language()
		: type(NONE) {}
	Language(Type type)
		: type(type) {}

	bool operator ==(Type type) {return this->type == type;}
	bool operator !=(Type type) {return this->type != type;}
	
	bool isCPP() {return this->type == CPP;}
	
	bool isGLSL() {return this->type >= GLSL_1_2 && this->type <= ESSL;}
	bool isOldGLSL() {return this->type == GLSL_1_2 || this->type == ESSL;}
	bool isHLSL() {return this->type == HLSL;}
	bool isShadingLanguage() {return this->type >= GLSL_1_2 && this->type <= HLSL;}
	
	bool isJS() {return this->type == JS;}

	bool supportsVector() {return this->type >= CPP && this->type <= HLSL;}
	bool supportsStruct() {return this->type >= CPP && this->type <= HLSL;}
};

/// @}

} // namespace digi

#endif
