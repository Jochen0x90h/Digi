#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/Find.h>
#include <digi/Utility/MapUtility.h>
#include <digi/Utility/foreach.h>
#include <digi/System/File.h>

#include "Engine.h"
#include "InlineFile.h"


namespace digi {

namespace
{
	template <typename Value>
	int getFreeHandle(std::vector<Value>& vector)
	{
		typename std::vector<Value>::const_iterator end = vector.end();
		for (typename std::vector<Value>::const_iterator it = vector.begin(); it != end; ++it)
		{
			// check for null pointer
			if (*it == Value())
				return int(it - vector.begin());
		}

		// add new entry and return its index
		int result = int(vector.size());
		add(vector);
		return result;
	}


	struct StateSetter
	{
		StateSetter()
		{
			// reset pixel store to alignment of 1
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		}
		
		~StateSetter()
		{		
			// reset pixel store
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

/*
			// reset textures
			glBindTexture(GL_TEXTURE_2D, 0);
		#if !defined(GL_ES)
			glBindTexture(GL_TEXTURE_3D, 0);
		#endif
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
*/			
			// reset vertex arrays
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		#if GL_MAJOR >= 3
			glBindVertexArray(0);
		#endif
		#if GL_MAJOR <= 2 || (!defined(GL_CORE) && !defined(GL_ES))
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		#endif

			// reset program
			glUseProgram(0);
		}
	};

	// less functors

	template <typename Type>
	struct NameLess
	{
		bool operator ()(const Type& info, StringRef name)
		{
			return info.name < name;
		}

		bool operator ()(StringRef name, const Type& info)
		{
			return name < info.name;
		}
	};

	typedef NameLess<TextureInfo> TextureLess;
	typedef NameLess<SceneInfo> SceneLess;
	typedef NameLess<AttributeInfo> AttributeLess;
	typedef NameLess<AttributeSetInfo> AttributeSetLess;
	typedef NameLess<ClipInfo> ClipLess;
	typedef NameLess<ObjectInfo> ObjectLess;

} // anonymous namespace


// Engine

Engine::Engine()
	: renderJobs(20000 * sizeof(RenderJob)), pickFBO(0), numAttributes(0), nextObjectId(1)
{
	this->dummyAttributeInfo.name = StringRef::emptyString;
	this->dummyAttributeInfo.type = P_INVALID;
	this->dummyAttributeInfo.semantic = StringRef::emptyString;
}

Engine::~Engine()
{
	this->clear();
}

int Engine::addFile(Pointer<EngineFile> file)
{
	if (file == null)
		return -1;

	// get a free file handle
	int handle = getFreeHandle(this->files);
	
	// set file
	this->files[handle] = file;
	
	return handle;
}

int Engine::loadFile(const fs::path& path, const EngineInfo& engineInfo)
{
	// set opengl state and reset on scope exit
	StateSetter setter;
	return this->addFile(new InlineFile(path, engineInfo));
}

int Engine::loadFile(const fs::path& path)
{
	Pointer<EngineLoader> loader = getValue(this->loaders, path.extension().string());
	if (loader != null)
	{
		// set opengl state and reset on scope exit
		StateSetter setter;
		return this->addFile(loader->load(path));
	}
	return -1;
}

void Engine::unloadFile(int fileHandle)
{
	// check file handle
	if (uint(fileHandle) < uint(this->files.size()))
	{
		if (Pointer<EngineFile> file = this->files[fileHandle])
		{
			this->files[fileHandle] = null;

			// delete scenes that are instances of this file
			foreach (Scene* scene, file->scenes)
			{
				// remove scene from containing group
				Group* group = this->groups[scene->groupIndex];
				group->erase(find(*group, scene));

				// delete scene
				this->deleteScene(scene);
			}
			file->scenes.clear();

			// this frees the OpenGL resources
			file->done();
		}
	}
}

void Engine::clear()
{
	foreach (Group* group, this->groups)
	{
		// delete NULL is ok
		delete group;
	}
	foreach (Scene* scene, this->scenes)
	{
		// delete NULL is ok
		delete scene;
	}
	this->groups.clear();
	this->scenes.clear();
	this->attributes.clear();
	this->numAttributes = 0;
	this->attributeSets.clear();
	//this->clips.clear();
	this->nextObjectId = 1;

	foreach (Pointer<EngineFile> file, this->files)
	{
		if (file != null)
		{
			file->scenes.clear();

			// this frees the OpenGL resources
			file->done();
		}
	}
	this->files.clear();
	
	glDeleteFramebuffers(1, &this->pickFBO);
	this->pickFBO = 0;
}

int Engine::createGroup()
{
	// get a free group handle
	int groupHandle = getFreeHandle(this->groups);
	
	// create new group
	this->groups[groupHandle] = new Group();

	// return handle (array index)
	return groupHandle;
}

void Engine::deleteGroup(int groupHandle)
{
	// check group handle
	if (uint(groupHandle) < uint(this->groups.size()))
	{
		// get group
		Group* group = this->groups[groupHandle];
		if (group != NULL)
		{
			// delete all scenes in group
			foreach (Scene* scene, *group)
			{
				// remove scene from file
				std::vector<Engine::Scene*>& scenes = this->files[scene->fileIndex]->scenes;
				scenes.erase(find(scenes, scene));

				this->deleteScene(scene);
			}

			// delete group
			delete group;

			// remove group from groups array
			this->groups[groupHandle] = NULL;
		}
	}
}

void Engine::updateGroup(int groupHandle)
{
	// check group handle
	if (uint(groupHandle) < uint(this->groups.size()))
	{
		Group* group = this->groups[groupHandle];
		if (group != NULL)
		{
			// update all scenes in group
			foreach (Scene* scene, *group)
			{
				this->updateScene(scene);
			}
		}
	}	
}

void Engine::renderGroup(int groupHandle, const float4x4& viewMatrix, const float4x4& projectionMatrix, int layerIndex)
{
	// check group handle
	if (uint(groupHandle) < uint(this->groups.size()))
	{
		Group* group = this->groups[groupHandle];
		if (group != NULL)
		{
			// align 16 since llvm uses vector types
			ALIGN(16) float4x4 vm = viewMatrix;
			ALIGN(16) float4x4 pm = projectionMatrix;

			// reset render queues
			RenderQueues renderQueues;
			renderQueues.begin = (RenderJob*)this->renderJobs.begin();
			renderQueues.end = (RenderJob*)this->renderJobs.end();
			renderQueues.alphaSort = NULL;
			
			// set opengl state and reset on scope exit
			StateSetter setter;

			// two vertex attributes are enabled by default
		#if GL_MAJOR <= 2 || (!defined(GL_CORE) && !defined(GL_ES))
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
		#endif

			// set point sprite settings
		#if !defined(GL_CORE) && !defined(GL_ES)
			glEnable(GL_POINT_SPRITE);
			glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
			glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_UPPER_LEFT);
		#endif

			// render solid
			foreach (Scene* scene, *group)
			{
				scene->sceneInfo.render(scene->instance, vm, pm, layerIndex, renderQueues);
			}

			// render transparent
			if (renderQueues.alphaSort != NULL)
			{
				// disable depth write
				glDepthMask(GL_FALSE);
				
				// set blend mode (pixel = pixel * (1 - alpha) + fragment)
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			
				// render sorted and reset last shader
				renderSorted(renderQueues.alphaSort);
			
				// reset blend mode
				glBlendFunc(GL_ONE, GL_ZERO);
				glDisable(GL_BLEND);

				// enable depth write
				glDepthMask(GL_TRUE);
			}

			// reset cull mode
			glDisable(GL_CULL_FACE);
			glCullFace(GL_BACK);

			// reset point sprite settings
		#if !defined(GL_CORE) && !defined(GL_ES)
			glDisable(GL_POINT_SPRITE);
			glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
		#endif

			// reset vertex attributes
		#if GL_MAJOR <= 2 || (!defined(GL_CORE) && !defined(GL_ES))
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(0);
		#endif
		}
	}
}

