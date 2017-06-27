#include <digi/Utility/Ascii.h>
#include <digi/Utility/foreach.h>
#include <digi/Utility/lexicalCast.h>
#include <digi/Utility/SetUtility.h>
#include <digi/Utility/VectorUtility.h>
#include <digi/System/Log.h>
#include <digi/System/MemoryDevices.h>
#include <digi/Math/All.h>
#include <digi/CodeGenerator/CodeWriter.h>
#include <digi/CodeGenerator/CodeWriterFunctions.h>
#include <digi/CodeGenerator/NameGenerator.h>

#include "BufferInfo.h"
#include "PrintHelper.h"
#include "CompileHelper.h"
#include "GeneratorCollector.h"
#include "compileShader.h"
#include "compileScene.h"
#include "generateSceneOpenGL.h"


namespace digi {

namespace
{
	// default number of enabled vertex attributes
	const int defaultNumVertexAttributes = 2;

// dynamic buffers

	// process deformer jobs of dynamic buffers (deformers, sprite particles)
	void processDynamicBuffers(CodeWriter& w, GeneratorCollector& collector, bool mapBuffer)
	{
		// iterate over dynamic buffers
		int index = 0;
		foreach (DynamicBufferInfo& dynamicBufferInfo, collector.dynamicBufferInfos)
		{
			w.beginScope();
				
			// bind output buffer
			w << "glBindBuffer(GL_ARRAY_BUFFER, instance.dynamicBuffers[" << index << "]);\n";
				
			if (mapBuffer)
				w << "ubyte* outputData = (ubyte*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);\n";
			w.writeLine();

			// iterate over deformer jobs
			foreach (DeformerJobInfoMap::iterator it, dynamicBufferInfo.deformerJobIterators)
			{
				const DeformerJobIndex& deformerJobIndex = it->first;
				DeformerJobInfo& deformerJobInfo = it->second;
				Pointer<Deformer> deformer = deformerJobInfo.deformer;
				Pointer<Shape> shape = deformerJobInfo.shape;
				
				if (deformer != null)
					w << "// deformer '" << deformer->name << "'\n";
				else
					w << "// copy\n";
				w.beginScope();

				// uniform from scene
				if (deformer != null)
				{
					w << "Deformer" << collector.deformerInfoMap[deformer].name
						<< "& deformer = instance.uniforms." << deformer->path << ";\n";

					// print uniform variables for transfer to vertex processing loop
					printShaderVariables(w, deformerJobInfo.uniformVariables);

					// execute uniform code
					w << deformerJobInfo.uniformCode;
				}

				// offset/size in dynamic buffer
				size_t bufferOffset = deformerJobInfo.dynamicBufferOffset;
				Pointer<ShapeInfo> shapeInfo = collector.shapeInfoMap[shape];
				std::string numVertices;
				std::string bufferSize;
				std::string nextGeneratedVertex;
				if (Pointer<ConstantMesh> constantMesh = dynamicCast<ConstantMesh>(shape))
				{
					// is a constant mesh
					//ConstantMeshInfo& constantMeshInfo = collector.constantMeshInfos[constantMesh];
					int nv = shapeInfo->getNumVertices();
					numVertices = Code() << nv;
					bufferSize = Code() << nv * deformerJobInfo.vertexSize;
				}
				else if (Pointer<TextMesh> textMesh = dynamicCast<TextMesh>(shape))
				{
					// is a text mesh
					//TextMeshInfo& textMeshInfo = collector.textMeshInfos[textMesh];
					StringRef name = shapeInfo->getName();
					w << "int numVertices = instance." << name << ".numSymbols * 4;\n";
					numVertices = "numVertices";
					bufferSize = Code() << "numVertices * " << deformerJobInfo.vertexSize;

					// iterator for vertices
					w << "TextVertex" /*<< textMeshInfo.name*/ << "* _g = "
						"instance." << name << ".vertices;\n";
					
					nextGeneratedVertex = "++_g;\n";
				}
				else if (Pointer<ParticleSystem> particleSystem = dynamicCast<ParticleSystem>(shape))
				{
					// is a particle system
					//ParticleSystemInfo& particleSystemInfo = collector.particleSystemInfos[particleSystem];
					StringRef name = shapeInfo->getName();
					w << "int numParticles = instance." << name << ".numParticles;\n";
					numVertices = "numParticles";
					bufferSize = Code() << "numParticles * " << deformerJobInfo.vertexSize;

					// iterator for alive particles
					w << "Particle" << name << "* _g = "
						"instance." << name << ".alive;\n";
					
					nextGeneratedVertex = "_g = _g->next;\n";
				}

				// input buffers
				typedef std::pair<const Pointer<VertexBuffer>, size_t> VertexBufferPair;
				std::map<Pointer<VertexBuffer>, size_t> vertexBuffers;
								
				// iterate over input vertex bindings
				//bool hasParticleSystem = false;
				foreach (DeformerJobInfo::VertexBindingPair& p, deformerJobInfo.vertexBindings)
				{
					DeformerJobIndex::const_iterator it = deformerJobIndex.find(p.first);
					if (it != deformerJobIndex.end())
					{
						const FieldOfObject& fieldOfObject = it->second;
						
						// get field by name and check if it is a BufferVertexField
						if (Pointer<Buffer> buffer = dynamicCast<Buffer>(fieldOfObject.object))
						{
							// is a buffer
								
							// get the (fat) vertex buffer that the buffer is part of
							Pointer<VertexBuffer> vertexBuffer = collector.bufferInfoMap[buffer]->scopes[SCOPE_DEFORMER].vertexBuffer;
								
							// try to insert vertex buffer with new index
							size_t index = vertexBuffers.size();
							if (vertexBuffers.insert(std::make_pair(vertexBuffer, index)).second)
							{
								// new vertex buffer: print pointer variable
								w << "ubyte* vertex" << ident(index) << " = global.deformerInputBuffers"
									"[" << vertexBuffer->bigBufferIndex << "] + " << vertexBuffer->bigBufferOffset << ";\n";
							}
						}
					}
				}

				// result vertex buffer
				if (mapBuffer)
					w << "ubyte* result = outputData + " << bufferOffset << ";\n";
				else
					w << "ubyte* result = instance.tempBuffer;\n";					

				// vertex loop (i_ does not clash with auto-generated variable names such as _i and i)
				w << "for (int i_ = 0; i_ < " << numVertices << "; ++i_)\n";
				w.beginScope();

				// get vertex code so that replacements can be made
				std::string vertexCode = deformerJobInfo.vertexCode;

				// get input vertex and copy into deformer input
				// iterate over input vertex bindings
				foreach (DeformerJobInfo::VertexBindingPair& p, deformerJobInfo.vertexBindings)
				{
					// get object/field from deformer job index
					DeformerJobIndex::const_iterator it = deformerJobIndex.find(p.first);
					if (it != deformerJobIndex.end())
					{
						const FieldOfObject& fieldOfObject = it->second;
						ShaderVariable& var = p.second;
						
						if (Pointer<Buffer> buffer = dynamicCast<Buffer>(fieldOfObject.object))
						{
							// is a buffer
								
							// get buffer info
							Pointer<BufferInfo> bufferInfo = collector.bufferInfoMap[buffer];
							//BufferFormat& format = bufferInfo->format;
							BufferInfo::Scope& scope = bufferInfo->scopes[SCOPE_DEFORMER];
								
							// get vertex buffer index
							size_t index = vertexBuffers[scope.vertexBuffer];
								
							// get field offset
							int fieldOffset = scope.vertexBuffer->fields[scope.fieldIndex].targetOffset;
								
							// source type. convert to VectorInfo so that combined formats are handled as one basic
							// type (e.g. X4Y4Z4W4 -> ushort). depacking of combined formats is done in compileDeformers()
							VectorInfo srcType = toVectorInfo(bufferInfo->format);
							
							// source value
							std::string srcTypeString = srcType.toString();
							if (srcType.numRows == 3)
								srcTypeString = "packed_" + srcTypeString;
							std::string srcValue = Code() << "*(" << srcTypeString << "*)(vertex" << ident(index)
								<< " + " << fieldOffset << ")";
								
							// print deformer variable (e.g. "float3 _0"). can only be int or float scalar or vector.
							printShaderVariable(w, var);

							// convert to deformer variable type if necessary and assign to deformer variable
							printAssign(w, srcValue, toBufferFormat(srcType), var.type);
							w << ";\n";
						}
						else
						{
							// not a buffer: generated field (e.g. field of particle)

							// the deformer variable (e.g. "_0")
							printShaderVariable(w, var);
								
							// get value from generated field
							w << " = _g->" << fieldOfObject.fieldName << ";\n";
						}						
					}
				}
									
				// replace result variables
				foreach (DeformerJobInfo::FieldInfo& fieldInfo, deformerJobInfo.fieldInfos)
				{
					std::string srcValue = deformerJobInfo.resultBindings[fieldInfo.name].name;
					VectorInfo dstType = toVectorInfo(fieldInfo.format);

					//w << "*(" << dstType.toString() << "*)(result + " << fieldInfo.offset << ") = " << srcValue << ";\n";

					std::string dstTypeString = dstType.toString();
					if (dstType.numRows == 3)
						dstTypeString = "packed_" + dstTypeString;
						
					std::string dstValue = Code() << "*(" << dstTypeString << "*)(result + " << fieldInfo.offset << ")";
						
					replaceString(vertexCode, srcValue, dstValue);
				}

				// execute per-vertex code
				w << vertexCode;
					
				w.writeLine();
					
				// next input vertex for all buffers
				foreach (VertexBufferPair& p, vertexBuffers)
				{
					size_t index = p.second;
					w << "vertex" << ident(index) << " += " << p.first->targetVertexSize << ";\n";
				}
				
				// next input vertex for generated particles/vertices
				w << nextGeneratedVertex;
				
				// next result vertex
				w << "result += " << deformerJobInfo.vertexSize << ";\n";

				// end loop
				w.endScope();			
					
				// set output buffer
				if (!mapBuffer)
					w << "glBufferSubData(GL_ARRAY_BUFFER, " << bufferOffset << ", " << bufferSize << ", instance.tempBuffer);\n";

				// end deformer
				w.endScope();
			}

			if (mapBuffer)
				w << "glUnmapBuffer(GL_ARRAY_BUFFER);\n";			
			w.endScope();
				
			// increment dynamic buffer index
			++index;
		}			
	}

// vertex state helpers

	struct GLType
	{
		// number of components (2nd parameter of glVertexAttribPointer)
		int numComponents;

		// component type (3rd parameter of glVertexAttribPointer)
		const char* componentType;
		
		// normalize int types (4th parameter of glVertexAttribPointer)
		bool normalize;

		GLType()
			: numComponents(0), componentType(NULL), normalize(false) {}
		GLType(int numComponents, const char* componentType, bool normalize)
			: numComponents(numComponents), componentType(componentType), normalize(normalize) {}
	};

	// convert buffer format to gl type (needed for glVertexAttribPointer)
	GLType getType(BufferFormat format)
	{
		VectorInfo type = toVectorInfo(format);
		bool normalize = format.type == BufferFormat::NORM || format.type == BufferFormat::UNORM;

		switch (type.type)
		{
		case VectorInfo::BYTE:
			return GLType(type.numRows, "GL_BYTE", normalize);
		case VectorInfo::UBYTE:
			return GLType(type.numRows, "GL_UNSIGNED_BYTE", normalize);
		case VectorInfo::SHORT:
			return GLType(type.numRows, "GL_SHORT", normalize);
		case VectorInfo::USHORT:
			return GLType(type.numRows, "GL_UNSIGNED_SHORT", normalize);
		case VectorInfo::INT:
			return GLType(type.numRows, "GL_INT", normalize);
		case VectorInfo::UINT:
			return GLType(type.numRows, "GL_UNSIGNED_INT", normalize);
		case VectorInfo::FLOAT:
			return GLType(type.numRows, "GL_FLOAT", normalize);
		default:
			return GLType();
		}
	}

	void setVertexAttribPointer(CodeWriter& w, int index, const BufferFormat& format, int vertexSize, size_t offset)
	{
		// get opengl type from field format
		GLType type = getType(format);
		
		// set pointer to field in buffer
		w << "glVertexAttribPointer("
			<< index << ", "
			<< type.numComponents << ", "
			<< type.componentType << ", "
			<< (type.normalize ? "true" : "false") << ", "
			<< vertexSize << ", "
			<< "(GLvoid*)" << offset << ");\n";
	}

