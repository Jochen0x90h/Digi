#ifndef digi_SceneConvert_MCCache_h
#define digi_SceneConvert_MCCache_h

#include <digi/Engine/MCFile.h>


namespace digi {

/// @addtogroup Engine
/// @{

class MCCache : public EngineLoader
{
public:
	MCCache();
	
	/// define a render layer and return its index
	///
	/// Empty outputCode removes the layer. If the layerName is equal to a render layer defined
	/// in Maya, then its properties are used. Only the objects are rendered that are part of the
	/// render layer. If layerName does not match a layer defined in Maya, the defaultRenderLayer 
	/// is used. If no layer is defined then a default color layer is generated. To enable
	/// picking, define a layer with name "pick".
	///
	/// The outputCode uses shader variables or deferred rendering variables to calculate the output 
	/// for the given layer.
	/// Shader variables:
	///   color (float3): the shaded and lighted color
	///   alpha (float3): one alpha channel for red, green and blue
	///   position (float3): view space position
	///   normal (float3): view space normal
	///   objectId (int): id of object (as returned by getObjectId())
	/// Defered rendering variables (zero if not supported by a shader):
	///   diffuseColor (float3): diffuse color
	///   specularColor (float3): phong specular color
	///   specularPower (float): phong specular power
	///   reflection (float3): reflection (reflectivity * reflecting light)
	///   incandescence (float3): self-illumination
	/// Output variables:
	///   sort (bool): object gets sorted when true (default false when not written)
	///   output (float4) or outputs (array of float4): the output data for the render target(s)
	/// Special functions:
	///   discard(): pixel is not drawn
	///
	/// The following output codes are predefined and used by passing a keyword to outputCode
	/// Color output code (outputCode = "color"):
	///   sort = alpha.x < 0.99f;
	///   if (max(color) < 0.01f & alpha.x < 0.01f) discard();
	///   output = vector4(color, alpha.x);
	/// Pick output code (outputCode = "pick"):
	///   if (max(color) < 0.01f & alpha.x < 0.01f) discard();
	///   output = vector4(convert_float(objectId), position.z, 0.0f, 1.0f);
	int setLayer(StringRef layerName, StringRef outputCode);

	virtual ~MCCache();

	virtual Pointer<EngineFile> load(const fs::path& path);

protected:

	struct LayerInfo
	{
		int index;
		std::string code;

		LayerInfo(int index = -2)
			: index(index) {}
	};
	
	std::map<std::string, LayerInfo> layerInfos;
	typedef std::pair<const std::string, LayerInfo> PassInfoPair;
};

/// @}

} // namespace digi

#endif
