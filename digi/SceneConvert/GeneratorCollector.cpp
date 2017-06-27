#include <digi/Utility/IntUtility.h>
#include <digi/Utility/SetUtility.h>
#include <digi/System/Log.h>
#include <digi/System/MemoryDevices.h>
#include <digi/CodeGenerator/CodeWriterFunctions.h>
#include <digi/Mesh/Mesh.h> // calcAcmr
#include <digi/Compress/DLZSS.h>

#include "CompileHelper.h"
#include "compileShader.h"
#include "compileScene.h"
#include "compileSceneWebGL.h"
#include "PrintHelper.h"
#include "GeneratorCollector.h"

namespace digi {

void printAssign(CodeWriter& w, const std::string& srcValue, BufferFormat srcFormat, const ShaderType& destType)
{
	w << " = ";
		
	if (destType.type == ShaderType::INT)
	{
		// int
		ConverterWriter::depackToInt(w, srcValue, srcFormat, destType.numRows, splat4(0));
	}
	else
	{
		// float
		ConverterWriter::depackToFloat(w, srcValue, srcFormat, destType.numRows, make_float4(0, 0, 0, 1));
	}
}

namespace
{
	// return true if extra scale/offset uniform parameters are needed for the given vertex field
	enum VertexFieldFlags
	{
		// indicates that scale has to be set via uniform
		NEEDS_SCALE_UNIFORM = 1,
		
		// indicates that scale has to be a vector
		NEEDS_SCALE_VECTOR = 2,
		
		// indicates that offset has to be set via uniform
		NEEDS_OFFSET_UNIFORM = 4,

		// indicates that offset has to be a vector
		NEEDS_OFFSET_VECTOR = 8
	};

	struct ScaleOffsetFlags
	{
		// scale of vertex input
		float4 scale;
		
		// offset of vertex input
		float4 offset;
		
		// combination of VertexFieldFlags
		int flags;
		
		ScaleOffsetFlags()
			: scale(splat4(1.0f)), offset(), flags(0) {}
	};

	// get scale/offset and flags for given 
	ScaleOffsetFlags getScaleOffsetFlags(const ScaleOffsetInfo& scaleOffsetInfo, int numComponents)
	{
		ScaleOffsetFlags sof;
		bool first = true;
		foreach (Pointer<BufferInfo> bufferInfo, scaleOffsetInfo.bufferInfos)
		{
			// get scale and offset from buffer info
			float4 scale = bufferInfo->scale;
			float4 offset = bufferInfo->offset;
			
			if (first)
			{
				first = false;
				sof.scale = scale;
				sof.offset = offset;
			}

			for (int i = 0; i < numComponents; ++i)
			{
				if (!epsilonEquals(scale[i], sof.scale[i], 1e-6f))
					sof.flags |= NEEDS_SCALE_UNIFORM;
				if (!epsilonEquals(scale[i], scale.x, 1e-6f))
					sof.flags |= NEEDS_SCALE_VECTOR;
				if (!epsilonEquals(offset[i], sof.offset[i], 1e-6f))
					sof.flags |= NEEDS_OFFSET_UNIFORM;
				if (!epsilonEquals(offset[i], offset.x, 1e-6f))
					sof.flags |= NEEDS_OFFSET_VECTOR;
			}
		}
		/*foreach (const ScaleOffsetInfo::ScaleOffset& scaleOffset, info.scaleOffsets)
		{
			float4 scale = scaleOffset.scale;
			float4 offset = scaleOffset.offset;
			
			if (first)
			{
				first = false;
				sof.scale = scale;
				sof.offset = offset;
			}

			for (int i = 0; i < numComponents; ++i)
			{
				if (!epsilonEquals(scale[i], sof.scale[i], 1e-6f))
					sof.flags |= NEEDS_SCALE_UNIFORM;
				if (!epsilonEquals(scale[i], scale.x, 1e-6f))
					sof.flags |= NEEDS_SCALE_VECTOR;
				if (!epsilonEquals(offset[i], sof.offset[i], 1e-6f))
					sof.flags |= NEEDS_OFFSET_UNIFORM;
				if (!epsilonEquals(offset[i], offset.x, 1e-6f))
					sof.flags |= NEEDS_OFFSET_VECTOR;
			}
		}*/
		if (scaleOffsetInfo.unitScale)
		{
			for (int i = 0; i < numComponents; ++i)
			{
				if (!epsilonEquals(1.0f, sof.scale[i], 1e-6f))
					sof.flags |= NEEDS_SCALE_UNIFORM;
				if (!epsilonEquals(0.0f, sof.offset[i], 1e-6f))
					sof.flags |= NEEDS_OFFSET_UNIFORM;
			}
		}
		return sof;
	}

	// add scale/offset members for fieldName to scaleOffsetType
	void addScaleOffset(const ScaleOffsetInfoMap& scaleOffsetInfos, const std::string& fieldName, int numComponents,
		Pointer<StructType> scaleOffsetType)
	{
		// get set of vertex fields for given field name. no vertex fields if this is a deformer output.
		ScaleOffsetInfoMap::const_iterator it = scaleOffsetInfos.find(fieldName);
		if (it != scaleOffsetInfos.end())
		{
			const ScaleOffsetInfo& scaleOffsetInfo = it->second;

			// get scale/offset flags
			int flags = getScaleOffsetFlags(scaleOffsetInfo, numComponents).flags;
			
			if (flags & NEEDS_SCALE_UNIFORM)
			{
				scaleOffsetType->addMember(fieldName + ".s", VectorInfo(VectorInfo::FLOAT,
					(flags & NEEDS_SCALE_VECTOR) ? numComponents : 1).toString());
			}
			if (flags & NEEDS_OFFSET_UNIFORM)
			{
				scaleOffsetType->addMember(fieldName + ".o", VectorInfo(VectorInfo::FLOAT,
					(flags & NEEDS_OFFSET_VECTOR) ? numComponents : 1).toString());		
			}
		}
	}

	void printShaderAssign(CodeWriter& w, const std::string& fieldName, BufferFormat srcFormat,
		const std::string& destTypeString, const ScaleOffsetInfoMap& scaleOffsetInfos)
	{
		// srcFormat is float type as shader inputs are always float
		ShaderType destType(destTypeString);

		w << "input." << fieldName;
		printAssign(w, "vertex." + fieldName, srcFormat, destType);
		
		// get set of vertex fields for given field name. no vertex fields if this is a deformer output.
		ScaleOffsetInfoMap::const_iterator it = scaleOffsetInfos.find(fieldName);
		if (it != scaleOffsetInfos.end())
		{
			const ScaleOffsetInfo& fields = it->second;
			
			// get scale/offset flags
			ScaleOffsetFlags sof = getScaleOffsetFlags(fields, destType.numRows);

			// scale
			if (sof.flags & NEEDS_SCALE_UNIFORM)
				w << " * s_o." << fieldName << ".s";
			else
			{
				// print constant scalar or vector
				float4 scale = sof.scale;
				if (any(scale != 1.0f))
					w << " * " << varVector(scale, destType.numRows, true);
			}
			
			// offset
			if (sof.flags & NEEDS_OFFSET_UNIFORM)
				w << " + s_o." << fieldName << ".o";
			else
			{
				// print constant scalar or vector
				float4 offset = sof.offset;
				if (any(offset != 0.0f))
					w << " + " << varVector(offset, destType.numRows, true);
			}
		}	
		w << ";\n";
	}

	void printDeformerAssign(CodeWriter& w, const std::string& fieldName, BufferFormat srcFormat,
		float4 scale, float4 offset, const ShaderType& destType)
	{
		// replace normalized types by integer types and apply normalization to scale
		if (srcFormat.isNormalized())
		{
			srcFormat.type = srcFormat.type == BufferFormat::NORM ? BufferFormat::INT : BufferFormat::UINT;
			scale /= getMaxValues(srcFormat.getNumBits(), srcFormat.isSigned());
		}

		// use target type for standard layouts, but keep number of components
		// this is because compileDeformers() can only handle int and float scalars and vectors.
		// conversion from e.g. ushort to float is done in processDynamicBuffers()
		if (srcFormat.getLayoutType() == BufferFormat::STANDARD)
		{
			VectorInfo srcType = toVectorInfo(srcFormat);
			srcType.type = destType.type == ShaderType::FLOAT ? VectorInfo::FLOAT : VectorInfo::INT;
			srcFormat = toBufferFormat(srcType);
		}

		printAssign(w, fieldName, srcFormat, destType);


		// apply scale/offset
		if (any(scale != 1.0f))
			w << " * " << varVector(scale, destType.numRows, true);
		if (any(offset != 0.0f))
			w << " + " << varVector(offset, destType.numRows, true);
		w << ";\n";
	}
	
	int triangleScore(int d0, int d1, int d2)
	{
		int s0 = (d0 >> 31) ^ (d0 << 1);
		int s1 = (d1 >> 31) ^ (d1 << 1);
		int s2 = (d2 >> 31) ^ (d2 << 1);
		return s0 + s1 + s2;
	}

