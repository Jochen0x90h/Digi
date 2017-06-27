#ifndef digi_Engine_Engine_h
#define digi_Engine_Engine_h

#include <map>

#include <digi/Utility/ArrayRef.h>
#include <digi/Utility/StringRef.h>
#include <digi/Utility/VectorUtility.h>
#include <digi/Utility/UTFTranscode.h>
#include <digi/System/FileSystem.h>

#include "DataMemory.h"
#include "EngineInfo.h"


namespace digi {

/// @addtogroup Engine
/// @{

class EngineLoader;
class EngineFile;


class Engine : public Object
{
public:
	
	Engine();

	virtual ~Engine();

	/// add an existing engine file that contains a scene. the engine takes ownership,
	// i.e. don't reuse it after passing to addFile
	int addFile(Pointer<EngineFile> file);

	/// load inline file, i.e. where code is compiled into the c++ project. no file loader is needed. needs current OpenGL context
	int loadFile(const fs::path& path, const EngineInfo& engineInfo);

	/// add a file loader for given extension
	void addLoader(const std::string& ext, Pointer<EngineLoader> loader) {this->loaders[ext] = loader;}

	/// load file using a file loader which is selected by the extension of the given path. needs current OpenGL context
	int loadFile(const fs::path& path);

	/// unload a file, delete its scene instances and release its OpenGL resources. needs current OpenGL context
	void unloadFile(int fileHandle);

	/// clear all data and release OpenGL resources. needs current OpenGL context
	void clear();

	/// create a group for rendering or picking
	int createGroup();

	/// delete a group and all contained scene instances. needs current OpenGL context
	void deleteGroup(int groupHandle);
	
	/// calculate all dependent values such as world matrices for all scenes in group. needs current OpenGL context
	void updateGroup(int groupHandle);

	/// render a group using the given view and projection matrix. default render state is assumed and left behind.
	/// needs current OpenGL context
	void renderGroup(int groupHandle, const float4x4& viewMatrix, const float4x4& projectionMatrix, int layerIndex = 0);

	/// pick an object in given group at device coordinates x, y which range from -1 to 1. If an object was hit,
	/// return its handle. Handles to objects can be obtained with getObjectHandle().
	/// only works if a pick render layer is present. needs current OpenGL context
	int pickGroup(int groupHandle, const float4x4& viewMatrix, const float4x4& projectionMatrix, float x, float y);
	int pickGroup(int groupHandle, const float4x4& viewMatrix, const float4x4& projectionMatrix, float2 position)
	{
		return this->pickGroup(groupHandle, viewMatrix, projectionMatrix, position.x, position.y);
	}


	/// get number of scenes in the given file. returns -1 on error
	int getNumScenes(int fileHandle);

	/// get name of a scene by index
	StringRef getSceneName(int fileHandle, int sceneIndex);

	/// create a scene instance in the given group by index. returns a scene handle, -1 on error. needs current OpenGL context
	int createScene(int fileHandle, int sceneIndex, int groupHandle);

	/// create a scene instance in the given group by name. returns a scene handle, -1 on error. needs current OpenGL context
	int createScene(int fileHandle, StringRef sceneName, int groupHandle);
	
	/// delete a scene instance. needs current OpenGL context
	void deleteScene(int sceneHandle);
	
	/// get name of a scene
	StringRef getSceneName(int sceneHandle);
	
	/// calculate all dependent values such as world matrices for given scene. needs current OpenGL context
	void updateScene(int sceneHandle);

	/// get bounding box of scene
	BoundingBox getBoundingBox(int sceneHandle);


	/// get number of nodes in a scene. returns -1 on error
	int getNumNodes(int sceneHandle);
	
	/// get name of a node by index
	StringRef getNodeName(int sceneHandle, int nodeIndex);

	/// get type of a node by index. returns N_INVALID on error
	NodeType getNodeType(int sceneHandle, int nodeIndex);


	/// get number of attributes of a scene. returns -1 on error
	int getNumAttributes(int sceneHandle);
	
	/// get name of a scene attribute by index
	StringRef getAttributeName(int sceneHandle, int attributeIndex);
	
	/// get handle for a scene attribute by index. returns -1 on error
	int getAttributeHandle(int sceneHandle, int attributeIndex);

	/// get handle for a scene attribute by name. returns -1 on error
	int getAttributeHandle(int sceneHandle, StringRef attributeName);

	/// get attribute name
	StringRef getAttributeName(int attributeHandle)
	{
		// check attribute handle
		if (uint(attributeHandle) < this->numAttributes)
			return this->attributes[attributeHandle].attributeInfo->name;
		return StringRef();
	}

