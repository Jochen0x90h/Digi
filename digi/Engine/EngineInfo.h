#ifndef digi_Engine_EngineInfo_h
#define digi_Engine_EngineInfo_h

#include <digi/Math/All.h>
#include "RenderTypes.h"
#include "RenderJob.h"
#include "ParameterType.h"


namespace digi {

/// @addtogroup Engine
/// @{

struct NodeInfo;
struct AttributeInfo;
struct TextureBinding;
struct AttributeSetInfo;
struct ClipInfo;
struct ObjectInfo;


template <typename Type>
struct ArrayData
{
	typedef Type value_type;
	typedef const Type* iterator;
	typedef const Type* const_iterator;

	const Type* data;
	uint length;
	
	uint size() const {return this->length;}
	iterator begin() const {return this->data;}
	iterator end() const {return this->data + this->length;}
	
	const Type& operator[](uint index) const
	{
		return this->data[index];
	}		
};

/// texture info element for global list of textures
struct TextureInfo
{	
	// name of texture
	const char* name;

	// texture type (T_TEXTURE and array length starting at bit 8)
	uint type;

	// size of global data
	uint globalSize;
	
	// initGlobal(global, data)
	typedef void (*InitGlobal)(void*, uint8_t*);

	// doneGlobal(global)
	typedef void (*DoneGlobal)(void*);
	
	// copy(global, destination)
	typedef void (*Copy)(const void*, void*);

	
	InitGlobal initGlobal;
	DoneGlobal doneGlobal;
	Copy copy;
};

/// scene info element for global list of scenes
struct SceneInfo
{
	// name of scene
	const char* name;

	// list of scene node infos
	ArrayData<NodeInfo> nodeInfos;

	// list of scene attribute infos
	ArrayData<AttributeInfo> attributeInfos;

	// list of texture bindings
	ArrayData<TextureBinding> textureBindings;

	// list of attribute set infos
	ArrayData<AttributeSetInfo> attributeSetInfos;
	
	// list of clip infos
	ArrayData<ClipInfo> clipInfos;

	// list of object infos
	ArrayData<ObjectInfo> objectInfos;

	// size of global data
	uint globalSize;

	// size of instance data
	uint instanceSize;

	// void initGlobal(global, data)
	typedef void (*InitGlobal)(void*, uint8_t*);

	// void doneGlobal(global)
	typedef void (*DoneGlobal)(void*);

	// void initInstance(global, instance)
	typedef void (*InitInstance)(const void*, void*);

	// void doneInstance(instance)
	typedef void (*DoneInstance)(void*);

	// void addClip(instance, index, tracks, time, weight)
	typedef void (*AddClip)(void*, int, float*, float, float);

	// void update(instance)
	typedef void (*Update)(void*);

	// void getBoundingBox(instance, boundingBox)
	typedef void (*GetBoundingBox)(void*, float4x2&);

	// void render(instance, viewMatrix, projectionMatrix, layerIndex, renderQueues)
	typedef void (*Render)(void*, const float4x4&, const float4x4&, int, RenderQueues&);


	InitGlobal initGlobal;
	DoneGlobal doneGlobal;
	InitInstance initInstance;
	DoneInstance doneInstance;
	AddClip addClip;
	Update update;
	GetBoundingBox getBoundingBox;
	Render render;
};

/// info for scene node
struct NodeInfo
{
	// name of node
	const char* name;
	
	// type of node (e.g. "N_CAMERA")
	uint type;
};

/// info for scene attribute
struct AttributeInfo
{	
	// name of attribute
	const char* name;

	// type of attribute (e.g. "P_FLOAT3")
	uint type;

	// offset in instance data
	uint offset;
	
	// semantic (e.g. "color" or "min=0;max=1")
	const char* semantic;
};

/// binding of a attribute to a texture
struct TextureBinding
{
	uint textureIndex;
	uint type;
	uint offset;
};

/// info for attribute set (character)
struct AttributeSetInfo
{
	const char* name;

	// offset in instance data
	uint offset;

	// number of tracks that this attribute set controls		
	uint numTracks;
	
	// index of first clip in clips array
	uint clipIndex;
	
	// number of clips
	uint numClips;
};

/// info for clip
struct ClipInfo
{
	// name of clip
	const char* name;
	
	// index of clip used by addClip function
	uint index;
	
	// length of clip in seconds	
	float length;
};

/// info for object
struct ObjectInfo
{
	const char* name;

	// offset of id in instance
	size_t offset;
};

/// info for list of textures and scenes
struct EngineInfo
{
	ArrayData<TextureInfo> textureInfos;
	ArrayData<SceneInfo> sceneInfos;
};

/// @}

} // namespace digi

#endif
