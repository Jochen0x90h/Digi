#ifndef digi_SceneConvert_BufferInfo_h
#define digi_SceneConvert_BufferInfo_h

#include <map>

#include <digi/Utility/ArrayUtility.h>
#include <digi/Data/DataWriter.h>
#include <digi/ImageConvert/BufferConverter.h>
#include <digi/CodeGenerator/Type.h>
#include <digi/CodeGenerator/TypeInfo.h>
#include <digi/CodeGenerator/NameGenerator.h>
#include <digi/Scene/VertexField.h>
#include <digi/Scene/Instancer.h>
#include <digi/Scene/Shape.h>

#include "ShaderType.h"
#include "ShaderOptions.h"
#include "SplittedGlobalPrinter.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"


namespace digi {

/// @addtogroup SceneConvert
/// @{

const int SCOPE_DEFORMER = 0;
const int SCOPE_SHADER = 1;
const int NUM_SCOPES = 2;


// info for buffer of BufferVertexField
class BufferInfo : public Object
{
public:

	// the shapes (constant meshes) that use this buffer
	std::set<Pointer<Shape> > shapes;

	// set of all semantics that describe the usage of the buffer (e.g. POSITION, TEXCOORD)
	int semanticFlags;
	
	// target format of buffer
	BufferFormat format;

	// scale for field value
	float4 scale;

	// offset for field value
	float4 offset;

	// usage scope. buffers used by deformers go into system memory,
	// buffers used by shaders go into graphics memory
	struct Scope
	{
		// used by deformer or shader	
		bool used;
	
		// the vertex buffer that this field is part of
		Pointer<VertexBuffer> vertexBuffer;
		
		// field index in vertex buffer
		int fieldIndex;

		Scope()
			: used(false), fieldIndex() {}
	};

	Scope scopes[NUM_SCOPES];
		
	BufferInfo()
			: semanticFlags(0), scale(splat4(1.0f)), offset(splat4(0.0f)) {}

	virtual ~BufferInfo();
};
typedef std::map<Pointer<Buffer>, Pointer<BufferInfo> > BufferInfoMap;
typedef std::pair<const Pointer<Buffer>, Pointer<BufferInfo> > BufferInfoPair;


// info for deformer job. one deformer may be used to deform many shapes or paricle systems
struct DeformerJobInfo
{
	// the shape that this job belongs to
	Pointer<Shape> shape;

	// the deformer that this job belongs to
	Pointer<Deformer> deformer;
	
	// output fields that are used by the shaders
	std::set<std::string> usedOutputFields;

	// extra fields to copy from input to output
	std::set<std::string> extraFields;

	// compile result
	bool hasError;
	std::string uniformCode;
	std::string vertexCode;
	std::vector<ShaderVariable> uniformVariables;

	// binding of vertex buffers to deformer (e.g. "position" -> float3 "_0")
	std::map<std::string, ShaderVariable> vertexBindings;
	typedef std::pair<const std::string, ShaderVariable> VertexBindingPair;


	// binding of deformer to result buffer (e.g. "position" -> float3 "res0")
	std::map<std::string, ShaderVariable> resultBindings;

	// output field info
	struct FieldInfo
	{
		// name of field (e.g. "position", used to map to vertex shader input)
		std::string name;

		// format info
		BufferFormat format;
		
		// offset of field (in bytes) in vertex element
		int offset;
		
		
		FieldInfo()
			: offset() {}
		FieldInfo(const std::string& name, const BufferFormat& format, int offset)
			: name(name), format(format), offset(offset) {}
	};

	// field infos for output vertex
	std::vector<FieldInfo> fieldInfos;

	// size of output vertex
	int vertexSize;

	// name of output vertex type
	//std::string outputTypeName;

	// index of output dynamic buffer
	int dynamicBufferIndex;
	
	// offset (in bytes) in dynamic buffer
	size_t dynamicBufferOffset;

	// size (in bytes) in dynamic buffer
	size_t dynamicBufferSize;

	DeformerJobInfo()
		: hasError(), vertexSize(), dynamicBufferIndex(), dynamicBufferOffset(), dynamicBufferSize() {}
		
	// set output type. creates field infos and calculates vertex size
	void setOutputType(Pointer<StructType> type, Type::AlignMode alignMode);
	
	// get an unique string for the output format (not human readable!) to combine vertex buffers of the same format
	std::string getFormatString();

	// write output struct type
	//void writeStruct(CodeWriter& w, Type::Alignment alignment);
};

// describes where data of a vertex field comes from: buffer or field of particle system
struct FieldOfObject
{
	// Buffer or ParticleSystem
	Pointer<Object> object;
	
	// name of field for particle system, empty otherwise
	std::string fieldName;
	