	/// get attribute type. returns P_INVALID on error
	AttributeType getAttributeType(int attributeHandle)
	{
		// check attribute handle
		if (uint(attributeHandle) < this->numAttributes)
		{
			return AttributeType(this->attributes[attributeHandle].attributeInfo->type & 0xff);
		}
		return P_INVALID;
	}

	/// get number of array elements. returns 0 if not an arry, -1 on error
	int getAttributeNumElements(int attributeHandle)
	{
		// check attribute handle
		if (uint(attributeHandle) < this->numAttributes)
		{
			return AttributeType(this->attributes[attributeHandle].attributeInfo->type >> 8);
		}
		return -1;
	}

	/// get attribute semantic
	StringRef getAttributeSemantic(int attributeHandle)
	{
		// check attribute handle
		if (uint(attributeHandle) < this->numAttributes)
		{
			return this->attributes[attributeHandle].attributeInfo->semantic;
		}
		return StringRef();
	}

	void setBool(int attributeHandle, bool v)
	{
		// check attribute handle
		if (uint(attributeHandle) < this->numAttributes)
		{
			Attribute attribute = this->attributes[attributeHandle];
			
			// set attribute if type matches
			if (attribute.attributeInfo->type == P_BOOL)
				*(bool*)attribute.pointer = v;
		}
	}
	
	bool getBool(int attributeHandle)
	{
		// check attribute handle
		if (uint(attributeHandle) < this->numAttributes)
		{
			Attribute attribute = this->attributes[attributeHandle];
		
			// set attribute if type matches
			if (attribute.attributeInfo->type == P_BOOL)
				return *(bool*)attribute.pointer;
		}
		return bool();
	}

	void setInt(int attributeHandle, int v)
	{
		// check attribute handle
		if (uint(attributeHandle) < this->numAttributes)
		{		
			Attribute attribute = this->attributes[attributeHandle];
			
			// set attribute if type matches
			if (attribute.attributeInfo->type >= P_INT && attribute.attributeInfo->type <= P_INT4)
				*(int*)attribute.pointer = v;
		}
	}

	int getInt(int attributeHandle)
	{
		// check attribute handle
		if (uint(attributeHandle) < this->numAttributes)
		{		
			Attribute attribute = this->attributes[attributeHandle];
			
			// set attribute if type matches
			if (attribute.attributeInfo->type >= P_INT && attribute.attributeInfo->type <= P_INT4)
				return *(int*)attribute.pointer;
		}
		return int();
	}

	void setFloat(int attributeHandle, float v)
	{
		// check attribute handle
		if (uint(attributeHandle) < this->numAttributes)
		{		
			Attribute attribute = this->attributes[attributeHandle];
			
			// set attribute if type matches
			if (attribute.attributeInfo->type >= P_FLOAT && attribute.attributeInfo->type <= P_FLOAT4)
				*(float*)attribute.pointer = v;
		}
	}
	
	float getFloat(int attributeHandle)
	{
		// check attribute handle
		if (uint(attributeHandle) < this->numAttributes)
		{		
			Attribute attribute = this->attributes[attributeHandle];
			
			// set attribute if type matches
			if (attribute.attributeInfo->type >= P_FLOAT && attribute.attributeInfo->type <= P_FLOAT4)
				return *(float*)attribute.pointer;
		}
		return float();
	}

	void setFloat2(int attributeHandle, const float2& v)
	{
		// check attribute handle
		if (uint(attributeHandle) < this->numAttributes)
		{
			Attribute attribute = this->attributes[attributeHandle];
			
			// set attribute if type matches
			if (attribute.attributeInfo->type >= P_FLOAT2 && attribute.attributeInfo->type <= P_FLOAT4)
				*(float2*)attribute.pointer = v;
		}
	}
	
	float2 getFloat2(int attributeHandle)
	{
		// check attribute handle
		if (uint(attributeHandle) < this->numAttributes)
		{
			Attribute attribute = this->attributes[attributeHandle];
			
			// set attribute if type matches
			if (attribute.attributeInfo->type >= P_FLOAT2 && attribute.attributeInfo->type <= P_FLOAT4)
				return *(float2*)attribute.pointer;
		}
		return float2();
	}