int Engine::pickGroup(int groupHandle, const float4x4& viewMatrix, const float4x4& projectionMatrix, float x, float y)
{
	// lazily create 1x1 frame buffer for picking
	if (this->pickFBO == 0)
	{	
		// build frame buffer object for picking
		GLuint buffers[2];
		glGenRenderbuffers(2, buffers);
		glBindRenderbuffer(GL_RENDERBUFFER, buffers[0]);
		glRenderbufferStorage(GL_RENDERBUFFER,
		#if defined(GL_ES)
			GL_RGB565,
		#else
			GL_RGB8,
		#endif
			1, 1);
		glBindRenderbuffer(GL_RENDERBUFFER, buffers[1]);
		glRenderbufferStorage(GL_RENDERBUFFER,
		#if defined(GL_ES)
			GL_DEPTH_COMPONENT16,
		#else
			GL_DEPTH_COMPONENT,
		#endif
			1, 1);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		GLuint fbo;
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, buffers[0]);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffers[1]);
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
		// release render buffers, the frame buffer still holds a reference
		glDeleteRenderbuffers(2, buffers);
	
		// check if frame buffer ok
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			glDeleteFramebuffers(1, &fbo);
			return -1;
			//dError("could not create frame buffer for picking");
		}

		this->pickFBO = fbo;
	}

	// save viewport
	//GLint viewport[4];
	//glGetIntegerv(GL_VIEWPORT, viewport);

	// save clear color
	//float clearColor[4];
	//glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);

	// pick area
	float sx = 10000.0f;
	float sy = 10000.0f;
	float4x4 area = matrix4x4(
		sx, 0.0f, 0.0f, 0.0f,
		0.0f, sy, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-x * sx, -y * sy, 0.0f, 1.0f);

	// set fbo (1x1 render target)
	glBindFramebuffer(GL_FRAMEBUFFER, this->pickFBO);

	// render
	glViewport(0, 0, 1, 1);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	this->renderGroup(groupHandle, viewMatrix, area * projectionMatrix, -1);

	// get pixel
	uint8_t pixel[4];
	glReadPixels(0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

	// unset fbo
	glBindFramebuffer(GL_FRAMEBUFFER, 0);	

	// restore viewport
	//glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

	// restore clear color
	//glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

#if defined(GL_ES)
	return (pixel[0] >> 3) + (pixel[1] >> 2) * 32 + (pixel[2] >> 3) * 2048;
#else
	return pixel[0] + pixel[1] * 256 + pixel[2] * 65536;
#endif
}

int Engine::getNumScenes(int fileHandle)
{
	if (uint(fileHandle) < uint(this->files.size()))
	{
		if (Pointer<EngineFile> file = this->files[fileHandle])
			return int(file->getSceneInfos().size());
	}
	return -1;
}