	int enableVertexAttributes(CodeWriter& w, int lastNumVertexAttributes, int numVertexAttributes)
	{
		for (int i = lastNumVertexAttributes - 1; i >= numVertexAttributes; --i)
		{
			w << "glDisableVertexAttribArray(" << i << ");\n";
		}
		for (int i = lastNumVertexAttributes; i < numVertexAttributes; ++i)
		{
			w << "glEnableVertexAttribArray(" << i << ");\n";
		}
		return numVertexAttributes;
	}

	int enableVertexAttributes(CodeWriter& w, int lastNumVertexAttributes, Pointer<LayerInfo> layerInfo)
	{
		return enableVertexAttributes(w, lastNumVertexAttributes, int(layerInfo->vertexBindings.size()));
	}

	// set vertex state for mesh and sprites
	void setVertexState(CodeWriter& w, GeneratorCollector& collector,
		Pointer<ShapeInstancer> shapeInstancer, ShapeInstancerInfo& shapeInstancerInfo, int baseIndex,
		Pointer<LayerInfo> layerInfo,
		int& lastVertexBigBufferIndex)
	{
		// bind deformer output vertex buffer (shape deformer is only used if not in shaders)
		std::set<std::string> deformerOutputFields;
		DeformerInfoMap::iterator deformerInfoIt = collector.deformerInfoMap.find(shapeInstancerInfo.deformer);
		if (deformerInfoIt != collector.deformerInfoMap.end())
		{
			// deformer found (deformer may be null if all fields are only copied e.g. from particle system)
			DeformerInfo& deformerInfo = deformerInfoIt->second;
			DeformerJobInfoMap::iterator deformerJobIt = deformerInfo.jobInfos.find(shapeInstancerInfo.deformerJobIndex);
			if (deformerJobIt != deformerInfo.jobInfos.end())
			{
				DeformerJobInfo& deformerJobInfo = deformerJobIt->second;
									
				// iterate over output vertex fields of deformer
				bool first = true;
				foreach (const DeformerJobInfo::FieldInfo& fieldInfo, deformerJobInfo.fieldInfos)
				{
					// check if the shader uses the field
					std::map<std::string, ShaderVariable>::iterator it = layerInfo->vertexBindings.find(fieldInfo.name);
					if (it != layerInfo->vertexBindings.end())
					{
						// add to deformer output fields, prevents binding of shader input vertex buffer
						deformerOutputFields.insert(fieldInfo.name);

						// bind buffer before first vertex attribute is set
						if (first)
						{
							first = false;

							// bind vertex buffer if not already bound
							if (deformerJobInfo.dynamicBufferIndex + 0x1000000 != lastVertexBigBufferIndex)
							{
								lastVertexBigBufferIndex = deformerJobInfo.dynamicBufferIndex + 0x1000000;
								w << "glBindBuffer(GL_ARRAY_BUFFER, instance.dynamicBuffers["
									<< deformerJobInfo.dynamicBufferIndex << "]);\n";
							}
						}
											
						// get index from name. name has format "_#", e.g. "_0".
						int vertexAttributeIndex = lexicalCast<int>(it->second.name.substr(1));
							
						// set vertex field
						setVertexAttribPointer(w, vertexAttributeIndex, fieldInfo.format, deformerJobInfo.vertexSize,
							deformerJobInfo.dynamicBufferOffset + deformerJobInfo.vertexSize * baseIndex
								+ fieldInfo.offset);
					}
				}
			}
		}

		// bind shader input vertex buffers (only constant meshes can have shader input vertex buffers)
		{
			// step 1
			// collect for each buffer the field index and vertex attribute index
			std::map<int, std::map<Pointer<VertexBuffer>, std::vector<int2> > > vertexBuffers;
			foreach (LayerInfo::VertexBindingPair& p, layerInfo->vertexBindings)
			{
				const std::string& fieldName = p.first;
				if (!contains(deformerOutputFields, fieldName))
				{
					// get buffer info
					Pointer<VertexField> field = shapeInstancer->fields[fieldName];
					Pointer<BufferInfo> bufferInfo = collector.bufferInfoMap[staticCast<BufferVertexField>(field)->buffer];
					BufferInfo::Scope& scope = bufferInfo->scopes[SCOPE_SHADER];
											
					// get difference to last vertex big buffer index and encode by absolute value
					int diff = scope.vertexBuffer->bigBufferIndex - lastVertexBigBufferIndex;
					int code = diff >= 0 ? diff * 2 : -diff * 2 - 1;
											
					// store field index and vertex attribute index for vertex buffer
					int fieldIndex = scope.fieldIndex;
					int vertexAttributeIndex = lexicalCast<int>(p.second.name.substr(1)); // strip off '_' from name (e.g. "_0")
					vertexBuffers[code][scope.vertexBuffer] += vector2(fieldIndex, vertexAttributeIndex);
				}
			}
									
			// step 2
			// generate binding code sorted by vertex buffer
			typedef std::pair<const int, std::map<Pointer<VertexBuffer>, std::vector<int2> > > VertexBufferPair1;
			typedef std::pair<const Pointer<VertexBuffer>, std::vector<int2> > VertexBufferPair2;
			foreach (VertexBufferPair1& p, vertexBuffers)
			{
				foreach (VertexBufferPair2& q, p.second)
				{
					Pointer<VertexBuffer> vertexBuffer = q.first;
										
					// bind vertex buffer if not already bound
					if (vertexBuffer->bigBufferIndex != lastVertexBigBufferIndex)
					{
						lastVertexBigBufferIndex = vertexBuffer->bigBufferIndex;
						w << "glBindBuffer(GL_ARRAY_BUFFER, global.shaderInputBuffers["
							<< vertexBuffer->bigBufferIndex << "]);\n";
					}

					// set vertex fields
					const std::vector<int2>& indices = q.second;
					foreach (int2 fieldAndAttributeIndex, indices)
					{
						VertexBuffer::Field& field = vertexBuffer->fields[fieldAndAttributeIndex.x];
						int vertexAttributeIndex = fieldAndAttributeIndex.y;
		
						// set vertex field
						int vertexSize = vertexBuffer->targetVertexSize;
						setVertexAttribPointer(w, vertexAttributeIndex, field.targetFormat, vertexSize,
							vertexBuffer->bigBufferOffset + vertexSize * baseIndex + field.targetOffset);
					}
				}
			}
		}
	}
/*
	// set vertex state for sprites
	void setVertexState(CodeWriter& w, GeneratorCollector& collector, Pointer<Deformer> deformer, Pointer<LayerInfo> layerInfo, ShapeInfo& shapeInfo)
	{
		// bind deformer output vertex buffer. 
		{
			DeformerInfo& deformerInfo = collector.deformerInfos[deformer];
			DeformerJobInfo& deformerJobInfo = deformerInfo.jobInfos[shapeInfo.deformerJobIndex];

			// bind buffer before first vertex attribute is set
			w << "glBindBuffer(GL_ARRAY_BUFFER, instance.dynamicBuffers[" <<
				deformerJobInfo.dynamicBufferIndex << "]);\n";
								
			// iterate over output vertex fields of particle shape
			foreach (const DeformerJobInfo::FieldInfo& fieldInfo, deformerJobInfo.fieldInfos)
			{
				// check if the shader uses the field
				std::map<std::string, ShaderVariable>::iterator it = layerInfo->vertexBindings.find(fieldInfo.name);
				if (it != layerInfo->vertexBindings.end())
				{
					// get index from name. name has format "_#", e.g. "_0".
					int vertexAttributeIndex = lexicalCast<int>(it->second.name.substr(1));
									
					setVertexAttribPointer(w, vertexAttributeIndex, fieldInfo.format, deformerJobInfo.vertexSize,
						deformerJobInfo.dynamicBufferOffset + fieldInfo.offset);
				}
			}
		}
								
		// no shader input vertex buffers
	}
*/

// shader helpers

	void createHandles(CodeWriter& w, const std::vector<ShaderVariable>& variables)
	{
		foreach (const ShaderVariable& variable, variables)
		{
			if (variable.type.numElements == 0)
			{
				if (variable.type.type == ShaderType::TEXTURE)
				{
					// don't create handles for samplers
					continue;
				}
			}		
			
			w << "GLuint " << variable.name << ";\n";
		}
	}

	void getHandles(CodeWriter& w, const std::vector<ShaderVariable>& variables, int& textureIndex)
	{
		foreach (const ShaderVariable& variable, variables)
		{
			if (variable.type.numElements == 0)
			{
				if (variable.type.type == ShaderType::TEXTURE)
				{
					w << "glUniform1i(glGetUniformLocation(program, \"" << variable.name << "\"), " << textureIndex++ << ");\n";
					continue;
				}
			}

			w << "shader." << variable.name << " = glGetUniformLocation(program, \"" << variable.name << "\");\n";
		}
	}

	int getNumTextures(const std::vector<ShaderVariable>& variables)
	{
		int numTextures = 0;
		foreach (const ShaderVariable& variable, variables)
		{
			if (variable.type.numElements == 0 && variable.type.type == ShaderType::TEXTURE)
				++numTextures;
		}
		return numTextures;
	}

	void setUniforms(CodeWriter& w, const std::vector<ShaderVariable>& variables, bool prefixUniform)
	{
		const char* prefix = prefixUniform ? "uniform." : "";

		foreach (const ShaderVariable& variable, variables)
		{
			if (variable.type.type == ShaderType::INT || variable.type.type == ShaderType::FLOAT)
			{
				char type = variable.type.type == ShaderType::FLOAT ? 'f' : 'i';
				const char* cast = variable.type.type == ShaderType::FLOAT ? "(GLfloat*)" : "(GLint*)";
				const char* ref = variable.type.numElements == 0 ? "&" : "";
				int numElements = std::max(variable.type.numElements, 1);
			
				w << "glUniform" << ident(variable.type.numRows) << type << "v(shader." << variable.name << ", "
					<< numElements << ", " << cast << ref << prefix << variable.name << ");\n";
			}
		}
	}

	const char* textureTypes[] = {"?", "GL_TEXTURE_1D", "GL_TEXTURE_2D", "GL_TEXTURE_3D", "GL_TEXTURE_CUBE_MAP"};
	
	void setTextures(CodeWriter& w, const std::vector<ShaderVariable>& variables, bool prefixUniform, int textureIndex)
	{
		const char* prefix = prefixUniform ? "uniform." : "";

		foreach (const ShaderVariable& variable, variables)
		{
			if (variable.type.numElements == 0)
			{
				if (variable.type.type == ShaderType::TEXTURE)
				{
					// set sampler
					int samplerType = variable.type.numRows;
					
					// activate texture stage. texture 0 is active by default
					if (textureIndex != 0)
						w << "glActiveTexture(GL_TEXTURE" << ident(textureIndex) << ");\n";
					++textureIndex;

					// bind texture to texture stage
					w << "glBindTexture(" << textureTypes[samplerType] << ", " << prefix << variable.name << ");\n";
				}
			}	
		}
	}

	void resetTextures(CodeWriter& w, const std::vector<ShaderVariable>& variables, int textureIndex, int activeIndex)
	{
		reverse_foreach (const ShaderVariable& variable, variables)
		{
			if (variable.type.numElements == 0)
			{
				if (variable.type.type == ShaderType::TEXTURE)
				{
					// reset sampler
					int samplerType = variable.type.numRows;
					
					// activate texture stage. don't need to activate the texture that was still active
					--textureIndex;
					if (textureIndex != activeIndex)
						w << "glActiveTexture(GL_TEXTURE" << ident(textureIndex) << ");\n";

					// bind texture to texture stage
					w << "glBindTexture(" << textureTypes[samplerType] << ", 0);\n";
				}
			}
		}	
	}
	
	void getShaderGlobal(CodeWriter& w, Pointer<LayerInfo> layerInfo)
	{
		StringRef name = layerInfo->name;
		if (layerInfo->equalShaderLayerInfo != NULL)
			name = layerInfo->equalShaderLayerInfo->name;
		w << "ShaderGlobal" << name << "& shader = global." << name << ";\n";
	}
	
	void setShader(CodeWriter& w, Pointer<LayerInfo> layerInfo)
	{
		// get reference to global shader struct (contains uniform handles)
		getShaderGlobal(w, layerInfo);

		// set shader program
		w << "glUseProgram(shader.program);\n";

		// set material uniforms
		w << "const ShaderUniform" << layerInfo->name << "& uniform = instance." << layerInfo->name << ".uniform;\n";
		setUniforms(w, layerInfo->materialVariables, true);
		setTextures(w, layerInfo->materialVariables, true, 0);
		
		if (layerInfo->shapeType == Shader::MESH)
		{
			if (layerInfo->renderFlags == LayerInfo::DOUBLE_SIDED_FLAG)
				w << "glDisable(GL_CULL_FACE);\n";
			else if (layerInfo->renderFlags == (layerInfo->renderFlags & (LayerInfo::FRONT_SIDE_FLAG | LayerInfo::BACK_SIDE_FLAG)))
				w << "glEnable(GL_CULL_FACE);\n";
		}
	}
	