	void setFloat3(int attributeHandle, const float3& v)
	{
		// check attribute handle
		if (uint(attributeHandle) < this->numAttributes)
		{
			Attribute attribute = this->attributes[attributeHandle];
			
			// set attribute if type matches
			if (attribute.attributeInfo->type >= P_FLOAT3 && attribute.attributeInfo->type <= P_FLOAT4)
				*(float3*)attribute.pointer = v;
		}
	}
	
	float3 getFloat3(int attributeHandle)
	{
		// check attribute handle
		if (uint(attributeHandle) < this->numAttributes)
		{
			Attribute attribute = this->attributes[attributeHandle];
			
			// set attribute if type matches
			if (attribute.attributeInfo->type >= P_FLOAT3 && attribute.attributeInfo->type <= P_FLOAT4)
				return *(float3*)attribute.pointer;
		}
		return float3();
	}

	void setFloat4(int attributeHandle, const float4& v)
	{
		// check attribute handle
		if (uint(attributeHandle) < this->numAttributes)
		{
			Attribute attribute = this->attributes[attributeHandle];
			
			// set attribute if type matches
			if (attribute.attributeInfo->type == P_FLOAT4)
				*(float4*)attribute.pointer = v;
		}
	}

	float4 getFloat4(int attributeHandle)
	{
		// check attribute handle
		if (uint(attributeHandle) < this->numAttributes)
		{
			Attribute attribute = this->attributes[attributeHandle];
			
			// set attribute if type matches
			if (attribute.attributeInfo->type == P_FLOAT4)
				return *(float4*)attribute.pointer;
		}
		return float4();
	}

	/// get a matrix
	/// example:
	/// float4x4 viewMatrix = inv(renderInterface.getFloat4x4(cameraMatrix));
	/// renderInterface.renderGroup(group, time, viewMatrix, projectionMatrix);
	float4x4 getFloat4x4(int attributeHandle)
	{
		// check attribute handle
		if (uint(attributeHandle) < this->numAttributes)
		{
			Attribute attribute = this->attributes[attributeHandle];
			
			// check if type is float4x4
			if (attribute.attributeInfo->type == P_FLOAT4X4)
			{
				// return matrix
				return *(float4x4*)attribute.pointer;
			}
		}
		return float4x4Identity();
	}
	
	/// set float array attribute value. usable for float, float2, float3, float4, float4x4 and arrays of these
	void setFloatArray(int attributeHandle, ArrayRef<const float> a)
	{
		// check attribute handle
		if (uint(attributeHandle) < this->numAttributes)
		{
			Attribute attribute = this->attributes[attributeHandle];
		
			int type = attribute.attributeInfo->type;
			size_t numElements = max(type >> 8, 1);
			switch (type & 0xff)
			{
			case P_FLOAT:
				numElements *= 1;
				break;
			case P_FLOAT2:
				numElements *= 2;
				break;
			case P_FLOAT3:
				numElements *= 3;
				break;
			case P_FLOAT4:
				numElements *= 4;
				break;
			case P_FLOAT4X4:
				numElements *= 16;
				break;
			default:
				return;
			}
			float* p = (float*)attribute.pointer;
			std::copy(a.begin(), a.begin() + min(a.size(), numElements), p);
		}
	}

	/// get float array attribute value. usable for float, float2, float3, float4, float4x4 and arrays of these
	ArrayRef<float> getFloatArray(int attributeHandle)
	{
		// check attribute handle
		if (uint(attributeHandle) < this->numAttributes)
		{
			Attribute attribute = this->attributes[attributeHandle];
		
			int type = attribute.attributeInfo->type;
			int numElements = max(type >> 8, 1);
			switch (type & 0xff)
			{
			case P_FLOAT:
				numElements *= 1;
				break;
			case P_FLOAT2:
				numElements *= 2;
				break;
			case P_FLOAT3:
				numElements *= 3;
				break;
			case P_FLOAT4:
				numElements *= 4;
				break;
			case P_FLOAT4X4:
				numElements *= 16;
				break;
			default:
				return ArrayRef<float>();
			}
			float* p = (float*)attribute.pointer;
			return ArrayRef<float>(p, numElements);
		}
		return ArrayRef<float>();
	}
	
	/// get a projection matrix. view aspect is the physical aspect ratio of the view
	///	(i.e. as seen on a real monitor)
	/// example:
	/// float4x4 projectionMatrix = renderInterface.multProjection(cameraProjection, windowAspect);
	/// renderInterface.renderGroup(group, time, viewMatrix, projectionMatrix);
	float4x4 getFloatProjection(int attributeHandle, float viewAspect)
	{
		// check attribute handle
		if (uint(attributeHandle) < this->numAttributes)
		{
			Attribute attribute = this->attributes[attributeHandle];
			
			// check if type is projection
			if (attribute.attributeInfo->type == P_PROJECTION)
			{
				// calc projection matrix from projection structure
				return matrix4x4Projection(*(Projection*)attribute.pointer, viewAspect);
			}
		}
		return float4x4Identity();
	}
	