StringRef Engine::getSceneName(int fileHandle, int sceneIndex)
{
	// check file handle
	if (uint(fileHandle) < uint(this->files.size()))
	{
		if (Pointer<EngineFile> file = this->files[fileHandle])
		{
			ArrayRef<const SceneInfo> sceneInfos = file->getSceneInfos();
			
			if (uint(sceneIndex) < uint(sceneInfos.size()))
				return sceneInfos[sceneIndex].name;
		}
	}
	return StringRef();
}

int Engine::createScene(int fileHandle, int sceneIndex, int groupHandle)
{	
	// check file handle
	if (uint(fileHandle) < uint(this->files.size()))
	{
		if (Pointer<EngineFile> file = this->files[fileHandle])
		{
			ArrayRef<const SceneInfo> sceneInfos = file->getSceneInfos();

			// find scene by index
			if (uint(sceneIndex) < uint(sceneInfos.size()))
				return this->createScene(file, sceneIndex, groupHandle);
		}
	}
	return -1;
}

int Engine::createScene(int fileHandle, StringRef sceneName, int groupHandle)
{	
	// check file handle
	if (uint(fileHandle) < uint(this->files.size()))
	{
		if (Pointer<EngineFile> file = this->files[fileHandle])
		{
			ArrayRef<const SceneInfo> sceneInfos = file->getSceneInfos();

			// find scene by name
			ArrayRef<const SceneInfo>::iterator begin = sceneInfos.begin();
			ArrayRef<const SceneInfo>::iterator end = sceneInfos.end();
			ArrayRef<const SceneInfo>::iterator it = binaryFind(begin, end,
				sceneName, SceneLess());

			// check if scene was found
			if (it != end)
				return this->createScene(file, int(it - begin), groupHandle);
		}
	}
	return -1;
}

void Engine::deleteScene(int sceneHandle)
{
	// check scene handle
	if (uint(sceneHandle) < uint(this->scenes.size()))
	{
		// get scene instance
		Scene* scene = this->scenes[sceneHandle];
		if (scene != NULL)
		{
			// remove scene from containing group
			Group* group = this->groups[scene->groupIndex];
			group->erase(find(*group, scene));

			// remove scene from file
			std::vector<Engine::Scene*>& scenes = this->files[scene->fileIndex]->scenes;
			scenes.erase(find(scenes, scene));

			// delete scene
			this->deleteScene(scene);
		}
	}
}

StringRef Engine::getSceneName(int sceneHandle)
{
	// check scene handle
	if (uint(sceneHandle) < uint(this->scenes.size()))
	{		
		// get scene instance
		Scene* scene = this->scenes[sceneHandle];
		if (scene != NULL)
			return scene->sceneInfo.name;
	}
	return StringRef();
}

void Engine::updateScene(int sceneHandle)
{
	// check scene handle
	if (uint(sceneHandle) < uint(this->scenes.size()))
	{
		// get scene instance
		Scene* scene = this->scenes[sceneHandle];
		if (scene != NULL)
			this->updateScene(scene);
	}
}

BoundingBox Engine::getBoundingBox(int sceneHandle)
{
	ALIGN(16) float4x2 bb = {};

	// check scene handle
	if (uint(sceneHandle) < uint(this->scenes.size()))
	{	
		// get scene instance
		Scene* scene = this->scenes[sceneHandle];
		if (scene != NULL)
		{
			// get bounding box
			scene->sceneInfo.getBoundingBox(scene->instance, bb);
		}
	}
			
	BoundingBox boundingBox;
	boundingBox.center = getXYZ(bb.x);
	boundingBox.size = getXYZ(bb.y);
	return boundingBox;
}

int Engine::getNumNodes(int sceneHandle)
{
	// check scene handle
	if (uint(sceneHandle) < uint(this->scenes.size()))
	{		
		// get scene instance
		Scene* scene = this->scenes[sceneHandle];
		if (scene != NULL)
			return int(scene->sceneInfo.nodeInfos.size());
	}
	return -1;
}

StringRef Engine::getNodeName(int sceneHandle, int nodeIndex)
{
	// check scene handle
	if (uint(sceneHandle) < uint(this->scenes.size()))
	{	
		// get scene instance
		Scene* scene = this->scenes[sceneHandle];
		if (scene != NULL)
		{
			ArrayData<NodeInfo> nodeInfos = scene->sceneInfo.nodeInfos;

			// check index
			if (uint(nodeIndex) < uint(nodeInfos.size()))
				return nodeInfos[nodeIndex].name;
		}
	}
	return StringRef();
}

NodeType Engine::getNodeType(int sceneHandle, int nodeIndex)
{
	// check scene handle
	if (uint(sceneHandle) < uint(this->scenes.size()))
	{	
		// get scene instance
		Scene* scene = this->scenes[sceneHandle];
		if (scene != NULL)
		{
			ArrayData<NodeInfo> nodeInfos = scene->sceneInfo.nodeInfos;

			// check index
			if (uint(nodeIndex) < uint(nodeInfos.size()))
				return NodeType(nodeInfos[nodeIndex].type);
		}
	}
	return N_INVALID;
}

int Engine::getNumAttributes(int sceneHandle)
{
	// check scene handle
	if (uint(sceneHandle) < uint(this->scenes.size()))
	{		
		// get scene instance
		Scene* scene = this->scenes[sceneHandle];
		if (scene != NULL)
			return int(scene->sceneInfo.attributeInfos.size());
	}
	return -1;
}

