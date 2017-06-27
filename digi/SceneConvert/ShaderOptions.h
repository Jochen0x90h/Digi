#ifndef digi_SceneConvert_ShaderOptions_h
#define digi_SceneConvert_ShaderOptions_h


namespace digi {

/// @addtogroup SceneConvert
/// @{


enum Precision
{
	PRECISION_LOW,
	PRECISION_MEDIUM,
	PRECISION_HIGH,
};

struct ShaderOptions
{
	// name of layer
	std::string layerName;
	
	// index of layer
	int layerIndex;
	
	// code that writes color, alpha, position, normal, into output or outputs[]
	// also can write sort or discard flags
	std::string outputCode;

	// force that this layer is present even it it is not present in scene
	bool forceLayer;


	// number of available vertex uniform vectors (opengl: 1024, opengl es 2.0: 384, webgl: 251)
	int maxVertexUniforms;
	
	enum DoubleSidedMode
	{
		// render double sided in one pass (needs gl_FrontFacing)
		ONE_PASS,

		// render double sided in two passes
		TWO_PASS,

		// render only front side
		FRONT,
		
		// render only back side
		BACK
	};
	DoubleSidedMode doubleSidedMode;

	// calc as much as possible in vertex shader instead of pixel shader. for low-end systems
	bool avoidPixelShader;

	// precision of position calculations
	Precision positionPrecision;
	
	// precision of color calculations
	Precision colorPrecision;
	
	// precision of calculations with sample coordinates
	Precision samplePrecision;

	// add support for user clip planes
	bool userClipPlanes;


	ShaderOptions()
		: layerIndex(0), forceLayer(true),
		maxVertexUniforms(1024), doubleSidedMode(ONE_PASS), avoidPixelShader(false),
		positionPrecision(PRECISION_HIGH), colorPrecision(PRECISION_HIGH/*PRECISION_MEDIUM*/), samplePrecision(PRECISION_HIGH),
		userClipPlanes(false)
	{
	}
};


// alpha mode gets determined by ShaderPass (in compileShader)
enum SortMode
{
	// sort never
	SORT_NEVER,

	// sort is dependent on material
	SORT_MATERIAL,

	// sort always
	SORT_ALWAYS,
};


/// @}

} // namespace digi

#endif