	void setTexture(int attributeHandle, GLuint texture)
	{
		// check attribute handle
		if (uint(attributeHandle) < this->numAttributes)
		{
			Attribute attribute = this->attributes[attributeHandle];
			
			// set attribute if type matches
			uint type = attribute.attributeInfo->type & 0xFF;
			if (type >= P_TEXTURE_2D && type <= P_TEXTURE_CUBE)
				*(GLuint*)attribute.pointer = texture;
		}
	}
	
	GLuint getTexture(int attributeHandle)
	{
		// check attribute handle
		if (uint(attributeHandle) < this->numAttributes)
		{
			Attribute attribute = this->attributes[attributeHandle];
			
			// set attribute if type matches
			uint type = attribute.attributeInfo->type & 0xFF;
			if (type >= P_TEXTURE_2D && type <= P_TEXTURE_CUBE)
				return *(GLuint*)attribute.pointer;
		}
		return 0;
	}

	void setString(int attributeHandle, StringRef str)
	{
		// check attribute handle
		if (uint(attributeHandle) < this->numAttributes)
		{
			Attribute attribute = this->attributes[attributeHandle];
			
			// set attribute if type matches
			uint type = attribute.attributeInfo->type & 0xFF;
			if (type == P_STRING)
			{
				std::string& s = strings[attributeHandle];
				s.resize(str.size());
				std::copy(str.begin(), str.end(), s.begin());
				*(const char**)attribute.pointer = s.c_str();
			}
		}
	}

	void setString(int attributeHandle, const wchar_t* str)
	{
		// check attribute handle
		if (uint(attributeHandle) < this->numAttributes)
		{
			Attribute attribute = this->attributes[attributeHandle];
			
			// set attribute if type matches
			uint type = attribute.attributeInfo->type & 0xFF;
			if (type == P_STRING)
			{
				std::string& s = strings[attributeHandle];
				s.clear();
				utfTranscode(str, s);
				*(const char**)attribute.pointer = s.c_str();
			}
		}
	}

	
	/// get number of attribute sets of a scene. returns -1 on error
	int getNumAttributeSets(int sceneHandle);
	
	/// get name of a attribute set by index
	StringRef getAttributeSetName(int sceneHandle, int attributeSetIndex);

	/// get handle to a attribute set by index (Maya: Character Set). returns -1 on error
	int getAttributeSetHandle(int sceneHandle, int attributeSetIndex);

	/// get handle to a attribute set by name (Maya: Character Set). returns -1 on error
	int getAttributeSetHandle(int sceneHandle, StringRef attributeSetName);

	/// get attribute set name
	StringRef getAttributeSetName(int attributeSetHandle)
	{
		// check attribute set handle
		if (uint(attributeSetHandle) < uint(this->attributeSets.size()))
			return this->attributeSets[attributeSetHandle].attributeSetInfo->name;
		return StringRef();
	}

	/// get number of clips of a attribute set. returns -1 on error
	int getNumClips(int attributeSetHandle);
	
	/// get name of a clip by index
	StringRef getClipName(int attributeSetHandle, int clipIndex);

	/// get length of a clip
	float getClipLength(int attributeSetHandle, int clipIndex);
	
	/// get index of a clip by name. returns -1 on error
	int getClipIndex(int attributeSetHandle, StringRef clipName);

	/// set number of tracks for attribute set (default is 1)
	void setNumTracks(int attributeSetHandle, int numTracks);

	/// set a clip to a track, -1 for no clip. always call this function if a
	/// new clip starts or when clipTime or weight change discontinuously
	void setClip(int attributeSetHandle, int trackIndex, int clipIndex);

	/// set clip time and weight of a track
	void setTrackParameters(int attributeSetHandle, int trackIndex, float clipTime, float weight);


	/// get number of objects in a scene. returns -1 on error
	int getNumObjects(int sceneHandle);

	/// get an id of an object in a scene by index. this id is returned by pickGroup() if the object was hit.
	/// append the instance index in square brackets, e.g. "pCubeShape1[0]".
	int getObjectId(int sceneHandle, int objectIndex);

	/// get an id of an object in a scene by name. this id is returned by pickGroup() if the object was hit.
	/// append the instance index in square brackets, e.g. "pCubeShape1[0]".
	int getObjectId(int sceneHandle, StringRef objectName);