StringRef Engine::getAttributeName(int sceneHandle, int attributeIndex)
{
	// check scene handle
	if (uint(sceneHandle) < uint(this->scenes.size()))
	{	
		// get scene instance
		Scene* scene = this->scenes[sceneHandle];
		if (scene != NULL)
		{
			ArrayData<AttributeInfo> attributeInfos = scene->sceneInfo.attributeInfos;

			// check index
			if (uint(attributeIndex) < uint(attributeInfos.size()))
				return attributeInfos[attributeIndex].name;
		}
	}
	return StringRef();
}

int Engine::getAttributeHandle(int sceneHandle, int attributeIndex)
{
	// check scene handle
	if (uint(sceneHandle) < uint(this->scenes.size()))
	{	
		// get scene instance
		Scene* scene = this->scenes[sceneHandle];
		if (scene != NULL)
		{
			ArrayData<AttributeInfo> attributeInfos = scene->sceneInfo.attributeInfos;

			// check index
			if (uint(attributeIndex) < uint(attributeInfos.size()))
			{
				// check if attribute already has a handle
				int attributeHandle = scene->attributeHandles[attributeIndex];
				if (attributeHandle != -1)
					return attributeHandle;
				
				return this->getAttributeHandleInternal(sceneHandle, attributeIndex);
			}
		}
	}
	return -1;
}

int Engine::getAttributeHandle(int sceneHandle, StringRef attributeName)
{
	// check scene handle
	if (uint(sceneHandle) < uint(this->scenes.size()))
	{	
		// get scene instance
		Scene* scene = this->scenes[sceneHandle];
		if (scene != NULL)
		{
			ArrayData<AttributeInfo> attributeInfos = scene->sceneInfo.attributeInfos;

			// find attribute by name
			ArrayData<AttributeInfo>::const_iterator begin = attributeInfos.begin();
			ArrayData<AttributeInfo>::const_iterator end = attributeInfos.end();
			ArrayData<AttributeInfo>::const_iterator attributeInfo = binaryFind(
				begin,
				end,
				attributeName, AttributeLess());

			// check if attribute was found
			if (attributeInfo != end)
			{
				int attributeIndex = int(attributeInfo - begin);

				// check if attribute already has a handle
				int attributeHandle = scene->attributeHandles[attributeIndex];
				if (attributeHandle != -1)
					return attributeHandle;
				
				return this->getAttributeHandleInternal(sceneHandle, attributeIndex);
			}
		}
	}
	return -1;
}


int Engine::getNumAttributeSets(int sceneHandle)
{
	// check scene handle
	if (uint(sceneHandle) < uint(this->scenes.size()))
	{
		// get scene instance
		Scene* scene = this->scenes[sceneHandle];
		if (scene != NULL)
			return int(scene->sceneInfo.attributeSetInfos.size());
	}
	return -1;
}

StringRef Engine::getAttributeSetName(int sceneHandle, int attributeSetIndex)
{
	// check scene handle
	if (uint(sceneHandle) < uint(this->scenes.size()))
	{
		// get scene instance
		Scene* scene = this->scenes[sceneHandle];
		if (scene != NULL)
		{
			ArrayData<AttributeSetInfo> attributeSetInfos = scene->sceneInfo.attributeSetInfos;

			// check index
			if (uint(attributeSetIndex) < uint(attributeSetInfos.size()))
				return attributeSetInfos[attributeSetIndex].name;
		}
	}
	return StringRef();
}

int Engine::getAttributeSetHandle(int sceneHandle, int attributeSetIndex)
{
	// check scene handle
	if (uint(sceneHandle) < uint(this->scenes.size()))
	{
		// get scene instance
		Scene* scene = this->scenes[sceneHandle];
		if (scene != NULL)
		{
			ArrayData<AttributeSetInfo> attributeSetInfos = scene->sceneInfo.attributeSetInfos;

			// check index
			if (uint(attributeSetIndex) < uint(attributeSetInfos.size()))
			{
				// check if attribute set already has a handle
				int attributeSetHandle = scene->attributeSetHandles[attributeSetIndex];
				if (attributeSetHandle != -1)
					return attributeSetHandle;
			
				return this->getAttributeSetHandleInternal(sceneHandle, attributeSetIndex);
			}
		}
	}
	return -1;
}

int Engine::getAttributeSetHandle(int sceneHandle, StringRef attributeSetName)
{
	// check scene handle
	if (uint(sceneHandle) < uint(this->scenes.size()))
	{
		// get scene instance
		Scene* scene = this->scenes[sceneHandle];
		if (scene != NULL)
		{
			ArrayData<AttributeSetInfo> attributeSetInfos = scene->sceneInfo.attributeSetInfos;

			// find attribute by name
			ArrayData<AttributeSetInfo>::const_iterator begin = attributeSetInfos.begin();
			ArrayData<AttributeSetInfo>::const_iterator end = attributeSetInfos.end();
			ArrayData<AttributeSetInfo>::const_iterator attributeSetInfo = binaryFind(
				begin,
				end,
				attributeSetName, AttributeSetLess());

			// check if attribute was found
			if (attributeSetInfo != end)
			{
				int attributeSetIndex = int(attributeSetInfo - begin);

				// check if attribute set already has a handle
				int attributeSetHandle = scene->attributeSetHandles[attributeSetIndex];
				if (attributeSetHandle != -1)
					return attributeSetHandle;

				return this->getAttributeSetHandleInternal(sceneHandle, attributeSetIndex);
			}
		}
	}
	return -1;
}