	void resetShader(CodeWriter& w, Pointer<LayerInfo> layerInfo)
	{
		// reset textures in reverse order
		int numMaterialTextures = getNumTextures(layerInfo->materialVariables);
		int numTransformTextures = getNumTextures(layerInfo->transformVariables);
		int numTextures = numMaterialTextures + numTransformTextures;
		int activeTexture = numTextures - 1;
		resetTextures(w, layerInfo->transformVariables, numTextures, activeTexture);
		resetTextures(w, layerInfo->materialVariables, numMaterialTextures, activeTexture);		
	}

	void setCullMode(CodeWriter& w, Pointer<LayerInfo> layerInfo, int renderMode)
	{
		// if render mode is double sided but no double sided flag, it must be two pass rendering
		if ((renderMode & Mesh::DOUBLE_SIDED) && (layerInfo->renderFlags & LayerInfo::DOUBLE_SIDED_FLAG))
		{
			if (layerInfo->renderFlags != LayerInfo::DOUBLE_SIDED_FLAG)
				w << "glDisable(GL_CULL_FACE);\n";
		}
		else
		{
			// front, back or two pass double sided
			
			// check if the shader uses the frontFacing uniform variable (not the shader built-in variable)
			if (layerInfo->useFlags & SHADER_USES_FRONT_FACING)
			{
				w << "glUniform1f(shader.f_f, flip ? ";
				if ((renderMode & Mesh::BACK_SIDE) == 0)
					w << "-1.0f : 1.0f";
				else
					w << "1.0f : -1.0f";
				w << ");\n";
			}
			
			if (layerInfo->renderFlags != (layerInfo->renderFlags & (LayerInfo::FRONT_SIDE_FLAG | LayerInfo::BACK_SIDE_FLAG)))
				w << "glEnable(GL_CULL_FACE);\n";
			if (renderMode & Mesh::BACK_SIDE)
				w << "glCullFace(flip ? GL_BACK : GL_FRONT);\n";
			else
				w << "glCullFace(flip ? GL_FRONT : GL_BACK);\n";
		}	
	}

// render helper
	
	struct RenderJobsInfo
	{
		int objectIndex;
		int layerIndex;

		RenderJobsInfo(int objectIndex, int layerIndex)
			: objectIndex(objectIndex), layerIndex(layerIndex)
		{}
	};

	void renderBoundingBox(CodeWriter& w)
	{
		// world matrix is in matrix, bounding box is in bb
		w.beginScope();
		
		// multiply bounding box into world matrix
		w << "float4x4 bbMatrix = matrix * matrix4x4TranslateScale(bb.center, bb.size);\n";

		// set world matrix
		w << "glUniformMatrix4fv(hbbMatrix, 1, false, &bbMatrix.x.x);\n";

		// draw (12 lines, 24 indices)
		w << "glDrawElements(GL_LINES, 24, GL_UNSIGNED_SHORT, (GLvoid*)0);\n";

		w.endScope();
	}

	void createRenderJobs(CodeWriter& w, GeneratorCollector& collector, RenderJobsInfo& info,
		Pointer<Instancer> instancer, bool renderBoundingBoxes);

	void createRenderJobs(CodeWriter& w, GeneratorCollector& collector, RenderJobsInfo& info,
		const Instance& instance, StringRef parentMatrix, bool renderBoundingBoxes)
	{
		w.beginScope();
		w << "Transform& transform = instance.transforms[" << int(instance.transformIndex) << "];\n";				
		
		// check if visible
		w << "if (transform.visible)\n";
		w.beginScope();

		// shape particle instancers generate a parent transform
		if (parentMatrix.length() > 0)
			w << "float4x4 matrix = " << parentMatrix << " * transform.matrix;\n";
		else
			w << "float4x4& matrix = transform.matrix;\n";		

		createRenderJobs(w, collector, info, instance.instancer, renderBoundingBoxes);

		w.endScope(); // visible?
		w.endScope();
	}

	void createRenderJobs(CodeWriter& w, GeneratorCollector& collector, RenderJobsInfo& info,
		Pointer<Instancer> instancer, bool renderBoundingBoxes)
	{
		if (Pointer<MultiInstancer> multiInstancer = dynamicCast<MultiInstancer>(instancer))
		{
			foreach (Pointer<Instancer> instancer, multiInstancer->instancers)
				createRenderJobs(w, collector, info, instancer, renderBoundingBoxes);
		}
		else if (Pointer<ShapeInstancer> shapeInstancer = dynamicCast<ShapeInstancer>(instancer))
		{
			// shape instancer

			// get shape info
			ShapeInstancerInfo& shapeInstancerInfo = collector.shapeInstancerInfoMap[shapeInstancer];
				
			// get shader info
			//ShaderInfo& shaderInfo = collector.shaderInfoMap[shapeInstancer->shader];
				
			// get deformer/shader info
			//DeformerShaderInfo& deformerShaderInfo = shaderInfo.deformerShaderInfos
			//	[collector.deformersInShaders ? shapeInstancer->deformer : null];
				
			// get layer (may be missing if failed to compile)
			RenderInfoMap::iterator it = shapeInstancerInfo.renderInfoMap.find(info.layerIndex);
			if (it != shapeInstancerInfo.renderInfoMap.end())
			{
				RenderInfo& renderInfo = it->second;
				int renderMode = renderInfo.renderMode;
				Pointer<LayerInfo> layerInfo = renderInfo.layerInfo;
				
				// check if we are out of render jobs (should not occur, the global render job array has to be large enough)
				w << "if (jobIt != jobEnd)\n";
				w.beginScope();
				
				w << "BoundingBox& bb = instance.boundingBoxes[" << shapeInstancer->boundingBoxIndex << "];\n";
				if (renderBoundingBoxes)
					renderBoundingBox(w);

				w << "Shader" << layerInfo->name << "& shader = instance." << layerInfo->name << ";\n";
				w << "RenderJob* renderJob;\n";
					
				// add render job to shader queue or sorted queue
				if (layerInfo->sortMode == SORT_MATERIAL)
				{
					w << "if (!shader.sort)\n";
					w.beginScope();
				}
				if (layerInfo->sortMode != SORT_ALWAYS)
				{
					// not sorted: get render job from end of global render job array
					w << "renderJob = --jobEnd;\n";

					// append render job to shader queue
					w << "renderJob->next = shader.renderJobs;\n";
					w << "shader.renderJobs = renderJob;\n";

					// set render function
					w << "renderJob->render = &render_" << layerInfo->name << '_' << ident(renderMode) << ";\n";
				}
				if (layerInfo->sortMode == SORT_MATERIAL)
				{
					// mark render job as not sorted (not transparent)
					w << "renderJob->distance = -2e30f;\n";
				
					w.endScope();
					w << "else\n";
					w.beginScope();
				}
				if (layerInfo->sortMode != SORT_NEVER)
				{
					// sorted: get render job from begin of global render job array
					w << "renderJob = jobIt++;\n";
						
					// append render job to global alpha-sort queue
					w << "renderJob->next = renderQueues.alphaSort;\n";
					w << "renderQueues.alphaSort = renderJob;\n";

					// calc position of object in homogenous clip space
					w << "float4 position = viewProjectionMatrix * (matrix * vector4(bb.center, 1.0f));\n";

					// calc distance in normalized device space, ranges from [-1, 1]
					//w << "renderJob->distance = clamp(int((position.z / position.w + 1.0f) * 32768.0f), 0, 65535);\n"; // map to [0, 65535]
					w << "renderJob->distance = position.z / position.w;\n";						

					// set render function
					if (layerInfo->sortMode == SORT_ALWAYS)
						w << "renderJob->render = &render_" << layerInfo->name << '_' << ident(renderMode) << ";\n";
				}
				if (layerInfo->sortMode == SORT_MATERIAL)
				{
					// set second render function
					w << "renderJob->render = &render_" << Ascii::toUpperCase(layerInfo->name[0])
						<< layerInfo->name.substr(1) << '_' << ident(renderMode) << ";\n";
					w.endScope();
				}
					
				// copy object id if shader uses it
				if (layerInfo->useFlags & SHADER_USES_ID)
					w << "renderJob->id = instance.ids[" << info.objectIndex << "];\n";
				
				// set draw function
				w << "renderJob->draw = &draw_" << renderInfo.drawFunctionName << ";\n";
							
				// set pointer to instance
				w << "renderJob->instance = &instance;\n";
					
				// set object -> world matrix
				w << "renderJob->matrix = matrix;\n";
					
				w.endScope();
			}
		}
		else if (Pointer<ParticleInstancer> particleInstancer = dynamicCast<ParticleInstancer>(instancer))
		{
			// particle instancer
			
			// get particle instancer info
			ParticleInstancerInfo& particleInstancerInfo = collector.particleInstancerInfoMap[particleInstancer];

			// get particle system
			Pointer<ParticleSystem> particleSystem = staticCast<ParticleSystem>(particleInstancer->particleSystem);

			// get name
			StringRef name = collector.shapeInfoMap[particleSystem]->getName();
					
			// get uniform of particle instancer
			w << "Instancer" << particleInstancerInfo.name << "& uniform = instance.uniforms."
				<< particleInstancer->path << ";\n";
				
			// get particle iterator
			w << "Particle" << name << "* it = instance." << name << ".alive;\n";
				
			// create a render job for each particle
			w << "while (it)\n";
			w.beginScope();
			w << "Particle" << name << "& particle = *it;\n";
				
			// calc particle matrix and index
			w << "int index = 0;\n";
			w << "float4x4 particleMatrix;\n";
			w.beginScope();
			w << particleInstancer->code;
			w.endScope();
				
			// select shape to render according to index
			if (particleInstancer->instances.size() == 1)
			{
				// only one shape
				const std::vector<Instance>& instances = particleInstancer->instances[0];
				foreach (const Instance& instance, instances)
				{
					createRenderJobs(w, collector, info, instance, "particleMatrix", renderBoundingBoxes);
				}
			}
			else
			{
				// multiple shapes selected by per-particle shape index
				w << "switch (index)\n";
				w.beginScope();
				int index = 0;
				foreach (const std::vector<Instance>& instances, particleInstancer->instances)
				{
					w.decIndent();
					w << "case " << index++ << ":\n";
					w.incIndent();

					foreach (const Instance& instance, instances)
					{
						createRenderJobs(w, collector, info, instance, "particleMatrix", renderBoundingBoxes);
					}
					w << "break;\n";
				}
				w.endScope(); // switch (index)				
			}
				
			// next particle
			w << "it = it->next;\n";
			w.endScope(); // while
		}
	}