	//struct AttributeSetHandles
	//{
	//	int attributeSetHandle;
	//	std::vector<int> clipHandles;
	//};
	
	class Scene
	{
	public:
		Scene(const SceneInfo& sceneInfo, void* global)
			: sceneInfo(sceneInfo), instance(sceneInfo.instanceSize),
			attributeHandles(sceneInfo.attributeInfos.size(), -1),
			attributeSetHandles(sceneInfo.attributeSetInfos.size(), -1),
			clipHandles(sceneInfo.clipInfos.size(), -1),
			//objectHandles(sceneInfo.collisionObjectInfos.size(), -1),
			sceneIndex(), groupIndex(),fileIndex()
		{
			sceneInfo.initInstance(global, this->instance);
		}
		~Scene()
		{
			this->sceneInfo.doneInstance(this->instance);
		}

		const SceneInfo& sceneInfo;
		DataMemory instance;
		
		std::vector<int> attributeHandles;
		std::vector<int> attributeSetHandles;
		std::vector<int> clipHandles;
		
		//std::vector<int> objectHandles;
		
		// index of this scene
		int sceneIndex;

		// index of group that this scene belongs to
		int groupIndex;

		// index of file that this scene was instantiated from
		int fileIndex;
	};

protected:

	// disable copy constructor
	Engine(const Engine&) {}

	struct Attribute
	{
		const AttributeInfo* attributeInfo;
			
		// pointer to attribute (points into state of scene instance)
		uint8_t* pointer;
	};

	struct Track
	{
		// info of clip that is played in this track (NULL for none)
		const ClipInfo* clipInfo;

		// current time of animation clip
		float clipTime;
		
		// current weight
		float weight;

		Track()
			: clipInfo(), clipTime(), weight() {}
	};

	struct AttributeSet
	{
		const AttributeSetInfo* attributeSetInfo;

		Scene* scene;
		
		// pointer to attributes in scene
		float* attributes;

		std::vector<Track> tracks;

		AttributeSet()
			: tracks(1) {}
	};
/*
	struct Clip
	{
		const ClipInfo* clipInfo;

		Scene* scene;

		// pointer to track data
		float* tracks;
				
		// current time of animation clip
		float time;
		
		// current weight
		float weight;
	};
*/

	int createScene(Pointer<EngineFile> file, int sceneIndex, int groupHandle);
	void deleteScene(Scene* scene);

	virtual int getAttributeHandleInternal(int sceneHandle, int attributeIndex);
	virtual int getAttributeSetHandleInternal(int sceneHandle, int attributeSetIndex);
	//virtual int getClipHandleInternal(int attributeSetHandle, int clipIndex);
	
	void updateScene(Scene* scene);


	// memory for render jobs
	DataMemory renderJobs;

	// 1x1 frame buffer object target for picking
	GLuint pickFBO;
	
	// file loaders
	std::map<std::string, Pointer<EngineLoader> > loaders;
	
	// loaded files
	std::vector<Pointer<EngineFile> > files;
	
	typedef std::vector<Scene*> Group;

	// render groups (containing scene instances)
	std::vector<Group*> groups;

	// mapping form scene handle to scene pointer
	std::vector<Scene*> scenes;

	// attribute instances
	std::vector<Attribute> attributes;
	uint numAttributes;
	AttributeInfo dummyAttributeInfo;
	std::map<int, std::string> strings;

	// attribute set instances
	std::vector<AttributeSet> attributeSets;

	// clip instances
	//std::vector<Clip> clips;

	int nextObjectId;
};

// base class for scene file loader
class EngineLoader : public Object
{
public:
	virtual ~EngineLoader();

	// loads a scene file
	virtual Pointer<EngineFile> load(const fs::path& path) = 0;
};

// base class for loaded scene file
class EngineFile : public Object
{
	friend class Engine;

public:
	virtual ~EngineFile();

	virtual ArrayRef<const TextureInfo> getTextureInfos() = 0;
	virtual void* getTextureGlobal(int index) = 0;
	
	virtual ArrayRef<const SceneInfo> getSceneInfos() = 0;
	virtual void* getSceneGlobal(int index) = 0;

	// destroy the engine file, also frees OpenGL resources.
	// therefore the correct OpenGL context must be the current context
	virtual void done() = 0;

private:

	// all scenes that are instances of this file
	std::vector<Engine::Scene*> scenes;
};

/// @}

} // namespace digi

#endif