	template <typename Type>
	void setTriangle(Type* src, Type* dst0, size_t triangleCount, int sectionTriangleCount)
	{
		Type* dst1 = dst0 + triangleCount;
		Type* dst2 = dst1 + triangleCount;
		Type l0 = 0;
		Type l1 = 0;
		Type l2 = 0;
		for (size_t i = 0; i < sectionTriangleCount; ++i)
		{
			Type c0 = src[i * 3 + 0];
			Type c1 = src[i * 3 + 1];
			Type c2 = src[i * 3 + 2];
			
			Type n0 = c0;
			Type n1 = c1;
			Type n2 = c2;
			
			int score = triangleScore(Type(c0 - l0), Type(c1 - l1), Type(c2 - l2));
			int score1 = triangleScore(Type(c1 - l0), Type(c2 - l1), Type(c0 - l2));
			int score2 = triangleScore(Type(c2 - l0), Type(c0 - l1), Type(c1 - l2));
			if (score1 < score)
			{
				score = score1;
				n0 = c1;
				n1 = c2;
				n2 = c0;
			}
			if (score2 < score)
			{
				n0 = c2;
				n1 = c0;
				n2 = c1;
			}
			
			dst0[i] = l0 = n0;
			dst1[i] = l1 = n1;
			dst2[i] = l2 = n2;
		}
	}
	
} // anonymous namespace


typedef std::pair<const std::string, std::string> StringPair;

// GeneratorCollector
void GeneratorCollector::collectInstances(const std::vector<ShaderOptions>& shaderOptions, Pointer<Scene> scene,
	SceneInfo& sceneInfo)
{
	// named buffers
	foreach (const NamedBuffer& namedBuffer, scene->buffers)
	{
		BufferFormat format = namedBuffer.buffer->getFormat();
		this->namedBufferInfos += NamedBufferInfo(&namedBuffer, format, 1.0f, 0.0f);
	}


	// get layers from scene and insert into map
	std::map<std::string, Pointer<Layer> > layers;
	foreach (Pointer<Layer> layer, scene->layers)
	{
		layers[layer->name] = layer;
	}

	// get or create default layer (which has empty name)
	Pointer<Layer> defaultLayer = getValue(layers, std::string());
	if (defaultLayer == null)
		defaultLayer = new Layer();

	int numLayers = this->numLayers = int(shaderOptions.size());
	this->layers.resize(numLayers);

	// get layers for each layer index
	for (int layerIndex = 0; layerIndex < numLayers; ++layerIndex)
	{
		const ShaderOptions& options = shaderOptions[layerIndex];

		// find layer by name (use default layer of not found)
		Pointer<Layer> layer = getValue(layers, options.layerName, options.forceLayer ? defaultLayer : null);
		this->layers[layerIndex] = layer;
	}

	// collect objects of instances (buffers, primitives, deformers, shaders)
	foreach (const NamedInstance& instance, scene->instances)
	{
		// determine the layers that instance is part of
		std::vector<int> layers;
		for (int layerIndex = 0; layerIndex < numLayers; ++layerIndex)
		{
			if (this->isInLayer(layerIndex, instance))
				layers += layerIndex;
		}
		
		// collect objects of instance if it is part of at least one layer
		if (!layers.empty())
			this->collectInstance(shaderOptions, instance, layers, sceneInfo);
	}
}

void GeneratorCollector::collectInstance(const std::vector<ShaderOptions>& shaderOptions, const Instance& instance,
	const std::vector<int>& layers, SceneInfo& sceneInfo)
{
	// calc maximum transform index
	sceneInfo.maxTransformIndex = max(sceneInfo.maxTransformIndex, int(instance.transformIndex));

	this->collectInstance(shaderOptions, instance.instancer, instance.transformIndex, layers, sceneInfo);
}

void GeneratorCollector::collectInstance(const std::vector<ShaderOptions>& shaderOptions, Pointer<Instancer> instancer,
	int transformIndex, const std::vector<int>& layers, SceneInfo& sceneInfo)
{
	if (Pointer<MultiInstancer> multiInstancer = dynamicCast<MultiInstancer>(instancer))
	{
		foreach (Pointer<Instancer> instancer, multiInstancer->instancers)
			this->collectInstance(shaderOptions, instancer, transformIndex, layers, sceneInfo);
	}
	else if (Pointer<ShapeInstancer> shapeInstancer = dynamicCast<ShapeInstancer>(instancer))
	{
		// get/add shape instancer info
		ShapeInstancerInfo& shapeInstancerInfo = this->shapeInstancerInfoMap[shapeInstancer];

		// calc maximum bounding box index
		sceneInfo.maxBoundingBoxIndex = max(sceneInfo.maxBoundingBoxIndex, int(shapeInstancer->boundingBoxIndex));
		
		// collect transforms that are applied to each bounding box for getBoundingBox() of scene
		sceneInfo.boundingBoxTransforms[shapeInstancer->boundingBoxIndex].insert(transformIndex);			

		// shape
		Pointer<Shape> shape = shapeInstancer->shape;

		// deformer
		Pointer<Deformer> deformer = shapeInstancer->deformer;

		// get/create deformer info (also needed when deformers are in shaders to create Uniforms structure)
		this->deformerInfoMap[deformer];

		// shader
		Pointer<Shader> shader = shapeInstancer->shader;

		// get/create shader info
		ShaderInfo& shaderInfo = this->shaderInfoMap[shader];

		// collect combinations with deformers for deformers in shaders

		// get/create deformer/shader info (only shader if deformers are not in shaders)
		DeformerShaderInfo& deformerShaderInfo = shaderInfo.deformerShaderInfos
			[this->deformersInShaders ? deformer : null];

		// add shape instancers that use the shader or deformer/shader combo
		//deformerShaderInfo.shapeInstancers.insert(shapeInstancer);

		std::vector<std::string> deformerInputFields;
		std::set<std::string> deformerOutputFields;
		if (deformer != null)
		{
			// get deformer input and output fields
			deformer->inputType->getMembers(deformerInputFields);
			deformer->outputType->getMembers(deformerOutputFields);

			// build deformer job index (which is a map):
			// iterate over deformer input fields and get fields from shape instancer
			foreach (std::string& fieldName, deformerInputFields)
			{
				// get field from shape by name
				Pointer<VertexField> field = getValue(shapeInstancer->fields, fieldName);
				if (field == null)
					dError("missing deformer input field '" << fieldName << "'");
				
				if (Pointer<BufferVertexField> bufferField = dynamicCast<BufferVertexField>(field))
				{
					// add field name and buffer to deformerJobIndex
					shapeInstancerInfo.deformerJobIndex[fieldName] = FieldOfObject(bufferField->buffer);

					if (!this->deformersInShaders)
					{
						// buffers are not in shaders: get/add buffer info
						Pointer<BufferInfo>& bufferInfo = this->bufferInfoMap[bufferField->buffer];
						if (bufferInfo == null)
							bufferInfo = new BufferInfo();
						
						// add shape (constant mesh) to list of shapes that use the buffer
						bufferInfo->shapes.insert(shape);
						
						// add semantic flag
						bufferInfo->semanticFlags |= 1 << bufferField->semantic;
						
						// set buffer format as default output format (reduced resolution may be choosen later)
						bufferInfo->format = bufferField->buffer->getFormat();
					}
				}
				else if (Pointer<RefVertexField> refVertexField = dynamicCast<RefVertexField>(field))
				{
					// add field name and shape (e.g. particle system) to deformerJobIndex
					shapeInstancerInfo.deformerJobIndex[fieldName] =
						FieldOfObject(shapeInstancer->shape, refVertexField->fieldName);
				}
			}
		}

		std::vector<std::string> inputFields;
		if (this->deformersInShaders)
		{
			// deformers are in shaders: set as deformer/shader input fields
			inputFields.swap(deformerInputFields);
		}


		Shader::ShapeType shapeType;
		int shapeRenderMode = 0;
		if (Pointer<Mesh> mesh = dynamicCast<Mesh>(shape))
		{
			// mesh shape (renders a mesh)
			shapeType = Shader::MESH;
		
			// get render mode
			shapeRenderMode = mesh->renderMode;

			if (Pointer<TextMesh> textMesh = dynamicCast<TextMesh>(mesh))
			{
				// collect text mesh
				Pointer<ShapeInfo>& shapeInfo = this->shapeInfoMap[textMesh];
				if (shapeInfo == null)
				{
					Pointer<TextMeshInfo> textMeshInfo = new TextMeshInfo();
					shapeInfo = textMeshInfo;
					textMeshInfo->name = createVariableName(textMesh->name);
					textMeshInfo->numVertices = textMesh->maxNumSymbols * 4;
				}
			}
		}
		else if (Pointer<ParticleSystem> particleSystem = dynamicCast<ParticleSystem>(shape))
		{
			// particle system shape (renders sprites)
			shapeType = Shader::SPRITES;

			// collect the particle system
			collectParticleSystem(particleSystem);
		}
		shapeInstancerInfo.shapeType = shapeType;

		// get shader input fields for shape type (MESH or SPRITES)
		std::vector<Shader::InputField> shaderInputFields;
		shader->getInputFields(shapeInstancerInfo.shapeType, shaderInputFields);
		
		// extend deformer job index by pass-through fields (which are copied in
		// the deformer job from shape to dynamic vertex buffer).
		// also check if no shader input field is missing in the shape instancer.
		foreach (Shader::InputField& shaderInputField, shaderInputFields)
		{
			const std::string& fieldName = shaderInputField.name;
			Shader::Semantic semantic = shaderInputField.semantic;
			
			// ignore fields provided by deformer
			// mesh: also ignore particle inputs for mesh
			// sprites: also ignore texture coordinate and color
			if (!contains(deformerOutputFields, fieldName)
				&& !(shapeType == Shader::MESH && semantic == Shader::PARTICLE)
				&& !(shapeType == Shader::SPRITES && (semantic == Shader::TEXCOORD || semantic == Shader::COLOR)))
			{
				// get field from shape instancer by name
				Pointer<VertexField> field = getValue(shapeInstancer->fields, fieldName);
				
				// check if missing
				if (field == null)
					dError("missing shader input field '" << fieldName << "'");
			
				// add field name and shape to deformerJobIndex (which is a map)
				if (Pointer<RefVertexField> refVertexField = dynamicCast<RefVertexField>(field))
					shapeInstancerInfo.deformerJobIndex[fieldName] = FieldOfObject(shape, refVertexField->fieldName);
			
				// add to list of deformer/shader input fields
				inputFields += fieldName;
			}
		}
		
		// add used layers to target shader info
		foreach (int layerIndex, layers)
		{
			// determine render mode
			int renderMode = shapeRenderMode;
			const ShaderOptions& options = shaderOptions[layerIndex];
			if (renderMode & Mesh::DOUBLE_SIDED)
			{
				if (options.doubleSidedMode == ShaderOptions::ONE_PASS)
				{
					// if double sided in one pass there is no need for back side flag
					renderMode &= ~Mesh::BACK_SIDE;
				}
				else if (options.doubleSidedMode == ShaderOptions::FRONT)
				{
					// change double sided to front side
					renderMode &= ~(Mesh::DOUBLE_SIDED | Mesh::BACK_SIDE);
				}
				else if (options.doubleSidedMode == ShaderOptions::BACK)
				{
					// change double sided to back side
					renderMode &= ~Mesh::DOUBLE_SIDED;
					renderMode |= Mesh::BACK_SIDE;
				}
			}

			// create target shader info according to shape type (MESH, SPRITES) and render mode
			TargetShaderInfo& targetShaderInfo = deformerShaderInfo[ShapeTypeAndMode(shapeType/*, renderMode*/)];

			// create layer info if necessary
			Pointer<LayerInfo>& layerInfo = targetShaderInfo.layerInfoMap[layerIndex];
			if (layerInfo == null)
				layerInfo = new LayerInfo();
			
			// create render info
			RenderInfo& renderInfo = shapeInstancerInfo.renderInfoMap[layerIndex];
			renderInfo.layerInfo = layerInfo;
			renderInfo.renderMode = renderMode;
			
			layerInfo->shapeType = shapeType;
			layerInfo->renderModes.insert(renderMode);
			
			// accumulate render mode flags in layer info
			if (renderMode & Mesh::DOUBLE_SIDED)
			{
				// if two pass we render one side and then the other side
				if (options.doubleSidedMode == ShaderOptions::TWO_PASS)
					layerInfo->renderFlags |= LayerInfo::FRONT_SIDE_FLAG | LayerInfo::BACK_SIDE_FLAG;
				else
					layerInfo->renderFlags |= LayerInfo::DOUBLE_SIDED_FLAG;
			}
			else
			{
				// single sided: front or back
				if (renderMode & Mesh::BACK_SIDE)
					layerInfo->renderFlags |= LayerInfo::BACK_SIDE_FLAG;
				else
					layerInfo->renderFlags |= LayerInfo::FRONT_SIDE_FLAG;
			}
			
			// accumulate shape instancers that use the layer
			layerInfo->shapeInstancers.insert(shapeInstancer);
			
			// add scale/offset for deformer input fields (if deformer not in shader)
			if (!this->deformersInShaders)
			{
				foreach (const std::string& fieldName, deformerOutputFields)
				{
					// field is calculated by deformer, format is float
					layerInfo->scaleOffsetInfos[fieldName].unitScale = true;
				}
			}
			
			// add scale/offset for deformer/shader input fileds
			foreach (const std::string& fieldName, inputFields)
			{
				// get field from shape instancer by name
				Pointer<VertexField> field = getValue(shapeInstancer->fields, fieldName);
							
				// add buffer infos for fields of shape
				if (Pointer<BufferVertexField> bufferField = dynamicCast<BufferVertexField>(field))
				{
					// get/add buffer info
					Pointer<BufferInfo>& bufferInfo = this->bufferInfoMap[bufferField->buffer];
					if (bufferInfo == null)
						bufferInfo = new BufferInfo();
					
					// add shape (constant mesh) to list of shapes that use the buffer
					bufferInfo->shapes.insert(shape);
					
					// add semantic flag
					bufferInfo->semanticFlags |= 1 << bufferField->semantic;
					
					// set buffer format as default output format (reduced resolution may be choosen later)
					bufferInfo->format = bufferField->buffer->getFormat();

					// add vertex fields that are used by the deformer/shader combo (used to calc scale/offset of vertex inputs)
					layerInfo->scaleOffsetInfos[fieldName].bufferInfos.insert(bufferInfo);
				}
				else
				{
					// calculated field (e.g. by particle system), format is float
					layerInfo->scaleOffsetInfos[fieldName].unitScale = true;
				}
			}
		}
		

/*
		int shapeRenderMode = 0;
		if (Pointer<Mesh> mesh = dynamicCast<Mesh>(shapeInstancer->shape))
		{
			// mesh shape (renders a mesh)
			shapeInstancerInfo.shapeType = Shader::MESH;

			TargetShaderInfo& targetShaderInfo = deformerShaderInfo[Shader::MESH];

			// add shape instancers that use the target shader
			targetShaderInfo.shapeInstancers.insert(shapeInstancer);

			// get render mode
			shapeRenderMode = mesh->renderMode;
			
			if (!this->deformersInShaders)
			{
				// add scale = 1.0 and offset = 0.0 to scaleOffsetInfos of output fields because
				// the deformer passes its output vertex fields as float into the vertex shader
				foreach (const std::string& fieldName, deformerOutputFields)
				{
					targetShaderInfo.scaleOffsetInfos[fieldName].unitScale = true;
				}
			}

			// get shader input fields for MESH
			std::vector<Shader::InputField> shaderInputFields;
			shader->getInputFields(shapeInstancerInfo.shapeType, shaderInputFields);

			if (Pointer<ConstantMesh> constantMesh = dynamicCast<ConstantMesh>(mesh))
			{
				// iterate over fields
				foreach (ShapeInstancer::FieldPair& p, shapeInstancer->fields)
				{
					const std::string& fieldName = p.first;
					Pointer<VertexField> field = p.second;
									
					// add buffer infos for fields of shape
					if (Pointer<BufferVertexField> bufferField = dynamicCast<BufferVertexField>(field))
					{
						// get/add buffer info
						Pointer<BufferInfo>& bufferInfo = this->bufferInfoMap[bufferField->buffer];
						if (bufferInfo == null)
							bufferInfo = new BufferInfo();

						// add vertex fields that are used by the deformer/shader combo (used to calc scale/offset of vertex inputs)
						targetShaderInfo.scaleOffsetInfos[fieldName].bufferInfoMap.insert(bufferInfo);
						
						// add mesh to list of shapes that use the buffer
						bufferInfo->shapes.insert(constantMesh);
						
						// add semantic flag
						bufferInfo->semanticFlags |= 1 << bufferField->semantic;
						
						// set buffer format as default output format (reduced resolution may be choosen later)
						bufferInfo->format = bufferField->buffer->getFormat();
					}
				}
			}
			else if (Pointer<TextMesh> textMesh = dynamicCast<TextMesh>(mesh))
			{
				// iterate over fields
				foreach (ShapeInstancer::FieldPair& p, shapeInstancer->fields)
				{
					const std::string& fieldName = p.first;
					Pointer<VertexField> field = p.second;

					// tell shader that a scale/offset of 1.0f/0.0f is needed
					targetShaderInfo.scaleOffsetInfos[fieldName].unitScale = true;
				}
				
				// extend deformer job index by pass-through fields (which are copied in
				// the deformer job from particle system to dynamic vertex buffer).
				foreach (Shader::InputField& inputField, shaderInputFields)
				{
					// ignore particle inputs for mesh
					if (inputField.semantic != Shader::PARTICLE)
					{
						// check if field is not provided by the deformer
						if (!contains(deformerOutputFields, inputField.name))
						{
							// get field from shape instancer by name
							Pointer<VertexField> field = getValue(shapeInstancer->fields, inputField.name);
							//if (field == null)
							//	dError("missing field '" << inputField.name << "'");
						
							// add field name and text mesh to deformerJobIndex (which is a map)
							if (Pointer<RefVertexField> refVertexField = dynamicCast<RefVertexField>(field))
								shapeInstancerInfo.deformerJobIndex[inputField.name] =
									FieldOfObject(textMesh, refVertexField->fieldName);
						}
					}
				}
				
				Pointer<ShapeInfo>& shapeInfo = this->shapeInfoMap[textMesh];
				if (shapeInfo == null)
				{
					Pointer<TextMeshInfo> textMeshInfo = new TextMeshInfo();
					shapeInfo = textMeshInfo;
					textMeshInfo->name = createVariableName(textMesh->name);
					textMeshInfo->numVertices = textMesh->maxNumSymbols * 4;
				}
				/ *TextMeshInfo& textMeshInfo = this->textMeshInfos[textMesh];
				if (textMeshInfo.name.empty())
				{
					// create variable name from particle system name
					textMeshInfo.name = createVariableName(textMesh->name);
				}* /
			}
			
			// check if all shader input fields are present
			foreach (Shader::InputField& inputField, shaderInputFields)
			{
				// ignore particle inputs for meshes
				if (inputField.semantic != Shader::PARTICLE)
				{
					// check if field is not provided by the deformer
					if (!contains(deformerOutputFields, inputField.name))
					{
						// get field from shape instancer by name
						Pointer<VertexField> field = getValue(shapeInstancer->fields, inputField.name);
						if (field == null)
							dError("missing field '" << inputField.name << "'");
					}
				}
			}
		}
		else if (Pointer<ParticleSystem> particleSystem = dynamicCast<ParticleSystem>(shapeInstancer->shape))
		{
			// particle system shape (renders sprites)
			shapeInstancerInfo.shapeType = Shader::SPRITES;

			TargetShaderInfo& TargetShaderInfo = deformerShaderInfo[Shader::SPRITES];

			// add shape instancers that use the target shader
			TargetShaderInfo.shapeInstancers.insert(shapeInstancer);

			// extend deformer job index by pass-through fields (which are copied in
			// the deformer job from particle system to dynamic vertex buffer).
			std::vector<Shader::InputField> shaderInputFields;
			shader->getInputFields(Shader::SPRITES, shaderInputFields);
			foreach (Shader::InputField& inputField, shaderInputFields)
			{
				// ignore texture coordinate and color inputs for particles
				if (inputField.semantic != Shader::TEXCOORD && inputField.semantic != Shader::COLOR)
				{
					// check if field is not provided by the deformer
					if (!contains(deformerOutputFields, inputField.name))
					{
						// get field from shape instancer by name
						Pointer<VertexField> field = getValue(shapeInstancer->fields, inputField.name);
						if (field == null)
							dError("missing field '" << inputField.name << "'");
					
						// add field name and particle system to deformerJobIndex (which is a map)
						if (Pointer<RefVertexField> refVertexField = dynamicCast<RefVertexField>(field))
							shapeInstancerInfo.deformerJobIndex[inputField.name] =
								FieldOfObject(particleSystem, refVertexField->fieldName);
					}
				}
			}
			
			// collect the particle system
			collectParticleSystem(particleSystem);
		}
		
		// get target shader info for shape type (MESH or SPRITES)
		TargetShaderInfo& targetShaderInfo = deformerShaderInfo[shapeInstancerInfo.shapeType];

		// add used layers to target shader info
		foreach (int layerIndex, layers)
		{
			// create layer info if necessary
			Pointer<LayerInfo>& layerInfo = targetShaderInfo.layerInfoMap[layerIndex];
			if (layerInfo == null)
				layerInfo = new LayerInfo();
			
			// create render info
			RenderInfo& renderInfo = shapeInstancerInfo.renderInfoMap[layerIndex];
			renderInfo.layerInfo = layerInfo;

			// determine render mode and set to render info
			int renderMode = shapeRenderMode;
			const ShaderOptions& options = shaderOptions[layerIndex];
			if (renderMode & Mesh::DOUBLE_SIDED)
			{
				if (options.doubleSidedMode == ShaderOptions::ONE_PASS)
				{
					// if double sided in one pass there is no need for back side flag
					renderMode &= ~Mesh::BACK_SIDE;
				}
				else if (options.doubleSidedMode == ShaderOptions::FRONT)
				{
					// change double sided to front side
					renderMode &= ~(Mesh::DOUBLE_SIDED | Mesh::BACK_SIDE);
				}
				else if (options.doubleSidedMode == ShaderOptions::BACK)
				{
					// change double sided to back side
					renderMode &= ~Mesh::DOUBLE_SIDED;
					renderMode |= Mesh::BACK_SIDE;
				}
			}
			renderInfo.renderMode = renderMode;
			
			// accumulate render mode flags in layer info
			if (renderMode & Mesh::DOUBLE_SIDED)
				layerInfo->renderFlags |= LayerInfo::DOUBLE_SIDED_FLAG;
			else
			{
				if (renderMode & Mesh::BACK_SIDE)
					layerInfo->renderFlags |= LayerInfo::BACK_SIDE_FLAG;
				else
					layerInfo->renderFlags |= LayerInfo::FRONT_SIDE_FLAG;
			}
				
		}*/
	}
	else if (Pointer<ParticleInstancer> particleInstancer = dynamicCast<ParticleInstancer>(instancer))
	{
		// particle instancer (instances something else per particle, e.g. mesh or particle system)
		
		// collect the particle system
		collectParticleSystem(staticCast<ParticleSystem>(particleInstancer->particleSystem));
	
		// get/create info
		ParticleInstancerInfo& particleInstancerInfo = this->particleInstancerInfoMap[particleInstancer];

		// create variable name from particle instancer name
		particleInstancerInfo.name = createVariableName(particleInstancer->name);		

		// collect instances
		foreach (const std::vector<Instance>& instances, particleInstancer->instances)
		{
			foreach (const Instance& instance, instances)
			{
				this->collectInstance(shaderOptions, instance, layers, sceneInfo);
			}
		}
	}	
}

void GeneratorCollector::collectParticleSystem(Pointer<ParticleSystem> particleSystem)
{
	// add info for particle system
	Pointer<ShapeInfo>& shapeInfo = this->shapeInfoMap[particleSystem];
	if (shapeInfo == null)
	{
		Pointer<ParticleSystemInfo> particleSystemInfo = new ParticleSystemInfo();
		shapeInfo = particleSystemInfo;
		// create variable name from particle system name
		particleSystemInfo->name = createVariableName(particleSystem->name);

		// determine number of particles that need to be allocated/inited for the particle system
		int numParticles = 0;
		if (!particleSystem->initialState.empty())
		{
			numParticles = 0x7fffffff;
			foreach (const ParticleSystem::Field& field, particleSystem->initialState)
			{
				numParticles = min(numParticles, int(field.buffer->getNumElements()));
			}
		
			// number of particles to initialize in initInstance
			particleSystemInfo->numInitParticles = numParticles;
		}
		if (particleSystem->emitterUniformType != null)
			numParticles = max(numParticles, int(particleSystem->maxNumParticles));
		particleSystemInfo->numParticles = numParticles;
	}
}

void GeneratorCollector::chooseFormats(Pointer<BufferConverter> bufferConverter)
{
	int reduce = this->dataMode & SceneOptions::REDUCE_MASK;

	// iterate over all named buffers
	foreach (NamedBufferInfo& info, this->namedBufferInfos)
	{
		BufferFormat format = info.format;
		
		if (reduce == SceneOptions::REDUCE2 && format.isFloat32())
		{
			// determine scale/offset
			float* values = info.namedBuffer->buffer->getData<float>();
			size_t size = info.namedBuffer->buffer->getNumElements() * format.getNumComponents();
			float minValue = std::numeric_limits<float>::max();
			float maxValue = -std::numeric_limits<float>::max();
			for (int i = 0; i < size; ++i)
			{
				minValue = std::min(minValue, values[i]);
				maxValue = std::max(maxValue, values[i]);
			}
			//float scale = (maxValue - minValue) / 65536.0f;
			//float offset = minValue + 0.5f * scale;
			float scale = (maxValue - minValue) / 65535.0f;
			float offset = minValue;
		
			info.format = BufferFormat(BufferFormat::X16, BufferFormat::UINT);
			info.scale = scale;
			info.offset = offset;
		}
	}
	
	// iterate over all vertex buffers that were collected in collectInstance
	foreach (BufferInfoPair& p, this->bufferInfoMap)
	{
		Pointer<Buffer> buffer = p.first;
		Pointer<BufferInfo> bufferInfo = p.second;
		
		BufferFormat format = buffer->getFormat();
		int numChannels = format.getNumChannels();
				
		// reduce resolution of vertex buffers: determine destination format for float source formats
		if (reduce == SceneOptions::REDUCE2 && format.isFloat32())
		{
			const char* code =
				"struct Global\n"
				"{\n"
				"  float4 minValue;\n"
				"  float4 maxValue;\n"
				"};\n"
				"void main(float4 value, Global& global)\n"
				"{\n"
				"  global.minValue = min(global.minValue, value);\n"
				"  global.maxValue = max(global.maxValue, value);\n"
				"}\n";
			
			ALIGN(16) struct
			{
				float4 minValue;
				float4 maxValue;				
			} global;
			global.minValue = +std::numeric_limits<float>::max();
			global.maxValue = -std::numeric_limits<float>::max();
			bufferConverter->process(format, buffer->getData<void>(), code, &global, buffer->getNumElements());

			// calc min and max value for all components
			float minValue = global.minValue[0];
			float maxValue = global.maxValue[0];
			for (int i = 1; i < numChannels; ++i)
			{
				minValue = min(minValue, global.minValue[i]);
				maxValue = max(maxValue, global.maxValue[i]);
			}
			
			// determine format and calc scale/offset
			BufferFormat::Layout baseLayout;
			BufferFormat::Type type;
			float4 scale = splat4(1.0f);
			float4 offset = splat4(0.0f);
			if (bufferInfo->semanticFlags == (1 << BufferVertexField::TANGENT))
			{
				baseLayout = BufferFormat::X8;
				type = BufferFormat::NORM;

				// omit scale/offset if in typical range -1 to 1
				if (minValue < -1.01f || maxValue > 1.01f)
				{
					scale = (global.maxValue - global.minValue) * 0.5f;
					offset = (global.minValue + global.maxValue) * 0.5f;
				}
			}
			else if (bufferInfo->semanticFlags == (1 << BufferVertexField::COLOR))
			{
				baseLayout = BufferFormat::X8;
				type = BufferFormat::UNORM;
				
				// omit scale/offset if in typical range 0 to 1
				if (minValue < -0.01f || maxValue > 1.01f)
				{
					scale = global.maxValue - global.minValue;
					offset = global.minValue;
				}
			}
			else if (bufferInfo->semanticFlags == (1 << BufferVertexField::TEXCOORD))
			{
				baseLayout = BufferFormat::X16;
				type = BufferFormat::UNORM;
				
				// omit scale/offset if in typical range 0 to 1
				if (minValue < -0.0001f || maxValue > 1.0001f)
				{
					scale = global.maxValue - global.minValue;
					offset = global.minValue;
				}
			}
			else
			{
				// other, e.g. position
				baseLayout = BufferFormat::X16;
				type = BufferFormat::UINT;
			
				scale = (global.maxValue - global.minValue) / 65536.0f;
				offset = global.minValue + 0.5f * scale;
			}
			
			// set scale/offset of unused channels to 1.0/0.0
			switch (numChannels)
			{
			case 1:
				scale.y = 1.0f;
				offset.y = 0.0f;
				// fall through
			case 2:
				scale.z = 1.0f;
				offset.z = 0.0f;
				// fall through
			case 3:
				scale.w = 1.0f;
				offset.w = 0.0f;
			}

			// set packed format and scale/offset for conversion from float to packed format
			bufferInfo->format = BufferFormat(baseLayout, numChannels, type);
			bufferInfo->scale = scale;
			bufferInfo->offset = offset;
		}
	}
}

void GeneratorCollector::writeUniformOutput(CodeWriter& w)
{
	// output to used deformers
	foreach (DeformerInfoPair& p, this->deformerInfoMap)
	{
		const Pointer<Deformer>& deformer = p.first;			
		if (deformer != null)
		{
			foreach (const std::string& assignment, deformer->assignments)
			{
				w << "uniforms." << deformer->path << assignment << ";\n";
			}
		}
	}

	// output to used shaders
	foreach (ShaderInfoPair& p, this->shaderInfoMap)
	{
		const Pointer<Shader>& shader = p.first;
		foreach (const std::string& assignment, shader->assignments)
		{
			w << "uniforms." << shader->path << assignment << ";\n";
		}
	}

	// output to used particle systems
	foreach (ShapeInfoPair& p, this->shapeInfoMap)
	{
		if (Pointer<ParticleSystem> particleSystem = dynamicCast<ParticleSystem>(p.first))
		{
			foreach (const std::string& assignment, particleSystem->emitterAssignments)
			{
				w << "uniforms.e." << particleSystem->path << assignment << ";\n";
			}
			foreach (const std::string& assignment, particleSystem->assignments)
			{
				w << "uniforms." << particleSystem->path << assignment << ";\n";
			}
		}
	}

	// output to used particle instancers
	foreach (ParticleInstancerInfoPair& p, this->particleInstancerInfoMap)
	{
		const Pointer<ParticleInstancer>& particleInstancer = p.first;
		foreach (const std::string& assignment, particleInstancer->assignments)
		{
			w << "uniforms." << particleInstancer->path << assignment << ";\n";
		}
	}
}

bool GeneratorCollector::compileShaders(Compiler& compiler, const std::vector<ShaderOptions>& shaderOptions,
	Language mainLanguage, Language shaderLanguage)
{
	bool anyHasError = false;
	int numLayers = int(shaderOptions.size());

	// compile shaders (also tell shapes which fields are actually needed by the shaders)
	foreach (ShaderInfoPair& shaderInfoPair, this->shaderInfoMap)
	{
		Pointer<Shader> shader = shaderInfoPair.first;
		ShaderInfo& shaderInfo = shaderInfoPair.second;

		// create uniform type name from shader name
		shaderInfo.typeName = createVariableName(shader->name);

		// iterate over deformer/shader combos
		foreach (DeformerShaderInfoPair& p, shaderInfo.deformerShaderInfos)
		{
			// get deformer, is null if deformers are not in shaders
			Pointer<Deformer> deformer = p.first;
			DeformerShaderInfo& deformerShaderInfo = p.second;

			// get input and output fields from deformer (if deformer present and in shaders)
			std::map<std::string, std::string> deformerInputFields;
			std::set<std::string> deformerOutputFields;
			if (deformer != null)
			{				
				deformer->inputType->getMembers(deformerInputFields);
				deformer->outputType->getMembers(deformerOutputFields);
			}

			// iterate over shape type (mesh or sprites)
			foreach (TargetShaderInfoPair& p, deformerShaderInfo)
			{
				Shader::ShapeType shapeType = p.first.type;
				//int renderMode = p.first.mode;
				TargetShaderInfo& targetShaderInfo = p.second;
				
				std::vector<Shader::InputField> shaderInputFields;
				shader->getInputFields(shapeType, shaderInputFields);
				
				
				// iterate over layers and compile shaders
				TargetShaderInfo::LayerInfoMap::iterator it = targetShaderInfo.layerInfoMap.begin();
				while (it != targetShaderInfo.layerInfoMap.end())
				{
					TargetShaderInfo::LayerInfoMap::iterator next = it;
					++next;
					int layerIndex = it->first;
					Pointer<LayerInfo> layerInfo = it->second;
					int renderFlags = layerInfo->renderFlags;
					const ShaderOptions& options = shaderOptions[layerIndex];


					// create struct for vertex attributes from input fields. vertex attributes are always float
					Pointer<StructType> vertexType = new StructType();
					Pointer<StructType> scaleOffsetType = new StructType();			
					Pointer<StructType> inputType = new StructType();
					
					// iterate over inputs of deformer (if in shader)
					foreach (const StringPair& p, deformerInputFields)
					{
						const std::string& fieldName = p.first;
						int numComponents = VectorInfo(p.second).numRows;
						std::string fieldType = VectorInfo(VectorInfo::FLOAT, numComponents).toString();				
						vertexType->addMember(fieldName, fieldType);
					
						// add scale/offset uniform variables if shader is rendered with different buffer scale/offsets 
						addScaleOffset(layerInfo->scaleOffsetInfos, fieldName, numComponents,
							scaleOffsetType);
					}

					// iterate over inputs of shader
					foreach (Shader::InputField& inputField, shaderInputFields)
					{
						const std::string& fieldName = inputField.name;
						const std::string& fieldType = inputField.type;

						// don't add vertex attribute if it is a deformer input or output
						// mesh: don't add if particle
						// sprites: don't add if texcoord or color
						if (!contains(deformerInputFields, fieldName) && !contains(deformerOutputFields, fieldName)
							&& !(shapeType == Shader::MESH && inputField.semantic == Shader::PARTICLE)
							&& !(shapeType == Shader::SPRITES && (inputField.semantic == Shader::TEXCOORD || inputField.semantic == Shader::COLOR)))
						{
							// add to vertex type (always float)
							int numComponents = VectorInfo(fieldType).numRows;
							std::string floatType = VectorInfo(VectorInfo::FLOAT, numComponents).toString();	
							vertexType->addMember(fieldName, floatType);
							
							// add scale/offset uniform variables if shader is rendered with different buffer scale/offsets 
							addScaleOffset(layerInfo->scaleOffsetInfos, fieldName, numComponents,
								scaleOffsetType);
						}
						
						// add to shader input type
						inputType->addMember(fieldName, fieldType);
					}
										
					// store scale/offset type for use in transform code before draw
					if (!scaleOffsetType->isEmpty())
						layerInfo->scaleOffsetType = scaleOffsetType;

					
					std::string code;
					Pointer<IODevice> dev = new StringRefDevice(code);
					
					// generate input c++ code for combo of deformer and shader (mesh)
					{
						CodeWriter w(dev);
						
						// material variables
						writeBindingVariable(w, "viewMatrix", "float4x4");
						writeBindingVariable(w, "projectionMatrix", "float4x4");
						writeBindingVariable(w, "viewProjectionMatrix", "float4x4");
						writeBindingVariable(w, "material", shader->materialType);
						if (deformer != null)
							writeBindingVariable(w, "deformer", deformer->uniformType);
						
						// transform variables
						writeBindingVariable(w, "matrix", "float4x4"); // world matrix of shape
						writeBindingVariable(w, "s_o", scaleOffsetType);
						writeBindingVariable(w, "objectId", "int");
						
						// vertex shader variables
						writeBindingVariable(w, "vertex", vertexType);
						writeBindingVariable(w, "pPosition", "float4");
						
						// pixel shader variables
						writeBindingVariable(w, "output", "float4");
						writeBindingVariable(w, "outputs", Type::create("[16]float4"));

						// special purpose outputs
						writeBindingVariable(w, "sort", "bool");

						if (shapeType == Shader::MESH)
						{
							// deprecated: vertex shader view space position for clip planes
							writeBindingVariable(w, "vPosition", "float4");
							
							if (renderFlags != LayerInfo::FRONT_SIDE_FLAG && renderFlags != LayerInfo::BACK_SIDE_FLAG)
							{
								// the shapes have not the same cull front or back mode: use frontFacing built-in variable
								// note: frontFacing is accessed by nodes (SamplerInfoNode)
								writeBindingVariable(w, "frontFacing", "float");
							}
							
							// flip flag indicates if scale is negative
							writeBindingVariable(w, "flip", "bool");
						}
						
						if (shapeType == Shader::SPRITES)
						{
							// size of viewport
							w << "float viewport[4];\n";

							// output: vertex shader sprite size
							writeBindingVariable(w, "pointSize", "float");
							
							// input: pixel shader texture coordinate of sprite
							writeBindingVariable(w, "pointCoord", "float2");
						}

						// input: pixel shader texture coordinate of sprite
						writeBindingVariable(w, "fragCoord", "float4");
						
						// prototype for discard function
						w << "extern \"C\" void discard();\n";
						
						// shader function
						w << "extern \"C\" void main()\n";
						w.beginScope();

						// shader input variable
						inputType->writeVariable(w, "input");

						w.beginScope();

						// deformer (if in shader)
						if (deformer != null)
						{
							// deformer output variable
							deformer->outputType->writeVariable(w, "output");

							w.beginScope();

							// deformer input variable
							deformer->inputType->writeVariable(w, "input");
							
							// copy fields from vertex attributes into deformer input
							foreach (const StringPair& p, deformerInputFields)
							{
								const std::string& fieldName = p.first;
								const std::string& fieldType = p.second;
								BufferFormat srcFormat(BufferFormat::Layout(BufferFormat::X32 + VectorInfo(fieldType).numRows - 1),
									BufferFormat::FLOAT);
									
								// assign deformer input, also apply scale/offset of packed vertex buffers
								printShaderAssign(w, fieldName, srcFormat, fieldType, layerInfo->scaleOffsetInfos);
							}
							
							// deformer code			
							w << deformer->code;
							
							w.endScope();
						}

						// copy fields from vertex attributes into shader input
						foreach (Shader::InputField& inputField, shaderInputFields)
						{
							const std::string& fieldName = inputField.name;
							const std::string& fieldType = inputField.type;
							if (shapeType == Shader::MESH && inputField.semantic == Shader::PARTICLE)
							{
								// particle attribute: this is a mesh shader, therefore use zero
								w << "input." << fieldName << " = 0.0f;\n";
							}
							else if (shapeType == Shader::SPRITES && inputField.semantic == Shader::TEXCOORD)
							{
								// texture coordinate handled after calculation of sprite texture coordinate
							}
							else if (shapeType == Shader::SPRITES && inputField.semantic == Shader::COLOR)
							{
								// vertex color: this is a sprite shader, therefore use zero
								w << "input." << fieldName << " = 0.0f;\n";
							}
							else
							{							
								BufferFormat srcFormat(BufferFormat::Layout(BufferFormat::X32 + VectorInfo(fieldType).numRows - 1),
									BufferFormat::FLOAT);
								
								// don't copy vertex attribute if it is calculated by deformer
								if (!contains(deformerOutputFields, fieldName))
								{
									// assign shader input, also apply scale/offset of packed vertex buffers
									printShaderAssign(w, fieldName, srcFormat, fieldType,
										layerInfo->scaleOffsetInfos);
								}
								else
								{
									// get vertex attribute from deformer output
									w << "input." << fieldName << " = output." << fieldName << ";\n";
								}
							}
						}
						w.endScope();
			
						// the variables worldViewMatrix, position, viewVector, normal, tangent, bitangent
						// can also be used by user supplied output code

						// calc world view matrix
						w << "float4x4 worldViewMatrix = viewMatrix * matrix;\n";
				
						// calc surface position in view space
						w << "float3 position = transformPosition(worldViewMatrix, input.position);\n";
						w << "float4 position4 = vector4(position, 1.0f);\n";

						if (shapeType == Shader::MESH)
						{
							if (options.userClipPlanes && shaderLanguage != Language::ESSL)
							{
								// output view space position for user-defined clip planes
								w << "vPosition = position4;\n";
							}
						
							// output projected position
							//w << "pPosition = projectionMatrix * position4;\n";
							w << "float4 projectedPosition = projectionMatrix * position4;\n";
							w << "pPosition = projectedPosition;\n";

							// check if at least one shape is either draw front or draw back
							bool needFlip = (renderFlags & (LayerInfo::FRONT_SIDE_FLAG | LayerInfo::BACK_SIDE_FLAG)) != 0;
							if (needFlip)
							{
								// set flip flag to flip normal and culling mode if determinant of transform is negative
								w << "float3 mx = worldViewMatrix.x.xyz;\n";
								w << "float3 my = worldViewMatrix.y.xyz;\n";
								w << "float3 mz = worldViewMatrix.z.xyz;\n";
								w << "float3 cx = cross(my, mz);\n";
								w << "bool neg = mac(mx, cx) < 0.0f;\n";
								w << "flip = neg;\n";
								if (renderFlags == LayerInfo::FRONT_SIDE_FLAG)
								{
									// all shapes are draw front
									w << "float frontFacing = neg ? -1.0f : 1.0f;\n";
								}
								if (renderFlags == LayerInfo::BACK_SIDE_FLAG)
								{
									// all shapes are draw back
									w << "float frontFacing = neg ? 1.0f : -1.0f;\n";
								}
							}
							
							w << "float3 normal;\n";
							w << "float3 tangent;\n";
							w << "float3 bitangent;\n";
							if (shader->geometry == Shader::POSITION)
							{
								// only position
								w << "normal = 0.0f;\n";
								w << "tangent = 0.0f;\n";
								w << "bitangent = 0.0f;\n";
							}
							else if (shader->geometry == Shader::POSITION_NORMAL)
							{
								// position and normal
								w.beginScope();
					
								// calc normal matrix: inverse transpose of rotation component (omit division by determinant)
								if (!needFlip)
								{
									w << "float3 mx = worldViewMatrix.x.xyz;\n";
									w << "float3 my = worldViewMatrix.y.xyz;\n";
									w << "float3 mz = worldViewMatrix.z.xyz;\n";
									w << "float3 cx = cross(my, mz);\n";
								}
								w << "float3 cy = cross(mz, mx);\n";
								w << "float3 cz = cross(mx, my);\n";

								// calc normal (N) (vs() forces execution in vertex shader)
								w << "float3 n = input.normal;\n";
								w << "normal = normalize(vs(cx * n.x + cy * n.y + cz * n.z))"
									" * frontFacing;\n"; // (frontFacing ? 1.0f : -1.0f)

								w << "tangent = 0.0f;\n";
								w << "bitangent = 0.0f;\n";
					
								w.endScope();		
							}
							else
							{
								// position, tangent, bitangent and normal
								w.beginScope();

								// tangent space in view space
								//! handle flipped/left handed tangent spaces
								w << "float3 t = vs(transformDirection(worldViewMatrix, input.tangent));\n";
								w << "float3 b = vs(transformDirection(worldViewMatrix, input.bitangent));\n";

								// calc normal (N) (vs() forces execution in vertex shader)
								w << "float3 n = normalize(vs(cross(t, b)));\n";		
								w << "normal = n * (frontFacing ? 1.0f : -1.0f);\n";

								if (shader->geometry == Shader::POSITION_TANGENTS)
								{
									// calc tangent (T) and bitangent (B)
									w << "tangent = normalize(t);\n";
									w << "bitangent = cross(n, tangent);\n";			
								}
								else
								{
									// shader uses only normal even if tangent space is provided
									w << "tangent = 0.0f;\n";
									w << "bitangent = 0.0f;\n";
								}

								w.endScope();		
							}
						}
						
						if (shapeType == Shader::SPRITES)
						{
							// prevent multiple loads from projection matrix. todo: get global value numbering working
							w << "float4x4 pm = projectionMatrix;\n";
							
							// output projected position
							w << "float4 pp = pm * position4;\n";
							w << "pPosition = pp;\n";
							
							// sprite parameters
							w << "float3 sprite = input.sprite;\n";
							
							// calc projected point sprite size 
							w << "float2 ps = (pm * vector4(sprite.xy, 0.0f, 0.0f)).xy;\n";
							
							// calc screen size (pp.xy + ps/2) / pp.w - (pp.xy - ps/2) / pp.w = ps / pp.w
							w << "float2 ss = ps / pp.w * (vector2(viewport[2], viewport[3]) * 0.5f);\n";
							
							// calc point sprite rotation and scale matrix (unit sprite to rotated/scaled sprite,
							// where unit sprite is in range (-0.5, 0.5))
							w << "float c = cos(sprite.z);\n";
							w << "float s = sin(sprite.z);\n";
							w << "float2x2 m = matrix2x2(c * ss.x, s * ss.x, -s * ss.y, c * ss.y);\n";


							// calc square bounding box of rotated/scaled sprite
							w << "float bb = max(max(abs(m.x + m.y), abs(m.x - m.y)));\n";

							// point sprite size is bounding box
							w << "pointSize = bb;\n";
							
							// calc reverse transform from rotated/scaled sprite to unit sprite.
							// multiplication with bounding box is to scale pointCoord to bounding box size
							w << "float2x2 n_ = inv(m) * bb;\n";
							w << "float2x2 n = matrix2(vs(n_.x), vs(-n_.y));\n"; // negative to flip y coordinate of pontCoord
							
							// transform pointCoord from space of rotated/scaled sprite back to space of unit sprite
							w << "float2 uc = n * (pointCoord - 0.5f);\n";

							// mask point sprite (check if unit sprite coordinate in range (-0.5, 0.5))
							w << "if (max(abs(uc)) >= 0.5f) discard();\n";

							// calc texture coordinate for particle textures
							w << "float2 texCoord = uc + 0.5f;\n";
							
							// copy texCoord into shader input
							foreach (Shader::InputField& inputField, shaderInputFields)
							{
								const std::string& fieldName = inputField.name;
								//const std::string& fieldType = inputField.type;
								if (inputField.semantic == Shader::TEXCOORD)
								{
									// uv coordinate
									w << "input." << fieldName << " = texCoord;\n";
								}
							}

							w << "float3 normal = vector3(0.0f, 0.0f, 1.0f);\n";
							w << "float3 tangent = vector3(c, s, 0.0f);\n";
							w << "float3 bitangent = vector3(-s, c, 0.0f);\n";

							if (!options.avoidPixelShader)
							{
								// calc correct view space position on sprite
								w << "position += tangent * uc.x * sprite.x + bitangent * uc.y * sprite.y;\n";
								w << "position4 = vector4(position, 1.0f);\n";
							}
						}
						
						// view vector (V) in view space (points from surface to camera)
						w << "float3 viewVector = -normalize(position);\n";

						// the shader code. can use position, normal, tangent, bitangent, frontFacing, sprite, viewVector
						w << shader->code;

						// layer specific output code
						this->printCode(w, options.outputCode);
						
						// end main function
						w.endScope();
					}

					// create comment and variable name for shader
					if (shapeType == Shader::MESH)
					{
						layerInfo->comment = Code() << "mesh, ";
						layerInfo->name = "m";//str(boost::format("m%1%") % renderMode);
					}
					else if (shapeType == Shader::SPRITES)
					{
						layerInfo->comment = Code() << "sprites, ";
						layerInfo->name = "p";
					}
					if (deformer)
					{
						layerInfo->comment += Code() << "deformer '" << deformer->name << "', ";
						layerInfo->name += createVariableName(deformer->name);
					}				
					layerInfo->comment += Code() << "shader '" << shader->name << "'";
					layerInfo->name += shaderInfo.typeName;
					if (numLayers != 1 || !shaderOptions[0].layerName.empty())
					{
						layerInfo->comment += Code() << ", layer '" << options.layerName << "'";
						layerInfo->name += Code() << '_' << layerIndex;
					}
					
					// handle languages where structs are splitted into arrays of int, float and objects (e.g. javascript)
					int3 deformerOffset = {};
					int3 materialOffset = {};
					if (this->uniformsPrinter)
					{
						if (deformer != null)
							deformerOffset = this->uniformsPrinter->getOffset('.' + deformer->path);
						materialOffset = this->uniformsPrinter->getOffset('.' + shader->path);
					}
					
					// compile shader (may contain a deformer)
					int2 numTextures;
					bool hasError = !compileShader(
						compiler, code,
						deformerOffset, // offsets of deformer uniform in scene uniforms
						materialOffset, // offsets of shader material in scene uniforms
						layerInfo->materialCode,
						layerInfo->transformCode,
						layerInfo->vertexShader,
						layerInfo->pixelShader,
						layerInfo->transferVariables,
						layerInfo->materialVariables,
						layerInfo->materialLargeArrays,
						layerInfo->transformVariables,
						layerInfo->vertexBindings,
						layerInfo->sortMode,
						layerInfo->useFlags,
						numTextures,
						mainLanguage,
						shaderLanguage,
						options,
						true); // flip y
					
					if (hasError)
					{
						anyHasError = true;
						dError(layerInfo->comment << " failed to compile");
						targetShaderInfo.layerInfoMap.erase(it);
					}
					else
					{
						this->shaderUseFlags |= layerInfo->useFlags;
						this->numTextures = max(this->numTextures, numTextures);
						
						// check if this layerInfo has the same vertex/pixel shader as another layerInfo
						std::pair<EqualShaderlayerInfoSet::iterator, bool> p = this->equalShaderlayerInfos.insert(layerInfo);
						if (!p.second)
						{
							layerInfo->equalShaderLayerInfo = *p.first;
							layerInfo->equalShaderLayerInfo->sharedShader = true;
							layerInfo->sharedShader = true;
						}
						
						// iterate over shape instancers that use this layer info
						foreach (const Pointer<ShapeInstancer>& shapeInstancer, layerInfo->shapeInstancers)
						{
							ShapeInstancerInfo& shapeInstancerInfo = this->shapeInstancerInfoMap[shapeInstancer];

							// get deformer which is not in shader
							Pointer<Deformer> deformer = !this->deformersInShaders ? shapeInstancer->deformer : null;
							std::set<std::string> deformerOutputFields;
							if (deformer != null)
								deformer->outputType->getMembers(deformerOutputFields);
							
							// get deformer info
							DeformerInfo& deformerInfo = this->deformerInfoMap[deformer];

							// get deformer job info (also if deformer is null or in shader)
							DeformerJobInfo& deformerJobInfo = deformerInfo.jobInfos[shapeInstancerInfo.deformerJobIndex];
							deformerJobInfo.shape = shapeInstancer->shape;
							
							// iterate over vertex fields that are used by the shader
							foreach (const LayerInfo::VertexBindingPair& p, layerInfo->vertexBindings)
							{
								// name of field (e.g. "position")
								const std::string& fieldName = p.first;							
								
								// check if field is provided by deformer (if present and not in shader)
								if (contains(deformerOutputFields, fieldName))
								{
									// tell deformer job that the field is used by a shader
									deformerJobInfo.usedOutputFields.insert(fieldName);
								}
								else
								{								
									// get field from shape by name
									Pointer<VertexField> field = shapeInstancer->fields[fieldName];
									
									if (Pointer<BufferVertexField> bufferField = dynamicCast<BufferVertexField>(field))
									{
										// tell buffer that it is used by a deformer/shader combo
										this->bufferInfoMap[bufferField->buffer]->scopes[SCOPE_SHADER].used = true;
									}
									else if (Pointer<RefVertexField> refField = dynamicCast<RefVertexField>(field))
									{
										// add extra field that is passed through the deformer
										deformerJobInfo.extraFields.insert(fieldName);
									}
								}
							}

							// delete deformer job again if empty
							// do it this way because the fields are accumulated for each layer.
							if (deformerJobInfo.usedOutputFields.empty() && deformerJobInfo.extraFields.empty())
								deformerInfo.jobInfos.erase(shapeInstancerInfo.deformerJobIndex);
						}
					}
					it = next;
				}				
			}
		}
	}
	
	// build list of all layers (with successful compilation) for convenience
	foreach (ShaderInfoPair& shaderInfoPair, this->shaderInfoMap)
	{
		ShaderInfo& shaderInfo = shaderInfoPair.second;
		
		// iterate over deformer/shader combos (only one with deformer = null when deformers not in shaders)
		foreach (DeformerShaderInfoPair& p, shaderInfo.deformerShaderInfos)
		{
			DeformerShaderInfo& deformerShaderInfo = p.second;
			
			// iterate over shader types (MESH, SPRITE)
			foreach (TargetShaderInfoPair& p, deformerShaderInfo)
			{
				TargetShaderInfo& TargetShaderInfo = p.second;
				
				// iterate over layers
				foreach (TargetShaderInfo::LayerInfoPair& p, TargetShaderInfo.layerInfoMap)
				{
					Pointer<LayerInfo> layerInfo = p.second;
					this->layerInfos.push_back(layerInfo);
				}
			}
		}
	}
	
	// return if any layer has a compile error
	return anyHasError;
}

void GeneratorCollector::printCode(CodeWriter& w, const std::string& code)
{
	if (code.empty() || code == "color")
	{
		// default color output code
		w << "sort = alpha.x < 0.99f;\n";
		w << "if (max(color) < 0.01f & alpha.x < 0.01f) discard();\n";
		w << "output = vector4(color, alpha.x);\n";
	}
	else if (code == "depth4x8") {
		// encode depth into 4 components of 8 bit depth
		//w << "float4 splitDepth = frac((projectedPosition.z/projectedPosition.w * 0.5 + 0.5) * vector4(255.0f*256.0f*256.0f, 255.0f*256.0f, 255.0f, 255.0f/256.0f)) * (256.0f/255.0f);\n";
		w << "float4 splitDepth = frac(fragCoord.z * vector4(255.0f*256.0f*256.0f, 255.0f*256.0f, 255.0f, 255.0f/256.0f)) * (256.0f/255.0f);\n";
		w << "output = splitDepth - splitDepth.xxyz * vector4(0.0f, 1.0f/256.0f, 1.0f/256.0f, 1.0f/256.0f);\n";
	}
	else if (code == "pick")
	{
		// default code for picking
		w << "if (max(color) < 0.01f & alpha.x < 0.01f) discard();\n";
		if (this->flags & TARGET_X5Y6Z5)
			w << "output = vector4(convert_float(objectId & 0x1f) * 0.032258f, convert_float(objectId >> 5 & 0x3f) * 0.015873f, convert_float(objectId >> 11) * 0.032258f, 1.0f);\n";
		else if (this->flags & TARGET_X8Y8Z8)
			w << "output = vector4(convert_float(objectId & 0xff) * 0.0039216f, convert_float(objectId >> 8 & 0xff) * 0.0039216f, convert_float(objectId >> 16) * 0.0039216f, 1.0f);\n";
		else
			w << "output = vector4(convert_float(objectId), 0.0f, 0.0f, 1.0f);\n";
	}
	else
	{
		// user-supplied output code
		w << code << '\n';
	}

}

struct TypeIndex
{
	Pointer<StructType> type;
	int index;
	
