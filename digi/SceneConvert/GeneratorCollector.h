#ifndef digi_SceneConvert_GeneratorCollector_h
#define digi_SceneConvert_GeneratorCollector_h

#include <map>

#include <digi/Scene/Scene.h>
#include <digi/EngineVM/Compiler.h>

#include "BufferInfo.h"
#include "SceneOptions.h"


namespace digi {

/// @addtogroup SceneConvert
/// @{

// print assigment of buffer field srcVar with format scrFormat to destination variable
void printAssign(CodeWriter& w, const std::string& srcValue, BufferFormat srcFormat, const ShaderType& destType);


struct SceneInfo
{
	// maximum transform index. used to determine size of transforms array in instance struct.
	int maxTransformIndex;

	// maximum bounding box index. used to determine size of boundingBoxes array in instance struct.
	int maxBoundingBoxIndex;

	// for each bounding box the transforms of shapes that use it
	std::map<int, std::set<int> > boundingBoxTransforms;
	typedef std::pair<const int, std::set<int> > BoundingBoxPair;

	SceneInfo()
		: maxTransformIndex(0), maxBoundingBoxIndex(0) {}
};

class GeneratorCollector
{
public:
	
	enum Flags
	{
		// render target is 565
		TARGET_X5Y6Z5 = 1,

		// render target is 888
		TARGET_X8Y8Z8 = 2,
		
		// no 8 bit index buffers
		NO_INDEX_8_BIT = 4,

		// no 32 bit index buffers
		NO_INDEX_32_BIT = 8,
	};
	
	GeneratorCollector(bool deformersInShaders, int dataMode,
		Type::AlignMode deformerAlignMode, Type::AlignMode shaderAlignMode, int flags)
		: deformersInShaders(deformersInShaders), dataMode(dataMode),
		deformerAlignMode(deformerAlignMode), shaderAlignMode(shaderAlignMode), flags(flags),
		sceneUseFlags(), shaderUseFlags(), numTextures()
	{
	}
	
	// collect instances from scene and build infos
	void collectInstances(const std::vector<ShaderOptions>& shaderOptions, Pointer<Scene> scene, SceneInfo& sceneInfo);

protected:
	void collectInstance(const std::vector<ShaderOptions>& shaderOptions, const Instance& instance,
		const std::vector<int>& layers, SceneInfo& sceneInfo);
	void collectInstance(const std::vector<ShaderOptions>& shaderOptions, Pointer<Instancer> instancer,
		int transformIndex, const std::vector<int>& layers, SceneInfo& sceneInfo);
	void collectParticleSystem(Pointer<ParticleSystem> particleSystem);
public:

	// choose target formats for buffer fields
	void chooseFormats(Pointer<BufferConverter> bufferConverter);

	// write output from scene to uniform struct for deformers, shaders, particle systems and particle instancers
	void writeUniformOutput(CodeWriter& w);

	// compile all shaders or deformer/shader combos
	bool compileShaders(Compiler& compiler, const std::vector<ShaderOptions>& shaderOptions,
		Language mainLanguage, Language shaderLanguage);

protected:
	void printCode(CodeWriter& w, const std::string& code);
public:

	// compile all deformer jobs (which were created in compileShaders())
	bool compileDeformers(Compiler& compiler, Language mainLanguage, Language deformerLanguage);

	// compile particle expressions
	bool compileParticleSystems(Compiler& compiler, Language mainLanguage);

	// write vertex buffers and add set offset/size infos
	int64_t writeBuffers(DataWriter& d, Pointer<BufferConverter> bufferConverter);
	
	// returns true if instance is in given layer or layer is a default layer
	bool isInLayer(int layerIndex, const NamedInstance& instance);
	
	// build render infos. returns number of vertex arrays needed for all shapes in all layers
	int buildRenderInfos();
	
	// get maximum number of quads to render one shape (e.g. text mesh)
	int getMaxNumQuads();
	

	// true if deformers are executed in the vertex shader (scene option)
	bool deformersInShaders;

	// vertex compression (scene option)
	int dataMode;
	
	// alignments
	Type::AlignMode deformerAlignMode;
	Type::AlignMode shaderAlignMode;
	
	// flags (see GeneratorCollector::Flags)
	int flags;
		
	// used to determine offsets in uniforms struct, transforms array and bounding boxes array when the shaders
	// and render jobs are compiled. set when scene is compiled
	Pointer<SplittedGlobalPrinter> uniformsPrinter;
	Pointer<SplittedGlobalPrinter> transformsPrinter;
	Pointer<SplittedGlobalPrinter> boundingBoxesPrinter;

	// layers
	int numLayers;
	std::vector<Pointer<Layer> > layers;
	
	
	// infos for collected objects
	ShapeInstancerInfoMap shapeInstancerInfoMap;
	BufferInfoMap bufferInfoMap;
	ShaderInfoMap shaderInfoMap;
	DeformerInfoMap deformerInfoMap;
	
	// shape infos (constant mesh, text mesh, particle system)
	ShapeInfoMap shapeInfoMap;
	
	ParticleInstancerInfoMap particleInstancerInfoMap;

	// all layer infos in one list for convenience
	std::vector<Pointer<LayerInfo> > layerInfos;


	struct EqualShaderLayerInfoLess
	{
		bool operator ()(Pointer<LayerInfo> a, Pointer<LayerInfo> b) const
		{
			return a->vertexShader < b->vertexShader || (a->vertexShader == b->vertexShader && a->pixelShader < b->pixelShader);
		}
	};
	typedef std::set<Pointer<LayerInfo>, EqualShaderLayerInfoLess> EqualShaderlayerInfoSet;
	EqualShaderlayerInfoSet equalShaderlayerInfos;
	
	
	struct ScaleOffsetArrayLess
	{
		bool operator ()(const std::vector<float>& a, const std::vector<float>& b) const
		{
			size_t size = a.size();
			if (size < b.size())
				return true;
			if (size > b.size())
				return false;
			for (size_t i = 0; i < size; ++i)
			{
				if (a[i] < b[i])
					return true;
			}
			return false;
		}
	};
	typedef std::map<std::vector<float>, int, ScaleOffsetArrayLess> ScaleOffsetArrayMap;
	typedef std::pair<const std::vector<float>, int> ScaleOffsetArrayPair;
	ScaleOffsetArrayMap scaleOffsetArrayMap;
	
	std::vector<NamedBufferInfo> namedBufferInfos;
	
	// for each scope (deformer and shader) a list of big vertex buffers (each containing a list of vertex buffers)
	std::vector<BigVertexBuffer> bigVertexBuffers[2];

	// offset and size in data file for index buffers
	std::vector<BigIndexBuffer> bigIndexBuffers;

	// list of dynamic buffers, each has a list of deformer jobs
	std::vector<DynamicBufferInfo> dynamicBufferInfos;
	
//int numTextMeshes;

	// scene use flags, e.g. SCENE_USES_SEED
	int sceneUseFlags;
	
	// combined shader use flags, e.g. SHADER_USES_VIEWPORT
	int shaderUseFlags;
	
	// number of textures used in vertex shader (x) and pixel shader (y)
	int2 numTextures;
};

/// @}

} // namespace digi

#endif