int Engine::getNumClips(int attributeSetHandle)
{
	// check attribute set handle
	if (uint(attributeSetHandle) < uint(this->attributeSets.size()))
	{
		AttributeSet& attributeSet = this->attributeSets[attributeSetHandle];
		const AttributeSetInfo* attributeSetInfo = attributeSet.attributeSetInfo;
		if (attributeSetInfo != NULL)
		{
			return attributeSetInfo->numClips;
		}
	}
	return -1;
}

StringRef Engine::getClipName(int attributeSetHandle, int clipIndex)
{
	// check attribute set handle
	if (uint(attributeSetHandle) < uint(this->attributeSets.size()))
	{
		AttributeSet& attributeSet = this->attributeSets[attributeSetHandle];
		const AttributeSetInfo* attributeSetInfo = attributeSet.attributeSetInfo;
		if (attributeSetInfo != NULL)
		{				
			// check clip index
			if (uint(clipIndex) < attributeSetInfo->numClips)
				return attributeSet.scene->sceneInfo.clipInfos[attributeSetInfo->clipIndex + clipIndex].name;
		}
	}
	return StringRef();
}

float Engine::getClipLength(int attributeSetHandle, int clipIndex)
{
	// check attribute set handle
	if (uint(attributeSetHandle) < uint(this->attributeSets.size()))
	{
		AttributeSet& attributeSet = this->attributeSets[attributeSetHandle];
		const AttributeSetInfo* attributeSetInfo = attributeSet.attributeSetInfo;
		if (attributeSetInfo != NULL)
		{				
			// check clip index
			if (uint(clipIndex) < attributeSetInfo->numClips)
				return attributeSet.scene->sceneInfo.clipInfos[attributeSetInfo->clipIndex + clipIndex].length;
		}
	}
	return 0.0f;
}

int Engine::getClipIndex(int attributeSetHandle, StringRef clipName)
{
	// check attribute set handle
	if (uint(attributeSetHandle) < uint(this->attributeSets.size()))
	{
		AttributeSet& attributeSet = this->attributeSets[attributeSetHandle];
		const AttributeSetInfo* attributeSetInfo = attributeSet.attributeSetInfo;
		if (attributeSetInfo != NULL)
		{		
			// find clip by name
			ArrayData<ClipInfo>::const_iterator begin = attributeSet.scene->sceneInfo.clipInfos.begin() + attributeSetInfo->clipIndex;
			ArrayData<ClipInfo>::const_iterator end = begin + attributeSetInfo->numClips;
			ArrayData<ClipInfo>::const_iterator clipInfo = binaryFind(
				begin,
				end,
				clipName, ClipLess());
				
			// check if clip was found
			if (clipInfo != end)
			{
				// check if clip already has a handle
				int clipIndex = int(clipInfo - begin);
				return clipIndex;
			}
		}
	}
	return -1;
}

void Engine::setNumTracks(int attributeSetHandle, int numTracks)
{
	// check attribute set handle
	if (uint(attributeSetHandle) < uint(this->attributeSets.size()))
	{
		AttributeSet& attributeSet = this->attributeSets[attributeSetHandle];
	
		attributeSet.tracks.resize(numTracks);
	}
}

void Engine::setClip(int attributeSetHandle, int trackIndex, int clipIndex)
{
	// check attribute set handle
	if (uint(attributeSetHandle) < uint(this->attributeSets.size()))
	{
		AttributeSet& attributeSet = this->attributeSets[attributeSetHandle];
		const AttributeSetInfo* attributeSetInfo = attributeSet.attributeSetInfo;

		// check track index
		if (uint(trackIndex) < uint(attributeSet.tracks.size()))
		{
			Track& track = attributeSet.tracks[trackIndex];

			// check clip index
			if (uint(clipIndex) < attributeSetInfo->numClips)
				track.clipInfo = &attributeSet.scene->sceneInfo.clipInfos[attributeSetInfo->clipIndex + clipIndex];
			else
				track.clipInfo = NULL;
			track.clipTime = 0.0f;
			track.weight = 0.0f;
		}
	}	
}

void Engine::setTrackParameters(int attributeSetHandle, int trackIndex, float clipTime, float weight)
{
	// check attribute set handle
	if (uint(attributeSetHandle) < uint(this->attributeSets.size()))
	{
		AttributeSet& attributeSet = this->attributeSets[attributeSetHandle];
		const AttributeSetInfo* attributeSetInfo = attributeSet.attributeSetInfo;

		// check track index
		if (uint(trackIndex) < uint(attributeSet.tracks.size()))
		{
			Track& track = attributeSet.tracks[trackIndex];

			track.clipTime = clipTime;
			track.weight = weight;
		}
	}
}


