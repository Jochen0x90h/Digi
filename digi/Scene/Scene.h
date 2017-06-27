#ifndef digi_Scene_Scene_h
#define digi_Scene_Scene_h

#include "Instancer.h"
#include "Shader.h"


namespace digi {

/// @addtogroup Scene
/// @{

class Scene : public Object
{
public:

	template <typename Type>
	struct NameLess
	{
		bool operator ()(const Type& a, const Type& b)
		{
			return a.name < b.name;
		}
		bool operator ()(const Type& a, const std::string& b)
		{
			return a.name < b;
		}
		bool operator ()(const std::string& a, const Type& b)
		{
			return a < b.name;
		}
	};


	// scene node
	struct Node
	{
		// name of node
		std::string name;
		
		// type of node (e.g. N_CAMERA)
		uint type;


		Node() : type() {}
		Node(const std::string& name, uint type)
			: name(name), type(type) {}
	};
	
	// scene attribute
	struct Attribute
	{
		// name of attribute
		std::string name;
				
		// type of attribute (e.g. P_FLOAT3. for arrays the number of elements is stored in bit 8 to 31)
		uint type;

		// path of attribute in state struct
		std::string path;
		
		// semantic (e.g. "color" or "min=0;max=1")
		std::string semantic;


		Attribute() : type() {}
		Attribute(const std::string& name, uint type, const std::string& path)
			: name(name), type(type), path(path) {}
	};	
	typedef NameLess<Attribute> AttributeLess;

	// binding of texture to scene attribute
	struct TextureBinding
	{
		// name of texture
		std::string textureName;

		// type of attribute (e.g. P_TEXTURE_2D, for texture sequence the number of textures is in bit 8 to 31)
		uint type;

		// path of attribute in state struct
		std::string path;

		
		TextureBinding() : type() {}
		TextureBinding(const std::string& textureName, uint type, const std::string& path)
			: textureName(textureName), type(type), path(path) {}
	};

	// attribute set
	struct AttributeSet
	{
		// name of attribute set
		std::string name;
		
		// path in state struct (is usually array of float)
		std::string path;
		
		// number of tracks that this attribute set controls
		uint numTracks;
				
		// index of first clip in clips array
		uint clipIndex;
		
		// number of clips
		uint numClips;
		
		AttributeSet()
			: numTracks(), clipIndex(), numClips() {}
	};

	// animation clip
	struct Clip
	{
		// name of clip
		std::string name;

		// index of clip used by addClip function
		uint index;

		// length of clip in seconds	
		float length;
		
		Clip()
			: index(), length() {}
		Clip(std::string name, uint index, float length)
			: name(name), index(index), length(length) {}
	};
/*
	struct CollisionObject
	{
		// name of object with instance index (e.g. "pCubeShape1[0]")
		std::string name;

		// index of transform to use from array of transforms
		uint transformIndex;
		
		// index of bounding box to use from array of bounding boxes
		uint boundingBoxIndex;

		Pointer<Primitive> primitive;
		
		CollisionObject()
			: transformIndex(), boundingBoxIndex() {}
		CollisionObject(std::string name, uint transformIndex, uint boundingBoxIndex, Pointer<Primitive> primitive)
			: name(name), transformIndex(transformIndex), boundingBoxIndex(boundingBoxIndex), primitive(primitive) {}
	};
*/
	Scene() {}

	Scene(const std::string& name)
		: name(name) {}

	virtual ~Scene();

	// name of scene
	std::string name;

	// render layers
	std::vector<Pointer<Layer> > layers;

	// type of scene state structure
	Pointer<Type> stateType;

	// number of transforms and bounding boxes
	uint transformCount;
	uint boundingBoxCount;

	// scene nodes
	std::vector<Node> nodes;

	// scene attributes that can be controlled from the outside
	std::vector<Attribute> attributes;
	
	// bindings of texture attributes to textures
	std::vector<TextureBinding> textureBindings;
	
	// attribute sets (Maya: character sets)
	std::vector<AttributeSet> attributeSets;

	// animation clips
	std::vector<Clip> clips;
	
	// named data buffers e.g. for animation tracks
	std::vector<NamedBuffer> buffers;

	// code that initializes the scene state
	std::string initStateCode;
	
	// code for update function
	std::string updateCode;
	
	// code for addClip function
	std::string addClipCode;
	
	// instances
	std::vector<NamedInstance> instances;
/*
	std::vector<Instance> renderInstances;
	
	// collision objects e.g. for hit-test
	std::vector<NamedInstance> collisionInstances;
*/
	
	// sort scene attributes by name
	void sortAttributes();
	
	
	static Pointer<Scene> load(ObjectReader& r);
	void save(ObjectWriter& w);
};	


// serializer for NamedBuffer
template <typename Serializer>
Serializer& operator &(Serializer& s, NamedBuffer& namedBuffer)
{
	s & namedBuffer.name & namedBuffer.buffer;
	return s;
}

/// @}

} // namespace digi

#endif