	TypeIndex()
		: index() {}
};

bool GeneratorCollector::compileDeformers(Compiler& compiler,
	Language mainLanguage, Language deformerLanguage)
{
	bool hasError = false;
	
	// compile deformers
	foreach (DeformerInfoPair& deformerInfoPair, this->deformerInfoMap)
	{
		// deformer may be null for sprite particles with no deformer or deformer in shader.
		// the deformer jobs then only copy the pass-through extra fields
		Pointer<Deformer> deformer = deformerInfoPair.first;
		DeformerInfo& deformerInfo = deformerInfoPair.second;

		// get deformer input and output fields
		std::map<std::string, std::string> deformerInputFields;
		std::vector<std::pair<std::string, std::string> > deformerOutputFields;
		if (deformer != null)
		{
			deformer->inputType->getMembers(deformerInputFields);
			deformer->outputType->getMembers(deformerOutputFields);
		}

		// iterate over deformer jobs
		foreach (DeformerJobInfoPair& p, deformerInfo.jobInfos)
		{
			const DeformerJobIndex& deformerJobIndex = p.first;
			DeformerJobInfo& deformerJobInfo = p.second;

			// set deformer
			deformerJobInfo.deformer = deformer;
			
			// get name->type map for generative fields (from particle system or text mesh)
			std::map<std::string, std::string> generatedFields;
			typedef std::map<std::string, std::string>::iterator GeneratedFieldsIt;
			if (Pointer<ParticleSystem> particleSystem = dynamicCast<ParticleSystem>(deformerJobInfo.shape))
			{
				particleSystem->particleType->getMembers(generatedFields);				
			}
			else if (Pointer<TextMesh> textMesh = dynamicCast<TextMesh>(deformerJobInfo.shape))
			{
				// only int and float scalar and vector are supported for now
				generatedFields["position"] = "float3";
				generatedFields["symbol"] = "float2";
				
				// todo: these fields are constant
				generatedFields["normal"] = "float3";
				generatedFields["tangent"] = "float3";
				generatedFields["bitangent"] = "float3";
			}
			
			// vertex -> input -> deformer -> output -> result
			
			// create struct for vertex attributes from deformer input fields
			Pointer<StructType> vertexType = new StructType();
			foreach (const StringPair& p, deformerInputFields)
			{
				const std::string& fieldName = p.first;
				const std::string& inputFieldType = p.second;
				
				// get input type from buffer or particle system
				DeformerJobIndex::const_iterator it = deformerJobIndex.find(fieldName);
				if (it != deformerJobIndex.end())
				{
					const FieldOfObject& fieldOfObject = it->second;
					
					if (Pointer<Buffer> buffer = dynamicCast<Buffer>(fieldOfObject.object))
					{
						// is a buffer
						Pointer<BufferInfo> bufferInfo = this->bufferInfoMap[buffer];
						BufferFormat& format = bufferInfo->format;
						
						VectorInfo vertexFieldType;
						if (format.getLayoutType() == BufferFormat::STANDARD)
						{
							// standard format: use type from deformer input and number of components from buffer,
							// because compileDeformers() only supports int or float.
							vertexFieldType = VectorInfo(VectorInfo(inputFieldType).type, toVectorInfo(format).numRows);
						}
						else
						{
							// combined format: use int
							vertexFieldType = VectorInfo(VectorInfo::INT, 1);
						}						
						vertexType->addMember(fieldName, vertexFieldType.toString());
					}
					else
					{
						// not a buffer
						
						// check if generated field (using field name in vertex generator)
						GeneratedFieldsIt it = generatedFields.find(fieldOfObject.fieldName);
						if (it != generatedFields.end())
						{
							// add to input vertex type
							const std::string& vertexFieldType = it->second;
							vertexType->addMember(fieldName, vertexFieldType);
						}
					}
				}
			}

			// add pass-though extra fields to input vertex type
			foreach (const std::string& fieldName, deformerJobInfo.extraFields)
			{
				// lookup type of field in vertex generator (particle system)
				DeformerJobIndex::const_iterator it = deformerJobIndex.find(fieldName);
				if (it != deformerJobIndex.end())
				{
					const FieldOfObject& fieldOfObject = it->second;
				
					// check if generated field (using field name in vertex generator)
					GeneratedFieldsIt it = generatedFields.find(fieldOfObject.fieldName);
					if (it != generatedFields.end())
					{
						// add to input vertex type
						const std::string& fieldType = it->second;
						vertexType->addMember(fieldName, fieldType);
					}
				}
			}

			// create struct for result vertex attributes. vertex attributes can be int, uint or float
			Pointer<StructType> resultType = new StructType();
			foreach (const StringPair& p, deformerOutputFields)
			{
				const std::string& fieldName = p.first;
				const std::string& fieldType = p.second;

				// deformer needs to write to output only if a shader has requested the attribute
				if (contains(deformerJobInfo.usedOutputFields, fieldName))
					resultType->addMember(fieldName, fieldType);
			}

			// add pass-though extra fields to result type
			foreach (const std::string& fieldName, deformerJobInfo.extraFields)
			{
				// lookup type of field in vertex generator (particle system)
				DeformerJobIndex::const_iterator it = deformerJobIndex.find(fieldName);
				if (it != deformerJobIndex.end())
				{
					const FieldOfObject& fieldOfObject = it->second;
			
					// check if generated field (using field name in vertex generator)
					GeneratedFieldsIt it = generatedFields.find(fieldOfObject.fieldName);
					if (it != generatedFields.end())
					{
						// add to result type
						const std::string& fieldType = it->second;
						resultType->addMember(fieldName, fieldType);
					}
				}
			}

			// deformer code (read from input, deform, write to output)
			std::string code;
			Pointer<IODevice> dev = new StringRefDevice(code);
			{
				CodeWriter w(dev);

				// deformer uniform variable
				if (deformer != null)
					writeBindingVariable(w, "deformer", deformer->uniformType);

				// input vertex variable (gets data from the input buffers)
				writeBindingVariable(w, "vertex", vertexType);
				
				// result variable (gets data from deformer output and extra fields)
				writeBindingVariable(w, "result", resultType);

				w.writeLine();
				
				// deformer vertex processing function
				w << "extern \"C\" void main()\n";
				w.beginScope();

				if (deformer != null)
				{
					// deformer input variable (gets data from input vertex)
					deformer->inputType->writeVariable(w, "input");

					// deformer output variable (gets calculated by deformer)
					deformer->outputType->writeVariable(w, "output");
					
					// copy fields from vertex attributes into deformer input
					foreach (const StringPair& p, deformerInputFields)
					{
						const std::string& fieldName = p.first;
						const std::string& inputFieldType = p.second;
						
						// get input type from buffer or particle system
						DeformerJobIndex::const_iterator it = deformerJobIndex.find(p.first);
						if (it != deformerJobIndex.end())
						{
							const FieldOfObject& fieldOfObject = it->second;
							
							w << "input." << fieldName;
							
							if (Pointer<Buffer> buffer = dynamicCast<Buffer>(fieldOfObject.object))
							{
								// is a buffer
								Pointer<BufferInfo> bufferInfo = this->bufferInfoMap[buffer];

								// assign from vertex to input (e.g. "input.position = vertex.position")
								printDeformerAssign(w, "vertex." + fieldName, bufferInfo->format,
									bufferInfo->scale, bufferInfo->offset, ShaderType(inputFieldType));
							}
							else
							{
								// not a buffer
								
								// check if generated field (using field name in vertex generator)
								GeneratedFieldsIt it = generatedFields.find(fieldOfObject.fieldName);
								if (it != generatedFields.end())
								{
									const std::string& srcFieldType = it->second;
									BufferFormat srcFormat = toBufferFormat(VectorInfo(srcFieldType));
									
									// assign from vertex to input (e.g. "input.position = vertex.position")
									printDeformerAssign(w, "vertex." + fieldName, srcFormat,
										splat4(1.0f), splat4(0.0f), ShaderType(inputFieldType));
								}
							}
						}				
					}

					// deformer code
					w << deformer->code;

					// copy vertex fields from deformer output into output element
					foreach (const StringPair& p, deformerOutputFields)
					{
						const std::string& fieldName = p.first;

						// deformer needs to write to output only if a shader has requested the attribute
						if (contains(deformerJobInfo.usedOutputFields, fieldName))
							w << "result." << fieldName << " = output." << fieldName << ";\n";
					}
				}
					
				// copy pass-though extra fields from input vertex into result
				foreach (const std::string& fieldName, deformerJobInfo.extraFields)
				{
					w << "result." << fieldName << " = vertex." << fieldName << ";\n";
				}

				w.endScope();
			}

			// handle languages where structs are splitted into arrays of int, float and objects (e.g. javascript)
			int3 deformerOffset = {};
			if (this->uniformsPrinter != null && deformer != null)
			{
				deformerOffset = this->uniformsPrinter->getOffset('.' + deformer->path);
			}

			// compile deformer
			hasError |= deformerJobInfo.hasError = !compileDeformer(
				compiler, code,
				deformerOffset,
				deformerJobInfo.uniformCode,
				deformerJobInfo.vertexCode,
				deformerJobInfo.uniformVariables,
				deformerJobInfo.vertexBindings,
				deformerJobInfo.resultBindings,
				mainLanguage,
				deformerLanguage);

			if (deformerJobInfo.hasError)
			{
				dError("Deformer '" << deformer->name << "' failed to compile");
			}
			else
			{			
				// set output type which contains only elements used by shaders (creates field infos and calculates vertex size)
				deformerJobInfo.setOutputType(resultType, this->deformerAlignMode);

				foreach (DeformerJobInfo::VertexBindingPair& p, deformerJobInfo.vertexBindings)
				{
					// get object/field from deformer job index
					DeformerJobIndex::const_iterator it = deformerJobIndex.find(p.first);
					if (it != deformerJobIndex.end())
					{
						const FieldOfObject& fieldOfObject = it->second;
					
						if (Pointer<Buffer> buffer = dynamicCast<Buffer>(fieldOfObject.object))
						{
							// is a buffer

							// tell buffer that it is used by a deformer
							this->bufferInfoMap[buffer]->scopes[SCOPE_DEFORMER].used = true;
						}					
					}
				}
			}

			#ifndef NDEBUG
			{
				Pointer<IODevice> file = File::create("deformerUniform.cpp");
				file->write(deformerJobInfo.uniformCode.c_str(), deformerJobInfo.uniformCode.length());
				file->close();
			}
			{
				Pointer<IODevice> file = File::create("deformerVertex.cpp");
				file->write(deformerJobInfo.vertexCode.c_str(), deformerJobInfo.vertexCode.length());
				file->close();
			}
			#endif
		}
	}

	return hasError;
}

bool GeneratorCollector::compileParticleSystems(Compiler& compiler, Language mainLanguage)
{
	bool hasError = false;
	
	// compile particle systems
	foreach (ShapeInfoPair& p, this->shapeInfoMap)
	{
		if (Pointer<ParticleSystem> particleSystem = dynamicCast<ParticleSystem>(p.first))
		{
			Pointer<ParticleSystemInfo> particleSystemInfo = staticCast<ParticleSystemInfo>(p.second);
		
			// particle code
			std::string code;
			Pointer<IODevice> dev = new StringRefDevice(code);
			{
				CodeWriter w(dev);

				// binding variables
				if (particleSystem->emitterUniformType != null)
					writeBindingVariable(w, "emitter", particleSystem->emitterUniformType);
				writeBindingVariable(w, "uniform", particleSystem->uniformType);
				writeBindingVariable(w, "particle", particleSystem->particleType);
				writeBindingVariable(w, "index", "int");
				writeBindingVariable(w, "id", "int");
				writeBindingVariable(w, "seed", "int");
				writeBindingVariable(w, "alive", "bool");

				w.writeLine();
				
				// particle create function
				w << "extern \"C\" void create()\n";
				w.beginScope();
				w.beginScope();
				w << particleSystem->emitterCode;
				w.endScope();
				w.beginScope();
				w << particleSystem->createCode;
				w.endScope();
				w.endScope();

				// particle update function
				w << "extern \"C\" void update()\n";
				w.beginScope();
				w << particleSystem->updateCode;
				w.endScope();
			}

			// handle languages where structs are splitted into arrays of int, float and objects (e.g. javascript)
			int3 emitterOffset = int3();
			int3 uniformOffset = int3();
			if (this->uniformsPrinter != null)
			{
				if (particleSystem->emitterUniformType != null)
					emitterOffset = this->uniformsPrinter->getOffset(".e." + particleSystem->path);
				uniformOffset = this->uniformsPrinter->getOffset('.' + particleSystem->path);
			}

			// compile particle system
			hasError |= particleSystemInfo->hasError = !compileParticleSystem(
				compiler, code,
				emitterOffset,
				uniformOffset,
				particleSystemInfo->particlePrinter,
				particleSystemInfo->createUniformCode,
				particleSystemInfo->createParticleCode,
				particleSystemInfo->updateUniformCode,
				particleSystemInfo->updateParticleCode,
				this->sceneUseFlags,
				mainLanguage);
		
			if (particleSystemInfo->hasError)
			{
				dError("Particle system '" << particleSystem->name << "' failed to compile");
			}
		}
	}
	return hasError;
}


// write buffers

struct VertexBuffers
{
	Pointer<VertexBuffer> vertexBuffers[NUM_SCOPES];
};

// goup of shapes that use a group of vertex buffers
struct Group
{
	std::set<Pointer<ConstantMesh> > shapes;
	std::set<Pointer<VertexBuffer> > vertexBufferSets[NUM_SCOPES];
	std::vector<Pointer<IndexBuffer> > indexBuffers;
};

// less operator to compare fat vertices
struct VertexLess
{
	std::vector<uint8_t>& vertices;
	size_t vertexSize;
	
