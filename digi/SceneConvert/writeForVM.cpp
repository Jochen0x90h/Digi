#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/Find.h>
#include <digi/Utility/SetUtility.h>
#include <digi/System/MemoryDevices.h>
#include <digi/Engine/ParameterType.h>

#include "generateSceneOpenGL.h"
#include "generateTextureOpenGL.h"
#include "writeForVM.h"


namespace digi {

void writeForVM(Pointer<SceneFile> sceneFile, ObjectWriter& ow, const SceneOptions& options)
{
	// create converters
	Pointer<ConverterContext> converterContext = new ConverterContext();
	Pointer<ImageConverter> imageConverter = new ImageConverter(converterContext);
	Pointer<BufferConverter> bufferConverter = new BufferConverter(converterContext);
	
	// write textures
	uint numTextures = uint(sceneFile->textures.size());
	ow & numTextures;
	std::set<std::string> textureNames;
	foreach (Pointer<Texture> texture, sceneFile->textures)
	{
		const std::string& name = texture->name;
		textureNames.insert(name);
		
		// write name
		ow & name;
		
		// generate texture
		uint type = P_TEXTURE_2D;
		std::vector<uint8_t> data;
		std::string code;
		{
			CodeWriter w(new StringRefDevice(code));
			DataWriter d(new ContainerDevice<std::vector<uint8_t>&>(data), false); //! little endian

			if (Pointer<TextureImage> textureImage = dynamicCast<TextureImage>(texture))
			{
				generateTextureOpenGL(imageConverter, textureImage->image, w, d, texture->type, options);
			}
			else if (Pointer<TextureSequence> textureSequence = dynamicCast<TextureSequence>(texture))
			{
				uint length = textureSequence->imageSequence.rbegin()->first + 1;
				type |= length << 8;
				
				generateTextureSequenceOpenGL(imageConverter, textureSequence->imageSequence, w, d, texture->type,
					options);
			}			

			w.close();
			d.close();
		}

		// write type
		ow & type;

		// write data
		ow & data;
		
		// write code
		ow & code;
	}
	
	// write scenes
	uint numScenes = uint(sceneFile->scenes.size());
	ow & numScenes;
	foreach (Pointer<Scene> scene, sceneFile->scenes)
	{
		const std::string& name = scene->name;
		
		// write name
		ow & name;

		// generate scene
		std::vector<uint8_t> data;
		std::string code;
		{
			CodeWriter w(new StringRefDevice(code));
			DataWriter d(new ContainerDevice<std::vector<uint8_t>&>(data), false ); //! little endian

			SceneStatistics stats;
			generateSceneOpenGL(bufferConverter, scene, w, d, options, stats);

			w.close();
			d.close();
		}

		// write data
		ow & data;

		// write code
		ow & code;


		// sort attributes and attribute sets because render interface on target uses binary search
		scene->sortAttributes();

		// node infos
		{
			size_t numNodes = scene->nodes.size();
			ow & numNodes;
			foreach (const Scene::Node& node, scene->nodes)
			{
				// name
				ow & node.name;
			
				// type
				ow & node.type;
			}			
		}		

		// attribute infos
		{
			size_t numAttributes = scene->attributes.size();
			ow & numAttributes;
			foreach (const Scene::Attribute& attribute, scene->attributes)
			{
				// name
				ow & attribute.name;
				
				// type
				ow & attribute.type;

				// path to attribute in State struct
				ow & attribute.path;
				
				// semantic
				ow & attribute.semantic;
			}
		}		

		// texture bindings
		{
			// count number of valid texture bindings
			size_t numTextureBindings = 0;
			foreach (const Scene::TextureBinding& textureBinding, scene->textureBindings)
			{
				// find texture by name
				if (contains(textureNames, textureBinding.textureName))
					++numTextureBindings;
			}

			ow & numTextureBindings;
			foreach (const Scene::TextureBinding& textureBinding, scene->textureBindings)
			{
				// find texture by name
				if (contains(textureNames, textureBinding.textureName))
				{			
					// texture index
					//ow & textureIndices[texture];
					
					// texture name
				ow & textureBinding.textureName;

					// type
					ow & textureBinding.type;
				
					// path
					ow & textureBinding.path;
				}
			}				
		}
		
		// attribute sets
		{
			size_t numParameterSets = scene->attributeSets.size();
			ow & numParameterSets;
			foreach (const Scene::AttributeSet& attributeSet, scene->attributeSets)
			{
				ow & attributeSet.name;
				ow & attributeSet.path;
				ow & attributeSet.numTracks;
				ow & attributeSet.clipIndex;
				ow & attributeSet.numClips;
			}	
		}

		// clips
		{
			size_t numClips = scene->clips.size();
			ow & numClips;
			foreach (const Scene::Clip& clip, scene->clips)
			{
				ow & clip.name;
				ow & clip.index;
				ow & clip.length;
			}
		}
				
		// collision objects
		{
			// get and sort pick objects
			size_t numPickObjects = scene->instances.size();
			typedef std::pair<std::string, uint> PickInfo;
			std::vector<PickInfo> pickInfos;
			foreach (const NamedInstance& instance, scene->instances)
			{
				uint index = int(pickInfos.size());
				pickInfos += PickInfo(instance.name, index);
			}
			sort(pickInfos);
			
			// write pick objects
			ow & numPickObjects;
			foreach (const PickInfo& pickInfo, pickInfos)
			{
				// name
				ow & pickInfo.first;
				
				// index of object
				ow & pickInfo.second;
			}
		}		
	}
}	

} // namespace digi