	FieldOfObject()
		{}
	FieldOfObject(Pointer<Object> object)
		: object(object) {}
	FieldOfObject(Pointer<Object> object, std::string fieldName)
		: object(object), fieldName(fieldName) {}
};
inline bool operator <(const FieldOfObject& a, const FieldOfObject& b)
{
	return a.object < b.object || (a.object == b.object && a.fieldName < b.fieldName);
}

// deformer job index, is a map of vertex field names and their data sources
typedef std::map<std::string, FieldOfObject> DeformerJobIndex;

// map for DeformerJobInfo's
typedef std::map<DeformerJobIndex, DeformerJobInfo> DeformerJobInfoMap;
typedef std::pair<const DeformerJobIndex, DeformerJobInfo> DeformerJobInfoPair;

// info for deformer
struct DeformerInfo
{
	// mangled uniform variable name (generated from deformer name)
	std::string name;

	// deformer jobs. the string uniquely identifies a deformer job
	DeformerJobInfoMap jobInfos;


	DeformerInfo()
		{}
};
typedef std::pair<const Pointer<Deformer>, DeformerInfo> DeformerInfoPair;
typedef std::map<Pointer<Deformer>, DeformerInfo> DeformerInfoMap;


// helper struct for scale/offset uniform falgs calculation: buffers that are automatically reduced in precision
// (e.g. float -> int16) need extra scale/offset uniform shader variables to restore original value range
struct ScaleOffsetInfo
{
	// buffers of a field needed to calc scale/offset uniform flags
	std::set<Pointer<BufferInfo> > bufferInfos;

	// additional scale/offset of 1.0f/0.0f e.g. for deformer output that has float format
	bool unitScale;

	ScaleOffsetInfo()
		: unitScale(false) {}
};
typedef std::map<std::string, ScaleOffsetInfo> ScaleOffsetInfoMap;


// info for layer (combo of layer, shader, deformer if in shader, shape type)
class LayerInfo : public Object
{
public:

	// shape type (MESH or SPRITES)
	Shader::ShapeType shapeType;
	
	//int renderMode;
	std::set<int> renderModes;
	
	enum RenderFlags
	{
		// at least one shape is double sided
		DOUBLE_SIDED_FLAG = 1,
		
		// at least one shape visible from the front side
		FRONT_SIDE_FLAG = 2,
		
		// at least one shape visible from the back side
		BACK_SIDE_FLAG = 4
	};
	
	// render modes of the shapes that use this shader
	int renderFlags;

	// shape instancers that use this layer info
	std::set<Pointer<ShapeInstancer> > shapeInstancers;

	// for each field name the set of vertex fields accessed by this deformer/shader combo
	ScaleOffsetInfoMap scaleOffsetInfos;

	// scale/offset for packed vertex buffers
	Pointer<StructType> scaleOffsetType;


	// deformer name (if any), shader name and layer index (if more than one layer)
	std::string comment;

	// name for types and variables (generated from shader, deformer and layer)
	std::string name;

	// material code, calculates transfer and material variables
	std::string materialCode;

	// transform code, uses transfer and calculates transform variables
	std::string transformCode;
	
	// vertex/pixel shader code
	std::string vertexShader;
	std::string pixelShader;
	
	// pointer to other layerInfo if it has the same vertex/pixel shader (by string comparison)
	Pointer<LayerInfo> equalShaderLayerInfo;

	// true if shader is shared between layerInfos
	bool sharedShader;

	// transfer between material and transform
	std::vector<ShaderVariable> transferVariables;
	
	// shader variables
	std::vector<ShaderVariable> materialVariables; // uniform variables including textures
	std::vector<ShaderVariable> materialLargeArrays; // large uniform arrays that are stored in textures
	std::vector<ShaderVariable> transformVariables; // uniform variables
		
	// binding of vertexs buffer to shader (e.g. "position" -> float3 "_0")
	std::map<std::string, ShaderVariable> vertexBindings;
	typedef std::pair<const std::string, ShaderVariable> VertexBindingPair;
	
	SortMode sortMode;

	// flags that indicate which input variables (e.g. id) were used
	int useFlags;

	// offset of shader code in data file
	int64_t vertexShaderOffset;
	int64_t pixelShaderOffset;


	LayerInfo()
		: shapeType(), renderFlags(),
		sharedShader(false), sortMode(),
		useFlags(0), vertexShaderOffset(), pixelShaderOffset() {}
	
	virtual ~LayerInfo();
};
typedef std::map<int, Pointer<LayerInfo> > LayerInfoMap;
typedef std::pair<const int, Pointer<LayerInfo> > LayerInfoPair;


// for everything that is actually rendered we have one render info
struct RenderInfo
{
	// index of layer
	//int layerIndex;
	
	// layer info
	Pointer<LayerInfo> layerInfo;

	// the effective render mode (render mode from shape with optional overrides)
	int renderMode;
	