/*
int Engine::getClipHandle(int attributeSetHandle, int clipIndex)
{
	// check attribute set handle
	if (uint(attributeSetHandle) < uint(this->attributeSets.size()))
	{
		AttributeSet& attributeSet = this->attributeSets[attributeSetHandle];
		const AttributeSetInfo* attributeSetInfo = attributeSet.attributeSetInfo;
		if (attributeSetInfo != NULL)
		{		
			// check clip index
			if (uint(clipIndex) < attributeSetInfo->numClips)
			{
				// check if clip already has a handle
				int globalClipIndex = attributeSetInfo->clipIndex + clipIndex;
				int clipHandle = attributeSet.scene->clipHandles[globalClipIndex];
				if (clipHandle != -1)
					return clipHandle;
			
				return this->getClipHandleInternal(attributeSetHandle, clipIndex);
			}
		}
	}
	return -1;
}

int Engine::getClipHandle(int attributeSetHandle, StringRef clipName)
{
	// check attribute set handle
	if (uint(attributeSetHandle) < uint(this->attributeSets.size()))
	{
		AttributeSet& attributeSet = this->attributeSets[attributeSetHandle];
		const AttributeSetInfo* attributeSetInfo = attributeSet.attributeSetInfo;
		if (attributeSetInfo != NULL)
		{		
			// find clip by name
			ArrayData<ClipInfo>::const_iterator begin = attributeSet.scene->sceneInfo.clipInfos.begin() + attributeSetInfo->clipIndex;
			ArrayData<ClipInfo>::const_iterator end = begin + attributeSetInfo->numClips;
			ArrayData<ClipInfo>::const_iterator clipInfo = binaryFind(
				begin,
				end,
				clipName, ClipLess());
				
			// check if clip was found
			if (clipInfo != end)
			{
				// check if clip already has a handle
				int clipIndex = int(clipInfo - begin);
				int globalClipIndex = attributeSetInfo->clipIndex + clipIndex;
				int clipHandle = attributeSet.scene->clipHandles[globalClipIndex];
				if (clipHandle != -1)
					return clipHandle;
				
				return this->getClipHandleInternal(attributeSetHandle, clipIndex);
			}
		}
	}
	return -1;
}

StringRef Engine::getClipName(int clipHandle)
{
	// check clip handle
	if (uint(clipHandle) < uint(this->clips.size()))
	{
		Clip& clip = this->clips[clipHandle];
		const ClipInfo* clipInfo = clip.clipInfo;
		if (clipInfo != NULL)
		{
			return clipInfo->name;	
		}
	}
	return StringRef();
}

float Engine::getClipLength(int clipHandle)
{
	// check clip handle
	if (uint(clipHandle) < uint(this->clips.size()))
	{
		Clip& clip = this->clips[clipHandle];
		const ClipInfo* clipInfo = clip.clipInfo;
		if (clipInfo != NULL)
		{
			return clipInfo->length;
		}
	}
	return 0.0f;
}

void Engine::setClipParameters(int clipHandle, float time, float weight)
{
	// check clip handle
	if (uint(clipHandle) < uint(this->clips.size()))
	{
		Clip& clip = this->clips[clipHandle];
		clip.time = time;
		clip.weight = weight;
	}
}
*/

int Engine::getNumObjects(int sceneHandle)
{
	// check scene handle
	if (uint(sceneHandle) < uint(this->scenes.size()))
	{
		// get scene instance
		Scene* scene = this->scenes[sceneHandle];
		if (scene != NULL)
		{
			ArrayData<ObjectInfo> objectInfos = scene->sceneInfo.objectInfos;
			return objectInfos.length;
		}
	}
	return -1;	
}

int Engine::getObjectId(int sceneHandle, int objectIndex)
{
	// check scene handle
	if (uint(sceneHandle) < uint(this->scenes.size()))
	{
		// get scene instance
		Scene* scene = this->scenes[sceneHandle];
		if (scene != NULL)
		{
			ArrayData<ObjectInfo> objectInfos = scene->sceneInfo.objectInfos;

			if (uint(objectIndex) < uint(objectInfos.length))
			{ 
				// get offset of object id in scene instance
				size_t offset = objectInfos[objectIndex].offset;
			
				int* idPointer = (int*)(scene->instance + offset);
				if (*idPointer == 0)
				{		
					// get object id
					//! recycle id's when a scene instance is deleted
					int id = this->nextObjectId++;
					
					// set object id 
					*idPointer = id;
				}
				
				// return id as handle
				return *idPointer;
			}
		}
	}
	return -1;
}

int Engine::getObjectId(int sceneHandle, StringRef objectName)
{
	// check scene handle
	if (uint(sceneHandle) < uint(this->scenes.size()))
	{
		// get scene instance
		Scene* scene = this->scenes[sceneHandle];
		if (scene != NULL)
		{
			ArrayData<ObjectInfo> objectInfos = scene->sceneInfo.objectInfos;

			// find object info by name
			ArrayData<ObjectInfo>::const_iterator objectInfosEnd = objectInfos.end();
			ArrayData<ObjectInfo>::const_iterator objectInfo = binaryFind(
				objectInfos.begin(),
				objectInfosEnd,
				objectName, ObjectLess());

			// check if object info was found
			if (objectInfo != objectInfosEnd)
			{
				// get offset of object id in scene instance
				size_t offset = objectInfo->offset;
			
				int* idPointer = (int*)(scene->instance + offset);
				if (*idPointer == 0)
				{		
					// get object id
					//! recycle id's when a scene instance is deleted
					int id = this->nextObjectId++;
					
					// set object id 
					*idPointer = id;
				}
				
				// return id as handle
				return *idPointer;
			}
		}
	}
	return -1;
}

