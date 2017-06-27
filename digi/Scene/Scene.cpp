#include <digi/Utility/VectorUtility.h>

#include "Scene.h"


namespace digi {


// Scene::Node

typedef Scene::NameLess<Scene::Node> NodeLess;

static void swap(Scene::Node& a, Scene::Node& b)
{
	std::swap(a.name, b.name);
	std::swap(a.type, b.type);
}

template <typename Serializer>
Serializer& operator &(Serializer& s, Scene::Node& node)
{
	s & node.name;
	s & node.type;
	return s;
}


// Scene::Attribute

static void swap(Scene::Attribute& a, Scene::Attribute& b)
{
	std::swap(a.name, b.name);
	std::swap(a.type, b.type);
	std::swap(a.path, b.path);
	std::swap(a.semantic, b.semantic);
}

template <typename Serializer>
Serializer& operator &(Serializer& s, Scene::Attribute& attribute)
{
	s & attribute.name;
	s & attribute.type;
	s & attribute.path;
	s & attribute.semantic;	
	return s;
}
		

// Scene::TextureBinding

template <typename Serializer>
Serializer& operator &(Serializer& s, Scene::TextureBinding& textureBinding)
{
	s & textureBinding.textureName;
	s & textureBinding.type;
	s & textureBinding.path;
	return s;
}


// Scene::AttributeSet

typedef Scene::NameLess<Scene::AttributeSet> AttributeSetLess;

static void swap(Scene::AttributeSet& a, Scene::AttributeSet& b)
{
	swap(a.name, b.name);
	swap(a.path, b.path);
	std::swap(a.numTracks, b.numTracks);
	std::swap(a.clipIndex, b.clipIndex);
	std::swap(a.numClips, b.numClips);
}

template <typename Serializer>
Serializer& operator &(Serializer& s, Scene::AttributeSet& attributeSet)
{
	s & attributeSet.name & attributeSet.path & attributeSet.numTracks & attributeSet.clipIndex
		 & attributeSet.numClips;
	return s;
}


// Scene::Clip

typedef Scene::NameLess<Scene::Clip> ClipLess;

static void swap(Scene::Clip& a, Scene::Clip& b)
{
	swap(a.name, b.name);
	std::swap(a.index, b.index);
	std::swap(a.length, b.length);
}

template <typename Serializer>
Serializer& operator &(Serializer& s, Scene::Clip& clip)
{
	s & clip.name & clip.index & clip.length;
	return s;
}


// Scene

Scene::~Scene()
{
}

void Scene::sortAttributes()
{
	// sort nodes
	sort(this->nodes, NodeLess());

	// sort attributes
	sort(this->attributes, AttributeLess());
	
	// sort attribute sets
	sort(this->attributeSets, AttributeSetLess());
	
	// sort clips
	foreach (const AttributeSet& attributeSet, this->attributeSets)
	{
		std::vector<Clip>::iterator begin = this->clips.begin() + attributeSet.clipIndex;
		std::sort(begin, begin + attributeSet.numClips, ClipLess());
	}
}

// serialize

template <typename Serializer>
void serialize(Serializer& s, Scene& scene)
{
	s & scene.name;
	
	s & scene.layers;
	
	s & scene.stateType;
	s & scene.transformCount;
	s & scene.boundingBoxCount;
	
	s & scene.nodes;
	s & scene.attributes;
	s & scene.textureBindings;
	s & scene.attributeSets;
	s & scene.clips;
		
	s & scene.buffers;
	
	s & scene.initStateCode;
	s & scene.updateCode;
	s & scene.addClipCode;
		
	s & scene.instances;
	//s & scene.renderInstances;
	//s & scene.collisionInstances;
}


Pointer<Scene> Scene::load(ObjectReader& r)
{
	Pointer<Scene> scene = new Scene();
	VersionReader v(r);
	serialize(r, *scene);
	return scene;	
}

void Scene::save(ObjectWriter& w)
{
	VersionWriter v(w, 1);
	serialize(w, *this);
}


} // namespace digi