	// index of first vertex array (vertex array object)
	int vertexArrayIndex;
	
	// index of global scale offset array
	int scaleOffsetArrayIndex;

	// size of global scale offset array (in float4 elements)
	int scaleOffsetArraySize;

	// scale/offset array for packed vertex buffers. gets applied to the inputs of the vertex shader
	//std::vector<float> scaleOffset;

	// name of draw function
	std::string drawFunctionName;

	RenderInfo()
		: renderMode(),
		vertexArrayIndex(), scaleOffsetArrayIndex(), scaleOffsetArraySize() {}
};
typedef std::pair<const int, RenderInfo> RenderInfoPair;
typedef std::map<int, RenderInfo> RenderInfoMap;


// info for shape instancer (ShapeInstancer)
struct ShapeInstancerInfo
{	
	// name of render function for shape
	//std::string renderFunctionName;
	
	// index for deformer job consisting of field names and their associated objects (buffer, generator)
	DeformerJobIndex deformerJobIndex;

	// mesh or sprites
	Shader::ShapeType shapeType;


	// the deformer from the shape instancer if deformers are NOT in shaders
	Pointer<Deformer> deformer;
	
	
	//std::vector<RenderInfo> renderInfos;
	RenderInfoMap renderInfoMap;

	ShapeInstancerInfo()
		: shapeType() {}
};
typedef std::map<Pointer<ShapeInstancer>, ShapeInstancerInfo> ShapeInstancerInfoMap;
typedef std::pair<const Pointer<ShapeInstancer>, ShapeInstancerInfo> ShapeInstancerInfoPair;


class ShapeInfo : public Object
{
public:

	virtual ~ShapeInfo();
	
	// get type of shape
	virtual Shader::ShapeType getType() = 0;
	
	// get name (empty for constant mesh)
	virtual StringRef getName() = 0;
	
	// get maximum number of vertices
	virtual int getNumVertices() = 0;
	
	// get number of vertex arrays (number of index buffer sections for constant mesh)
	virtual int getNumVertexArrays() = 0;
	
	// get base index for given vertex array index
	virtual int getBaseIndex(int vertexArrayIndex) = 0;
};
typedef std::pair<const Pointer<Shape>, Pointer<ShapeInfo> > ShapeInfoPair;
typedef std::map<Pointer<Shape>, Pointer<ShapeInfo> > ShapeInfoMap;

// info for constant mesh
class ConstantMeshInfo : public ShapeInfo
{
public:

	int numVertices;
	
	// index buffer that can consist of multiple sections to avoid 32 bit indices
	Pointer<IndexBuffer> indexBuffer;


	ConstantMeshInfo()
		: numVertices() {}

	virtual ~ConstantMeshInfo();
	virtual Shader::ShapeType getType();
	virtual StringRef getName();
	virtual int getNumVertices();
	virtual int getNumVertexArrays();
	virtual int getBaseIndex(int vertexArrayIndex);
};

// info for text mesh
struct TextMeshInfo : public ShapeInfo
{
public:

	// mangled uniform variable name
	std::string name;

	int numVertices;
	
	int64_t indicesOffset;
	int64_t stringsOffset;
	int64_t symbolsOffset;


	TextMeshInfo()
		: numVertices(), indicesOffset(), stringsOffset(), symbolsOffset() {}

	virtual ~TextMeshInfo();
	virtual Shader::ShapeType getType();
	virtual StringRef getName();
	virtual int getNumVertices();
	virtual int getNumVertexArrays();
	virtual int getBaseIndex(int vertexArrayIndex);
};

// info for paticle system (which is a shape)
class ParticleSystemInfo : public ShapeInfo
{
public:

	// mangled uniform variable name
	std::string name;

	// number of particles that get preallocated
	int numParticles;

	// number of particles that get initialized in initInstance
	int numInitParticles;

	// offset in particle data
	int64_t offset;

	bool hasError;
	Pointer<SplittedGlobalPrinter> particlePrinter;
	std::string createUniformCode;
	std::string createParticleCode;
	std::string updateUniformCode;
	std::string updateParticleCode;


	ParticleSystemInfo()
		: numParticles(), numInitParticles(), offset(),
		hasError(false) {}

	virtual ~ParticleSystemInfo();
	virtual Shader::ShapeType getType();
	virtual StringRef getName();
	virtual int getNumVertices();
	virtual int getNumVertexArrays();
	virtual int getBaseIndex(int vertexArrayIndex);
};




// info for deformer/shader combination (deformer may be null)
struct TargetShaderInfo
{
/*
	// shapes that use this deformer/shader combo
	std::set<Pointer<ShapeInstancer> > shapeInstancers;

	// for each field name the set of vertex fields accessed by this deformer/shader combo
	ScaleOffsetInfoMap scaleOffsetInfos;

	// scale/offset for packed vertex buffers
	Pointer<StructType> scaleOffsetType;
*/