/*
StringRef Engine::getSceneNameByIndex(int fileHandle, int sceneIndex)
{
	if (uint(fileHandle) >= uint(this->files.size()))
		return StringRef();
	Pointer<EngineFile> file = this->files[fileHandle];
	if (file == null)
		return StringRef(); 

	if (uint(sceneIndex) >= uint(file->getNumScenes()))
		return StringRef();
	return file->getSceneName();
}

int Engine::getSceneNumParametersByIndex(int fileHandle, int sceneIndex)
{
	if (uint(fileHandle) >= uint(this->files.size()))
		return 0;
	Pointer<EngineFile> file = this->files[fileHandle];
	if (file == null)
		return 0; 

	if (uint(sceneIndex) >= uint(file->sceneInfos.size()))
		return 0;
	SceneInfo* sceneInfo = file->sceneInfos[sceneIndex];
	return int(sceneInfo.attributeInfos.size());
}

StringRef Engine::getSceneParameterNameByIndex(int fileHandle, int sceneIndex, int attributeIndex)
{
	if (uint(fileHandle) >= uint(this->files.size()))
		return StringRef();
	Pointer<EngineFile> file = this->files[fileHandle];
	if (file == null)
		return StringRef(); 

	if (uint(sceneIndex) >= uint(file->sceneInfos.size()))
		return StringRef();
	SceneInfo* sceneInfo = file->sceneInfos[sceneIndex];
	if (uint(attributeIndex) >= uint(sceneInfo.attributeInfos.size()))
		return StringRef();
	return sceneInfo.attributeInfos[attributeIndex].name;
}

AttributeType Engine::getSceneParameterTypeByIndex(int fileHandle, int sceneIndex, int attributeIndex)
{
	if (uint(fileHandle) >= uint(this->files.size()))
		return P_INVALID;
	Pointer<EngineFile> file = this->files[fileHandle];
	if (file == null)
		return P_INVALID; 

	if (uint(sceneIndex) >= uint(file->sceneInfos.size()))
		return P_INVALID;
	SceneInfo* sceneInfo = file->sceneInfos[sceneIndex];
	if (uint(attributeIndex) >= uint(sceneInfo.attributeInfos.size()))
		return P_INVALID;
	return sceneInfo.attributeInfos[attributeIndex].type;
}
*/


int Engine::createScene(Pointer<EngineFile> file, int sceneIndex, int groupHandle)
{
	// check group handle
	if (uint(groupHandle) < uint(this->groups.size()))
	{
		Group* group = this->groups[groupHandle];
		if (group != NULL)
		{
			ArrayRef<const SceneInfo> sceneInfos = file->getSceneInfos();			
			void* global = file->getSceneGlobal(sceneIndex);

			// get a free scene handle
			int sceneHandle = getFreeHandle(this->scenes);

			// set opengl state and reset on scope exit
			StateSetter setter;

			// create scene instance
			Scene* scene = new Scene(sceneInfos[sceneIndex], global);
			scene->sceneIndex = sceneHandle;
			scene->groupIndex = groupHandle;
			this->scenes[sceneHandle] = scene;

			// add scene to group
			*group += scene;

			// add scene to file
			file->scenes += scene;

			// assign textures
			ArrayRef<const TextureInfo> textureInfos = file->getTextureInfos();
			ArrayData<AttributeInfo> attributeInfos = scene->sceneInfo.attributeInfos;
			ArrayData<TextureBinding> textureBindings = scene->sceneInfo.textureBindings;
			foreach (const TextureBinding& textureBinding, textureBindings)
			{
				const TextureInfo& textureInfo = textureInfos[textureBinding.textureIndex];
				void* global = file->getTextureGlobal(textureBinding.textureIndex);
				//const AttributeInfo& attributeInfo = attributeInfos[textureBinding.attributeIndex];
			
				// pointer to attribute
				void* pointer = scene->instance + textureBinding.offset;
		
				// copy texture (or array of textures) if correct type
				if (textureBinding.type == textureInfo.type)
					textureInfo.copy(global, pointer);
			}	

			// return handle (array index)
			return sceneHandle;
		}
	}
	return -1;
}

void Engine::deleteScene(Scene* scene)
{
	// calc memory range of scene instance
	//uint8_t* instanceBegin = scene->instance.begin();
	//uint8_t* instanceEnd = scene->instance.end();

	// delete attributes of this instance
	foreach (int attributeHandle, scene->attributeHandles)
	{
		if (attributeHandle != -1)
		{
			Attribute& attribute = this->attributes[attributeHandle];
			
			uint type = attribute.attributeInfo->type & 0xFF;
			if (type == P_STRING)
				this->strings.erase(attributeHandle);
			
			attribute.attributeInfo = &this->dummyAttributeInfo;
			attribute.pointer = NULL;
		}
	}
	
	// delete attribute sets of this instance
	foreach (int attributeSetHandle, scene->attributeSetHandles)
	{
		if (attributeSetHandle != -1)
		{
			AttributeSet& attributeSet = this->attributeSets[attributeSetHandle];
			attributeSet.attributeSetInfo = NULL;
			attributeSet.attributes = NULL;
		}
	}
/*	
	// delete clips of this instance
	foreach (int clipHandle, scene->clipHandles)
	{
		if (clipHandle != -1)
		{
			Clip& clip = this->clips[clipHandle];
			clip.clipInfo = NULL;
			clip.tracks = NULL;
		}
	}
*/
	// remove from scenes array
	this->scenes[scene->sceneIndex] = NULL;
	
	// delete scene
	delete scene;	
}