	void bindAndDraw(CodeWriter& w, GeneratorCollector& collector,
		Pointer<ShapeInstancer> shapeInstancer, ShapeInstancerInfo& shapeInstancerInfo,
		RenderInfo& renderInfo,
		int quadsIndexType, bool useVertexArrayObjects, int draw,
		int& lastVertexBigBufferIndex, int& lastIndexBigBufferIndex)
	{
		const char* indexTypes[] = {"GL_UNSIGNED_BYTE", "GL_UNSIGNED_SHORT", "GL_UNSIGNED_INT"};

		bool bindBuffers = useVertexArrayObjects ^ (draw != 0);
		if (draw == 2)
		{
			// only draw
			useVertexArrayObjects = false;
			bindBuffers = false;
		}

		// get shape info
		Pointer<ShapeInfo> shapeInfo = collector.shapeInfoMap[shapeInstancer->shape];
		int numVertexArrays = shapeInfo->getNumVertexArrays();
		for (int vertexArrayIndex = 0; vertexArrayIndex < numVertexArrays; ++vertexArrayIndex)
		{
			if (useVertexArrayObjects)
			{
				w << "glBindVertexArray(instance.vertexArrays[" << renderInfo.vertexArrayIndex + vertexArrayIndex << "]);\n";
			}
		
			if (bindBuffers)
			{
				// enable vertex arrays
				if (useVertexArrayObjects)
					enableVertexAttributes(w, 0, renderInfo.layerInfo);

				// set vertex state
				setVertexState(w, collector,
					shapeInstancer, shapeInstancerInfo, shapeInfo->getBaseIndex(vertexArrayIndex),
					renderInfo.layerInfo,
					lastVertexBigBufferIndex);
			}
			
			// do shape specific bind and draw
			if (Pointer<ConstantMeshInfo> constantMeshInfo = dynamicCast<ConstantMeshInfo>(shapeInfo))
			{
				IndexBuffer::Section& section = constantMeshInfo->indexBuffer->sections[vertexArrayIndex];
				if (bindBuffers && section.bigBufferIndex != lastIndexBigBufferIndex)
				{
					// set index buffer
					lastIndexBigBufferIndex = section.bigBufferIndex;
					w << "glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global.indexBuffers[" << section.bigBufferIndex
						<< "]);\n";
				}

				if (draw)
				{
					// draw (set uniform, bind vertex/index buffers, set cull mode, draw)
					w << "glDrawElements(GL_TRIANGLES, "
						<< section.indexCount << ", "
						<< indexTypes[section.type] << ", "
						<< "(GLvoid*)" << section.bigBufferOffset << ");\n"; // offset in index buffer
				
				}
			}
			else if (Pointer<TextMeshInfo> textMeshInfo = dynamicCast<TextMeshInfo>(shapeInfo))
			{
				if (bindBuffers)
				{
					// set index buffer
					w << "glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global.quads);\n";
				}
				
				if (draw)
				{
					// draw text symbols
					w << "glDrawElements(GL_TRIANGLES, "
						"instance." << shapeInfo->getName() << ".numSymbols * 6, "
						<< indexTypes[quadsIndexType] << ", "
						"(GLvoid*)0);\n";
				}
			}
			else if (Pointer<ParticleSystemInfo> particleSystemInfo = dynamicCast<ParticleSystemInfo>(shapeInfo))
			{
				if (draw)
				{
					// draw particles
					w << "glDrawArrays(GL_POINTS, 0, "
						"instance." << shapeInfo->getName() << ".numParticles);\n";
				}
			}
		}
	}

// utility

	void writeQuotedString(CodeWriter& w, const std::string& str)
	{
		const char* line = getData(str);
		size_t len = str.length();
		
		size_t i = 0;
		int lineNumber = 1;
		while (i < len)
		{
			// get line until new-line character
			size_t s = i;
			while (i < len && line[i] != '\n')
			{
				++i;
			} 
			
			// write quoted line
			w << '"';
			w.writeString(line + s, i - s);
			w << "\\n\"";
			if (lineNumber % 10 == 0)
				w << " // " << lineNumber;
			w.writeLine();
			++lineNumber;
			
			// skip new-line character
			++i;
		}
	}