	// layer infos for each layer index
	typedef std::map<int, Pointer<LayerInfo> > LayerInfoMap;
	typedef std::pair<const int, Pointer<LayerInfo> > LayerInfoPair;
	LayerInfoMap layerInfoMap;

	TargetShaderInfo() {}
};

struct ShapeTypeAndMode
{
	Shader::ShapeType type;
	int mode;
	
	ShapeTypeAndMode()
		: type(), mode() {}
	ShapeTypeAndMode(Shader::ShapeType type)
		: type(type), mode(0) {}
	//ShapeTypeAndMode(Shader::ShapeType type, int mode)
	//	: type(type), mode(mode) {}

};
inline bool operator <(const ShapeTypeAndMode& a, const ShapeTypeAndMode& b)
{
	return a.type < b.type || (a.type == b.type && a.mode < b.mode);
}

typedef std::map<ShapeTypeAndMode, TargetShaderInfo> DeformerShaderInfo;
typedef std::pair<const ShapeTypeAndMode, TargetShaderInfo> TargetShaderInfoPair;

typedef std::pair<const Pointer<Deformer>, DeformerShaderInfo> DeformerShaderInfoPair;
typedef std::map<Pointer<Deformer>, DeformerShaderInfo> DeformerShaderInfoMap;


struct ShaderInfo
{
	// uniform type name (for uniform struct type, generated from shader name)
	std::string typeName;

	// deformers in shaders: deformers that are combined with this shader (deformer = null possible for no deformer)
	// deformers not in shaders: only one entry with deformer = null
	DeformerShaderInfoMap deformerShaderInfos;
	

	ShaderInfo() {}
};
typedef std::pair<const Pointer<Shader>, ShaderInfo> ShaderInfoPair;
typedef std::map<Pointer<Shader>, ShaderInfo> ShaderInfoMap;


struct OffsetSize
{
	// offset in data file
	int64_t offset;
	
	// size of data
	size_t size;
	
	OffsetSize()
		: offset(), size() {}
		
	OffsetSize(int64_t offset, size_t size)
		: offset(offset), size(size) {}
};

struct ParticleInfo
{
	// offset/size in data file
	OffsetSize buffer;

	// write buffers and structs and add set offset/size infos
	void writeBuffers(Pointer<BufferConverter> bufferConverter, ShapeInfoMap& shapeInfos,
		CodeWriter& w, DataWriter& d);
};


// info for shape particle instancer
struct ParticleInstancerInfo
{
	// mangled uniform variable name
	std::string name;

	
	ParticleInstancerInfo()
		{}
};
typedef std::pair<const Pointer<ParticleInstancer>, ParticleInstancerInfo> ParticleInstancerInfoPair;
typedef std::map<Pointer<ParticleInstancer>, ParticleInstancerInfo> ParticleInstancerInfoMap;



struct NamedBufferInfo
{
	const NamedBuffer* namedBuffer;
	BufferFormat format;
	float scale;
	float offset;
	
	NamedBufferInfo()
		: namedBuffer(), scale(), offset() {}
	NamedBufferInfo(const NamedBuffer* namedBuffer, BufferFormat format, float scale, float offset)
		: namedBuffer(namedBuffer), format(format), scale(scale), offset(offset) {}
};

// info for big vertex buffer that contains multiple vertex buffers of same format
struct BigVertexBuffer
{
	// first vertex buffer for format
	Pointer<VertexBuffer> vertexBuffer;

	// number of vertices in big buffer
	size_t vertexCount;


	BigVertexBuffer()
		: vertexCount() {}

	BigVertexBuffer(Pointer<VertexBuffer> vertexBuffer, size_t vertexCount)
		: vertexBuffer(vertexBuffer), vertexCount(vertexCount) {}
};

// info for big index buffer that contains multiple index buffers of same format
struct BigIndexBuffer
{
	// size of one index (1, 2 or 4 bytes)
	int indexSize;

	// number of vertices in big buffer
	size_t indexCount;

	
	BigIndexBuffer()
		: indexSize(), indexCount() {}
		
	BigIndexBuffer(int indexSize, size_t indexCount)
		: indexSize(indexSize), indexCount(indexCount) {}
};

// info for dynamic buffer for deformer output (contains multiple sub buffers of the same vertex format)
struct DynamicBufferInfo
{
	// size (in bytes) of this dynamic buffer
	size_t size;

	// maximum size of a sub-buffer in this dynamic buffer
	size_t maxBufferSize;
	
	// deformer jobs to do to recalculate this dynamic buffer
	std::vector<DeformerJobInfoMap::iterator> deformerJobIterators;
	
	DynamicBufferInfo()
		: size(), maxBufferSize() {}
};

/// @}

} // namespace digi

#endif