int Engine::getAttributeHandleInternal(int sceneHandle, int attributeIndex)
{
	// search for free attribute handle
	std::vector<Attribute>::iterator it = this->attributes.begin();
	std::vector<Attribute>::iterator end = this->attributes.end();
	for (; it != end; ++it)
	{
		if (it->attributeInfo == &this->dummyAttributeInfo)
			break;
	}
	int attributeHandle = int(it - this->attributes.begin());
	if (it == end)
	{
		// add attribute instance
		add(this->attributes);

		// cache number of attribute instances
		this->numAttributes = attributeHandle + 1;
	}

	// get scene
	Scene* scene = this->scenes[sceneHandle];

	// get attribute info
	const AttributeInfo* attributeInfo = &scene->sceneInfo.attributeInfos[attributeIndex];

	// get pointer to attribute data
	uint8_t* pointer = scene->instance + attributeInfo->offset;

	// set handle
	scene->attributeHandles[attributeIndex] = attributeHandle;

	// add attribute instance
	Attribute& attribute = this->attributes[attributeHandle];
	attribute.attributeInfo = attributeInfo;
	attribute.pointer = pointer;
		
	// return handle (array index)
	return attributeHandle;
}

int Engine::getAttributeSetHandleInternal(int sceneHandle, int attributeSetIndex)
{
	// search for free attribute set handle
	std::vector<AttributeSet>::iterator it = this->attributeSets.begin();
	std::vector<AttributeSet>::iterator end = this->attributeSets.end();
	for (; it != end; ++it)
	{
		if (it->attributeSetInfo == NULL)
			break;
	}
	int attributeSetHandle = int(it - this->attributeSets.begin());
	if (it == end)
	{
		// add attribute set instance
		add(this->attributeSets);
	}

	// get scene
	Scene* scene = this->scenes[sceneHandle];

	// get attribute set info
	const AttributeSetInfo* attributeSetInfo = &scene->sceneInfo.attributeSetInfos[attributeSetIndex];

	// get pointer to attributes in scene
	float* attributes = (float*)(scene->instance + attributeSetInfo->offset);

	// set handle
	scene->attributeSetHandles[attributeSetIndex] = attributeSetHandle;

	// add attributeSet instance
	AttributeSet& attributeSet = this->attributeSets[attributeSetHandle];
	attributeSet.attributeSetInfo = attributeSetInfo;
	attributeSet.scene = scene;
	attributeSet.attributes = attributes;
		
	// return handle (array index)
	return attributeSetHandle;	
}
/*
int Engine::getClipHandleInternal(int attributeSetHandle, int clipIndex)
{
	// search for free clip handle
	std::vector<Clip>::iterator it = this->clips.begin();
	std::vector<Clip>::iterator end = this->clips.end();
	for (; it != end; ++it)
	{
		if (it->clipInfo == NULL)
			break;
	}
	int clipHandle = int(it - this->clips.begin());
	if (it == end)
	{
		// add attribute set instance
		this->clips.add();
	}
	
	// get attribute set
	AttributeSet& attributeSet = this->attributeSets[attributeSetHandle];
	
	// get scene
	Scene* scene = attributeSet.scene;
	
	// get global clip index
	int globalClipIndex = attributeSet.attributeSetInfo->clipIndex + clipIndex;

	// get clip info
	const ClipInfo* clipInfo = &scene->sceneInfo.clipInfos[globalClipIndex];

	// set handle
	scene->clipHandles[globalClipIndex] = clipHandle;

	// add clip instance
	Clip& clip = this->clips[clipHandle];
	clip.clipInfo = clipInfo;
	clip.scene = scene;
	clip.tracks = attributeSet.tracks;
	clip.time = 0.0f;
	clip.weight = 0.0f;
	
	// return handle (array index)
	return clipHandle;
}
*/
void Engine::updateScene(Scene* scene)
{
	// iterate over attribute sets of scene, clear them and add tracks
	foreach (int attributeSetHandle, scene->attributeSetHandles)
	{
		if (attributeSetHandle != -1)
		{
			AttributeSet& attributeSet = this->attributeSets[attributeSetHandle];
			const AttributeSetInfo* attributeSetInfo = attributeSet.attributeSetInfo;
			if (attributeSetInfo != NULL)
			{
				// clear
				float* attributesBegin = attributeSet.attributes;
				float* attributesEnd = attributesBegin + attributeSetInfo->numTracks;
				for (float* it = attributesBegin; it != attributesEnd; ++it)
				{
					*it = 0.0f;
				}

				// add tracks
				foreach (Track& track, attributeSet.tracks)
				{
					if (track.clipInfo != NULL && abs(track.weight) >= 0.0001f)
					{
						scene->sceneInfo.addClip(scene->instance, track.clipInfo->index, attributeSet.attributes, track.clipTime, track.weight);
					}
				}
			}					
		}
	}

	/*
	// iterate over clips and add them to their attribute set
	foreach (int clipHandle, scene->clipHandles)
	{
		if (clipHandle != -1)
		{
			Clip& clip = this->clips[clipHandle];
			const ClipInfo* clipInfo = clip.clipInfo;
			if (abs(clip.weight) >= 0.0001f)
				scene->sceneInfo.addClip(scene->instance, clipInfo->index, clip.tracks, clip.time, clip.weight);
		}
	}
	*/
		
	// update scene by calling the scene code.
	// this is the last instruction therefore it may be a tail-call, i.e. jump to the function.
	// on windows the function has to call _alloca (32 bit) or __chkstk (64 bit) if it allocates more than 4k on the 
	// stack. see llvm::X86FrameLowering::emitPrologue
	scene->sceneInfo.update(scene->instance);
}


// EngineLoader

EngineLoader::~EngineLoader()
{
}

// EngineFile

EngineFile::~EngineFile()
{
}

} // namespace digi