	// for field/type pairs of vertex structures
	typedef std::pair<const std::string, std::string> StringPair;

} // anonymous namespace


// start

// generateSceneOpenGL
bool generateSceneOpenGL(Pointer<BufferConverter> bufferConverter, Pointer<Scene> scene,
	CodeWriter& w, DataWriter& d, const SceneOptions& options, SceneStatistics& stats)
{	
	// gets true if a compilation failed
	bool hasError = false;

	// api dependent options
	int collectorFlags;
	Language shaderLanguage;
	bool useVertexArrayObjects;
	if (options.api == GraphicsApi::GL && options.api.version < 300)
	{
		// opengl 2.1
		collectorFlags = GeneratorCollector::TARGET_X8Y8Z8;
		shaderLanguage = Language::GLSL_1_2;
		useVertexArrayObjects = false;
	}
	else if (options.api == GraphicsApi::GL)
	{
		// opengl 3.x
		collectorFlags = GeneratorCollector::TARGET_X8Y8Z8;
		shaderLanguage = Language::GLSL_1_5;
		useVertexArrayObjects = true;
	}
	else if (options.api == GraphicsApi::GLES)
	{
		// opengl es
		collectorFlags = GeneratorCollector::TARGET_X5Y6Z5 | GeneratorCollector::NO_INDEX_32_BIT;
		shaderLanguage = Language::ESSL;
		useVertexArrayObjects = false;
	}
	else
	{
		return false;
	}

	// when using expand to float for IE 11 then also don't use 8 bit indices
	if (options.dataMode & SceneOptions::USE_FLOAT)
		collectorFlags |= GeneratorCollector::NO_INDEX_8_BIT;

	bool deformersInShaders = options.deformersInShaders;
	const std::vector<ShaderOptions>& shaderOptions = options.shaderOptions;

	Compiler compiler(Compiler::INTERMEDIATE);
	
	GeneratorCollector collector(deformersInShaders, options.dataMode,
		Type::COMPONENT_ALIGN, Type::EXTEND_TO_4, collectorFlags);

	// scene info: maximum transform and bounding box index etc.
	SceneInfo sceneInfo;

	// collect layers and objects of instances (buffers, primitives, deformers, shaders)
	collector.collectInstances(shaderOptions, scene, sceneInfo);

	// choose packed formats and calc scale/offset
	collector.chooseFormats(bufferConverter);

	// compile shaders (also store in vertex buffers infos which fields are needed)
	hasError |= collector.compileShaders(compiler, shaderOptions, Language::CPP, shaderLanguage);

	// compile deformers (after shaders because they tell the deformers the used fields)
	hasError |= collector.compileDeformers(compiler, Language::CPP, Language::CPP);

	stats.numTextures = max(stats.numTextures, collector.numTextures);

	// generate and write buffers for rendering, including format conversion
	int64_t dataOffset = collector.writeBuffers(d, bufferConverter);
	int numShaderInputBuffers = int(collector.bigVertexBuffers[SCOPE_SHADER].size());
	int numDeformerInputBuffers = int(collector.bigVertexBuffers[SCOPE_DEFORMER].size());
	int numIndexBuffers = int(collector.bigIndexBuffers.size());
	int numDynamicBuffers = int(collector.dynamicBufferInfos.size());

	// generate and write buffers for particle init state
	ParticleInfo particleInfo;
	particleInfo.writeBuffers(bufferConverter, collector.shapeInfoMap, w, d);
	size_t particleBufferSize = particleInfo.buffer.size;

	// determine maximum number of quads that have to be rendered in one draw call
	uint maxNumQuads = collector.getMaxNumQuads();
	int quadsIndexType = maxNumQuads <= 256/4 ? 0 : (maxNumQuads <= 65536/4 ? 1 : 2);

	// count number of vertex array objects
	int numVertexArrayObjects = collector.buildRenderInfos();


	// write shader code
	foreach (Pointer<LayerInfo> layerInfo, collector.layerInfos)
	{
		if (layerInfo->equalShaderLayerInfo == NULL)
		{
			if (options.shadersInCode)
			{
				// output shaders into code
				w.writeComment(layerInfo->comment);

				w << "static const char* vs" << layerInfo->name << " =\n";
				w.incIndent();
				writeQuotedString(w, layerInfo->vertexShader);
				w.decIndent();
				w << ";\n";

				w << "static const char* ps" << layerInfo->name << " =\n";
				w.incIndent();
				writeQuotedString(w, layerInfo->pixelShader);
				w.decIndent();
				w << ";\n";
				w.writeLine();
			}
			else
			{
				// output shader code into data (store offset in data file)
				layerInfo->vertexShaderOffset = d.getPosition();
				d.writeData(layerInfo->vertexShader.c_str(), layerInfo->vertexShader.size() + 1);
				layerInfo->pixelShaderOffset = d.getPosition();
				d.writeData(layerInfo->pixelShader.c_str(), layerInfo->pixelShader.size() + 1);
			}
		}
		else if (options.shadersInCode)
		{
			// shader is equal to another shader
			w.writeComment(layerInfo->comment);
			w << "// equals " << layerInfo->equalShaderLayerInfo->comment << "\n";
			w.writeLine();
		}
	}


	// write structs

	// named buffers (reference global data, e.g. animation tracks)
	{
		Pointer<StructType> buffers = new StructType();
		foreach (const NamedBufferInfo& info, collector.namedBufferInfos)
		{
			const std::string& name = info.namedBuffer->name;
			BufferFormat format = info.format;
			VectorInfo typeInfo = toVectorInfo(format);

			buffers->addMember(name, typeInfo.toString() + '*');
						
			// macros for buffers
			if (info.scale == 1.0 && info.offset == 0.0)
			{
				w << "#define macro" << name << "(x) x\n";
				w << "#define macro" << name << "Inv(x) x\n";
			}
			else
			{
				w << "#define macro" << name << "(x) (x * " << info.scale << " + " << info.offset << ")\n";
				float s = 1.0 / info.scale;
				float o = -info.offset * s;
				w << "#define macro" << name << "Inv(x) (x * " << s << " + " << o << ")\n";
			}
		}
		buffers->writeType(w, "Buffers");
	}
	w.writeLine();

	// state structure
	scene->stateType->writeType(w, "State");
	w.writeLine();
	
	
	// deformer uniform structures ("Deformer<name>")
	foreach (DeformerInfoPair& deformerInfoPair, collector.deformerInfoMap)
	{
		const Pointer<Deformer>& deformer = deformerInfoPair.first;
		if (deformer != null)
		{
			DeformerInfo& deformerInfo = deformerInfoPair.second;
		
			// create type name from deformer name
			deformerInfo.name = createVariableName(deformer->name);
		
			deformer->uniformType->writeType(w, "Deformer" + deformerInfo.name);
		}
	}		
	w.writeLine();

	// shape structs
	foreach (ShapeInfoPair& p, collector.shapeInfoMap)
	{
		if (Pointer<TextMesh> textMesh = dynamicCast<TextMesh>(p.first))
		{
			// macros for text meshes
			StringRef name = p.second->getName();
		
			w << "#define macro" << name << "(scale, space, column, align, text, symbols, c, s)\\\n";
			w.incIndent();
			
			// get symbol table
			//w << "TextSymbols& symbols = global." << name << ";\\\n";

			// get text buffer
			w << "TextData& data = instance." << name << ";\\\n";
			
			// call external function to convert string to symbol vertices
			w << "text2symbols(scale, space, column, align, text, symbols, "
				<< textMesh->maxNumSymbols << ", data);\\\n";
			
			// output bounding box
			w << "c = data.center;\\\n";
			w << "s = data.size;\n";

			w.decIndent();
		
		}
		else if (Pointer<ParticleSystem> particleSystem = dynamicCast<ParticleSystem>(p.first))
		{
			// particle system structures ("Emitters<name>", "ParticleSystem<name>", "Particle<name>" and "Particles<name>")
			StringRef name = p.second->getName();
			
			// struct "Emitters<name>" (uniform)
			if (particleSystem->emitterUniformType != null)
				particleSystem->emitterUniformType->writeType(w, "Emitters" + name);

			// struct "ParticleSystem<name>" (uniform)
			particleSystem->uniformType->writeType(w, "ParticleSystem" + name);
			
			// struct "Particle<name>" (next pointer, particle members)
			Pointer<StructType> particleType = staticCast<StructType>(particleSystem->particleType);
			w.beginStruct("Particle" + name);
			w << "Particle" << name << "* next;\n";
			particleType->writeMembers(w);
			w.endStruct();
			
			// struct "Particles<name>" (alive and dead pointers, number of particles etc.)
			w.beginStruct("Particles" + name);
			w << "Particle" << name << "* particles;\n";
			w << "Particle" << name << "* alive;\n";
			w << "Particle" << name << "* dead;\n";
			w << "int numParticles;\n";
			w << "int id;\n";
			w.endStruct();
			w.writeLine();
		}
	}

	// particle instancer uniform structures ("Instancer<name>")
	foreach (ParticleInstancerInfoPair& p, collector.particleInstancerInfoMap)
	{
		const Pointer<ParticleInstancer>& shapeParticleInstancer = p.first;
		ParticleInstancerInfo& particleShapeInfo = p.second;
		
		// struct "Instancer<name>"
		shapeParticleInstancer->uniformType->writeType(w, "Instancer" + particleShapeInfo.name);
		w.writeLine();
	}


	// write structs Material<name>
	foreach (ShaderInfoPair& shaderInfoPair, collector.shaderInfoMap)
	{
		const Pointer<Shader>& shader = shaderInfoPair.first;
		ShaderInfo& shaderInfo = shaderInfoPair.second;

		// struct Material<name>
		shader->materialType->writeType(w, "Material" + shaderInfo.typeName);
		w.writeLine();
	}

	// write structs ShaderGlobal<name>, ShaderUniform<name>, ShaderTransfer<name>, Shader<name>
	foreach (Pointer<LayerInfo> layerInfo, collector.layerInfos)
	{
		if (layerInfo->equalShaderLayerInfo == null)
		{
			// struct ShaderGlobal<name>
			w.beginStruct("ShaderGlobal" + layerInfo->name);
			{
				// program handle
				w << "GLuint program;\n";			

				// handles for uniform arrays (material/transform -> vertex/pixel shader)
				createHandles(w, layerInfo->materialVariables);
				createHandles(w, layerInfo->transformVariables);

				if (layerInfo->useFlags & SHADER_USES_SCALE_OFFSET)
					w << "GLuint s_o;\n";
				if (layerInfo->useFlags & SHADER_USES_FRONT_FACING)
					w << "GLuint f_f;\n";
					
				// sequence number of instance that is currently set
				//w << "int sequence;\n";
			}
			w.endStruct();
		}
		
		// struct ShaderUniform<name> (material -> vertex/pixel shader, set when new instance is encountered)
		printShaderStruct(w, "ShaderUniform" + layerInfo->name, layerInfo->materialVariables);

		// struct ShaderTransfer<name> (material -> transform)
		printShaderStruct(w, "ShaderTransfer" + layerInfo->name, layerInfo->transferVariables);

		// struct Shader<name> (instance data)
		w.beginStruct("Shader" + layerInfo->name);
		{					
			// material -> shader uniform variables
			w << "ShaderUniform" << layerInfo->name << " uniform;\n";
	
			// material -> transform transfer variables
			w << "ShaderTransfer" << layerInfo->name << " transfer;\n";
	
			// sort flag if sorting depends on material
			if (layerInfo->sortMode == SORT_MATERIAL)
				w << "bool sort;\n";

			// all render jobs that are rendered with this shader
			if (layerInfo->sortMode != SORT_ALWAYS)
				w << "RenderJob* renderJobs;\n";
		}
		w.endStruct();
		w.writeLine();
	}

	// write struct Uniforms (contains all uniforms for deformers, particle systems, shaders)
	{
		Pointer<StructType> uniforms = new StructType();

		// deformers
		foreach (DeformerInfoPair& p, collector.deformerInfoMap)
		{
			const Pointer<Deformer>& deformer = p.first;
			DeformerInfo& deformerInfo = p.second;			
			if (deformer != null)
				uniforms->addMember(deformer->path, "Deformer" + deformerInfo.name);
		}
		
		// shapes
		foreach (ShapeInfoPair& p, collector.shapeInfoMap)
		{
			if (Pointer<ParticleSystem> particleSystem = dynamicCast<ParticleSystem>(p.first))
			{
				// particle system
				StringRef name = p.second->getName();
				if (particleSystem->emitterUniformType != null)
					uniforms->addMember("e." + particleSystem->path, "Emitters" + name);
				uniforms->addMember(particleSystem->path, "ParticleSystem" + name);
			}
		}

		// particle instancers
		foreach (ParticleInstancerInfoPair& p, collector.particleInstancerInfoMap)
		{
			const Pointer<ParticleInstancer>& shapeParticleInstancer = p.first;
			ParticleInstancerInfo& particleInstancerInfo = p.second;
			uniforms->addMember(shapeParticleInstancer->path, "Instancer" + particleInstancerInfo.name);
		}
		
		// shaders
		foreach (ShaderInfoPair& p, collector.shaderInfoMap)
		{
			const Pointer<Shader>& shader = p.first;
			ShaderInfo& shaderInfo = p.second;
			uniforms->addMember(shader->path, "Material" + shaderInfo.typeName);
		}
		
		uniforms->writeType(w, "Uniforms");
		w.writeLine();
	}
	
	
	// write struct Global (contains global data for all instances of a scene)
	{
		w.beginStruct("Global");
				
		// add named buffers (e.g. animation tracks)
		w << "Buffers buffers;\n";
		
		// add buffers
		if (numShaderInputBuffers > 0)
			w << "GLuint shaderInputBuffers[" << numShaderInputBuffers << "];\n";
		if (numDeformerInputBuffers > 0)
			w << "ubyte* deformerInputBuffers[" << numDeformerInputBuffers << "];\n";
		if (numIndexBuffers > 0)
			w << "GLuint indexBuffers[" << numIndexBuffers << "];\n";
		if (maxNumQuads > 0)
			w << "GLuint quads;\n";
		if (particleBufferSize > 0)
			w << "ubyte* particleBuffer;\n"; // initial state of particle systems
	
		// scale/offset arrays for packed vertex buffers
		if (!collector.scaleOffsetArrayMap.empty())
			w << "const float* scaleOffset[" << collector.scaleOffsetArrayMap.size() << "];\n";
		
		// add shader global for each shader (handles for program and uniforms)
		foreach (Pointer<LayerInfo> layerInfo, collector.layerInfos)
		{
			if (layerInfo->equalShaderLayerInfo == NULL)
				w << "ShaderGlobal" << layerInfo->name << ' ' << layerInfo->name << ";\n";
		}

		if (options.renderBoundingBoxes)
		{
			w << "GLuint bbProgram;\n";
			w << "GLuint bbViewProjectionMatrix;\n";
			w << "GLuint bbMatrix;\n";

			w << "GLuint bbVertexBuffer;\n";
			w << "GLuint bbIndexBuffer;\n";
			if (useVertexArrayObjects)
				w << "GLuint bbVertexArray;\n";
		}

		// global sequence number
		w << "int sequence;\n";
				
		w.endStruct(); // "Global"
		w.writeLine();	
	}
	
	// struct "Instance"
	{
		w.beginStruct("Instance");
		
		w << "Global* global;\n";
		if (!scene->instances.empty())
			w << "int ids[" << int(scene->instances.size()) << "];\n"; // object indices for picking
		w << "State state;\n"; // scene state
		w << "Uniforms uniforms;\n"; // uniforms of deformers, particle systems, shaders

		// shapes
		foreach (ShapeInfoPair& p, collector.shapeInfoMap)
		{
			if (Pointer<TextMesh> textMesh = dynamicCast<TextMesh>(p.first))
			{
				// text mesh
				StringRef name = p.second->getName();
				w << "TextData " << name << ";\n";
			}
			else if (Pointer<ParticleSystem> particleSystem = dynamicCast<ParticleSystem>(p.first))
			{
				// particle system
				StringRef name = p.second->getName();
				w << "Particles" << name << ' ' << name << ";\n";
			}
		}
		
		// shaders
		foreach (Pointer<LayerInfo> layerInfo, collector.layerInfos)
		{
			w << "Shader" << layerInfo->name << ' ' << layerInfo->name << ";\n";
		}
		
		// transform/bounding box arrays
		w << "Transform transforms[" << scene->transformCount << "];\n";
		w << "BoundingBox boundingBoxes[" << scene->boundingBoxCount << "];\n";
		
		// sequence numbers
		w << "int sceneSequence;\n"; // scene sequence number
		w << "int deformerSequence;\n"; // update deformers if new scene sequence number
		w << "int renderSequence;\n"; // render sequence number
		w << "int seed;\n"; // random seed

		// dynamic vertex buffers (deformer output)
		if (numDynamicBuffers > 0)
		{
			w << "GLuint dynamicBuffers[" << numDynamicBuffers << "];\n";
			if (!options.mapBuffer)
				w << "ubyte* tempBuffer;\n";
		}

		// vertex array objects
		if (useVertexArrayObjects)
			w << "GLuint vertexArrays[" << numVertexArrayObjects << "];\n";

		w.endStruct();
		w.writeLine();
	}
	
	
	// init function for scene global data
	w << "void initGlobal(void* pGlobal, ubyte* data)\n";
	{
		w.beginScope();
		w << "Global& global = *(Global*)pGlobal;\n";
		w.writeLine();
	
		// data buffers
		{
			int64_t offset = dataOffset;
			
			// named buffers (e.g. animation tracks)
			if (!scene->buffers.empty())
			{
				foreach (const NamedBufferInfo& info, collector.namedBufferInfos)
				{
					BufferFormat format = info.format;
					VectorInfo typeInfo = toVectorInfo(format);
					size_t elementCount = info.namedBuffer->buffer->getNumElements();
					
					offset += -offset & (format.getComponentSize() - 1);
					size_t count = elementCount * format.getNumComponents();
					w << "global.buffers." << info.namedBuffer->name << " = "
						"(" << typeInfo.toString() << "*)(data + " << offset << ");\n";
					offset += count * format.getComponentSize();
				}
				w.writeLine();
			}

			// deformer vertex buffers
			if (numDeformerInputBuffers > 0)
			{
				std::vector<BigVertexBuffer>& bigVertexBuffers = collector.bigVertexBuffers[SCOPE_DEFORMER];
				int index = 0;
				foreach (BigVertexBuffer& bigVertexBuffer, bigVertexBuffers)
				{
					offset += -offset & (bigVertexBuffer.vertexBuffer->dataAlign - 1);
					size_t dataSize = bigVertexBuffer.vertexCount * bigVertexBuffer.vertexBuffer->dataVertexSize;
					w << "global.deformerInputBuffers[" << index << "] = data + " << offset << ";\n";
					++index;
					offset += dataSize;
				}
				w.writeLine();
			}

			// shader vertex buffers
			if (numShaderInputBuffers > 0)
			{
				std::vector<BigVertexBuffer>& bigVertexBuffers = collector.bigVertexBuffers[SCOPE_SHADER];
				w << "glGenBuffers(" << numShaderInputBuffers << ", global.shaderInputBuffers);\n";
				int index = 0;
				foreach (BigVertexBuffer& bigVertexBuffer, bigVertexBuffers)
				{
					w << "glBindBuffer(GL_ARRAY_BUFFER, global.shaderInputBuffers[" << index << "]);\n";
					++index;

					offset += -offset & (bigVertexBuffer.vertexBuffer->dataAlign - 1);
					size_t dataSize = bigVertexBuffer.vertexCount * bigVertexBuffer.vertexBuffer->dataVertexSize;
					w << "glBufferData(GL_ARRAY_BUFFER, " << dataSize << ", "
						"data + " << offset << ", GL_STATIC_DRAW);\n";
					offset += dataSize;
				}
				w.writeLine();
			}
			
			// shader index buffers
			if (numIndexBuffers > 0)
			{
				w << "glGenBuffers(" << numIndexBuffers << ", global.indexBuffers);\n";
				int index = 0;
				foreach (BigIndexBuffer& bigIndexBuffer, collector.bigIndexBuffers)
				{
					w << "glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global.indexBuffers[" << index << "]);\n";
					++index;

					offset += -offset & (bigIndexBuffer.indexSize - 1);
					size_t dataSize = bigIndexBuffer.indexCount * bigIndexBuffer.indexSize;
					w << "glBufferData(GL_ELEMENT_ARRAY_BUFFER, " << dataSize << ", "
						"data + " << offset << ", GL_STATIC_DRAW);\n";
					offset += dataSize;
				}
				w.writeLine();
			}
			
			// create index buffer for quads
			if (maxNumQuads > 0)
			{
				int numQuads = maxNumQuads;
				int numVertices = numQuads * 4;
				int numIndices = numQuads * 6;
				
				const char* types[] = {"ubyte", "ushort", "uint"};
				const char* type = types[quadsIndexType];
				int size = numIndices << quadsIndexType;
				
				w.beginScope();
				w << "glGenBuffers(1, &global.quads);\n";
				w << "glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global.quads);\n";
				w << type << "* quads = (" << type << "*)malloc(" << size << ");\n";
				w << "for (int v = 0, i = 0; v < " << numVertices << "; v += 4, i += 6)\n";
				w.beginScope();
				w << "quads[i + 0] = v + 0;\n";
				w << "quads[i + 1] = v + 1;\n";
				w << "quads[i + 2] = v + 3;\n";
				w << "quads[i + 3] = v + 1;\n";
				w << "quads[i + 4] = v + 2;\n";
				w << "quads[i + 5] = v + 3;\n";
				w.endScope();
				w << "glBufferData(GL_ELEMENT_ARRAY_BUFFER, " << size << ", quads, GL_STATIC_DRAW);\n";
				w << "free(quads);\n";
				w.endScope();
				w.writeLine();
			}			
		}

		// scale/offset arrays for packed vertex buffers
		if (!collector.scaleOffsetArrayMap.empty())
		{
			foreach (GeneratorCollector::ScaleOffsetArrayPair& p, collector.scaleOffsetArrayMap)
			{
				w << "static const float so" << ident(p.second) << "[] = ";
				w.writeArray(p.first);
				w << ";\n";
				w << "global.scaleOffset[" << p.second << "] = so" << ident(p.second) << ";\n";
			}
			w.writeLine();
		}
		
		// get buffers for text mesh symbol tables
		//foreach (TextMeshInfoPair& p, collector.textMeshInfos)
		//{
		//	const Pointer<TextMesh>& textMesh = p.first;
		//	TextMeshInfo& textMeshInfo = p.second;
		//	const std::string& name = textMeshInfo.name;
			
			//w.beginScope();
			//w << "TextSymbols& symbols = global." << name << ";\n";
			//w << "symbols.numSymbols = " << textMesh->symbols.size() << ";\n";
			//w << "symbols.indices = (ushort*)(data + " << textMeshInfo.indicesOffset << ");\n";
			//w << "symbols.strings = data + " << textMeshInfo.stringsOffset << ";\n";
			//w << "symbols.rects = (float*)(data + " << textMeshInfo.symbolsOffset << ");\n";
			//w.endScope();
		//}

		// get buffer for initial state of particle systems (references global data)
		if (particleBufferSize > 0)
		{
			w << "global.particleBuffer = data + " << particleInfo.buffer.offset << ";\n";
			w.writeLine();
		}

		// init shaders for all layers
		foreach (Pointer<LayerInfo> layerInfo, collector.layerInfos)
		{
			// check if shader is not reused from other layer
			if (layerInfo->equalShaderLayerInfo == null)
			{
				w.beginScope();

				// get shader struct
				w << "ShaderGlobal" << layerInfo->name << "& shader = "
					"global." << layerInfo->name << ";\n";

				// create vertex and pixel shader
				if (options.shadersInCode)
				{
					w << "GLuint vertexShader = createVertexShader(vs" << layerInfo->name << ", "
						"\"" << layerInfo->comment << "\");\n";
					w << "GLuint pixelShader = createPixelShader(ps" << layerInfo->name << ", "
						"\"" << layerInfo->comment << "\");\n";
				}
				else
				{
					w << "GLuint vertexShader = createVertexShader((const char*)("
						"data + " << layerInfo->vertexShaderOffset << "), "
						"\"" << layerInfo->comment << "\");\n";
					w << "GLuint pixelShader = createPixelShader((const char*)("
						"data + " << layerInfo->pixelShaderOffset << "), "
						"\"" << layerInfo->comment << "\");\n";
				}

				// create program
				w << "GLuint program = shader.program = glCreateProgram();\n";
				w << "glAttachShader(program, vertexShader);\n";
				w << "glAttachShader(program, pixelShader);\n";

				// bind vertex attribute locations
				foreach (const LayerInfo::VertexBindingPair& p, layerInfo->vertexBindings)
				{
					// get name in shader (e.g. "_0")
					const std::string& name = p.second.name;
			
					// the name is also the index when '_' is stripped off
					w << "glBindAttribLocation(program, " << name.substr(1) << ", \"" << name << "\");\n";
				}
		
				// link program
				w << "glLinkProgram(program);\n";

				w << "glDeleteShader(vertexShader);\n";
				w << "glDeleteShader(pixelShader);\n";

				// get handles for material and transform uniform arrays from shader (glGetUniformLocation)
				if (getNumTextures(layerInfo->materialVariables) + getNumTextures(layerInfo->transformVariables) > 0)
					w << "glUseProgram(program);\n";
				int textureIndex = 0;
				getHandles(w, layerInfo->materialVariables, textureIndex);
				getHandles(w, layerInfo->transformVariables, textureIndex);
				if (layerInfo->useFlags & SHADER_USES_SCALE_OFFSET)
					w << "shader.s_o = glGetUniformLocation(program, \"s_o\");\n";
				if (layerInfo->useFlags & SHADER_USES_FRONT_FACING)
					w << "shader.f_f = glGetUniformLocation(program, \"f_f\");\n";
				w.endScope();
			}
		}
		
		// bounding boxes
		if (options.renderBoundingBoxes)
		{
			w.writeComment("bounding box shader");
			w.beginScope();
			
			// create shaders
			w << "GLuint vertexShader = createVertexShader(\""
				"uniform mat4 viewProjectionMatrix;"
				"uniform mat4 matrix;"
				"attribute vec4 position;"
				"void main()"
				"{"
					"gl_Position = viewProjectionMatrix * matrix * position;"
				"}"
				"\", \"boundingBox\");\n";
			w << "GLuint pixelShader = createPixelShader(\""
				"void main()"
				"{"
					"gl_FragColor = vec4(1.0);"
				"}"
				"\", \"boundingBox\");\n";

			// create program
			w << "GLuint program = global.bbProgram = glCreateProgram();\n";
			w << "glAttachShader(program, vertexShader);\n";
			w << "glAttachShader(program, pixelShader);\n";

			// bind vertex attribute locations
			w << "glBindAttribLocation(program, 0, \"position\");\n";

			// link program
			w << "glLinkProgram(program);\n";

			w << "glDeleteShader(vertexShader);\n";
			w << "glDeleteShader(pixelShader);\n";

			// get uniform locations
			w << "global.bbViewProjectionMatrix = glGetUniformLocation(program, \"viewProjectionMatrix\");\n";
			w << "global.bbMatrix = glGetUniformLocation(program, \"matrix\");\n";

			if (useVertexArrayObjects)
			{
				// generate and bind vertex array object
				w << "glGenVertexArrays(1, &global.bbVertexArray);\n";
				w << "glBindVertexArray(global.bbVertexArray);\n";

				// enable vertex attribute 0
				w << "glEnableVertexAttribArray(0);\n";
			}

			// create vertex buffer
			w << "static const float vb[] = {"
				"-1.0f, -1.0f, -1.0f, "
				"-1.0f, -1.0f, 1.0f, "
				"-1.0f, 1.0f, -1.0f, "
				"-1.0f, 1.0f, 1.0f, "
				"1.0f, -1.0f, -1.0f, "
				"1.0f, -1.0f, 1.0f, "
				"1.0f, 1.0f, -1.0f, "
				"1.0f, 1.0f, 1.0f};\n";
			w << "glGenBuffers(1, &global.bbVertexBuffer);\n";
			w << "glBindBuffer(GL_ARRAY_BUFFER, global.bbVertexBuffer);\n";
			w << "glBufferData(GL_ARRAY_BUFFER, 24 * 4, vb, GL_STATIC_DRAW);\n";

			if (useVertexArrayObjects)
			{
				// set bounding box vertex buffer (8 positions) to vertex attribute 0
				w << "glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (GLvoid*)0);\n";
			}				

			// create index buffer
			w << "static const ushort ib[] = {0, 1, 0, 2, 3, 1, 3, 2,  0, 4, 1, 5, 2, 6, 3, 7,  4, 5, 4, 6, 7, 5, 7, 6};\n";
			w << "glGenBuffers(1, &global.bbIndexBuffer);\n";
			w << "glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global.bbIndexBuffer);\n";
			w << "glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * 2, ib, GL_STATIC_DRAW);\n";
			
			w.endScope();
		}
		
		w.endScope();
	}
	w.writeLine();
	
	// done function for scene global data
	w << "void doneGlobal(void* pGlobal)\n";
	{
		w.beginScope();
		w << "Global& global = *(Global*)pGlobal;\n";
		w.writeLine();

		// delete shader input buffers (static vertex buffers)
		if (numShaderInputBuffers > 0)
		{
			w << "glDeleteBuffers(" << numShaderInputBuffers << ", global.shaderInputBuffers);\n";
		}

		// delete index buffers (static index buffers)
		if (numIndexBuffers > 0)
		{
			w << "glDeleteBuffers(" << numIndexBuffers << ", global.indexBuffers);\n";
		}
		
		// iterate over all layers and delete programs
		foreach (Pointer<LayerInfo> layerInfo, collector.layerInfos)
		{
			if (layerInfo->equalShaderLayerInfo == NULL)
				w << "glDeleteProgram(global." << layerInfo->name << ".program);\n";
		}

		// bounding boxes
		if (options.renderBoundingBoxes)
		{
			w << "glDeleteProgram(global.bbProgram);\n";
								
			w << "glDeleteBuffers(1, &global.bbVertexBuffer);\n";
			w << "glDeleteBuffers(1, &global.bbIndexBuffer);\n";
			if (useVertexArrayObjects)
				w << "glDeleteVertexArrays(1, &global.bbVertexArray);\n";
		}
				
		w.endScope();
	}
	w.writeLine();		
	
	// init function for scene instance
	w << "void initInstance(const void* pGlobal, void* pInstance)\n";
	{
		w.beginScope();
		w << "Global& global = *(Global*)pGlobal;\n";
		w << "Instance& instance = *(Instance*)pInstance;\n";
		w << "instance.global = &global;\n";
		w.writeLine();

		// create textures for large uniform arrays
		{
			bool haveLargeArrays = false;
			foreach (Pointer<LayerInfo> layerInfo, collector.layerInfos)
			{
				foreach (ShaderVariable& variable, layerInfo->materialLargeArrays)
				{
					if (!haveLargeArrays)
					{
						haveLargeArrays = true;
						w << "GLuint texture;\n";
					}
					w << "glGenTextures(1, &texture);\n";
					w << "glBindTexture(GL_TEXTURE_2D, instance." << layerInfo->name
						<< ".uniform." << variable.name << " = texture);\n";
					w << "glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, " << variable.type.numElements
						<< ", 1, 0, GL_RGBA, GL_FLOAT, NULL);\n";
					w << "glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);\n";
					w << "glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);\n";
					w << "glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);\n";
					w << "glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);\n";
				}
			}
			if (haveLargeArrays)
				w.writeLine();
		}
		
		// create dynamic buffers for deformer output (and temp buffer if glMapBuffer is not supported)
		if (numDynamicBuffers > 0)
		{
			w << "glGenBuffers(" << numDynamicBuffers << ", instance.dynamicBuffers);\n";
			size_t maxSize = 0;
			int index = 0;
			foreach (DynamicBufferInfo& dynamicBufferInfo, collector.dynamicBufferInfos)
			{
				w << "glBindBuffer(GL_ARRAY_BUFFER, instance.dynamicBuffers[" << index << "]);\n";
				w << "glBufferData(GL_ARRAY_BUFFER, " << dynamicBufferInfo.size << ", NULL, GL_DYNAMIC_DRAW);\n";
				maxSize = max(maxSize, dynamicBufferInfo.maxBufferSize);
				++index;					
			}
			if (!options.mapBuffer)
				w << "instance.tempBuffer = (ubyte*)malloc(" << maxSize << ");\n";
			w.writeLine();
		}
		
		// create vertex array objects (opengl 3.x)
		if (useVertexArrayObjects)
		{
			w << "glGenVertexArrays(" << numVertexArrayObjects << ", instance.vertexArrays);\n";

			// iterate over shapes
			//int vertexArrayObjectIndex = 0;
			int lastVertexBigBufferIndex = -1;
			foreach (ShapeInstancerInfoPair& p, collector.shapeInstancerInfoMap)
			{
				Pointer<ShapeInstancer> shapeInstancer = p.first;
				ShapeInstancerInfo& shapeInstancerInfo = p.second;
						
				foreach (RenderInfoPair& p, shapeInstancerInfo.renderInfoMap)
				{
					RenderInfo& renderInfo = p.second;
				
					// bind vertex array object, then bind vertex and index buffers (don't draw)
					int lastIndexBigBufferIndex = -1;
					bindAndDraw(w, collector, shapeInstancer, shapeInstancerInfo,
						renderInfo,
						quadsIndexType, useVertexArrayObjects, 0,
						lastVertexBigBufferIndex, lastIndexBigBufferIndex);
				
				}
			}
			w.writeLine();
		}
		
		// init shapes
		foreach (ShapeInfoPair& p, collector.shapeInfoMap)
		{
			if (Pointer<TextMesh> textMesh = dynamicCast<TextMesh>(p.first))
			{
				// text mesh
				StringRef name = p.second->getName();
			
				w.beginScope();

				// get text data
				w << "TextData& text = instance." << name << ";\n";

				// allocate vertices (4 for each symbol)
				w << "text.vertices = (TextVertex*)"
					"malloc(" << textMesh->maxNumSymbols * 4 << " * sizeof(TextVertex));\n";

				w.endScope();
			}
			else if (Pointer<ParticleSystem> particleSystem = dynamicCast<ParticleSystem>(p.first))
			{
				// particle system
				Pointer<ParticleSystemInfo> particleSystemInfo = staticCast<ParticleSystemInfo>(p.second);
				const std::string& name = particleSystemInfo->name;
				int numInitParticles = particleSystemInfo->numInitParticles;
				int numParticles = particleSystemInfo->numParticles - numInitParticles;
					
				w.beginScope();
					
				//! start time for particle system
				w << "float time = 0.0f;\n";
					
				// get particle pool of particle system
				w << "Particles" << name << "& pp = instance." << name << ";\n";
					
				// allocate particles
				w << "Particle" << name << "* p = (Particle" << name << "*)"
					"malloc(" << numParticles << " * sizeof(Particle" << name << "));\n";
				w << "pp.particles = p;\n";
									
				// link uninitialized particles which are dead
				w << "Particle" << name << "* end = p + " << numParticles << ";\n";
				w << "Particle" << name << "* dead = NULL;\n";
				w << "for (; p != end; ++p)\n";
				w.beginScope();
				w << "p->next = dead;\n";
				w << "dead = p;\n";
				w.endScope();
				w << "pp.dead = dead;\n";
					
				// check if there are particles with initial state
				if (numInitParticles > 0)
				{
					// link initialized particles which are alive
					w << "end += " << numInitParticles << ";\n";
					w << "ParticleInit" << name << "* i = (ParticleInit" << name << "*)"
						"(global.particleBuffer + " << particleSystemInfo->offset << ");\n";
					w << "Particle" << name << "* alive = NULL;\n";
					w << "for (; p != end; ++p, ++i)\n";
					w.beginScope();
						
					// copy fields from init state
					w << "ParticleInit" << name << "& init = *i;\n";
					w << "Particle" << name << "& particle = *p;\n";
					foreach (const ParticleSystem::Field& field, particleSystem->initialState)
					{
						w << "particle." << field.name << " = init." << field.name << ";\n";
					}
						
					// create code for particle
					w.beginScope();
					w << particleSystem->createCode;
					w.endScope();
						
					w << "p->next = alive;\n";
					w << "alive = p;\n";
					w.endScope();

					// particles which got initialized are alive
					w << "pp.alive = alive;\n";
						
					// set number of alive particles
					w << "pp.numParticles = " << numInitParticles << ";\n";

					// set id for next particle that is created
					w << "ps.id = " << numInitParticles << ";\n";
				}
					
				w.endScope();
			
			}
		}
				
		// init state of scene
		{
			w.beginScope();
			w << "State& state = instance.state;\n";
			
			// paste in init state code
			w << scene->initStateCode;
			w.endScope();
		}
		w.endScope();
	}
	w.writeLine();

	// done function for scene instance
	w << "void doneInstance(void* pInstance)\n";
	{
		w.beginScope();
		w << "Instance& instance = *(Instance*)pInstance;\n";
		w.writeLine();

		// delete textures for large uniform arrays
		foreach (Pointer<LayerInfo> layerInfo, collector.layerInfos)
		{
			foreach (ShaderVariable& variable, layerInfo->materialLargeArrays)
			{
				w << "glDeleteTextures(1, &instance." << layerInfo->name
					<< ".uniform." << variable.name << ");\n";
			}
		}

		// delete dynamic vertex buffers for deformer output
		if (numDynamicBuffers > 0)
		{
			w << "glDeleteBuffers(" << numDynamicBuffers << ", instance.dynamicBuffers);\n";
			if (!options.mapBuffer)
				w << "free(instance.tempBuffer);\n";
		}

		// delete shapes
		foreach (ShapeInfoPair& p, collector.shapeInfoMap)
		{
			if (Pointer<TextMesh> textMesh = dynamicCast<TextMesh>(p.first))
			{
				StringRef name = p.second->getName();
				w << "free(instance." << name << ".vertices);\n";
			}
			else if (Pointer<ParticleSystem> particleSystem = dynamicCast<ParticleSystem>(p.first))
			{
				StringRef name = p.second->getName();
				w << "free(instance." << name << ".particles);\n";
			}
		}

		// delete vertex array objects
		if (useVertexArrayObjects)
		{
			w << "glDeleteVertexArrays(" << numVertexArrayObjects << ", instance.vertexArrays);\n";
		}				

		w.endScope();
	}
	w.writeLine();
			
	// addClip function for scene
	w << "void addClip(void* pInstance, int index, float* tracks, float time, float weight)\n";
	{
		w.beginScope();
	
		if (!scene->attributeSets.empty())
		{
			w << "Instance& instance = *(Instance*)pInstance;\n";
			w << "Global& global = *instance.global;\n";
			w.writeLine();
			
			w << "State& state = instance.state;\n";
			w << "const Buffers& buffers = global.buffers;\n";
			w.writeLine();		
			w << scene->addClipCode;
		}
			
		w.endScope();
	}
	w.writeLine();
	
	// update function for scene
	w << "void update(void* pInstance)\n";
	{
		w.beginScope();
		w << "Instance& instance = *(Instance*)pInstance;\n";
		w << "Global& global = *instance.global;\n";
		w << "++instance.sceneSequence;\n";
		w.writeLine();
		
		w << "State& state = instance.state;\n";
		w << "const Buffers& buffers = global.buffers;\n";
		w << "Uniforms& uniforms = instance.uniforms;\n";
		w << "Transform* transforms = instance.transforms;\n";
		w << "BoundingBox* boundingBoxes = instance.boundingBoxes;\n";
		w << "int& seed = instance.seed;\n";
		w.writeLine();		

		// the update code
		w << scene->updateCode;

		// write output to uniform struct for used deformers, shaders, particle systems and particle instancers
		collector.writeUniformOutput(w);
		
		// update shapes (particle systems)
		foreach (ShapeInfoPair& p, collector.shapeInfoMap)
		{
			if (Pointer<ParticleSystem> particleSystem = dynamicCast<ParticleSystem>(p.first))
			{
				// particle system
				StringRef name = p.second->getName();
				
				w.beginScope();

				// get particle pool of particle system
				w << "Particles" << name << "& pp = instance." << name << ";\n";
				
				// uniform
				w << "ParticleSystem" << name << "& uniform = uniforms." << particleSystem->path << ";\n";			

				// load variables from particle pool
				w << "Particle" << name << "* it = pp.alive;\n";
				w << "Particle" << name << "** last = &pp.alive;\n";
				w << "Particle" << name << "* dead = pp.dead;\n";
				w << "int numParticles = pp.numParticles;\n";

				// iterate over alive particles
				w << "while (it)\n";
				w.beginScope();
				w << "Particle" << name << "& particle = *it;\n";
				w << "bool alive = true;\n";
				
				// update code of particle
				w.beginScope();
				w << particleSystem->updateCode;
				w.endScope();
							
				// check if particle is still alive
				w << "if (alive)\n";
				w.beginScope();
				
				// yes: link into alive queue
				w << "*last = it;\n";
				w << "last = &it->next;\n";
			
				// next particle
				w << "it = it->next;\n";

				w.endScope();
				w << "else\n";
				w.beginScope();

				// no: link into dead queue
				w << "Particle" << name << "* next = it->next;\n";
				w << "it->next = dead;\n";
				w << "dead = it;\n";
				
				// next particle
				w << "it = next;\n";

				// decrement number of particles
				w << "--numParticles;\n";
				
				w.endScope();			
				w.endScope();

				// check if we have emitters
				if (particleSystem->emitterUniformType != null)
				{
					w.writeLine();
					w.beginScope();
					w << "Emitters" << name << "& emitter = instance.uniforms.e." << particleSystem->path << ";\n";			
					
					// the index selects the active emitter and counts overall number of particles to create
					w << "int index = emitter.numNewParticles;\n";

					// load id from particle pool
					w << "int id = pp.id;\n";

					// emit new particles, take from dead particles
					w << "while (dead && index > 0)\n";
					w.beginScope();
					w << "--index;\n";

					// get reference to particle state
					w << "Particle" << name << "& particle = *dead;\n";
					
					// code for all emitters (current one selected by index) and particle creation
					w.beginScope();
					w << particleSystem->emitterCode;				
					w.endScope();
					w.beginScope();
					w << particleSystem->createCode;
					w.endScope();
					
					// increment number of particles
					w << "++numParticles;\n";
					
					// increment particle id
					w << "++id;\n";
					
					// link this particle into alive list and move to next particle
					w << "*last = dead;\n";
					w << "last = &dead->next;\n";
					w << "dead = dead->next;\n";
					w.endScope();

					// write back to particle pool
					w << "pp.id = id;\n";
					w.endScope();
				}

				// end alive list and write back to particle pool
				w << "*last = NULL;\n";
				w << "pp.dead = dead;\n";
				w << "pp.numParticles = numParticles;\n";						
				w.endScope();
			}
		}

		w.endScope();
	}
	w.writeLine();
/*
	// undef macros for text meshes
	foreach (TextMeshInfoPair& p, collector.textMeshInfos)
	{
		TextMeshInfo& textMeshInfo = p.second;
		const std::string& name = textMeshInfo.name;
	
		w << "#undef macro" << name << "\n";
	}
	w.writeLine();
*/
	// getBoundingBox function for scene
	w << "void getBoundingBox(void* pInstance, float4x2& boundingBox)\n";
	{
		w.beginScope();
		w << "Instance& instance = *(Instance*)pInstance;\n";
		w << "Transform* transforms = instance.transforms;\n";
		w << "BoundingBox* boundingBoxes = instance.boundingBoxes;\n";
		w << "float3 minP = {};\n";
		w << "float3 maxP = {};\n";
		
		bool first = true;
		foreach (const SceneInfo::BoundingBoxPair& boundingBoxPair, sceneInfo.boundingBoxTransforms)
		{
			if (!boundingBoxPair.second.empty())
			{
				w.beginScope();
				int boundingBoxIndex = boundingBoxPair.first;
				w << "BoundingBox& bb = boundingBoxes[" << boundingBoxIndex << "];\n";
				w << "float3 bbs = bb.size;\n";
				
				foreach (int transformIndex, boundingBoxPair.second)
				{
					w.beginScope();
					w << "float4x4& m = transforms[" << transformIndex << "].matrix;\n";
					
					w << "float3 c = transformPosition(m, bb.center);\n";
					w << "float3 s = abs(m.x.xyz) * bbs.x + abs(m.y.xyz) * bbs.y + abs(m.z.xyz) * bbs.z;\n";
					if (first)
					{
						first = false;
						w << "minP = c - s;\n";
						w << "maxP = c + s;\n";
					}
					else
					{
						w << "minP = min(minP, c - s);\n";
						w << "maxP = max(maxP, c + s);\n";
					}
					w.endScope();
				}
				w.endScope();
			}
		}
		w << "boundingBox.x.xyz = (maxP + minP) * 0.5f;\n";
		w << "boundingBox.y.xyz = (maxP - minP) * 0.5f;\n";
		w.endScope();
	}
	w.writeLine();


	// write "set state" functions for all layers
	foreach (Pointer<LayerInfo> layerInfo, collector.layerInfos)
	{
		// create functions for each used render mode
		foreach (int renderMode, layerInfo->renderModes)
		{
			// render function
			w << "static void render_" << layerInfo->name << '_' << ident(renderMode) << "(RenderJob* renderJob)\n";
			{
				w.beginScope();
				w << "Instance& instance = *(Instance*)renderJob->instance;\n";
				w << "Global& global = *instance.global;\n";

				int numVertexAttributes = defaultNumVertexAttributes;
				if (layerInfo->sortMode == SORT_ALWAYS)
				{
					// set shader
					setShader(w, layerInfo);
					if (!useVertexArrayObjects)
						numVertexAttributes = enableVertexAttributes(w, numVertexAttributes, layerInfo);
				}
				else
				{
					// get reference to global shader struct (contains uniform handles)
					getShaderGlobal(w, layerInfo);
				}
				
				// get reference to transfer variables (material -> transform)
				w << "const ShaderTransfer" << layerInfo->name << "& transfer = "
					"instance." << layerInfo->name << ".transfer;\n";
			
				// get reference to world matrix (object space -> world space)
				w << "float4x4& matrix = renderJob->matrix;\n";
			
				// need flip flag if at least one shape is rendered from the front or back side
				if (layerInfo->useFlags & SHADER_USES_FLIP)
					w << "bool flip;\n";
				if (layerInfo->useFlags & SHADER_USES_ID)
					w << "int objectId = renderJob->id;\n";
			
				// create variables for shader uniforms
				printShaderVariables(w, layerInfo->transformVariables);
			
				// transfer transform into variables
				w << layerInfo->transformCode;

				// set variables to shader uniforms
				setUniforms(w, layerInfo->transformVariables, false);
				setTextures(w, layerInfo->transformVariables, false, getNumTextures(layerInfo->materialVariables));
				
				bool twoPassDoubleSided = renderMode & Mesh::DOUBLE_SIDED
					&& (layerInfo->renderFlags & LayerInfo::DOUBLE_SIDED_FLAG) == 0;
				if (layerInfo->shapeType == Shader::MESH)
				{
					// if two pass double sided we render front-back when opaque and back-front when transparent
					if (twoPassDoubleSided)
					{
						if (layerInfo->sortMode == SORT_MATERIAL)
							w << "flip ^= renderJob->distance > -1e30f;\n";
						if (layerInfo->sortMode == SORT_ALWAYS)
							renderMode ^= Mesh::BACK_SIDE;
					}
					
					// set cull mode. flip front and back if world matrix has negative determinant
					setCullMode(w, layerInfo, renderMode);
				}
				
				// call draw function
				w << "renderJob->draw(&instance, &shader);\n";
				
				// check if we have to render the other side separately (two pass double sided)
				if (twoPassDoubleSided)
				{
					// set cull mode to opposite side
					setCullMode(w, layerInfo, renderMode ^ Mesh::BACK_SIDE);
										
					// call draw function again
					w << "renderJob->draw(&instance, &shader);\n";
				}

				if (layerInfo->sortMode == SORT_ALWAYS)
				{
					// reset shader
					enableVertexAttributes(w, numVertexAttributes, defaultNumVertexAttributes);
					resetShader(w, layerInfo);
				}				
				w.endScope();
			}
			
			// generate second render function if sorting depends on material
			if (layerInfo->sortMode == SORT_MATERIAL)
			{
				w << "static void render_" << Ascii::toUpperCase(layerInfo->name[0]) << layerInfo->name.substr(1)
					<< '_' << ident(renderMode) << "(RenderJob* renderJob)\n";
				{
					w.beginScope();
					w << "Instance& instance = *(Instance*)renderJob->instance;\n";
					w << "Global& global = *instance.global;\n";
					
					// set shader
					setShader(w, layerInfo);
					int numVertexAttributes = defaultNumVertexAttributes;
					if (!useVertexArrayObjects)
						numVertexAttributes = enableVertexAttributes(w, numVertexAttributes, layerInfo);
					
					// render
					w << "render_" << layerInfo->name << '_' << ident(renderMode) << "(renderJob);\n";
					
					// reset shader
					enableVertexAttributes(w, numVertexAttributes, defaultNumVertexAttributes);
					resetShader(w, layerInfo);
					
					w.endScope();
				}
			}
		}
	}

	// write draw functions for all shape instancers
	foreach (ShapeInstancerInfoPair& p, collector.shapeInstancerInfoMap)
	{
		Pointer<ShapeInstancer> shapeInstancer = p.first;
		ShapeInstancerInfo& shapeInstancerInfo = p.second;
		
		foreach (RenderInfoPair& p, shapeInstancerInfo.renderInfoMap)
		{
			RenderInfo& renderInfo = p.second;
			Pointer<LayerInfo> layerInfo = renderInfo.layerInfo;

			// render function (returns resetShader function)
			w << "static void draw_" << renderInfo.drawFunctionName << "(void* pInstance, void* pShader)\n";
			w.beginScope();
			w << "Instance& instance = *(Instance*)pInstance;\n";
			w << "Global& global = *instance.global;\n";
			
			// check if we have a scale offset array to set
			if (layerInfo->useFlags & SHADER_USES_SCALE_OFFSET)
			{
				getShaderGlobal(w, layerInfo);
				w << "glUniform4fv(shader.s_o, " << renderInfo.scaleOffsetArraySize << ", "
					"global.scaleOffset[" << renderInfo.scaleOffsetArrayIndex << "]);\n";
			}

			int lastVertexBigBufferIndex = -1;
			int lastIndexBigBufferIndex = -1;
			bindAndDraw(w, collector, shapeInstancer, shapeInstancerInfo,
				renderInfo, quadsIndexType, useVertexArrayObjects, 1,
				lastVertexBigBufferIndex, lastIndexBigBufferIndex);
			w.endScope();
		}
	}
	w.writeLine();
	

	// main render function
	w << "void render(void* pInstance, "
		"const float4x4& viewMatrix, const float4x4& projectionMatrix, int layerIndex, "
		"RenderQueues& renderQueues)\n";
	{
		w.beginScope();
		w << "Instance& instance = *(Instance*)pInstance;\n";
		w << "Global& global = *instance.global;\n";
		w << "instance.renderSequence = ++global.sequence;\n";
		w << "float4x4 viewProjectionMatrix = projectionMatrix * viewMatrix;\n";
		if (collector.shaderUseFlags & SHADER_USES_VIEWPORT)
		{
			w << "float viewport[4];\n";
			w << "glGetFloatv(GL_VIEWPORT, viewport);\n";
		}
		w.writeLine();
		
		// update deformers if necessary
		if (numDynamicBuffers > 0)
		{
			w << "if (instance.sceneSequence != instance.deformerSequence)\n";
			w.beginScope();
			w << "instance.deformerSequence = instance.sceneSequence;\n";
			w.writeLine();
			
			// process sources of dynamic buffers (deformers, sprite particle instancers)
			processDynamicBuffers(w, collector, options.mapBuffer);			

			w.endScope(); // if
			w.writeLine();
		}
				
		// render selected layer
		bool hasLayers = collector.numLayers != 1 || !shaderOptions[0].layerName.empty();
		if (hasLayers)
		{
			w << "switch (layerIndex)\n";
			w.beginScope();
		}
				
		for (int layerIndex = 0; layerIndex < collector.numLayers; ++layerIndex)
		{
			const ShaderOptions& so = shaderOptions[layerIndex];
		
			if (hasLayers)
			{
				w.decIndent(); w << "case " << so.layerIndex << ":\n"; w.incIndent();		
				w.beginScope();
			}
						
			// setup: iterate over shaders and calc material uniform and transfer variables
			// todo: render all non-transparent primitives immediately if possible
			bool textureBound = false;
			foreach (ShaderInfoPair& shaderInfoPair, collector.shaderInfoMap)
			{
				Pointer<Shader> shader = shaderInfoPair.first;
				ShaderInfo& shaderInfo = shaderInfoPair.second;		
				
				// iterate over deformer/shader combos (only one with deformer = null when deformers not in shaders)
				foreach (DeformerShaderInfoPair& p, shaderInfo.deformerShaderInfos)
				{
					Pointer<Deformer> deformer = p.first;
					DeformerShaderInfo& deformerShaderInfo = p.second;

					// iterate over shader type (MESH, PARTICLE)
					foreach (TargetShaderInfoPair& p, deformerShaderInfo)
					{
						// get layer if it exists
						Pointer<LayerInfo> layerInfo = getValue(p.second.layerInfoMap, layerIndex);
						
						if (layerInfo != null)
						{
							w.writeComment(layerInfo->comment);
							w.beginScope();

							/*
								behavior dependent on sort mode:
								SORT_NEVER    : render immediately
								SORT_MATERIAL : check sort value at runtime. defer and sort if true (shader output code writes to sort flag)
								SORT_ALWAYS   : always defer and sort
							*/

							// calc shader material
						
							// get reference to shader global data (program and uniform handles)
							//w << "ShaderGlobal" << layerInfo->name << "& shader = global." << layerInfo->name << ";\n";

							// deformer uniform (from scene)
							if (deformer)
							{
								DeformerInfo& deformerInfo = collector.deformerInfoMap[deformer];
								w << "Deformer" << deformerInfo.name << "& deformer = instance.uniforms." << deformer->path << ";\n";
							}
						
							// shader material (from scene)
							w << "Material" << shaderInfo.typeName << "& material = instance.uniforms." << shader->path << ";\n";

							// shader instance
							w << "Shader" << layerInfo->name << "& si = instance." << layerInfo->name << ";\n";
										
							// uniform (to vertex and pixel shader)
							w << "ShaderUniform" << layerInfo->name << "& uniform = si.uniform;\n";

							// uniform large arrays that are stored in textures
							printShaderVariables(w, layerInfo->materialLargeArrays);

							// transfer (to transform program)
							w << "ShaderTransfer" << layerInfo->name << "& transfer = si.transfer;\n";

							if (layerInfo->sortMode == SORT_MATERIAL)
								w << "bool sort = false;\n";
							
							// write code that transfers material into shader variables (uniform and transfer)
							w << layerInfo->materialCode;

							// set large arrays to textures
							foreach (const ShaderVariable& variable, layerInfo->materialLargeArrays)
							{
								w << "glBindTexture(GL_TEXTURE_2D, uniform." << variable.name << ");\n";
								w << "glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, " << variable.type.numElements
									<< ", 1, GL_RGBA, GL_FLOAT, " << variable.name << ");\n";
								textureBound = true;
							}

							// set sort flag
							if (layerInfo->sortMode == SORT_MATERIAL)
								w << "si.sort = sort;\n";

							// clear render job queue of shader
							if (layerInfo->sortMode != SORT_ALWAYS)
								w << "si.renderJobs = NULL;\n";
			
							w.endScope();
						}
					}
				}
			}
			if (textureBound)
				w << "glBindTexture(GL_TEXTURE_2D, 0);\n";
			w.writeLine();

			int numVertexAttributes = defaultNumVertexAttributes;
				
			// create render jobs
			{
				// render bounding boxes
				if (options.renderBoundingBoxes)
				{
					// set bounding box shader
					w << "glUseProgram(global.bbProgram);\n";

					if (useVertexArrayObjects)
					{
						// bind vertex array object
						w << "glBindVertexArray(global.bbVertexArray);\n";
					}
					else
					{
						// enable vertex attribute 0
						numVertexAttributes = enableVertexAttributes(w, numVertexAttributes, 1);

						// bind bounding box vertex buffer (8 positions) and set to vertex attribute 0
						w << "glBindBuffer(GL_ARRAY_BUFFER, global.bbVertexBuffer);\n";
						w << "glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (GLvoid*)0);\n";

						// bind bounding box index buffer (12 lines)
						w << "glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, global.bbIndexBuffer);\n";
					}
									
					// set view projection matrix
					w << "glUniformMatrix4fv(global.bbViewProjectionMatrix, 1, false, &viewProjectionMatrix.x.x);\n";

					// get uniform location of world matrix 
					w << "GLuint hbbMatrix = global.bbMatrix;\n";
				}
				
				// (recursively) iterate over instances
				w << "RenderJob* jobIt = renderQueues.begin;\n";
				w << "RenderJob* jobEnd = renderQueues.end;\n";
				int objectIndex = 0;
				foreach (const NamedInstance& instance, scene->instances)
				{
					// render if layer is default layer or if instance is in layer
					if (collector.isInLayer(layerIndex, instance))
					{
						RenderJobsInfo info(objectIndex, layerIndex);
						createRenderJobs(w, collector, info, instance, StringRef(), options.renderBoundingBoxes);
					}
					++objectIndex;
				}
				
				// write back only begin iterator. the shader jobs are allocated from the end and thrown away after rendering
				w << "renderQueues.begin = jobIt;\n";
			}
						
			// render per-shader render job queues that contain non-transparent render jobs
			foreach (ShaderInfoPair& shaderInfoPair, collector.shaderInfoMap)
			{
				ShaderInfo& shaderInfo = shaderInfoPair.second;
				
				// iterate over deformer/shader combos (only one with deformer = null when deformers not in shaders)
				foreach (DeformerShaderInfoPair& p, shaderInfo.deformerShaderInfos)
				{
					DeformerShaderInfo& deformerShaderInfo = p.second;
					
					// iterate over shader type (MESH, PARTICLE)
					foreach (TargetShaderInfoPair& p, deformerShaderInfo)
					{
						// get layer if it exists
						Pointer<LayerInfo> layerInfo = getValue(p.second.layerInfoMap, layerIndex);
						
						if (layerInfo != null && layerInfo->sortMode != SORT_ALWAYS)
						{
							w.beginScope();
							
							// set shader
							setShader(w, layerInfo);
							
							// set vertex bindings
							if (!useVertexArrayObjects)
								numVertexAttributes = enableVertexAttributes(w, numVertexAttributes, layerInfo);
							
							// render
							w << "RenderJob* current = instance." << layerInfo->name << ".renderJobs;\n";
							w << "while (current)\n";
							w.beginScope();
							w << "current->render(current);\n";
							w << "current = current->next;\n";
							w.endScope();
							
							// reset shader
							resetShader(w, layerInfo);
							
							w.endScope();
						}
					}
				}
			}

			// reset vertex bindings
			enableVertexAttributes(w, numVertexAttributes, defaultNumVertexAttributes);

			if (hasLayers)
			{
				w.endScope(); // case
				w << "break;\n";
			}
		}
		if (hasLayers)
			w.endScope(); // switch
		
		w.endScope(); // render()
	}
	w.writeLine();

	return !hasError;
}

} // namespace digi