	VertexLess(std::vector<uint8_t>& vertices, int vertexSize) : vertices(vertices), vertexSize(vertexSize) {}
	
	bool operator ()(int a, int b)
	{
		uint8_t* av = &this->vertices[a * this->vertexSize];
		uint8_t* bv = &this->vertices[b * this->vertexSize];
		return memcmp(av, bv, this->vertexSize) < 0;
	}
};

struct Batch
{	
	// index remap access -> optimized
	std::vector<int> mapIndexAccess2Optimized;


	// index remap batch -> uniform
	std::vector<int> mapIndexBatch2Uniform;
};			

int64_t GeneratorCollector::writeBuffers(DataWriter& d, Pointer<BufferConverter> bufferConverter)
{
	int64_t dataOffset = d.getPosition();

	bool compress = (this->dataMode & SceneOptions::COMPRESS) != 0;
	DataConverter::Mode dstMode = compress ? DataConverter::NATIVE :
		(d.isBigEndian() ? DataConverter::BE : DataConverter::LE);

	DlzssCompressor c(d);
	//LocoStreamCompressor c(d);

	// write named buffers
	foreach (NamedBufferInfo& info, this->namedBufferInfos)
	{
		BufferFormat srcFormat = info.namedBuffer->buffer->getFormat();
		BufferFormat dstFormat = info.format;
		size_t elementCount = info.namedBuffer->buffer->getNumElements();
		
		// convert data
	/*
		size_t size = info.namedBuffer->buffer->getMemorySize();
		std::vector<uint8_t> data(size);
		uint8_t* dstData = data.data();
		bufferConverter->convert(
			srcFormat, info.namedBuffer->buffer->getData<void>(),
			dstFormat, dstMode, dstData, dstFormat.getMemorySize(),
			elementCount);

		// write data
		d.align(dstFormat.getComponentSize());
		d.writeData(dstData, size);
	*/
		const char* code =
			"struct Global\n"
			"{\n"
			"  float scale;\n"
			"  float offset;\n"
			"};\n"
			"float4 main(float4 value, Global& global)\n"
			"{\n"
			"  return value * global.scale + global.offset;\n"
			"}\n";
		
		// inverse scale/offset
		struct
		{
			float scale;
			float offset;
		} global;
		global.scale = 1.0f / std::max(info.scale, 1e-20f);
		global.offset = -info.offset * global.scale;
		int componentSize = dstFormat.getComponentSize();
		std::vector<uint8_t> data(elementCount * componentSize);
		uint8_t* dstData = data.data();
		bufferConverter->convert(
			srcFormat, info.namedBuffer->buffer->getData<void>(),
			code, &global,
			dstFormat, dstMode, dstData, dstFormat.getMemorySize(),
			elementCount);
		
		if (!compress)
		{
			// write data
			d.align(dstFormat.getComponentSize());
			d.writeData(dstData, data.size());
		}
		else
		{
			// compress data
			if (componentSize == 1)
				c.compress(data);
			else if (componentSize == 2)
				c.compress((uint16_t*)dstData, elementCount);
			else if (componentSize == 4)
				c.compress((uint32_t*)dstData, elementCount);
		}
	}


	// phase 1
	// create vertex buffers and shape groups

	// create groups of shapes that have fields in common. each bin also contains all vertex buffers
	std::map<Pointer<Shape>, Group*> groupMap;
	std::vector<Group> groups(this->shapeInstancerInfoMap.size());
	
	// assign groups for all geometry shapes
	{
		int i = 0;
		foreach (ShapeInstancerInfoPair& p, this->shapeInstancerInfoMap)
		{
			if (Pointer<ConstantMesh> constantMesh = dynamicCast<ConstantMesh>(p.first->shape))
			{
				Group* group = &groups[i++];
				
				// add shape (constant mesh)
				group->shapes.insert(constantMesh);
				
				// assign to map
				groupMap[constantMesh] = group;
			}
		}
	}
	
	// put all buffers that are used by the same set of shapes into one vertex buffer.
	// example: position and normal are shard by two shapes, but each has its own uv set.
	// result will be three vertex buffers: position/normal, uv, uv.
	// also join all groups into one group whose shapes and vertex buffers are connected.
	std::map<std::set<Pointer<Shape> >, VertexBuffers> vertexBufferMap;
	foreach (BufferInfoPair& p, this->bufferInfoMap)
	{
		// get field buffer and buffer info
		Pointer<Buffer> buffer = p.first;
		Pointer<BufferInfo> bufferInfo = p.second;
		
		// lookup vertex buffers (one for each scope) by shape set (set of shapes that use the field buffer)
		VertexBuffers& vertexBuffers = vertexBufferMap[bufferInfo->shapes];
		
		// iterate over scopes (deformer, shader)
		for (int si = 0; si < NUM_SCOPES; ++si)
		{
			BufferInfo::Scope& scope = bufferInfo->scopes[si];
			
			// check if vertex buffer is used by scope (deformer or shader)
			if (scope.used)
			{
				// get/create vertex buffer where we put the field buffer into
				Pointer<VertexBuffer>& vertexBuffer = vertexBuffers.vertexBuffers[si];
				if (vertexBuffer == null)
					vertexBuffer = new VertexBuffer();
				
				// store vertex buffer and field index into buffer info
				scope.vertexBuffer = vertexBuffer;
				scope.fieldIndex = int(vertexBuffer->fields.size());
				
				// determine index group
				//int indexGroup = (bufferInfo->semanticFlags & 1 << BufferVertexField::INDEX) ? 0 : -1;
				
				// add field to vertex buffer
				vertexBuffer->fields += VertexBuffer::Field(buffer,
					bufferInfo->format, bufferInfo->scale, bufferInfo->offset);//, indexGroup);
			
				// get group for first shape that uses the buffer
				std::set<Pointer<Shape> >::iterator it = bufferInfo->shapes.begin();
				std::set<Pointer<Shape> >::iterator end = bufferInfo->shapes.end();
				Group* firstGroup = groupMap[*it];
				
				// iterate over groups for second to last shape and combine with first group
				++it;
				for (;it != end; ++it)
				{
					Group* group = groupMap[*it];
					
					// check if this is another group
					if (group != firstGroup)
					{
						// combine groups
						
						// add shapes from other group
						firstGroup->shapes.insert(group->shapes.begin(), group->shapes.end());
						group->shapes.clear();

						// add vertex buffers from other group
						firstGroup->vertexBufferSets[si].insert(group->vertexBufferSets[si].begin(), group->vertexBufferSets[si].end());
						group->vertexBufferSets[si].clear();
						
						// other shape now also points to the same group
						groupMap[*it] = firstGroup;
					}
				}
					
				// add vertex buffer
				firstGroup->vertexBufferSets[si].insert(vertexBuffer);
			}
		}
	}
	
	

	// phase 2
	// create fat vertex buffers for each group from previous phase
	foreach (Group& group, groups)
	{
		// step 1
		// layout fat vertex buffers
		
		// calc field offsets and vertex sizes for vertex buffers
		size_t numVertices = std::numeric_limits<size_t>::max();
		int fatVertexOffset = 0;
		for (int si = 0; si < NUM_SCOPES; ++si)
		{
			std::set<Pointer<VertexBuffer> >& vertexBuffers = group.vertexBufferSets[si];
			Type::AlignMode alignMode = si == SCOPE_DEFORMER ? this->deformerAlignMode : this->shaderAlignMode;
			
			bool compress = (this->dataMode & SceneOptions::COMPRESS) != 0;
			bool useFloat = (this->dataMode & SceneOptions::USE_FLOAT) != 0 && si == SCOPE_SHADER;
			
			// iterate over vertex buffers of group (e.g. position/normal, uv, uv)
			foreach (Pointer<VertexBuffer> vertexBuffer, vertexBuffers)
			{
				// determine alignment of fields
				foreach (VertexBuffer::Field& field, vertexBuffer->fields)
				{
					BufferFormat& format = field.targetFormat;
					
					if (format.is32Bit() || useFloat)
					{
						// convert to float
						format = BufferFormat(BufferFormat::X32, format.getNumComponents(), BufferFormat::FLOAT);
					}
					else if (alignMode == Type::EXTEND_TO_4)
					{
						// extend to 4 byte
						if (format.layout == BufferFormat::X8 || format.layout == BufferFormat::XY8 || format.layout == BufferFormat::XYZ8)
							format.layout = BufferFormat::XYZW8;
						if (format.layout == BufferFormat::X16)
							format.layout = BufferFormat::XY16;
						if (format.layout == BufferFormat::XYZ16)
							format.layout = BufferFormat::XYZW16;
					}
					
					// data format (in file) is the same as target format (in memory) if no compression
					if (!compress)
						field.dataFormat = format;

					// determine number of vertices
					numVertices = min(numVertices, field.buffer->getNumElements());
				}

				// sort fields by alignment and layout
				sort(vertexBuffer->fields, VertexBuffer::FieldGreater());

				// iterate over sorted fields and determine field offsets and vertex size
				int dataOffset = 0;
				int dataMaxAlign = 1;
				int targetOffset = 0;
				int targetMaxAlign = 1;
				int fieldIndex = 0;
				foreach (VertexBuffer::Field& field, vertexBuffer->fields)
				{
					// handle target format
					BufferFormat targetFormat = field.targetFormat;

					// target calc align
					int targetAlign = 1;
					switch (alignMode)
					{
					case Type::NO_ALIGN:
						targetAlign = 1;
						break;
					case Type::COMPONENT_ALIGN:
						targetAlign = targetFormat.getComponentSize();
						break;
					case Type::VECTOR_ALIGN:
						targetAlign = upToPowerOfTwo(targetFormat.getMemorySize());
						break;
					case Type::EXTEND_TO_4:
						targetAlign = 4;
						break;
					}
										
					// get maximum align and align offset
					targetMaxAlign = max(targetMaxAlign, targetAlign);
					targetOffset += -targetOffset & (targetAlign - 1);
					
					// set offset of field in vertex element
					field.targetOffset = targetOffset;
					
					// increment offset
					targetOffset += targetFormat.getMemorySize();


					// handle data format
					BufferFormat dataFormat = field.dataFormat;

					// calc data align
					int dataAlign = compress ? dataFormat.getComponentSize() : targetAlign;
					
					// get maximum align and align offset
					dataMaxAlign = max(dataMaxAlign, dataAlign);
					dataOffset += -dataOffset & (dataAlign - 1);
					
					// set offset of field in vertex element
					field.dataOffset = dataOffset;
					
					// increment offset
					dataOffset += dataFormat.getMemorySize();


					// fix field index after sorting fields
					this->bufferInfoMap[field.buffer]->scopes[si].fieldIndex = fieldIndex++;
				}
/*
				// iterate over sorted fields and determine field offsets and vertex size
				int offset = 0;
				int maxAlign = 1;
				int fieldIndex = 0;
				foreach (VertexBuffer::Field& field, vertexBuffer->fields)
				{
					// special handling for uniform indices (bone indices)
					BufferFormat format = field.format;
					
					// get size of field in bytes
					int fieldSize = format.getMemorySize();
					
					// get maximum align and align offset
					int align = field.align;
					maxAlign = max(maxAlign, align);
					offset += -offset & (align - 1);			
					
					// set offset of field in vertex element
					field.fieldOffset = offset;
					
					// increment offset
					offset += fieldSize;
					
					// fix field index after sorting fields
					this->bufferInfoMap[field.buffer]->scopes[si].fieldIndex = fieldIndex++;
				}
*/
				// align vertex size
				int dataVertexSize = dataOffset + (-dataOffset & (dataMaxAlign - 1));
				int targetVertexSize = targetOffset + (-targetOffset & (targetMaxAlign - 1));
				
				// set vertex size to vertex buffer
				vertexBuffer->dataVertexSize = dataVertexSize;
				vertexBuffer->targetVertexSize = targetVertexSize;
				
				// set maximum alignment to vertex buffer
				vertexBuffer->dataAlign = dataMaxAlign;
				
				// set offset of vertex buffer element in fat vertex element
				vertexBuffer->fatVertexOffset = fatVertexOffset;
				fatVertexOffset += dataVertexSize;
			}
		}
		int fatVertexSize = fatVertexOffset;
		
		// check if there is any vertex data (empty groups exist)
		if (fatVertexSize > 0)
		{
			// step 2
			// create fat vertex buffer
			
			// allocate memory for fat vertex buffer
			size_t size = numVertices * fatVertexSize;
			std::vector<uint8_t> fatVertices(size);

			// copy/convert fields from field buffers into the fat vertex buffer using the destination format and offset/scale
			for (int si = 0; si < NUM_SCOPES; ++si)
			{
				std::set<Pointer<VertexBuffer> >& vertexBuffers = group.vertexBufferSets[si];
			
				// iterate over vertex buffers
				foreach (Pointer<VertexBuffer> vertexBuffer, vertexBuffers)
				{
					// iterate over fields
					foreach (VertexBuffer::Field& field, vertexBuffer->fields)
					{
						//todo: use component copy if compression is on. fatVertexOffset must be component aligned
						DataConverter::Mode fatMode = d.isBigEndian() ? DataConverter::BE : DataConverter::LE;
					
						// copy/convert field buffer into fat vertex buffer
						vertexBuffer->copyField(bufferConverter, field,
							fatMode, fatVertices.data(), fatVertexSize, numVertices);
					}
				}
			}
			

			// step 3
			// optimize fat vertex buffer (remove duplicates and create index buffer)

			// init mapping from sorted vertices to original vertices
			std::vector<int> sortedVertices(numVertices);
			for (size_t i = 0; i < numVertices; ++i)
				sortedVertices[i] = i;
				
			// sort fat vertex indices by fat vertex data to find duplicates
			VertexLess l(fatVertices, fatVertexSize);
			sort(sortedVertices, l);

			// mapping from new to old vertex index
			std::vector<int> mapIndexOptimized2Fat;		

			// mapping from old to new vertex index
			std::vector<int> mapIndexFat2Optimized(numVertices);
			
			// take the first fat vertex and create index
			int vertexIndex = sortedVertices[0];
			mapIndexOptimized2Fat += vertexIndex;
			int newVertexIndex = 0;
			mapIndexFat2Optimized[vertexIndex] = newVertexIndex;
			
			// now create an index for each unique fat vertex
			for (size_t i = 1; i < numVertices; ++i)
			{
				vertexIndex = sortedVertices[i];

				// check if last fat vertex is less than current fat vertex
				if (l(sortedVertices[i - 1], vertexIndex))
				{
					// yes: add this fat vertex
					mapIndexOptimized2Fat += vertexIndex;
					++newVertexIndex;
				}
			
				mapIndexFat2Optimized[vertexIndex] = newVertexIndex;
			}
			size_t numOptimizedVertices = mapIndexOptimized2Fat.size();
			

			// step 4
			// optimize order of primitives (triangles) for vertex cache.
			// then reorder vertices into the order in which they are accessed
			
			// get all primitives for the shapes in the group and create index buffers
			std::map<Pointer<ConstantMesh>, Pointer<IndexBuffer> > indexBuffers;
			foreach (Pointer<ConstantMesh> constantMesh, group.shapes)
			{
				Pointer<IndexBuffer>& indexBuffer = indexBuffers[constantMesh];
				if (indexBuffer == null)
				{
					indexBuffer = new IndexBuffer();
					group.indexBuffers += indexBuffer;
				}
			}

			// set maximum number of vertices that can be addressed by an index buffer
			int maxNumVertices = std::numeric_limits<int>::max();
			int overlap = 0;
			if (this->flags & NO_INDEX_32_BIT)
			{	
				maxNumVertices = 65536;
				overlap = 256;
			}
			
			
			//std::list<Batch> batches;
//bool hasBatching = false;
//int numUniforms = 64;

			// index orders
			// fat: the original fat vertex indices in constantMesh->indices
			// optimized: duplicate vertices are removed
			// access: vertices are in the order in which they get first accessed. If the vertex index is limited
			//   e.g. to 65536 then a vertex may appear more than once in the access order.

			// index remap optimized -> access
			std::vector<int> mapIndexOptimized2Access(numOptimizedVertices);

			// index remap uniform -> batch
			//std::vector<int> mapIndexUniform2Batch(numUniforms);
			
			std::vector<int> mapIndexAccess2Optimized;
			
			// iterate over primitives that are used by the shapes in the group
			typedef std::pair<const Pointer<ConstantMesh>, Pointer<IndexBuffer> > IndexBufferPair;
			foreach (IndexBufferPair& p, indexBuffers)
			{
				Pointer<ConstantMesh> constantMesh = p.first;
				Pointer<IndexBuffer> indexBuffer = p.second;
				
				int numIndices = int(constantMesh->indices.size());

				// remap vertex indices fat -> optimized
				std::vector<int> indices(numIndices);
				for (int i = 0; i < numIndices; ++i)
				{
					indices[i] = mapIndexFat2Optimized[constantMesh->indices[i]];
				}

// special code for triangles
				// calculate acmr for existing order and 16 entry cache
				//float acmr1 = MeshUtility::calcACMR(indices, 16);

				// optimize indices for vertex cache
				MeshUtility::optimizeVertexCache(numOptimizedVertices, indices, indices);

				// calculate acmr for optimized order
				//float acmr2 = MeshUtility::calcACMR(indices, 16);
				//dNotify("optimized ACMR for 16-entry FIFO vertex cache: " << acmr1 << " -> " << acmr2);	

				// loop for creating uniform (bone) batches
				std::vector<int>::iterator batchBegin = indices.begin();
				std::vector<int>::iterator end = batchBegin + 3 * (indices.size() / 3);
				//std::list<Batch>::iterator batch = batches.begin();
				//while (end != batchBegin)
				{				
					// create new batch if necessary
					//if (batch == batches.end())
					//	batch = batches.insert(batches.end(), Batch());
	
					// get vertex index remap access -> optimized
					//std::vector<int> mapIndexAccess2Optimized = batch->mapIndexAccess2Optimized;

					// get uniform index remap batch -> uniform
					//std::vector<int> mapIndexBatch2Uniform = batch->mapIndexBatch2Uniform;
					
					// create mapping uniform -> batch from batch -> uniform
					//fill(mapIndexUniform2Batch, -1);
					//size_t numUniforms = mapIndexBatch2Uniform.size();
					//for (size_t i = 0; i < numUniforms; ++i)
					//{
					//	mapIndexUniform2Batch[mapIndexBatch2Uniform[i]] = i;
					//}
					
					// divide indices into sections so that maximum vertex index (e.g. 65535) is not exceeded
					// also handle bone batching so that bone index does not exceed maximum (e.g. 63)

					// lowest usable vertex index in access order. gets incremented if more than one section is needed
					int baseIndex = 0;
					
					// index to where mapIndexOptimized2Access is set
					int endIndex = 0;
					
					// start with all indices set to -1 to indicate the vertices have not been accessed yet
					fill(mapIndexOptimized2Access, -1);
					
					// track minimum/maximum used index in a section in access order. if no triangle fits
					// then minIndex is used as new baseIndex
					int minIndex = maxNumVertices - overlap;
					int maxIndex = 0;						

					// loop for creating index sections
					std::vector<int>::iterator begin = batchBegin;
					std::vector<int>::iterator out = begin;
					while (end != begin)
					{
						// set remap optimized -> access for all vertices that are accessible for current base index.
						// mapIndexAccess2Optimized may already contain many vertices from other shapes (e.g. multi
						// material mesh)
						int newEndIndex = min(baseIndex + maxNumVertices, int(mapIndexAccess2Optimized.size()));
						for (int i = endIndex; i < newEndIndex; ++i)
						{
							mapIndexOptimized2Access[mapIndexAccess2Optimized[i]] = i;					
						}
						endIndex = newEndIndex;

						// iterate over triangles, get access order of vertices and remap indices to access order
						bool allVerticesFit = true;
						for (std::vector<int>::iterator it = out; it != end; it += 3)
						{
							// get optimized indices
							int optimized0 = it[0];
							int optimized1 = it[1];
							int optimized2 = it[2];
						
							// get access indices
							int& access0 = mapIndexOptimized2Access[optimized0];
							int& access1 = mapIndexOptimized2Access[optimized1];
							int& access2 = mapIndexOptimized2Access[optimized2];
							
							// calc how many new vertices are needed. vertices that were accessed
							// not yet or earlier than current base index have to be added
							int numVertices = int(mapIndexAccess2Optimized.size());
							int numNewVertices = (access0 < baseIndex ? 1 : 0)
								+ (access1 < baseIndex ? 1 : 0)
								+ (access2 < baseIndex ? 1 : 0);
							
							// determine if new vertices fit into section
							bool verticesFit = numNewVertices == 0
								|| numVertices + numNewVertices <= baseIndex + maxNumVertices;
							allVerticesFit &= verticesFit;
							
							// calc how many uniform (bone) indices are needed
							//int numUniforms = 0;
							//int numNewUnifors = 0;
							//bool uniformsFit = true;
							//if (hasBatching)
							//{
							//	numUniforms = int(mapIndexBatch2Uniform.size());
							//}
							
							// allocate new vertex indices if they fit into limit
							if (verticesFit)// && uniformsFit)
							{
								if (access0 < baseIndex)
								{
									// set optimized -> access
									access0 = numVertices++;
									
									// add to access -> optimized
									mapIndexAccess2Optimized += optimized0;
									
									// update index to where mapIndexOptimized2Access is set
									++endIndex;
								}
								if (access1 < baseIndex)
								{
									access1 = numVertices++;
									mapIndexAccess2Optimized += optimized1;
									++endIndex;
								}
								if (access2 < baseIndex)
								{
									access2 = numVertices; // no ++ needed as numVertices is recalculated
									mapIndexAccess2Optimized += optimized2;
									++endIndex;
								}
							
								// allocate new uniform (bone) indices if they fit into limit							
								//if (hasBatching)
								//{
								//	numNewUnifors = 0;
								//}

								// determine range of addressed vertices
								minIndex = min(minIndex, access0);
								maxIndex = max(maxIndex, access0);						
								minIndex = min(minIndex, access1);
								maxIndex = max(maxIndex, access1);						
								minIndex = min(minIndex, access2);
								maxIndex = max(maxIndex, access2);						

								// move triangle that didn't make it from out iterator to current position
								// this effectively swaps the triangle indices at it and out
								it[0] = out[0];
								it[1] = out[1];
								it[2] = out[2];

								// output indices in access order
								out[0] = access0;
								out[1] = access1;
								out[2] = access2;
								out += 3;
							}
						}

						if (minIndex > baseIndex)
						{						
							// actual minimum index is greater than base index: increase base index and do again so that
							// the first indexed vertex in the index buffer section has index 0. this way some more
							// triangles can be written to out iterator.
							baseIndex = minIndex;
						}
						else
						{
							// calc number of vertices and indices
							int vertexCount = maxIndex - minIndex + 1;
							int indexCount = out - begin;

							// check if any indices are in the section. may not be the case if mapIndexAccess2Optimized
							// contained already many vertices from previous shapes
							if (indexCount > 0)
							{
								// subtract base index from all indices
								for (std::vector<int>::iterator it = begin; it != out; ++it)
								{
									*it -= baseIndex;
								}
								
								// create index buffer section
								IndexBuffer::Section& section = add(indexBuffer->sections);

								// store offset of first indexed vertex number of indices
								section.baseIndex = baseIndex;
								section.indexCount = indexCount;

								// determine target format
								BufferFormat dstFormat;
								if ((this->flags & NO_INDEX_8_BIT) == 0 && vertexCount <= 256)
								{
									section.type = IndexBuffer::INDEX_8_BIT;
									dstFormat = BufferFormat(BufferFormat::X8, BufferFormat::UINT);
								}
								else if (vertexCount <= 65536)
								{
									section.type = IndexBuffer::INDEX_16_BIT;
									dstFormat = BufferFormat(BufferFormat::X16, BufferFormat::UINT);
								}
								else
								{
									section.type = IndexBuffer::INDEX_32_BIT;
									dstFormat = BufferFormat(BufferFormat::X32, BufferFormat::UINT);
								}
								
								// convert indices to target format
								size_t elementSize = dstFormat.getMemorySize();
								section.data.resize(indexCount * elementSize);
								bufferConverter->convert(
									BufferFormat(BufferFormat::X32, BufferFormat::UINT), &*begin,
									dstFormat, dstMode, section.data.data(), elementSize,
									indexCount);
								
								// set begin to where the unprocessed indices start
								begin = out;
							}

							// increase base index
							baseIndex += maxNumVertices - overlap;

							// reset min index
							minIndex = baseIndex + maxNumVertices - overlap;
						
							// finished if all vertices would match but maybe batch is full 
							if (allVerticesFit)
								break;
						}
					}
					batchBegin = out;
				}
			}
			//std::vector<int>& mapIndexAccess2Optimized = batches.begin()->mapIndexAccess2Optimized;
			int numAccessVertices = int(mapIndexAccess2Optimized.size());

			// set number of vertices and index buffer to shape info
			foreach (Pointer<ConstantMesh> constantMesh, group.shapes)
			{
				Pointer<ConstantMeshInfo> constantMeshInfo = new ConstantMeshInfo();
				this->shapeInfoMap[constantMesh] = constantMeshInfo;
				constantMeshInfo->numVertices = numAccessVertices;
				constantMeshInfo->indexBuffer = indexBuffers[constantMesh];
			}


			// step 5
			// copy data from fat vertex buffer into deformer and shader input vertex buffers
			for (int si = 0; si < NUM_SCOPES; ++si)
			{
				std::set<Pointer<VertexBuffer> >& vertexBuffers = group.vertexBufferSets[si];
			
				// iterate over vertex buffers
				foreach (Pointer<VertexBuffer> vertexBuffer, vertexBuffers)
				{
					// allocate memory for vertex data
					vertexBuffer->numVertices = numAccessVertices;
					vertexBuffer->data.resize(numAccessVertices * vertexBuffer->dataVertexSize);

					std::vector<uint8_t>::iterator srcBegin = fatVertices.begin() + vertexBuffer->fatVertexOffset;
					std::vector<uint8_t>::iterator dstIt = vertexBuffer->data.begin();
					int vertexSize = vertexBuffer->dataVertexSize;
					for (int i = 0; i < numAccessVertices; ++i)
					{
						int optimized = mapIndexAccess2Optimized[i];
					
						// get iterator to beginning of vertex in fat vertex buffer
						std::vector<uint8_t>::iterator srcIt = srcBegin + mapIndexOptimized2Fat[optimized] * fatVertexSize;

						// copy vertex
						std::copy(srcIt, srcIt + vertexSize, dstIt);
						
						dstIt += vertexSize;
					}
				}
			}
		}
	}


	// phase 3
	// write data of vertex buffers to file
	
	// write static derformer and shader input vertex buffers
	for (int si = 0; si < NUM_SCOPES; ++si)
	{
		// step 1: sort vertex buffers by vertex format (layout, type for all fields)
		std::map<std::string, std::vector<Pointer<VertexBuffer> > > vertexBuffersMap;
		typedef std::pair<const std::string, std::vector<Pointer<VertexBuffer> > > VertexBuffersPair;
		foreach (Group& group, groups)
		{
			std::set<Pointer<VertexBuffer> >& vertexBuffers = group.vertexBufferSets[si];
		
			// iterate over vertex buffers
			foreach (Pointer<VertexBuffer> vertexBuffer, vertexBuffers)
			{
				// add if vertex buffer is not empty
				if (vertexBuffer->numVertices > 0)
					vertexBuffersMap[vertexBuffer->getFormatString()] += vertexBuffer;
			}
		}

		// step 2: write big buffers consisting of vertex buffers of same foramt
		std::vector<BigVertexBuffer>& bigVertexBuffers = this->bigVertexBuffers[si];
		if (!compress)
		{
			// iterate over groups of different output formats
			foreach (VertexBuffersPair& p, vertexBuffersMap)
			{
				std::vector<Pointer<VertexBuffer> >& vertexBuffers = p.second;
				
				// get first vertex buffer
				Pointer<VertexBuffer> vertexBuffer = vertexBuffers.front();
				
				// align and get offset of big vertex buffer in data file
				int64_t dataOffset = d.align(vertexBuffer->dataAlign);
				
				// iterate over all buffers with same output format			
				size_t bigBufferOffset = 0;
				size_t vertexCount = 0;
				foreach (Pointer<VertexBuffer> vertexBuffer, vertexBuffers)
				{
					// set index of big vertex buffer that contains this buffer
					vertexBuffer->bigBufferIndex = int(bigVertexBuffers.size());

					// set offset of this buffer in big vertex buffer
					vertexBuffer->bigBufferOffset = bigBufferOffset;//size_t(d.align(vertexBuffer->dataAlign) - offset);
					bigBufferOffset += vertexBuffer->numVertices * vertexBuffer->dataVertexSize;
					vertexCount += vertexBuffer->numVertices;
					
					// write data (is already in target endian format)
					d.writeData(vertexBuffer->data.data(), vertexBuffer->data.size());
					
					// vertex data not needed any more
					std::vector<uint8_t> tmp;
					vertexBuffer->data.swap(tmp);
				}
				
				// add big vertex buffer (input for deformer or shader)
				bigVertexBuffers += BigVertexBuffer(vertexBuffer, vertexCount);
			}
		}
		else
		{
			// vertex buffer compression:
			// 1. reorder into temp buffer by component (first all x components, then all y components etc.)
			// 2. pack with diferential lzss
			// vertex buffer decompression:
			// 1. depack into temp buffer
			// 2. reorder into interleaved big buffer
			// 3. optional: convert to float, rescale per vertex buffer (IE 11)

			// store offset of compressed vertex data in data file for all big vertex buffers
			//bigVertexBuffers.offset = d.getPosition();
			
			// iterate over groups of different output formats
			foreach (VertexBuffersPair& p, vertexBuffersMap)
			{
				std::vector<Pointer<VertexBuffer> >& vertexBuffers = p.second;
														
				// set big buffer index/offset and determine number of vertices in big buffer
				size_t bigBufferOffset = 0;
				size_t numVertices = 0;
				foreach (Pointer<VertexBuffer> vertexBuffer, vertexBuffers)
				{
					// set index of big vertex buffer that contains this buffer
					vertexBuffer->bigBufferIndex = int(bigVertexBuffers.size());

					// set offset of this buffer in big vertex buffer
					vertexBuffer->bigBufferOffset = bigBufferOffset;
					bigBufferOffset += vertexBuffer->numVertices * vertexBuffer->targetVertexSize;
					
					numVertices += vertexBuffer->numVertices;
				}

				// get first vertex buffer
				Pointer<VertexBuffer> vertexBuffer = vertexBuffers.front();

				// add big vertex buffer (input for deformer or shader)
				bigVertexBuffers += BigVertexBuffer(vertexBuffer, numVertices);
				
				// determine sizes (using fields of first vertex buffer, all have same format)
				size_t sizes[5];
				sizes[1] = sizes[2] = sizes[4] = 0;
				foreach (const VertexBuffer::Field& field, vertexBuffer->fields)
				{
					int numComponents = field.dataFormat.getNumComponents();
					int componentSize = field.dataFormat.getComponentSize();
					
					// calc size of temp buffers
					sizes[componentSize] += numVertices * numComponents;
				}
				
				// temp buffers for compression
				std::vector<uint8_t> b1(sizes[1]);
				std::vector<uint16_t> b2(sizes[2]);
				std::vector<uint32_t> b4(sizes[4]);
				std::vector<uint8_t>::iterator it1 = b1.begin();
				std::vector<uint16_t>::iterator it2 = b2.begin();
				std::vector<uint32_t>::iterator it4 = b4.begin();
										
				// iterate over fields and copy data of all vertex buffers into temp buffers
				foreach (const VertexBuffer::Field& field, vertexBuffer->fields)
				{
					int numComponents = field.dataFormat.getNumComponents();
					int componentSize = field.dataFormat.getComponentSize();
				
					// iterate over components
					for (int componentIndex = 0; componentIndex < numComponents; ++componentIndex)
					{
						// iterate over all vertex buffers
						foreach (Pointer<VertexBuffer> vertexBuffer, vertexBuffers)
						{
							int vertexSize = vertexBuffer->dataVertexSize;
							size_t numVertices = vertexBuffer->numVertices;
							
							// copy component of current field
							uint8_t* src = vertexBuffer->data.data() + field.dataOffset + componentIndex * componentSize;
							if (componentSize == 1)
							{
								for (int i = 0; i < numVertices; ++i, src += vertexSize, ++it1)
									*it1 = *src;
							}
							else if (componentSize == 2)
							{
								for (int i = 0; i < numVertices; ++i, src += vertexSize, ++it2)
									*it2 = *(uint16_t*)src;
							}
							else if (componentSize == 4)
							{
								for (int i = 0; i < numVertices; ++i, src += vertexSize, ++it4)
									*it4 = *(uint32_t*)src;
							}
						}
					}
				}
				
				c.compress(b1);
				c.compress(b2);
				c.compress(b4);

				foreach (Pointer<VertexBuffer> vertexBuffer, vertexBuffers)
				{
					// vertex data not needed any more
					std::vector<uint8_t> tmp;
					vertexBuffer->data.swap(tmp);
				}
			}
		}
	}

	// write index buffers to file
	{
		// step 1: sort index buffer sections by index type
		std::map<IndexBuffer::Type, std::vector<IndexBuffer::Section*> > indexBufferMap;
		foreach (Group& group, groups)
		{
			// iterate over index buffers
			foreach (Pointer<IndexBuffer> indexBuffer, group.indexBuffers)
			{
				// iterate over sections
				foreach (IndexBuffer::Section& section, indexBuffer->sections)
				{
					// add if index buffer section is not empty
					if (section.indexCount > 0)
						indexBufferMap[section.type] += &section;
				}
			}
		}

		// step 2: write big buffers consisting of index buffers of same foramt
		typedef std::pair<const IndexBuffer::Type, std::vector<IndexBuffer::Section*> > IndexBufferPair;
		static const int sizes[] = {1, 2, 4};
		if (!compress)
		{
			// iterate over groups with different output formats
			foreach (IndexBufferPair& p, indexBufferMap)
			{
				int indexSize = sizes[p.first];
				
				// offset of index buffer in global data
				int64_t dataOffset = d.align(indexSize);
				
				// iterate over all buffers with same output format			
				size_t bigBufferOffset = 0;
				size_t indexCount = 0;
				foreach (IndexBuffer::Section* s, p.second)
				{
					IndexBuffer::Section& section = *s;
				
					// set index of big index buffer that contains this buffer
					section.bigBufferIndex = int(this->bigIndexBuffers.size());

					// set offset of this buffer in big index buffer
					section.bigBufferOffset = bigBufferOffset;//d.align(section.getAlign()) - dataOffset;
					bigBufferOffset += section.indexCount * indexSize;
					indexCount += section.indexCount;
					
					// write data (is already in target endian format)
					d.writeData(section.data.data(), section.data.size());
					
					// index data not needed any more
					std::vector<uint8_t> tmp;
					section.data.swap(tmp);
				}
				
				// add big index buffer
				this->bigIndexBuffers += BigIndexBuffer(indexSize, indexCount);
			}
		}
		else
		{
			// iterate over groups with different output formats
			foreach (IndexBufferPair& p, indexBufferMap)
			{
				int indexSize = sizes[p.first];
			
				// set big buffer index/offset and determine number of indices in big buffer
				size_t bigBufferOffset = 0;
				size_t indexCount = 0;
				foreach (IndexBuffer::Section* s, p.second)
				{
					IndexBuffer::Section& section = *s;
				
					// set index of big index buffer that contains this buffer
					section.bigBufferIndex = int(this->bigIndexBuffers.size());

					// set offset of this buffer in big index buffer
					section.bigBufferOffset = bigBufferOffset;
					bigBufferOffset += section.indexCount * indexSize;
					indexCount += section.indexCount;
				}
			
				// collect all indices
				std::vector<uint8_t> data(indexCount * indexSize);
				uint8_t* dst = data.data();
				size_t triangleCount = indexCount / 3;
				foreach (IndexBuffer::Section* s, p.second)
				{
					IndexBuffer::Section& section = *s;
					
					//data.insert(data.end(), section.data.begin(), section.data.end());
					
					int sectionTriangleCount = section.indexCount / 3;
					if (indexSize == 1)
						setTriangle((int8_t*)section.data.data(), (int8_t*)dst, triangleCount, sectionTriangleCount);
					else if (indexSize == 2)
						setTriangle((int16_t*)section.data.data(), (int16_t*)dst, triangleCount, sectionTriangleCount);
					else if (indexSize == 4)
						setTriangle((int32_t*)section.data.data(), (int32_t*)dst, triangleCount, sectionTriangleCount);
						
					dst += sectionTriangleCount * indexSize;

					// index data not needed any more
					std::vector<uint8_t> tmp;
					section.data.swap(tmp);
				}
				if (indexSize == 1)
					c.compress(data);
				else if (indexSize == 2)
					c.compress((uint16_t*)data.data(), indexCount);
				else if (indexSize == 4)
					c.compress((uint32_t*)data.data(), indexCount);
				
				// add big index buffer
				this->bigIndexBuffers += BigIndexBuffer(indexSize, indexCount);
			}
		}
	}


	// phase 4
	// build dynamic buffers for deformer output	
	{	
		// step 1: sort deformers by output vertex format
		
		// collect formats
		typedef std::vector<DeformerJobInfoMap::iterator> DynamicSources;
		std::map<std::string, DynamicSources> formats;

		// iterate over deformer infos (deformers which are not in shaders or the null deformer for particle systems)
		foreach (DeformerInfoPair& p, this->deformerInfoMap)
		{
			DeformerInfo& deformerInfo = p.second;
			
			// iterate over deformer job infos
			for (DeformerJobInfoMap::iterator it = deformerInfo.jobInfos.begin(); it != deformerInfo.jobInfos.end(); ++it)
			{
				DeformerJobInfo& deformerJobInfo = it->second;
				if (!deformerJobInfo.hasError)
					formats[deformerJobInfo.getFormatString()] += it;
			}
		}

		// allocate output buffer infos: one per vertex format
		this->dynamicBufferInfos.resize(formats.size());

		// step 2: build dynamic buffers
		typedef std::pair<const std::string, DynamicSources> FormatsPair;
		int dynamicBufferIndex = 0;
		foreach (FormatsPair& p, formats)
		{
			DynamicSources& dynamicSources = p.second;
			DynamicBufferInfo& dynamicBufferInfo = this->dynamicBufferInfos[dynamicBufferIndex];
			
			// maximum size of a buffer in this dynamic buffer
			size_t maxBufferSize = 0;

			// offset in dynamic buffer
			size_t dynamicBufferOffset = 0;
			
			// iterate over all deformers with same output vertex format
			foreach (DeformerJobInfoMap::iterator it, dynamicSources)
			{
				DeformerJobInfo& deformerJobInfo = it->second;
				
				// add deformer job info iterator to dynamic buffer info
				dynamicBufferInfo.deformerJobIterators += it;
				
				// get number of vertices to reserve in output dynamic buffer
				int numVertices = this->shapeInfoMap[deformerJobInfo.shape]->getNumVertices();
				size_t dynamicBufferSize = numVertices * deformerJobInfo.vertexSize;
				
				// set output dynamic buffer index, offset and size
				deformerJobInfo.dynamicBufferIndex = dynamicBufferIndex;
				deformerJobInfo.dynamicBufferOffset = dynamicBufferOffset;
				deformerJobInfo.dynamicBufferSize = dynamicBufferSize;

				// increment by size of output buffer of this buffer/deformer
				dynamicBufferOffset += dynamicBufferSize;
				
				maxBufferSize = max(maxBufferSize, dynamicBufferSize);
			}

			// set size of deformer output big buffer
			dynamicBufferInfo.size = dynamicBufferOffset;
			
			// set maximum size of a buffer in this dynamic buffer
			dynamicBufferInfo.maxBufferSize = maxBufferSize;
			
			++dynamicBufferIndex;
		}
	}
	/*
	// write buffers for text meshes (strings, indices, symbols)
	foreach (TextMeshInfoPair& p, this->textMeshInfos)
	{
		Pointer<TextMesh> textMesh = p.first;
		TextMeshInfo& textMeshInfo = p.second;
		
		// build symbol data
		std::string strings;
		std::vector<int> indices;
		std::vector<float> symbols;
		foreach (TextMesh::SymbolPair& p, textMesh->symbols)
		{
			TextMesh::Rect& rect = p.second;
			
			indices += int(strings.size());
			
			strings += p.first;
			strings += '\0';
			
			symbols += rect.x,
				rect.y,
				rect.w,
				rect.h;
		}
		
		// write symbol data
		textMeshInfo.indicesOffset = d.align(2);
		d.write<ushort>(indices.begin(), indices.size());
		textMeshInfo.stringsOffset = d.align(1);
		d.write<uint8_t>(strings.begin(), strings.size());
		textMeshInfo.symbolsOffset = d.align(4);
		d.write<float>(symbols.begin(), symbols.size());
	}*/

	if (compress)
		c.flush();
		
	return dataOffset;
}

bool GeneratorCollector::isInLayer(int layerIndex, const NamedInstance& instance)
{
	Pointer<Layer> layer = this->layers[layerIndex];
	
	if (layer == null)
		return false;
	
	// layer with empty name is default layer
	return layer->name.empty() || contains(instance.layers, layer);
}

int GeneratorCollector::buildRenderInfos()
{
	NameGenerator nameGenerator;
	
	// iterate over all render commands, i.e. shape instancers times layers
	int numVertexArrays = 0;
	foreach (ShapeInstancerInfoPair& p, this->shapeInstancerInfoMap)
	{
		Pointer<ShapeInstancer> shapeInstancer = p.first;
		ShapeInstancerInfo& shapeInstancerInfo = p.second;

		// create name for render function
		//shapeInstancerInfo.renderFunctionName = nameGenerator.get();

		// get shape info
		Pointer<ShapeInfo> shapeInfo = this->shapeInfoMap[shapeInstancer->shape];

		// set deformer if not in shader
		shapeInstancerInfo.deformer = !this->deformersInShaders ? shapeInstancer->deformer : null;
			
		// get shader and info
		Pointer<Shader> shader = shapeInstancer->shader;
		ShaderInfo& shaderInfo = this->shaderInfoMap[shader];

		// get info for deformer/shader combo
		DeformerShaderInfo& deformerShaderInfo = shaderInfo.deformerShaderInfos
			[this->deformersInShaders ? shapeInstancer->deformer : null];
/*
		// get target shader info for shape type (MESH or SPRITES)
		TargetShaderInfo& targetShaderInfo = deformerShaderInfo[shapeInstancerInfo.shapeType];
		

//! do per layer only for the used scale/offsets
		// create scale/offset array for packed vertex shader inputs
		int scaleOffsetArrayIndex = -1;
		int scaleOffsetArraySize = 0;
		if (targetShaderInfo.scaleOffsetType != null)
		{
			std::vector<float> scaleOffsetArray;
		
			// get members (e.g. "position.s", "position.o")
			std::vector<std::pair<std::string, std::string> > members;
			targetShaderInfo.scaleOffsetType->getMembers(members);
			
			// iterate over members
			foreach (const StringPair& member, members)
			{
				// old: write transform variable
				//targetShaderInfo.scaleOffsetType->writeVariable(w, "transform");
				
				// extract field name and scale or offset flag from member name
				// (e.g. "position.s": fieldName = "position", isScale = true)
				std::string fieldName = member.first.substr(0, member.first.size() - 2);
				bool isScale = *member.first.rbegin() == 's';

				// get scale/offset value
				float4 value;
				if (Pointer<BufferVertexField> field = dynamicCast<BufferVertexField>(shapeInstancer->fields[fieldName]))
				{
					Pointer<BufferInfo> bufferInfo = this->bufferInfoMap[field->buffer];
					
					// select scale or offset
					value = isScale ? bufferInfo->scale : bufferInfo->offset;
				}
				else
				{
					// default: no scale or offset
					value = isScale ? splat4(1.0f) : splat4(0.0f);
				}
				
				// get number of vector components from type (e.g. 3 from "float3")
				int numComponents = VectorInfo(member.second).numRows;

				// old: assign to transform member
				//w << "transform." << member.first << " = " << varVector(value, numComponents, true) << ";\n";
			
				// align scale/offset array
				int align = numComponents == 3 ? 4 : numComponents;
				scaleOffsetArray.resize((scaleOffsetArray.size() + align - 1) & ~(align - 1));
				
				// add value to scale/offset array
				scaleOffsetArray.insert(scaleOffsetArray.end(), &value.x, &value.x + numComponents);
			}
			
			// align scale/offset array to 4
			scaleOffsetArray.resize((scaleOffsetArray.size() + 3) & ~3);
			
			int index = int(this->scaleOffsetArrayMap.size());
			scaleOffsetArrayIndex = this->scaleOffsetArrayMap.insert(std::make_pair(scaleOffsetArray, index)).first->second;
			scaleOffsetArraySize = int(scaleOffsetArray.size() >> 2);
		}
*/
		// iterate over render infos
		RenderInfoMap::iterator it = shapeInstancerInfo.renderInfoMap.begin();
		while (it != shapeInstancerInfo.renderInfoMap.end())
		{
			RenderInfoMap::iterator next = it;
			++next;
			int layerIndex = it->first;
			RenderInfo& renderInfo = it->second;

			// get target shader info
			TargetShaderInfo& targetShaderInfo = deformerShaderInfo[ShapeTypeAndMode(shapeInstancerInfo.shapeType/*, renderInfo.renderMode*/)];

			// check if layer info is still there
			if (contains(targetShaderInfo.layerInfoMap, layerIndex))
			{
				// set vertex array (object) index
				renderInfo.vertexArrayIndex = numVertexArrays;

				// add number of index buffer sections
				numVertexArrays += shapeInfo->getNumVertexArrays();


//! do per layer only for the used scale/offsets
				// get layer info
				Pointer<LayerInfo> layerInfo = renderInfo.layerInfo;

				// create scale/offset array for packed vertex shader inputs
				int scaleOffsetArrayIndex = -1;
				int scaleOffsetArraySize = 0;
				if (layerInfo->scaleOffsetType != null)
				{
					std::vector<float> scaleOffsetArray;
				
					// get members (e.g. "position.s", "position.o")
					std::vector<std::pair<std::string, std::string> > members;
					layerInfo->scaleOffsetType->getMembers(members);
					
					// iterate over members
					foreach (const StringPair& member, members)
					{
						// old: write transform variable
						//targetShaderInfo.scaleOffsetType->writeVariable(w, "transform");
						
						// extract field name and scale or offset flag from member name
						// (e.g. "position.s": fieldName = "position", isScale = true)
						std::string fieldName = member.first.substr(0, member.first.size() - 2);
						bool isScale = *member.first.rbegin() == 's';

						// get scale/offset value
						float4 value;
						if (Pointer<BufferVertexField> field = dynamicCast<BufferVertexField>(shapeInstancer->fields[fieldName]))
						{
							Pointer<BufferInfo> bufferInfo = this->bufferInfoMap[field->buffer];
							
							// select scale or offset
							value = isScale ? bufferInfo->scale : bufferInfo->offset;
						}
						else
						{
							// default: no scale or offset
							value = isScale ? splat4(1.0f) : splat4(0.0f);
						}
						
						// get number of vector components from type (e.g. 3 from "float3")
						int numComponents = VectorInfo(member.second).numRows;

						// old: assign to transform member
						//w << "transform." << member.first << " = " << varVector(value, numComponents, true) << ";\n";
						/*
						for (int i = 0; i < numRows; ++i)
						{
							// convert value to string
							char buf[20];
							char* end = toString(buf, value[i], 6);
						
							// set offset or scale value by string-replacing ftransform[] by value
							std::string ftransform = arg("ftransform[%0]", index);
							replaceString(transformCode, ftransform, StringRef(buf, end - buf));
							++index;
						}
						*/
						
						// align scale/offset array
						int align = numComponents == 3 ? 4 : numComponents;
						scaleOffsetArray.resize((scaleOffsetArray.size() + align - 1) & ~(align - 1));
						
						// add value to scale/offset array
						scaleOffsetArray.insert(scaleOffsetArray.end(), &value.x, &value.x + numComponents);
					}
					
					// align scale/offset array to 4
					scaleOffsetArray.resize((scaleOffsetArray.size() + 3) & ~3);
					
					int index = int(this->scaleOffsetArrayMap.size());
					scaleOffsetArrayIndex = this->scaleOffsetArrayMap.insert(std::make_pair(scaleOffsetArray, index)).first->second;
					scaleOffsetArraySize = int(scaleOffsetArray.size() >> 2);
				}
				
				renderInfo.scaleOffsetArrayIndex = scaleOffsetArrayIndex;
				renderInfo.scaleOffsetArraySize = scaleOffsetArraySize;
				
				renderInfo.drawFunctionName = nameGenerator.get();
													
				++it;
			}
			else
			{
				// layer info was removed due to compile error: erase render info
				shapeInstancerInfo.renderInfoMap.erase(it);
			}
			it = next;
		}
	}
	return numVertexArrays;
}

int GeneratorCollector::getMaxNumQuads()
{
	uint maxNumQuads = 0;

	// text meshes
	foreach (ShapeInfoPair& p, this->shapeInfoMap)
	{
		if (Pointer<TextMesh> textMesh = dynamicCast<TextMesh>(p.first))
			maxNumQuads = std::max(maxNumQuads, textMesh->maxNumSymbols);
	}
	return maxNumQuads;
}

} // namespace digi
