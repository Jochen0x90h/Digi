#include <digi/Utility/Ascii.h>
#include <digi/Utility/foreach.h>
#include <digi/Utility/IntUtility.h>
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
#include "generateSceneWebGL.h"


namespace digi {

namespace
{
	// default number of enabled vertex attributes
	const int defaultNumVertexAttributes = 2;

	// convert buffer format to js array type (needed for named buffers)
	StringRef toJSTypedArray(BufferFormat format)
	{
		VectorInfo type = toVectorInfo(format);
		
		switch (type.type)
		{
		case VectorInfo::BYTE:
			return "Int8Array";
		case VectorInfo::UBYTE:
			return "Uint8Array";
		case VectorInfo::SHORT:
			return "Int16Array";
		case VectorInfo::USHORT:
			return "Uint16Array";
		case VectorInfo::INT:
			return "Int32Array";
		case VectorInfo::UINT:
			return "Uint32Array";
		case VectorInfo::FLOAT:
			return "Float32Array";
		case VectorInfo::DOUBLE:
			return "Float64Array";
		default:
			return StringRef();
		}
	}

// array helpers

	//void allocateTempArrays(const std::vector<ShaderVariable>& variables, std::vector<int>& intTempArrays, std::vector<int>& floatTempArrays)
	void allocateTempArrays(const std::vector<ShaderVariable>& variables, std::vector<std::set<int> >& intTempArrays,
		std::vector<std::set<int> >& floatTempArrays)
	{
		int intIndex = 0;
		int floatIndex = 0;
		foreach (const ShaderVariable& variable, variables)
		{
			int length = std::max(variable.type.numElements, 1) * variable.type.numRows;

			if (variable.type.type == ShaderType::INT)
			{
				/*
				if (intIndex == intTempArrays.size())
					intTempArrays.push_back(0);
				intTempArrays[intIndex] = max(length, intTempArrays[intIndex]);
				*/
				if (intIndex == intTempArrays.size())
					add(intTempArrays);
				intTempArrays[intIndex].insert(length);
				++intIndex;
			}
			else if (variable.type.type == ShaderType::FLOAT)
			{
				/*
				if (floatIndex == floatTempArrays.size())
					floatTempArrays.push_back(0);
				floatTempArrays[floatIndex] = max(length, floatTempArrays[floatIndex]);
				*/
				if (floatIndex == floatTempArrays.size())
					add(floatTempArrays);
				floatTempArrays[floatIndex].insert(length);
				++floatIndex;
			}
		}
	}

	void getTempArrays(CodeWriter& w, const std::vector<ShaderVariable>& variables)
	{
		int intIndex = 0;
		int floatIndex = 0;
		foreach (const ShaderVariable& variable, variables)
		{
			int length = std::max(variable.type.numElements, 1) * variable.type.numRows;

			/*
			if (variable.type.type == ShaderType::INT)
				w << "var " << variable.name << " = instance.iu" << ident(intIndex++) << ";\n";
			else if (variable.type.type == ShaderType::FLOAT)
				w << "var " << variable.name << " = instance.fu" << ident(floatIndex++) << ";\n";
			*/
			if (variable.type.type == ShaderType::INT)
				w << "var " << variable.name << " = instance.iu" << ident(intIndex++) << '_' << ident(length) << ";\n";
			else if (variable.type.type == ShaderType::FLOAT)
				w << "var " << variable.name << " = instance.fu" << ident(floatIndex++) << '_' << ident(length) << ";\n";
		}
	}

	void getArrayReferences(CodeWriter& w, const std::vector<ShaderVariable>& variables, StringRef prefix)
	{
		foreach (const ShaderVariable& variable, variables)
		{
			w << "var " << variable.name << " = " << prefix << variable.name << ";\n";
		}
	}

	void addArrays(CodeWriter& w, StringRef name, int3 size, bool newLine = true)
	{
		if (size.x > 0)
			w << 'i' << name << ": new Int32Array(" << size.x << "), ";
		if (size.y > 0)
			w << 'f' << name << ": new Float32Array(" << size.y << "), ";
		if (size.z > 0)
			w << 'o' << name << ": [], ";
		if (newLine)
			w << "\n";
	}
	
	void getArrays(CodeWriter& w, StringRef container, StringRef name, int3 size)
	{
		if (size.x > 0)
			w << "var i" << name << " = " << container << ".i" << name << ";\n";
		if (size.y > 0)
			w << "var f" << name << " = " << container << ".f" << name << ";\n";
		if (size.z > 0)
			w << "var o" << name << " = " << container << ".o" << name << ";\n";
	}

// dynamic buffers

	// process deformer jobs of dynamic buffers (deformers, sprite particles)
	void processDynamicBuffers(CodeWriter& w, GeneratorCollector& collector)
	{									
		// iterate over dynamic buffers
		int index = 0;
		int viewIndex = 0;
		foreach (DynamicBufferInfo& dynamicBufferInfo, collector.dynamicBufferInfos)
		{
			// bind output buffer
			w << "gl.bindBuffer(gl.ARRAY_BUFFER, instance.dynamicBuffer" << ident(index) << ");\n";

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
					//w << "Deformer" << this->deformerInfos[deformer].typeName
					//	<< "& deformer = instance.uniforms." << deformer->path << ";\n";

					// print uniform variables for transfer to vertex loop
					//printShaderVariables(w, deformerJobInfo.uniformVariables);

					// execute uniform code
					w << deformerJobInfo.uniformCode;
				}

				// offset/size in dynamic buffer
				size_t bufferOffset = deformerJobInfo.dynamicBufferOffset;
				Pointer<ShapeInfo> shapeInfo = collector.shapeInfoMap[shape];
				std::string numVertices;
				std::string bufferSize;
				bool hasParticleSystem = false;
				if (Pointer<ConstantMesh> constantMesh = dynamicCast<ConstantMesh>(deformerJobInfo.shape))
				{
					// is a constant mesh
					int nv = shapeInfo->getNumVertices();
					numVertices = Code(w.getLanguage()) << nv;
					bufferSize = Code(w.getLanguage()) << nv * deformerJobInfo.vertexSize;
				}
				else if (Pointer<TextMesh> textMesh = dynamicCast<TextMesh>(deformerJobInfo.shape))
				{
					// is a text mesh
					StringRef name = shapeInfo->getName();

				}
				else if (Pointer<ParticleSystem> particleSystem = dynamicCast<ParticleSystem>(deformerJobInfo.shape))
				{
					// is a particle system
					StringRef name = shapeInfo->getName();					
					w << "var numParticles = instance.particlePools." << name << ".numParticles;\n";
					numVertices = "numParticles";
					bufferSize = Code(w.getLanguage()) << "numParticles * " << deformerJobInfo.vertexSize;

					// iterator for alive particles
					w << "var _p = instance.particlePools." << name << ".next;\n"; // alive
					hasParticleSystem = true;
				}

				// input buffers
				typedef std::pair<const Pointer<VertexBuffer>, size_t> VertexBufferPair;
				std::map<Pointer<VertexBuffer>, size_t> vertexBuffers;
				//bool hasParticleSystem = false;
					
				// iterate over input vertex bindings
				foreach (DeformerJobInfo::VertexBindingPair& p, deformerJobInfo.vertexBindings)
				{
					DeformerJobIndex::const_iterator it = deformerJobIndex.find(p.first);
					if (it != deformerJobIndex.end())
					{
						const FieldOfObject& fieldOfObject = it->second;
						
						// get field by name and check if it is a BufferVertexField
						/*if (Pointer<Buffer> buffer = dynamicCast<Buffer>(fieldOfObject.object))
						{
							// is a buffer
								
							// get the (fat) vertex buffer that the buffer is part of
							Pointer<VertexBuffer> vertexBuffer = this->bufferInfos[buffer].scopes[SCOPE_DEFORMER].vertexBuffer;
								
							// try to insert vertex buffer with new index
							size_t index = vertexBuffers.size();
							if (vertexBuffers.insert(std::make_pair(vertexBuffer, index)).second)
							{
								// new vertex buffer: print pointer variable
								w << "ubyte* vertex" << ident(index) << " = global.deformerInputBuffers"
									"[" << vertexBuffer->bigBufferIndex << "] + " << vertexBuffer->bigBufferOffset << ";\n";
							}
						}
						else*/
						/*if (Pointer<ParticleSystem> particleSystem = dynamicCast<ParticleSystem>(fieldOfObject.object))
						{
							// is a particle system (can be only one)
							if (!hasParticleSystem)
							{
								hasParticleSystem = true;
								ParticleSystemInfo& particleSystemInfo = this->particleSystemInfos[particleSystem];
									
								// iterator for alive particles
								w << "var p_ = instance.particlePools." << particleSystemInfo.name << ".next;\n"; // alive
							}
						}*/
					}
				}

				// get views on result vertex buffer for each type (s8, u8, s16, u16, s32, u32, f32)
				w << "var f32d = instance.d" << ident(viewIndex) << ";\n";
				
				// index for each type size in result vertex buffer (1, 2, 4)
				w << "var i_4 = 0;\n";
					
				// vertex loop (i_ does not clash with auto-generated variable names such as _i and i)
				w << "for (var i_ = 0; i_ < " << numVertices << "; ++i_)\n";
				w.beginScope();

				if (hasParticleSystem)
					w << "var f_p = _p.f_p;\n";

				// get vertex code so that replacements can be made
				std::string vertexCode = deformerJobInfo.vertexCode;
					
				// replace vertex variables
				// iterate over input vertex bindings
				foreach (DeformerJobInfo::VertexBindingPair& p, deformerJobInfo.vertexBindings)
				{
					// get object/field from deformer job index
					DeformerJobIndex::const_iterator it = deformerJobIndex.find(p.first);
					if (it != deformerJobIndex.end())
					{
						const FieldOfObject& fieldOfObject = it->second;
						ShaderVariable& var = p.second;
						
						/*if (Pointer<Buffer> buffer = dynamicCast<Buffer>(fieldOfObject.object))
						{
							// is a buffer
								
							// get buffer info
							BufferInfo& bufferInfo = this->bufferInfos[buffer];
							BufferFormat& format = bufferInfo.format;
							BufferInfo::Scope& scope = bufferInfo.scopes[SCOPE_DEFORMER];
								
							// get vertex buffer index
							size_t index = vertexBuffers[scope.vertexBuffer];
								
							// get field offset
							int fieldOffset = scope.vertexBuffer->fields[scope.fieldIndex].fieldOffset;
								
							// source type and value
							VectorInfo srcType = toVectorInfo(bufferInfo.format);
							std::string srcValue = Code() << "*(" << srcType.toString() << "*)(vertex" << ident(index)
								<< " + " << fieldOffset << ")";
								
							// assign and convert if necessary (depacking of combined formats is done in compileDeformers())
							printAssign(w, srcValue, toBufferFormat(srcType), var.type);
							w << ";\n";
						}
						else*/ if (Pointer<ParticleSystem> particleSystem = dynamicCast<ParticleSystem>(fieldOfObject.object))
						{
							// is a particle system
							Pointer<ParticleSystemInfo> particleSystemInfo =
								staticCast<ParticleSystemInfo>(collector.shapeInfoMap[particleSystem]);
							int3 offset = particleSystemInfo->particlePrinter->getOffset('.' + fieldOfObject.fieldName);
								
							int numRows = max(1, var.type.numElements);
							for (int i = 0; i < numRows; ++i)
							{								
								// get variable name (e.g. "_0")
								std::string variableName = var.name;

								// add vector component (e.g. "_0x")
								if (numRows > 1)
									variableName += "xyzw"[i];

								// string-replace by array lookup (e.g. in particle state)
								std::string arrayLookup;
								if (var.type.type == ShaderType::FLOAT)
									arrayLookup = Code(w.getLanguage()) << "f_p[" << offset.y + i << "]";								
								replaceString(vertexCode, variableName, arrayLookup);
							}
						}
					}
				}

				// replace result variables
				foreach (DeformerJobInfo::FieldInfo& fieldInfo, deformerJobInfo.fieldInfos)
				{
					std::string variableNameBase = deformerJobInfo.resultBindings[fieldInfo.name].name;
					VectorInfo type = toVectorInfo(fieldInfo.format);
					int numRows = type.numRows;
						
					for (int i = 0; i < numRows; ++i)
					{
						// get variable name (e.g. "res0")
						std::string variableName = variableNameBase;

						// add vector component (e.g. "res0x")
						if (numRows > 1)
							variableName += "xyzw"[i];
							
						// string-replace by array lookup in result vertex
						std::string arrayLookup;
						if (type.type == VectorInfo::FLOAT)
							arrayLookup = Code(w.getLanguage()) << "f32d[i_4 + " << fieldInfo.offset / 4 + i << "]";							
						replaceString(vertexCode, variableName, arrayLookup);
					}
				}
					
				// execute per-vertex code
				w << vertexCode;

				w.writeLine();
					
				// next input vertex for generated particles/vertices
				if (hasParticleSystem)
					w << "_p = _p.next;\n";

				// next result vertex
				w << "i_4 += " << deformerJobInfo.vertexSize / 4 << ";\n";

				// end loop
				w.endScope();			
					
				// set output buffer
				w << "gl.bufferSubData(gl.ARRAY_BUFFER, " << bufferOffset << ", f32d);\n";

				// end deformer
				w.endScope();

				w.writeLine();
				++viewIndex;
			}			

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
			return GLType(type.numRows, "gl.BYTE", normalize);
		case VectorInfo::UBYTE:
			return GLType(type.numRows, "gl.UNSIGNED_BYTE", normalize);
		case VectorInfo::SHORT:
			return GLType(type.numRows, "gl.SHORT", normalize);
		case VectorInfo::USHORT:
			return GLType(type.numRows, "gl.UNSIGNED_SHORT", normalize);
		case VectorInfo::INT:
			return GLType(type.numRows, "gl.INT", normalize);
		case VectorInfo::UINT:
			return GLType(type.numRows, "gl.UNSIGNED_INT", normalize);
		case VectorInfo::FLOAT:
			return GLType(type.numRows, "gl.FLOAT", normalize);
		default:
			return GLType();
		}
	}

	void setVertexAttribPointer(CodeWriter& w, int index, const BufferFormat& format, int vertexSize, size_t offset)
	{
		// get opengl type from field format
		GLType type = getType(format);
		
		// set pointer to field in buffer
		w << "gl.vertexAttribPointer("
			<< index << ", "
			<< type.numComponents << ", "
			<< type.componentType << ", "
			<< (type.normalize ? "true" : "false") << ", "
			<< vertexSize << ", "
			<< offset << ");\n";
	}

	int enableVertexAttributes(CodeWriter& w, int lastNumVertexAttributes, int numVertexAttributes)
	{
		for (int i = lastNumVertexAttributes - 1; i >= numVertexAttributes; --i)
		{
			w << "gl.disableVertexAttribArray(" << i << ");\n";
		}
		for (int i = lastNumVertexAttributes; i < numVertexAttributes; ++i)
		{
			w << "gl.enableVertexAttribArray(" << i << ");\n";
		}
		return numVertexAttributes;
	}

	int enableVertexAttributes(CodeWriter& w, int lastNumVertexAttributes, Pointer<LayerInfo> layerInfo)
	{
		return enableVertexAttributes(w, lastNumVertexAttributes, int(layerInfo->vertexBindings.size()));
	}

	// set vertex state for mesh and sprites
	void setVertexState(CodeWriter& w, GeneratorCollector& collector, Pointer<LayerInfo> layerInfo,
		Pointer<ShapeInstancer> shapeInstancer, ShapeInstancerInfo& shapeInstancerInfo, int baseIndex,
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
								w << "gl.bindBuffer(gl.ARRAY_BUFFER, instance.dynamicBuffer"
									<< ident(deformerJobInfo.dynamicBufferIndex) << ");\n";
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
						w << "gl.bindBuffer(gl.ARRAY_BUFFER, global.shaderInputBuffer"
							<< ident(vertexBuffer->bigBufferIndex) << ");\n";
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

// shader helpers

	void getHandles(CodeWriter& w, const std::vector<ShaderVariable>& variables, int& textureIndex)
	{
		foreach (const ShaderVariable& variable, variables)
		{
			if (variable.type.numElements == 0)
			{
				if (variable.type.type == ShaderType::TEXTURE)
				{
					w << "gl.uniform1i(gl.getUniformLocation(program, '" << variable.name << "'), " << textureIndex++ << ");\n";
					continue;
				}
			}

			w << "shader." << variable.name << " = gl.getUniformLocation(program, '" << variable.name << "');\n"; 
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

				w << "gl.uniform" << ident(variable.type.numRows) << type << "v(shader." << variable.name << ", "
					<< prefix << variable.name << ");\n";
			}
		}
	}

	const char* textureTypes[] = {"?", "gl.TEXTURE_1D", "gl.TEXTURE_2D", "gl.TEXTURE_3D", "gl.TEXTURE_CUBE_MAP"};

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
					
					// activate texture stage
					if (textureIndex != 0)
						w << "gl.activeTexture(gl.TEXTURE" << ident(textureIndex) << ");\n";
					textureIndex++;
					
					// bind texture to texture stage
					w << "gl.bindTexture(" << textureTypes[samplerType] << ", " << prefix << variable.name << ");\n";
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

					// activate texture stage. don't net to activate the texture that was still active
					--textureIndex;
					if (textureIndex != activeIndex)
						w << "gl.activeTexture(gl.TEXTURE" << ident(textureIndex) << ");\n";
					
					// bind texture to texture stage
					w << "gl.bindTexture(" << textureTypes[samplerType] << ", null);\n";
				}
			}
		}	
	}

	void getShaderGlobal(CodeWriter& w, Pointer<LayerInfo> layerInfo)
	{
		StringRef name = layerInfo->name;
		if (layerInfo->equalShaderLayerInfo != NULL)
			name = layerInfo->equalShaderLayerInfo->name;
		w << "var shader = global.shaders." << name << ";\n";
	}

	void setShader(CodeWriter& w, Pointer<LayerInfo> layerInfo)
	{
		// get reference to global shader struct (contains uniform handles)
		getShaderGlobal(w, layerInfo);
	
		// get reference to global shader struct
		StringRef name = layerInfo->name;
		if (layerInfo->equalShaderLayerInfo != NULL)
			name = layerInfo->equalShaderLayerInfo->name;
		w << "var shader = global.shaders." << name << ";\n";

		// set shader program
		w << "gl.useProgram(shader.program);\n";

		// set material uniforms
		w << "var uniform = instance.shaders." << layerInfo->name << ".uniform;\n";
		setUniforms(w, layerInfo->materialVariables, true);
		setTextures(w, layerInfo->materialVariables, true, 0);

		if (layerInfo->shapeType == Shader::MESH)
		{
			if (layerInfo->renderFlags == LayerInfo::DOUBLE_SIDED_FLAG)
				w << "gl.disable(gl.CULL_FACE);\n";
			else if (layerInfo->renderFlags == (layerInfo->renderFlags & (LayerInfo::FRONT_SIDE_FLAG | LayerInfo::BACK_SIDE_FLAG)))
				w << "gl.enable(gl.CULL_FACE);\n";
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
				w << "gl.disable(gl.CULL_FACE);\n";
		}
		else
		{
			// front, back or two pass double sided
			
			// check if the shader uses the frontFacing uniform variable (not the shader built-in variable)
			if (layerInfo->useFlags & SHADER_USES_FRONT_FACING)
			{
				w << "gl.uniform1f(shader.f_f, flip ? ";
				if ((renderMode & Mesh::BACK_SIDE) == 0)
					w << "-1.0 : 1.0";
				else
					w << "1.0 : -1.0";
				w << ");\n";
			}
			
			if (layerInfo->renderFlags != (layerInfo->renderFlags & (LayerInfo::FRONT_SIDE_FLAG | LayerInfo::BACK_SIDE_FLAG)))
				w << "gl.enable(gl.CULL_FACE);\n";
			if (renderMode & Mesh::BACK_SIDE)
				w << "gl.cullFace(flip ? gl.BACK : gl.FRONT);\n";
			else
				w << "gl.cullFace(flip ? gl.FRONT : gl.BACK);\n";
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
		// set world matrix
		w << "gl.uniformMatrix4fv(hbbMatrix, false, bbMatrix);\n";

		// draw (12 lines, 24 indices)
		w << "gl.drawElements(gl.LINES, 24, gl.UNSIGNED_SHORT, 0);\n";
	}

	int createRenderJobs(CodeWriter& w, GeneratorCollector& collector, Compiler& compiler, RenderJobsInfo& info,
		Pointer<Instancer> instancer, int depth, int3 transformOffset,
		NameGenerator nameGenerator, bool renderBoundingBoxes);

	int createRenderJobs(CodeWriter& w, GeneratorCollector& collector, Compiler& compiler, RenderJobsInfo& info,
		const Instance& instance, int depth,
		NameGenerator nameGenerator, bool renderBoundingBoxes)
	{
		int3 transformOffset = collector.transformsPrinter->getOffset(Code() << '[' << instance.transformIndex << ']');

		// check if visible
		w << "if (itransforms[" << transformOffset.x << "])\n";
		w.beginScope();

		// pass nameGenerator by value so that new variable names are only generated for nested createRenderJobs
		int numRenderJobs = createRenderJobs(w, collector, compiler, info,
			instance.instancer, depth, transformOffset, nameGenerator, renderBoundingBoxes);

		w.endScope(); // visible?
			
		return numRenderJobs;
	}
		
	int createRenderJobs(CodeWriter& w, GeneratorCollector& collector, Compiler& compiler, RenderJobsInfo& info,
		Pointer<Instancer> instancer, int depth, int3 transformOffset,
		NameGenerator nameGenerator, bool renderBoundingBoxes)
	{
		if (Pointer<MultiInstancer> multiInstancer = dynamicCast<MultiInstancer>(instancer))
		{
			int numRenderJobs = 0;
			foreach (Pointer<Instancer> instancer, multiInstancer->instancers)
			{
				numRenderJobs += createRenderJobs(w, collector, compiler, info,
					instancer, depth, transformOffset, nameGenerator, renderBoundingBoxes);
			}
			return numRenderJobs;
		}
		else if (Pointer<ShapeInstancer> shapeInstancer = dynamicCast<ShapeInstancer>(instancer))
		{
			// shape (mesh, sprite particles)

			// get shape info
			ShapeInstancerInfo& shapeInstancerInfo = collector.shapeInstancerInfoMap[shapeInstancer];
				
			// get layer (may be missing if failed to compile)
			RenderInfoMap::iterator it = shapeInstancerInfo.renderInfoMap.find(info.layerIndex);
			if (it != shapeInstancerInfo.renderInfoMap.end())
			{
				RenderInfo& renderInfo = it->second;
				int renderMode = renderInfo.renderMode;
				Pointer<LayerInfo> layerInfo = renderInfo.layerInfo;

				// check if we are out of render jobs
				w << "if (jobIt != jobEnd)\n";
				w.beginScope();
				w << "var shader = instance.shaders." << layerInfo->name << ";\n";
				w << "var renderJob;\n";
					
				// add render job to shader queue or sorted queue
				if (layerInfo->sortMode == SORT_MATERIAL)
				{
					w << "if (!shader.sort)\n";
					w.beginScope();
				}
				if (layerInfo->sortMode != SORT_ALWAYS)
				{
					// not sorted: get render job from end of global render job array
					w << "renderJob = jobEnd;\n";
					w << "jobEnd = jobEnd.p;\n";

					// append render job to shader queue
					w << "renderJob.next = shader.renderJobs;\n";
					w << "shader.renderJobs = renderJob;\n";	
				
					// set render function
					w << "renderJob.render = global.render." << layerInfo->name << '_' << ident(renderMode) << ";\n";
				}
				if (layerInfo->sortMode == SORT_MATERIAL)
				{
					w.endScope();
					w << "else\n";
					w.beginScope();
				}
				if (layerInfo->sortMode != SORT_NEVER)
				{
					// sorted: get render job from begin of global render job array
					w << "renderJob = jobIt;\n";
					w << "jobIt = jobIt.n;\n";
						
					// append render job to global alpha-sort queue
					w << "renderJob.next = renderQueues.alphaSort;\n";
					w << "renderQueues.alphaSort = renderJob;\n";

					// set render function
					if (layerInfo->sortMode == SORT_ALWAYS)
						w << "renderJob.render = global.render." << layerInfo->name << '_' << ident(renderMode) << ";\n";
				}
				if (layerInfo->sortMode == SORT_MATERIAL)
				{
					// set second render function
					w << "renderJob.render = global.render." << Ascii::toUpperCase(layerInfo->name[0])
						<< layerInfo->name.substr(1) << '_' << ident(renderMode) << ";\n";
					w.endScope();
				}

				// create code that calculates matrix and z-distance (if sorting) of render job
				std::string code;
				{
					Pointer<IODevice> dev = new StringRefDevice(code);
					CodeWriter w(dev);
						
					// variables
					if (depth > 0)
					{
						// trick: variable names for output matrix of parent are reused for parentMatrix
						writeBindingVariable(w, "parentMatrix", "float4x4");
					}
					writeBindingVariable(w, "viewProjectionMatrix", "float4x4");
					writeBindingVariable(w, "transform", "Transform");
					writeBindingVariable(w, "bb", "BoundingBox");
					writeBindingVariable(w, "data", Type::create("{matrix float4x4,distance float}"));
					if (renderBoundingBoxes)
						writeBindingVariable(w, "bbMatrix", "float4x4"); // output
					w.writeLine();
						
					// main
					w << "extern \"C\" void main()\n";
					w.beginScope();
						
					// calc matrix and assign to render job
					if (depth > 0)
						w << "float4x4 matrix = parentMatrix * transform.matrix;\n";
					else
						w << "float4x4 matrix = transform.matrix;\n"; // no reference leads to better output code
					if (renderBoundingBoxes)
						w << "bbMatrix = matrix * matrix4x4TranslateScale(bb.center, bb.size);\n";
					
					w << "data.matrix = matrix;\n";
					if (layerInfo->sortMode != SORT_NEVER)
					{						
						// calc position of object in homogenous clip space
						w << "float4 position = viewProjectionMatrix * (matrix * vector4(bb.center, 1.0f));\n";

						// calc distance in normalized device space, ranges from [-1, 1]
						w << "data.distance = position.z / position.w;\n";						
					}
					w.endScope(); // main
					w.close();
				}
					
				// compile render job code
				std::string outputCode;
				int3 boundingBoxOffset = collector.boundingBoxesPrinter->getOffset(Code()
					<< '[' << shapeInstancer->boundingBoxIndex << ']');
				bool hasError = !compileRenderJobWebGL(
					compiler,
					code,
					transformOffset,
					boundingBoxOffset,
					nameGenerator,
					outputCode);
					
				if (hasError)
				{
					dError("Render job failed to compile");
				}
				else
				{
					// use the code
					w << "var data = renderJob.data;\n";
					w << outputCode;

					if (renderBoundingBoxes)
						renderBoundingBox(w);
											
					// mark render job as not sorted (not transparent)
					if (layerInfo->sortMode == SORT_MATERIAL)
						w << "if (!shader.sort)\n\tdata[16] = -2e30;\n";
									
					// copy object id if shader uses it
					if (layerInfo->useFlags & SHADER_USES_ID)
						w << "data[17] = instance.ids[" << info.objectIndex << "];\n";

					// set draw function
					w << "renderJob.draw = global.draw." << renderInfo.drawFunctionName << ";\n";

					// set pointer to instance
					w << "renderJob.instance = instance;\n";
						
					w.endScope();
				}
			}
			return 1;
		}
		else if (Pointer<ParticleInstancer> particleInstancer = dynamicCast<ParticleInstancer>(instancer))
		{
			// particle instancer
				
			// get particle instancer info
			//ParticleInstancerInfo& particleInstancerInfo = collector.particleInstancerInfoMap[particleInstancer];

			// get particle system
			Pointer<ParticleSystem> particleSystem = staticCast<ParticleSystem>(particleInstancer->particleSystem);

			// get particle system info
			Pointer<ParticleSystemInfo> particleSystemInfo = staticCast<ParticleSystemInfo>(collector.shapeInfoMap[particleSystem]);

			// get name
			StringRef name = particleSystemInfo->name;
						
			if (!particleSystemInfo->hasError)
			{					
				int3 particleSize = particleSystemInfo->particlePrinter->getSize();

				// create code that calculates shape index and particleMatrix matrix
				std::string code;
				Pointer<IODevice> dev = new StringRefDevice(code);
				{
					CodeWriter w(dev);

					// binding variables
					if (depth > 0)
						writeBindingVariable(w, "parentMatrix", "float4x4");
					writeBindingVariable(w, "transform", "Transform");
					writeBindingVariable(w, "uniform", particleInstancer->uniformType);
					writeBindingVariable(w, "particle", particleSystem->particleType);
					writeBindingVariable(w, "index", "int"); // output
					writeBindingVariable(w, "particleMatrix", "float4x4"); // output

					w.writeLine();
						
					// particle create function
					w << "extern \"C\" void main()\n";
					w.beginScope();
						
					// calc matrix and assign to render job
					if (depth > 0)
						w << "float4x4 matrix = parentMatrix * transform.matrix;\n";
					else
						w << "float4x4 matrix = transform.matrix;\n"; // no reference leads to better output code
						
					// calc index and particle matrix
					w << particleInstancer->code;

					w.endScope();
				}

				// handle languages where structs are splitted into arrays of int, float and objects (e.g. javascript)
				int3 uniformOffset = int3();
				if (collector.uniformsPrinter != null)
				{
					uniformOffset = collector.uniformsPrinter->getOffset('.' + particleInstancer->path);
				}

				// the compiled code
				std::string uniformCode;
				std::string particleCode;
											
				// use flags (e.g. SHAPE_PARTICLE_USES_INDEX)
				int useFlags = 0;
					
				// compile particle system
				bool hasError = !compileShapeParticleInstancer(
					compiler, code,
					transformOffset,
					uniformOffset,
					nameGenerator,
					uniformCode,
					particleCode,
					useFlags,
					Language::JS);
				
				if (hasError)
				{
					dError("Shape particle instancer '" << particleInstancer->name << "' failed to compile");
				}
				else
				{
					// uniforms already available
						
					// uniform code
					w << uniformCode;
						
					// get particle iterator
					w << "var _" << depth << " = instance.particlePools." << name << ".next;\n"; // alive

					// create a render job for each particle
					w << "while (_" << depth << ")\n"; // while (it)
					w.beginScope();
						
					// get particle arrays
					getArrays(w, Code() << '_' << depth, "_p", particleSize);
						
					// calc particle matrix and index
					bool usesIndex = (useFlags & SHAPE_PARTICLE_USES_INDEX) != 0;
					if (usesIndex)
						w << "var index = 0;\n";
					w << particleCode;
						
					int numRenderJobs = 0;

					// select shape to render according to index
					if (particleInstancer->instances.size() == 1 || !usesIndex)
					{
						// only one shape
						const std::vector<Instance>& instances = particleInstancer->instances[0];
						foreach (const Instance& instance, instances)
						{
							numRenderJobs += createRenderJobs(w, collector, compiler, info,
								instance, depth + 1, nameGenerator, renderBoundingBoxes);
						}
					}
					else
					{
						// multiple shapes selected by index
						w << "switch (index)\n";
						w.beginScope();
						int index = 0;
						int nrj = 0;
						foreach (const std::vector<Instance>& instances, particleInstancer->instances)
						{
							w.decIndent();
							w << "case " << index++ << ":\n";
							w.incIndent();

							foreach (const Instance& instance, instances)
							{
								nrj += createRenderJobs(w, collector, compiler, info,
									instance, depth + 1, nameGenerator, renderBoundingBoxes);
							}
							w << "break;\n";
							numRenderJobs = max(numRenderJobs, nrj);
						}
						w.endScope(); // switch (index)				
					}

					// next particle
					w << "_" << depth << " = _" << depth << ".next;\n"; // it = it->next
					w.endScope(); // while
						
					return numRenderJobs * particleSystemInfo->numParticles;
				}
			}
		}
		return 0;
	}

	void bindAndDraw(CodeWriter& w, GeneratorCollector& collector,
		Pointer<ShapeInstancer> shapeInstancer, ShapeInstancerInfo& shapeInstancerInfo,
		RenderInfo& renderInfo,
		int quadsIndexType, bool useVertexArrayObjects, int draw,
		int& lastVertexBigBufferIndex, int& lastIndexBigBufferIndex)
	{
		const char* indexTypes[] = {"gl.UNSIGNED_BYTE", "gl.UNSIGNED_SHORT", "gl.UNSIGNED_INT"};

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
				w << "gl.bindVertexArray(instance.vertexArrays[" << renderInfo.vertexArrayIndex + vertexArrayIndex << "]);\n";
			}
		
			if (bindBuffers)
			{
				// enable vertex arrays
				if (useVertexArrayObjects)
					enableVertexAttributes(w, 0, renderInfo.layerInfo);

				// set vertex state
				setVertexState(w, collector, renderInfo.layerInfo, shapeInstancer, shapeInstancerInfo,
					shapeInfo->getBaseIndex(vertexArrayIndex), lastVertexBigBufferIndex);
			}
			
			// do shape specific bind and draw
			if (Pointer<ConstantMeshInfo> constantMeshInfo = dynamicCast<ConstantMeshInfo>(shapeInfo))
			{
				IndexBuffer::Section& section = constantMeshInfo->indexBuffer->sections[vertexArrayIndex];
				if (bindBuffers && section.bigBufferIndex != lastIndexBigBufferIndex)
				{
					// set index buffer
					lastIndexBigBufferIndex = section.bigBufferIndex;
					w << "gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, global.indexBuffer" << ident(section.bigBufferIndex)
						<< ");\n";
				}

				if (draw)
				{
					// draw (set uniform, bind vertex/index buffers, set cull mode, draw)
					w << "gl.drawElements(gl.TRIANGLES, "
						<< section.indexCount << ", "
						<< indexTypes[section.type] << ", "
						<< section.bigBufferOffset << ");\n"; // offset in index buffer
				
				}
			}
			else if (Pointer<TextMeshInfo> textMeshInfo = dynamicCast<TextMeshInfo>(shapeInfo))
			{
				if (bindBuffers)
				{
					// set index buffer
					w << "gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, global.quads);\n";
				}
				
				if (draw)
				{
					// draw text symbols
					w << "glDrawElements(gl.TRIANGLES, "
						"instance." << shapeInfo->getName() << ".numSymbols * 6, "
						<< indexTypes[quadsIndexType] << ", "
						"0);\n";
				}
			}
			else if (Pointer<ParticleSystemInfo> particleSystemInfo = dynamicCast<ParticleSystemInfo>(shapeInfo))
			{
				if (draw)
				{
					// draw particles
					w << "gl.drawArrays(gl.POINTS, 0, "
						"instance.particlePools." << shapeInfo->getName() << ".numParticles);\n";
				}
			}
		}
	}

// utility

	void writeQuotedString(CodeWriter& w, const std::string& str)
	{
		// save indent
		int indent = w.getIndent();
		w.setIndent(0);
		
		const char* line = str.c_str();
		int len = int(str.length());
		
		w << "'";
		int i = 0;
		int lineNumber = 1;
		while (i < len)
		{
			// get line until new-line character
			int s = i;
			while (i < len && line[i] != '\n')
			{
				++i;
			} 
			
			// write line
			w.writeString(line + s, i - s);

			// skip new-line character
			++i;
			
			if (i == len)
				break;
			
			// write new line "\n" and line continuation "\\"
			w << "\\n\\\n";
			++lineNumber;
		}
		w << "';\n";
		
		// restore indent
		w.setIndent(indent);
	}

	void printJSVariable(CodeStream& w, StringRef name, const std::vector<ShaderVariable>& variables)
	{
		w << name << ": {";
		bool first = true;
		foreach (const ShaderVariable& variable, variables)
		{
			if (!first)
				w << ", ";
			first = false;

			w << variable.name << ": ";
			if (variable.type.type == ShaderType::TEXTURE)
			{
				// sampler
				w << "null";
			}
			else
			{
				// array
				w << "new ";
			
				switch (variable.type.type)
				{
				case ShaderType::INT:
					w << "Int32Array";
					break;
				case ShaderType::FLOAT:
					w << "Float32Array";
					break;
				}
			
				w << '(' << std::max(variable.type.numElements, 1) * variable.type.numRows << ')';
			}
		}
		w << "},\n";
	}

	// for field/type pairs of vertex structures
	typedef std::pair<const std::string, std::string> StringPair;

} // anonymous namespace


// start

// generateSceneWebGL
Pointer<SplittedGlobalPrinter> generateSceneWebGL(Pointer<BufferConverter> bufferConverter, Pointer<Scene> scene,
	CodeWriter& w, DataWriter& d, const SceneOptions& options, SceneStatistics& stats)
{	
	// gets true if a compilation failed
	bool hasError = false;

	// api dependent options
	int collectorFlags;
	bool useVertexArrayObjects;

	collectorFlags = GeneratorCollector::TARGET_X5Y6Z5 | GeneratorCollector::NO_INDEX_32_BIT;
	useVertexArrayObjects = false;
	
	// when using expand to float for IE 11 then also don't use 8 bit indices
	if (options.dataMode & SceneOptions::USE_FLOAT)
		collectorFlags |= GeneratorCollector::NO_INDEX_8_BIT;

	bool deformersInShaders = true;
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
	

	// struct "Uniforms" contains all uniforms for deformers, particle systems, shaders
	Pointer<StructType> uniforms = new StructType();
	{
		// deformers
		foreach (DeformerInfoPair& p, collector.deformerInfoMap)
		{
			const Pointer<Deformer>& deformer = p.first;
			
			if (deformer != null)
				uniforms->addMember(deformer->path, deformer->uniformType);
		}

		// particle systems
		foreach (ShapeInfoPair& p, collector.shapeInfoMap)
		{
			if (Pointer<ParticleSystem> particleSystem = dynamicCast<ParticleSystem>(p.first))
			{
				if (particleSystem->emitterUniformType != null)
					uniforms->addMember("e." + particleSystem->path, particleSystem->emitterUniformType);
				uniforms->addMember(particleSystem->path, particleSystem->uniformType);
			}
		}

		// particle instancers
		foreach (ParticleInstancerInfoPair& p, collector.particleInstancerInfoMap)
		{
			const Pointer<ParticleInstancer>& particleInstancer = p.first;
			//ParticleInstancerInfo& particleInstancerInfo = p.second;
	
			uniforms->addMember(particleInstancer->path, particleInstancer->uniformType);
		}

		// shaders
		foreach (ShaderInfoPair& shaderInfoPair, collector.shaderInfoMap)
		{
			const Pointer<Shader>& shader = shaderInfoPair.first;

			uniforms->addMember(shader->path, shader->materialType);
		}
	}

	// compile scene
	std::string initStateCode;
	std::string updateCode;
	Pointer<SplittedGlobalPrinter> statePrinter;
	{
		// generate input c++ code
		std::string code;
		std::map<std::string, int> buffers;
		{
			Pointer<IODevice> dev = new StringRefDevice(code);
			CodeWriter w2(dev);
			
			// build type for named buffers
			Pointer<StructType> buffersType = new StructType();
			foreach (const NamedBufferInfo& info, collector.namedBufferInfos)
			{
				const std::string& name = info.namedBuffer->name;
				BufferFormat format = info.format;
				VectorInfo typeInfo = toVectorInfo(format);
				size_t elementCount = info.namedBuffer->buffer->getNumElements();

				buffersType->addMember(name, new ArrayType(new NamedType(typeInfo.toString()), elementCount));
				
				// in js all buffers are stored in an array, therefore map buffer name to array index
				buffers[name] = int(buffers.size());
				
				// macros for buffers
				if (info.scale == 1.0 && info.offset == 0.0)
				{
					w2 << "#define macro" << name << "(x) x\n";
					w2 << "#define macro" << name << "Inv(x) x\n";
				}
				else
				{
					w2 << "#define macro" << name << "(x) (x * " << info.scale << " + " << info.offset << ")\n";
					float s = 1.0 / info.scale;
					float o = -info.offset * s;
					w2 << "#define macro" << name << "Inv(x) (x * " << s << " + " << o << ")\n";
				}
			}
			
			// variables
			writeBindingVariable(w2, "state", scene->stateType);
			writeBindingVariable(w2, "buffers", buffersType);
			writeBindingVariable(w2, "uniforms", uniforms);
			writeBindingVariable(w2, "transforms", new ArrayType(new NamedType("Transform"), scene->transformCount));
			writeBindingVariable(w2, "boundingBoxes", new ArrayType(new NamedType("BoundingBox"), scene->boundingBoxCount));
			writeBindingVariable(w2, "seed", "int");
			w2.writeLine();
			
			// initState
			w2 << "extern \"C\" void initState()\n";
			w2.beginScope();
			w2 << scene->initStateCode;
			w2.endScope();
			
			// update
			w2 << "extern \"C\" void update()\n";
			w2.beginScope();
			w2 << scene->updateCode;

			// write output to uniform struct for used deformers, shaders, particle systems and particle instancers
			collector.writeUniformOutput(w2);
			
			w2.endScope();
		}
						
		// compile scene
		bool sceneHasError = !compileSceneWebGL(
			compiler,
			code,
			buffers,
			statePrinter,
			collector.uniformsPrinter,
			collector.transformsPrinter,
			collector.boundingBoxesPrinter,				
			initStateCode,
			updateCode,
			collector.sceneUseFlags);
		hasError |= sceneHasError;

		if (sceneHasError)
		{
			dError("scene '" << scene->name << "' failed to compile");			
			return null;
		}
	}
	int3 stateSize = statePrinter->getSize();
	int3 uniformsSize = collector.uniformsPrinter ? collector.uniformsPrinter->getSize() : int3();
	int3 transformsSize = collector.transformsPrinter ? collector.transformsPrinter->getSize() : int3();
	int3 boundingBoxesSize = collector.boundingBoxesPrinter ? collector.boundingBoxesPrinter->getSize() : int3();
	

	// compile shaders (also store in vertex buffers infos which fields are needed)
	hasError |= collector.compileShaders(compiler, shaderOptions, Language::JS, Language::ESSL);

	// compile deformers (after shaders because they tell the deformers the used fields)
	hasError |= collector.compileDeformers(compiler, Language::JS, Language::JS);

	// compile particle systems
	hasError |= collector.compileParticleSystems(compiler, Language::JS);

	stats.numTextures = max(stats.numTextures, collector.numTextures);
	
	// generate buffers for rendering, including format conversion
	int64_t dataOffset = collector.writeBuffers(d, bufferConverter);
	int numShaderInputBuffers = int(collector.bigVertexBuffers[SCOPE_SHADER].size());
	int numDeformerInputBuffers = int(collector.bigVertexBuffers[SCOPE_DEFORMER].size());
	int numIndexBuffers = int(collector.bigIndexBuffers.size());
	int numDynamicBuffers = int(collector.dynamicBufferInfos.size());

	// determine maximum number of quads that have to be rendered in one draw call
	uint maxNumQuads = collector.getMaxNumQuads();
	int quadsIndexType = maxNumQuads <= 256/4 ? 0 : (maxNumQuads <= 65536/4 ? 1 : 2);

	// count number of vertex array objects
	int numVertexArrayObjects = collector.buildRenderInfos();


	// temp arrays for transform uniforms
	//std::vector<int> intTempArrays;
	//std::vector<int> floatTempArrays;
	std::vector<std::set<int> > intTempArrays;
	std::vector<std::set<int> > floatTempArrays;

	// global shader structs
	w << "shaders:\n";
	w.beginScope();
	foreach (Pointer<LayerInfo> layerInfo, collector.layerInfos)
	{
		w.writeComment(layerInfo->comment);
		w << layerInfo->name << ":\n";
		w.beginScope();

		//setResetShader(w, layerInfo);

		w.endScope(','); // shader
	}
	w.endScope(','); // shaders
	w.writeLine();							

	// init function for scene global data
	w << "initGlobal: function(global, data)\n";
	{
		w.beginScope();

		// write shader code and collect sizes of temp arrays
		foreach (Pointer<LayerInfo> layerInfo, collector.layerInfos)
		{
			if (layerInfo->equalShaderLayerInfo == NULL)
			{
				{
					// output shaders into code
					w.writeComment(layerInfo->comment);

					w << "var vs" << layerInfo->name << " =\n";
					writeQuotedString(w, layerInfo->vertexShader);

					w << "var ps" << layerInfo->name << " =\n";
					writeQuotedString(w, layerInfo->pixelShader);
					w.writeLine();
				}
			
				// allocate temp arrays for material large arrays for transfer into textures and transofrm uniforms.
				// material variables are stored in the instance
				allocateTempArrays(layerInfo->materialLargeArrays, intTempArrays, floatTempArrays);
				allocateTempArrays(layerInfo->transformVariables, intTempArrays, floatTempArrays);
			}
		}

		// data buffers
		{
			bool compress = options.dataMode & SceneOptions::COMPRESS;
			int64_t offset = dataOffset;
		
			if (options.dataMode & SceneOptions::COMPRESS)
			{
				w << "var d = new engine.Decompressor(new Uint8Array(data, " << offset << "));\n";
			}
		
			// named buffers (e.g. animation tracks)
			if (!scene->buffers.empty())
			{
				// write buffers as array
				w << "global.buffers = \n";
				w << "[\n";
				w.incIndent();
				if (!compress)
				{
					foreach (const NamedBufferInfo& info, collector.namedBufferInfos)
					{
						BufferFormat format = info.format;
						
						offset += -offset & (format.getComponentSize() - 1);
						StringRef typedArray = toJSTypedArray(format);
						size_t count = info.namedBuffer->buffer->getNumElements() * format.getNumComponents();
						w << "new " << typedArray << "(data, " << offset << ", " << count << "),\n";
						offset += count * format.getComponentSize();
					}
				}
				else
				{
					foreach (const NamedBufferInfo& info, collector.namedBufferInfos)
					{
						BufferFormat format = info.format;

						size_t count = info.namedBuffer->buffer->getNumElements() * format.getNumComponents();
						w << "d.decompress" << format.getComponentSize() * 8 << "(" << count << "),\n";
					}
				}
				w.decIndent();
				w << "];\n";
			}		

			// shader vertex buffers
			if (numShaderInputBuffers > 0)
			{
				std::vector<BigVertexBuffer>& bigVertexBuffers = collector.bigVertexBuffers[SCOPE_SHADER];
				if (!compress)
				{
					// iterate over big vertex buffers
					int index = 0;
					foreach (BigVertexBuffer& bigVertexBuffer, bigVertexBuffers)
					{
						w << "gl.bindBuffer(gl.ARRAY_BUFFER, global.shaderInputBuffer" << ident(index)
							<< " = gl.createBuffer());\n";
						++index;

						offset += -offset & (bigVertexBuffer.vertexBuffer->dataAlign - 1);
						size_t dataSize = bigVertexBuffer.vertexCount * bigVertexBuffer.vertexBuffer->dataVertexSize;
						w << "gl.bufferData(gl.ARRAY_BUFFER, new Int8Array(data, " << offset
							<< ", " << dataSize << "), gl.STATIC_DRAW);\n";
						offset += dataSize;
					}
				}
				else
				{
					// compressed: decompress vertex buffers
					
					// iterate over big vertex buffers
					int index = 0;
					foreach (BigVertexBuffer& bigVertexBuffer, bigVertexBuffers)
					{
						// get first vertex buffer (all vertex buffers have same layout)
						Pointer<VertexBuffer> vertexBuffer = bigVertexBuffer.vertexBuffer;

						// determine some field properties
						size_t sizes[5];
						sizes[1] = sizes[2] = sizes[4] = 0;
						int maxComponentSize = 1;
						int allComponentSizes = 0;
						foreach (VertexBuffer::Field& field, vertexBuffer->fields)
						{
							int srcComponentSize = field.dataFormat.getComponentSize();
							int dstComponentSize = field.targetFormat.getComponentSize();

							sizes[srcComponentSize] += bigVertexBuffer.vertexCount * field.dataFormat.getNumComponents();
							
							// maximum component size
							maxComponentSize = std::max(maxComponentSize, dstComponentSize);
							
							// accumulate all component sizes
							allComponentSizes |= dstComponentSize;
						}

						// create vertex buffer
						size_t bufferSize = bigVertexBuffer.vertexCount * vertexBuffer->targetVertexSize;
						if (countSetBits(allComponentSizes) != 1)
							w << "var vb = new ArrayBuffer(" << bufferSize << ");\n";
						for (int dstComponentSize = 1; dstComponentSize <= 4; dstComponentSize <<= 1)
						{
							// decompress into temp buffer
							if (sizes[dstComponentSize] > 0)
							{
								w << "var b" << dstComponentSize << " = "
									"d.decompress" << dstComponentSize * 8 << "(" << sizes[dstComponentSize] << ");\n";
							}

							if (dstComponentSize & allComponentSizes)
							{
								// vb or vb1, vb2, vb4
								w << "var vb";
								if (dstComponentSize != allComponentSizes)
									w << ident(dstComponentSize);
								w << " = new ";
								
								if (dstComponentSize == 4)
									w << "Float";
								else
									w << "Uint";
								
								w << ident(dstComponentSize * 8) << "Array(";
								
								// create view on vb or create directly using buffer size
								if (dstComponentSize != allComponentSizes)
									w << "vb";
								else
									w << bufferSize / dstComponentSize;
								
								w << ");\n";
							}
						}

						// iterate over vertices of all vertex buffers in big vertex buffer
						w << "for (var i = 0, j = 0; i < " << bigVertexBuffer.vertexCount << "; ++i, j += "
							<< vertexBuffer->targetVertexSize / maxComponentSize << ")\n";
						w.beginScope();

						// iterate over fields
						int srcOffset[5];
						srcOffset[1] = srcOffset[2] = srcOffset[4] = 0;
						foreach (VertexBuffer::Field& field, vertexBuffer->fields)
						{
							int numComponents = field.dataFormat.getNumComponents();
							int srcComponentSize = field.dataFormat.getComponentSize();
							int dstComponentSize = field.targetFormat.getComponentSize();
							
							int dstOffset = field.targetOffset / dstComponentSize;
							int dstMul = maxComponentSize / dstComponentSize;

							bool dstFloat = field.targetFormat.isFloat32();
							bool srcSign = field.dataFormat.isSigned();
							
							// iterate over components
							for (int componentIndex = 0; componentIndex < numComponents; ++componentIndex)
							{
								// write into destination
								w << "vb";
								if (dstComponentSize != allComponentSizes)
									w << ident(dstComponentSize);
								w << "[j";
								if (dstMul > 1)
									w << " * " << dstMul;
								w << " + " << dstOffset++ << "] = ";
								
								if (dstFloat && srcSign)
									w << "(";
								
								// read from source
								w << "b" << srcComponentSize << "[i + " << srcOffset[srcComponentSize] << "]";
								srcOffset[srcComponentSize] += bigVertexBuffer.vertexCount;
								
								// scale according to type if destination is float
								if (dstFloat)
								{
									// convert to signed
									if (srcSign)
									{
										if (srcComponentSize < 4)
											w << " << " << (32 - srcComponentSize * 8) << ")";
										else
											w << " | 0)";
									}
									if (field.dataFormat.isNormalized())
									{
										static const float scales[] = {0, 255.0f, 65535.0f, 0, 4294967295.0f};
										float scale = srcSign ? 2147483647.0f : scales[srcComponentSize];
										w << " * " << 1.0f / scale;
									}
									else if (srcSign)
										w << " >> " << (32 - srcComponentSize * 8);
								}
								
								w << ";\n";
							}
						}
						w.endScope();

						w << "gl.bindBuffer(gl.ARRAY_BUFFER, global.shaderInputBuffer" << ident(index)
							<< " = gl.createBuffer());\n";
						w << "gl.bufferData(gl.ARRAY_BUFFER, vb, gl.STATIC_DRAW);\n";
						++index;
					}
				}
				w.writeLine();
			}
			
			// shader index buffers
			if (numIndexBuffers > 0)
			{
				if (!compress)
				{
					int index = 0;
					foreach (BigIndexBuffer& bigIndexBuffer, collector.bigIndexBuffers)
					{
						w << "gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, global.indexBuffer" << ident(index)
							<< " = gl.createBuffer());\n";
						++index;

						offset += -offset & (bigIndexBuffer.indexSize - 1);
						size_t dataSize = bigIndexBuffer.indexCount * bigIndexBuffer.indexSize;
						w << "gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Int8Array(data, " << offset
							<< ", " << dataSize << "), gl.STATIC_DRAW);\n";
						offset += dataSize;
					}
				}
				else
				{
					// decompress index buffers
					int index = 0;
					foreach (BigIndexBuffer& bigIndexBuffer, collector.bigIndexBuffers)
					{
						int indexSize = bigIndexBuffer.indexSize;
						w << "var b = d.decompress" << indexSize * 8 << "(" << bigIndexBuffer.indexCount << ");\n";

						// reorder indices
						w << "var ib = new Uint" << indexSize * 8 << "Array(" << bigIndexBuffer.indexCount << ");\n";
						size_t numTriangles = bigIndexBuffer.indexCount / 3;
						w << "for (var i = 0, j = 0; i < " << numTriangles << "; ++i, j += 3)\n";
						w.beginScope();
						w << "ib[j] = b[i];\n";
						w << "ib[j + 1] = b[i + " << numTriangles << "];\n";
						w << "ib[j + 2] = b[i + " << numTriangles * 2 << "];\n";
						w.endScope();
						
						w << "gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, global.indexBuffer" << ident(index++)
							<< " = gl.createBuffer());\n";
						w << "gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, ib, gl.STATIC_DRAW);\n";
					}
				}
				w.writeLine();
			}
		}

		// scale/offset arrays for packed vertex buffers
		if (!collector.scaleOffsetArrayMap.empty())
		{
			foreach (GeneratorCollector::ScaleOffsetArrayPair& p, collector.scaleOffsetArrayMap)
			{
				w << "global.scaleOffset" << ident(p.second) << " = new Float32Array(";
				w.writeArray(p.first);
				w << ");\n";
			}
			w.writeLine();
		}

		// init shaders for all layers
		w << "var shaders = global.shaders;\n";
		foreach (Pointer<LayerInfo> layerInfo, collector.layerInfos)
		{
			// check if shader is not reused from other layer
			if (layerInfo->equalShaderLayerInfo == null)
			{
				//w.writeComment(layerInfo->comment);
				w.beginScope();
				
				// get shader struct
				w << "var shader = shaders." << layerInfo->name << ";\n";

				// create vertex and pixel shader
				w << "var vertexShader = engine.createVertexShader(vs" << layerInfo->name << ", "
					"\"" << layerInfo->comment << "\");\n";
				w << "var pixelShader = engine.createPixelShader(ps" << layerInfo->name << ", "
					"\"" << layerInfo->comment << "\");\n";
				
				// create program
				w << "var program = shader.program = gl.createProgram();\n";
				w << "gl.attachShader(program, vertexShader);\n";
				w << "gl.attachShader(program, pixelShader);\n";

				// bind vertex attribute locations
				foreach (const LayerInfo::VertexBindingPair& p, layerInfo->vertexBindings)
				{
					// get name in shader (e.g. "_0")
					const std::string& name = p.second.name;
			
					// the name is also the index when '_' is stripped off
					w << "gl.bindAttribLocation(program, " << name.substr(1) << ", '" << name << "');\n";
				}
	
				// link program
				w << "gl.linkProgram(program);\n";

				w << "gl.deleteShader(vertexShader);\n";
				w << "gl.deleteShader(pixelShader);\n";

				// get handles for material and transform uniform arrays from shader (uniformLocation)
				if (getNumTextures(layerInfo->materialVariables) + getNumTextures(layerInfo->transformVariables) > 0)
					w << "gl.useProgram(program);\n";					
				int textureIndex = 0;
				getHandles(w, layerInfo->materialVariables, textureIndex);
				getHandles(w, layerInfo->transformVariables, textureIndex);
				if (layerInfo->useFlags & SHADER_USES_SCALE_OFFSET)
					w << "shader.s_o = gl.getUniformLocation(program, 's_o');\n";
				if (layerInfo->useFlags & SHADER_USES_FRONT_FACING)
					w << "shader.f_f = gl.getUniformLocation(program, 'f_f');\n";
				w.endScope();
			}
		}
			
		// bounding boxes
		if (options.renderBoundingBoxes)
		{
			w.writeComment("bounding box shader");
			w.beginScope();
			
			// create shaders
			w << "var vertexShader = engine.createVertexShader(\""
				"precision highp float;"
				"uniform mat4 viewProjectionMatrix;"
				"uniform mat4 matrix;"
				"attribute vec4 position;"
				"void main()"
				"{"
					"gl_Position = viewProjectionMatrix * matrix * position;"
				"}"
				"\", \"boundingBox\");\n";
			w << "var pixelShader = engine.createPixelShader(\""
				"precision mediump float;"
				"void main()"
				"{"
					"gl_FragColor = vec4(1.0);"
				"}"
				"\", \"boundingBox\");\n";

			// create program
			w << "var program = global.bbProgram = gl.createProgram();\n";
			w << "gl.attachShader(program, vertexShader);\n";
			w << "gl.attachShader(program, pixelShader);\n";

			// bind vertex attribute locations
			w << "gl.bindAttribLocation(program, 0, \"position\");\n";

			// link program
			w << "gl.linkProgram(program);\n";

			w << "gl.deleteShader(vertexShader);\n";
			w << "gl.deleteShader(pixelShader);\n";

			// get uniform locations
			w << "global.bbViewProjectionMatrix = gl.getUniformLocation(program, \"viewProjectionMatrix\");\n";
			w << "global.bbMatrix = gl.getUniformLocation(program, \"matrix\");\n";

			if (useVertexArrayObjects)
			{
				// generate and bind vertex array object
				w << "global.bbVertexArray = gl.createVertexArray();\n";
				w << "gl.bindVertexArray(global.bbVertexArray);\n";

				// enable vertex attribute 0
				w << "gl.enableVertexAttribArray(0);\n";
			}

			// create vertex buffer
			w << "var vb = new Float32Array(["
				"-1, -1, -1, "
				"-1, -1, 1, "
				"-1, 1, -1, "
				"-1, 1, 1, "
				"1, -1, -1, "
				"1, -1, 1, "
				"1, 1, -1, "
				"1, 1, 1]);\n";
			w << "global.bbVertexBuffer = gl.createBuffer();\n";
			w << "gl.bindBuffer(gl.ARRAY_BUFFER, global.bbVertexBuffer);\n";
			w << "gl.bufferData(gl.ARRAY_BUFFER, vb, gl.STATIC_DRAW);\n";

			if (useVertexArrayObjects)
			{
				// set bounding box vertex buffer (8 positions) to vertex attribute 0
				w << "gl.vertexAttribPointer(0, 3, gl.FLOAT, false, 0, 0);\n";
			}				

			// create index buffer
			w << "var ib = new Uint16Array([0, 1, 0, 2, 3, 1, 3, 2,  0, 4, 1, 5, 2, 6, 3, 7,  4, 5, 4, 6, 7, 5, 7, 6]);\n";
			w << "global.bbIndexBuffer = gl.createBuffer();\n";
			w << "gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, global.bbIndexBuffer);\n";
			w << "gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, ib, gl.STATIC_DRAW);\n";
			
			w.endScope();
		}		
		w.endScope(',');
	}
	w.writeLine();

	// done function for scene global data
	w << "doneGlobal: function(global)\n";
	{
		w.beginScope();

		// delete shader input buffers (static vertex buffers)
		for (int i = 0; i < numShaderInputBuffers; ++i)
		{
			w << "gl.deleteBuffer(global.shaderInputBuffer" << ident(i) << ");\n";
		}

		// delete index buffers (static index buffers)
		for (int i = 0; i < numIndexBuffers; ++i)
		{
			w << "gl.deleteBuffer(global.indexBuffer" << ident(i) << ");\n";
		}
		
		// iterate over all layers and delete programs
		foreach (Pointer<LayerInfo> layerInfo, collector.layerInfos)
		{
			w << "gl.deleteProgram(global.shaders." << layerInfo->name << ".program);\n";
		}

		// bounding boxes
		if (options.renderBoundingBoxes)
		{
			w << "gl.deleteProgram(global.bbProgram);\n";
								
			w << "gl.deleteBuffer(global.bbVertexBuffer);\n";
			w << "gl.deleteBuffer(global.bbIndexBuffer);\n";
			if (useVertexArrayObjects)
				w << "gl.deleteVertexArray(global.bbVertexArray);\n";
		}

		w.endScope(',');
	}
	w.writeLine();


	// write "set state" functions for all layers
	w << "render: ";
	w.beginScope();
	foreach (Pointer<LayerInfo> layerInfo, collector.layerInfos)
	{
		// create functions for each used render mode
		foreach (int renderMode, layerInfo->renderModes)
		{
			// set state function
			w << layerInfo->name << '_' << ident(renderMode) << ": function(renderJob)\n";
			{
				w.beginScope();
				w << "var instance = renderJob.instance;\n";
				w << "var global = instance.global;\n";

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
				w << "var transfer = instance.shaders." << layerInfo->name << ".transfer;\n";
				getArrayReferences(w, layerInfo->transferVariables, "transfer.");
			
				// get reference to world matrix (object space -> world space)
				// also contains distance in renderJob.data[16] and objectId in renderJob.data[17]
				w << "var matrix = renderJob.data;\n";
			
				// need flip flag if at least one shape is rendered from the front or back side
				if (layerInfo->useFlags & SHADER_USES_FLIP)
					w << "var flip;\n";
				if (layerInfo->useFlags & SHADER_USES_ID)
					w << "var objectId = matrix[17];\n";
			
				// get temp arrays for shader uniforms
				getTempArrays(w, layerInfo->transformVariables);
			
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
							w << "flip ^= matrix[16] > -1e30;\n";
						if (layerInfo->sortMode == SORT_ALWAYS)
							renderMode ^= Mesh::BACK_SIDE;
					}

					// set cull mode. flip front and back if world matrix has negative determinant
					setCullMode(w, layerInfo, renderMode);
				}
				
				// call draw function
				w << "renderJob.draw(instance, shader);\n";
				
				// check if we have to render the other side separately (two pass double sided)
				if (twoPassDoubleSided)
				{
					// set cull mode to opposite side
					setCullMode(w, layerInfo, renderMode ^ Mesh::BACK_SIDE);
										
					// call draw function again
					w << "renderJob.draw(instance, shader);\n";
				}

				if (layerInfo->sortMode == SORT_ALWAYS)
				{
					// reset shader
					enableVertexAttributes(w, numVertexAttributes, defaultNumVertexAttributes);
					resetShader(w, layerInfo);
				}				
				w.endScope(',');
			}
			
			// generate extra set function if sorting depends on material
			if (layerInfo->sortMode == SORT_MATERIAL)
			{
				w << Ascii::toUpperCase(layerInfo->name[0]) << layerInfo->name.substr(1)
					<< '_' << ident(renderMode) << ": function(renderJob)\n";
				{
					w.beginScope();
					w << "var instance = renderJob.instance;\n";
					w << "var global = instance.global;\n";
					
					// set shader
					setShader(w, layerInfo);
					int numVertexAttributes = defaultNumVertexAttributes;
					if (!useVertexArrayObjects)
						numVertexAttributes = enableVertexAttributes(w, numVertexAttributes, layerInfo);
					
					// render
					w << "global.render." << layerInfo->name << '_' << renderMode << "(renderJob);\n";
					
					// reset shader
					enableVertexAttributes(w, numVertexAttributes, defaultNumVertexAttributes);
					resetShader(w, layerInfo);
					
					w.endScope(',');
				}
			}
		}
	}
	w.endScope(',');

	// write draw functions for all shape instancers
	w << "draw: ";
	w.beginScope();
	foreach (ShapeInstancerInfoPair& p, collector.shapeInstancerInfoMap)
	{
		Pointer<ShapeInstancer> shapeInstancer = p.first;
		ShapeInstancerInfo& shapeInstancerInfo = p.second;
		
		foreach (RenderInfoPair& p, shapeInstancerInfo.renderInfoMap)
		{
			RenderInfo& renderInfo = p.second;
			Pointer<LayerInfo> layerInfo = renderInfo.layerInfo;

			// render function (returns resetShader function)
			w << renderInfo.drawFunctionName << ": function(instance, shader)\n";
			w.beginScope();
			w << "var global = instance.global;\n";
			
			// check if we have a scale offset array to set
			if (layerInfo->useFlags & SHADER_USES_SCALE_OFFSET)
			{
				getShaderGlobal(w, layerInfo);
				w << "gl.uniform4fv(shader.s_o, global.scaleOffset" << ident(renderInfo.scaleOffsetArrayIndex) << ");\n";
			}

			int lastVertexBigBufferIndex = -1;
			int lastIndexBigBufferIndex = -1;
			bindAndDraw(w, collector, shapeInstancer, shapeInstancerInfo,
				renderInfo, quadsIndexType, useVertexArrayObjects, 1,
				lastVertexBigBufferIndex, lastIndexBigBufferIndex);
			
			w.endScope(',');
		}
	}
	w.endScope(',');
	w.writeLine();

	// creates and returns instance of scene
	w << "createInstance: function(global, renderer)\n";
	{
		w.beginScope();

		// create instance variable with initializer that defines the structure
		{
			w << "var instance = \n";
			w.beginScope();
			w << "global: global,\n";
			w << "renderer: renderer,\n";
			if (!scene->instances.empty())
				w << "ids: new Uint32Array(" << int(scene->instances.size()) << "),\n";
			
			// scene state
			addArrays(w, "state", stateSize);
			
			// uniforms state (transfer from scene to shaders, deformers, particle instancers)
			addArrays(w, "uniforms", uniformsSize);
				
			// shaders
			w << "shaders:\n";
			w.beginScope();
			foreach (Pointer<LayerInfo> layerInfo, collector.layerInfos)
			{
				w << layerInfo->name << ":\n";
				w.beginScope();

				printJSVariable(w, "uniform", layerInfo->materialVariables);
				printJSVariable(w, "transfer", layerInfo->transferVariables);

				w.endScope(',');
			}
			w.endScope(',');
						
			addArrays(w, "transforms", transformsSize);
			addArrays(w, "boundingBoxes", boundingBoxesSize);
			
			w << "sceneSequence: 0,\n"; // scene sequence number
			w << "deformerSequence: 0,\n"; // deformer sequence number
			w << "renderSequence: 0,\n"; // render sequence number
			if (collector.sceneUseFlags & SCENE_USES_SEED)
				w << "seed: 0,\n"; // random seed

			// temp arrays
			{
				w << "viewProjectionMatrix: new Float32Array(16),\n";
				int intIndex = 0;
				int floatIndex = 0;
				/*
				foreach (int length, intTempArrays)
					w << "iu" << ident(intIndex++) << ": new Int32Array(" << length << "),\n";
				foreach (int length, floatTempArrays)
					w << "fu" << ident(floatIndex++) << ": new Float32Array(" << length << "),\n";
				*/
				foreach (const std::set<int>& lengths, intTempArrays)
				{
					foreach (int length, lengths)
						w << "iu" << ident(intIndex) << '_' << ident(length) << ": new Int32Array(" << length << "),\n";
					intIndex++;
				}
				foreach (const std::set<int>& lengths, floatTempArrays)
				{
					foreach (int length, lengths)
						w << "fu" << ident(floatIndex) << '_' << ident(length) << ": new Float32Array(" << length << "),\n";
					floatIndex++;
				}
				if (options.renderBoundingBoxes)
					w << "bbMatrix: new Float32Array(16)\n";
			}
			
			w.endScope(';'); // var instance
		}
		w.writeLine();

		
		// create textures for large uniform arrays
		bool haveLargeArrays = false;
		foreach (Pointer<LayerInfo> layerInfo, collector.layerInfos)
		{
			foreach (ShaderVariable& variable, layerInfo->materialLargeArrays)
			{
				if (!haveLargeArrays)
				{
					haveLargeArrays = true;
					w << "var ";
				}
				w << "texture = gl.createTexture();\n";
				w << "gl.bindTexture(gl.TEXTURE_2D, instance.shaders." << layerInfo->name
					<< ".uniform." << variable.name << " = texture);\n";
				w << "gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, " << variable.type.numElements
					<< ", 1, 0, gl.RGBA, gl.FLOAT, null);\n";
				w << "gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);\n";
				w << "gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);\n";
				w << "gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);\n";
				w << "gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);\n";
			}
		}
		if (haveLargeArrays)
		{
			w << "gl.bindTexture(gl.TEXTURE_2D, null);\n";
			w.writeLine();
		}

		// create dynamic vertex buffers for deformer output
		if (numDynamicBuffers > 0)
		{
			size_t maxSize = 0;
			int index = 0;
			foreach (DynamicBufferInfo& dynamicBufferInfo, collector.dynamicBufferInfos)
			{
				w << "gl.bindBuffer(gl.ARRAY_BUFFER, instance.dynamicBuffer" << ident(index) << " = gl.createBuffer());\n";
				w << "gl.bufferData(gl.ARRAY_BUFFER, " << dynamicBufferInfo.size << ", gl.DYNAMIC_DRAW);\n";
				maxSize = max(maxSize, dynamicBufferInfo.maxBufferSize);

				++index;					
			}
			w << "gl.bindBuffer(gl.ARRAY_BUFFER, null);\n";
			w << "var d = new ArrayBuffer(" << maxSize << ");\n";

			// we need views for each buffer size to pass to bufferSubData
			int viewIndex = 0;
			foreach (DynamicBufferInfo& dynamicBufferInfo, collector.dynamicBufferInfos)
			{
				foreach (DeformerJobInfoMap::iterator it, dynamicBufferInfo.deformerJobIterators)
				{
					size_t size = it->second.dynamicBufferSize;
					w << "instance.d" << viewIndex << " = new Float32Array(d, 0, " << size / 4 << ");\n";
					++viewIndex;
				}
			}
			w.writeLine();			
		}

		// init shapes
		w << "var particlePools = instance.particlePools = {};\n";
		foreach (ShapeInfoPair& p, collector.shapeInfoMap)
		{
			if (Pointer<ParticleSystem> particleSystem = dynamicCast<ParticleSystem>(p.first))
			{
				// particle system
				Pointer<ParticleSystemInfo> particleSystemInfo = staticCast<ParticleSystemInfo>(p.second);
				if (!particleSystemInfo->hasError)
				{
					const std::string& name = particleSystemInfo->name;
					int numInitParticles = particleSystemInfo->numInitParticles;
					int numParticles = particleSystemInfo->numParticles - numInitParticles;
					int3 particleSize = particleSystemInfo->particlePrinter->getSize();
				
					w.beginScope();
					
					// create particle pool of particle system
					w << "var _9 = particlePools." << name << " = {};\n"; // pp
					
					// create uninitialized particles which are _3
					w << "var _3 = null;\n"; // dead = NULL
					w << "for (var _0 = 0; _0 < " << numParticles << "; ++_0)\n";
					w.beginScope();
					w << "_3 = {";
					addArrays(w, "_p", particleSize, false);
					w << "next: _3};\n";
					w.endScope();		
					w << "_9.dead = _3;\n"; // pp.dead = dead
					
					// check if there are particles with initial state
					if (numInitParticles > 0)
					{
						w << "var _2 = null;\n"; // alive = NULL
						
						// create initialized particles which are alive
						w << "for (var i_ = 0; i_ < " << numInitParticles << "; ++i_)\n";
						w.beginScope();
						w << "var particle = {";
						addArrays(w, "_p", particleSize, false);
						w << "next: _2};\n";
						w << "_2 = particle;\n";
						w.endScope();
						w << "_9.next = _2;\n"; // pp.alive = alive
					}
					else
					{
						//w << "_9.alive = null;\n"; // pp.alive = NULL					
						w << "_9.next = null;\n"; // pp.alive = NULL
					}
		
					// set number of alive particles
					w << "_9.numParticles = " << numInitParticles << ";\n";

					// set id for next particle that is created
					w << "_9.id = " << numInitParticles << ";\n";

					w.endScope();
				}
			}
		}
		w.writeLine();

		// add instance methods here to bind current instance variable
		w << "instance.update = function()\n";
		{
			w.beginScope();	
			// global is available in outer scope
			//w << "instance.sequence = ++global.sequence;\n";
			w << "++instance.sceneSequence;\n";
			w.writeLine();

			getArrays(w, "instance", "state", stateSize);
			w << "var buffers = global.buffers;\n";
			getArrays(w, "instance", "uniforms", uniformsSize);
			getArrays(w, "instance", "transforms", transformsSize);
			getArrays(w, "instance", "boundingBoxes", boundingBoxesSize);
			if (collector.sceneUseFlags & SCENE_USES_SEED)
				w << "var seed = instance.seed;\n";
			w.writeLine();

			// the update code
			w << updateCode;

			// update shapes (particle systems)
			foreach (ShapeInfoPair& p, collector.shapeInfoMap)
			{
				if (Pointer<ParticleSystem> particleSystem = dynamicCast<ParticleSystem>(p.first))
				{
					// particle system
					Pointer<ParticleSystemInfo> particleSystemInfo = staticCast<ParticleSystemInfo>(p.second);
					if (!particleSystemInfo->hasError)
					{
						const std::string& name = particleSystemInfo->name;
						int3 particleSize = particleSystemInfo->particlePrinter->getSize();
						
						w.beginScope();
			
						// get particle pool of particle system
						w << "var _9 = instance.particlePools." << name << ";\n";

						// uniform variables (iuniforms, funiforms) is available from scene update
						
						// get variables from particle pool
						w << "var _1 = _9.next;\n"; // it = pp.alive
						w << "var _2 = _9;\n";     // last = &pp.alive
						w << "var _3 = _9.dead;\n";  // dead = pp.dead
						w << "var _4 = _9.numParticles;\n"; // numParticles = pp.numParticles

						// uniform update code
						w << particleSystemInfo->updateUniformCode;
						
						// iterate over alive particles
						w << "while (_1)\n";
						w.beginScope();
						getArrays(w, "_1", "_p", particleSize);
						w << "var alive = true;\n";

						// particle update code
						w << particleSystemInfo->updateParticleCode;
											
						// check if particle is still alive
						w << "if (alive)\n";
						w.beginScope();
						
						// yes: link into alive queue
						w << "_2.next = _1;\n"; // *last = it
						w << "_2 = _1;\n"; // last = &it->next

						// next particle
						w << "_1 = _1.next;\n"; // it = it->next
						
						w.endScope();
						w << "else\n";
						w.beginScope();

						// no: link into dead queue
						w << "var _5 = _1.next;\n"; // next = it->next
						w << "_1.next = _3;\n"; // it->next = dead
						w << "_3 = _1;\n"; // dead = it
						
						// next particle
						w << "_1 = _5;\n"; // it = next

						// decrement number of particles
						w << "--_4;\n"; // --numParticles
						
						w.endScope();					
						w.endScope();
						
						// check if we have emitters
						if (particleSystem->emitterUniformType != null)
						{
							w.writeLine();

							// the index selects the active emitter and counts overall number of particles to create
							w << "var _6 = iuniforms[" << collector.uniformsPrinter->getOffset(
								Code() << ".e." << particleSystem->path << ".numNewParticles").x << "];\n";

							// load id from particle pool
							w << "var _7 = _9.id;\n"; // id = pp.id

							// uniform create code
							w << particleSystemInfo->createUniformCode;

							// emit new particles, take from dead particles
							w << "while (_3 && _6 > 0)\n"; // while (dead && index > 0)
							w.beginScope();
							getArrays(w, "_3", "_p", particleSize);
							w << "--_6;\n"; // --index
							
							// code for all emitters (current one selected by index) and particle creation
							w << particleSystemInfo->createParticleCode;
							
							// increment number of particles
							w << "++_4;\n"; // ++numParticles
												
							// increment particle id
							w << "++_7;\n"; // ++id

							// link this particle into alive list and move to next particle
							w << "_2.next = _3;\n"; // *last = dead
							w << "_2 = _3;\n"; // last = &dead->next
							w << "_3 = _3.next;\n"; // dead = dead->next
							w.endScope();

							// write back to particle pool
							w << "_9.id = _7;\n"; // pp.id = id
						}
						
						// end alive list and write back to particle pool
						w << "_2.next = null;\n"; // *last = NULL
						w << "_9.dead = _3;\n";
						w << "_9.numParticles = _4;\n";
						w.endScope();
					}
				}
			}

			// write random seed back to instance
			if (collector.sceneUseFlags & SCENE_USES_SEED)
				w << "instance.seed = seed;\n";
			w.endScope(';');
		}
		w.writeLine();
				
		// main render function
		w << "instance.render = function(viewMatrix, projectionMatrix, layerName, renderQueues)\n";
		{
			w.beginScope();
			
			// global is available in outer scope
			w << "instance.renderSequence = ++global.sequence;\n";
			w << "var viewProjectionMatrix = instance.viewProjectionMatrix;\n";
			w << "engine.matrix4x4Mul(projectionMatrix, viewMatrix, viewProjectionMatrix);\n";
			getArrays(w, "instance", "uniforms", uniformsSize);
			getArrays(w, "instance", "transforms", transformsSize);
			getArrays(w, "instance", "boundingBoxes", boundingBoxesSize);
			if (collector.shaderUseFlags & SHADER_USES_VIEWPORT)
				w << "var viewport = gl.getParameter(gl.VIEWPORT);\n";
			w.writeLine();
			
			// update deformers if necessary
			if (numDynamicBuffers > 0)
			{
				w << "if (instance.sceneSequence != instance.deformerSequence)\n";
				w.beginScope();
				w << "instance.deformerSequence = instance.sceneSequence;\n";
				w.writeLine();
				
				// process sources of dynamic buffers (deformers, sprite particle instancers)
				processDynamicBuffers(w, collector);

				w.endScope(); // if
				w.writeLine();
			}
			
			// render code for all layers
			bool hasLayers = collector.numLayers != 1 || !shaderOptions[0].layerName.empty();
			for (int layerIndex = 0; layerIndex < collector.numLayers; ++layerIndex)
			{
				const ShaderOptions& so = shaderOptions[layerIndex];

				if (hasLayers)
				{
					w << "if (layerName == \"" << so.layerName << "\")\n";
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
						DeformerShaderInfo& deformerShaderInfo = p.second;
			
						// iterate over shader types (MESH, PARTICLE)
						foreach (TargetShaderInfoPair& p, deformerShaderInfo)
						{
							// get layer if it exists
							Pointer<LayerInfo> layerInfo = getValue(p.second.layerInfoMap, layerIndex);
							
							if (layerInfo != null)
							{
								w.writeComment(layerInfo->comment);

								/*
									behavior dependent on sort mode:
									SORT_NEVER    : render immediately
									SORT_MATERIAL : check sort value at runtime. defer and sort if true (shader output code writes to sort flag)
									SORT_ALWAYS   : always defer and sort
								*/

								// calc shader material

								// deformer uniform (from scene)
								// is in uniforms arrays, see above
						
								// shader material (from scene)
								// is in uniforms arrays, see above
						
								// shader instance
								w << "var s_ = instance.shaders." << layerInfo->name << ";\n";

								// uniform (set to shader for each instance)
								w << "var uniform = s_.uniform;\n";
								getArrayReferences(w, layerInfo->materialVariables, "uniform.");
						
								// uniform large arrays that are stored in textures
								getTempArrays(w, layerInfo->materialLargeArrays);
								
								// transfer (to transform)
								//w << "var transfer = s_.transfer;\n";
								getArrayReferences(w, layerInfo->transferVariables, "s_.transfer.");

								if (layerInfo->sortMode == SORT_MATERIAL)
										w << "var sort = false;\n";
												
								// write code that transfers material into shader variables (uniform, transfer)
								w << layerInfo->materialCode;

								// set large arrays to textures
								foreach (const ShaderVariable& variable, layerInfo->materialLargeArrays)
								{
									w << "gl.bindTexture(gl.TEXTURE_2D, uniform." << variable.name << ");\n";
									w << "gl.texSubImage2D(gl.TEXTURE_2D, 0, 0, 0, " << variable.type.numElements
										<< ", 1, gl.RGBA, gl.FLOAT, " << variable.name << ");\n";
									textureBound = true;
								}

								// set sort flag
								if (layerInfo->sortMode == SORT_MATERIAL)
									w << "s_.sort = sort;\n";

								// clear render job queue of shader
								if (layerInfo->sortMode != SORT_ALWAYS)
									w << "s_.renderJobs = null;\n";
							}
						}
					}
				}
				if (textureBound)
					w << "gl.bindTexture(gl.TEXTURE_2D, null);\n";
				w.writeLine();
		
				int numVertexAttributes = defaultNumVertexAttributes;

				// create render jobs
				{
					// render bounding boxes
					if (options.renderBoundingBoxes)
					{
						// set bounding box shader
						w << "gl.useProgram(global.bbProgram);\n";

						if (useVertexArrayObjects)
						{
							// bind vertex array object
							w << "gl.bindVertexArray(global.bbVertexArray);\n";
						}
						else
						{
							// enable vertex attribute 0
							numVertexAttributes = enableVertexAttributes(w, numVertexAttributes, 1);

							// bind bounding box vertex buffer (8 positions) and set to vertex attribute 0
							w << "gl.bindBuffer(gl.ARRAY_BUFFER, global.bbVertexBuffer);\n";
							w << "gl.vertexAttribPointer(0, 3, gl.FLOAT, false, 0, 0);\n";

							// bind bounding box index buffer (12 lines)
							w << "gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, global.bbIndexBuffer);\n";
						}
										
						// set view projection matrix
						w << "gl.uniformMatrix4fv(global.bbViewProjectionMatrix, false, viewProjectionMatrix);\n";

						// get temp variable for world matrix
						w << "var bbMatrix = instance.bbMatrix;\n";

						// get uniform location of world matrix
						w << "var hbbMatrix = global.bbMatrix;\n";
					}
				
					// (recursively) iterate over instances
					w << "var jobIt = renderQueues.begin;\n";
					w << "var jobEnd = renderQueues.end;\n";
					int objectIndex = 0;
					int nrj = 0;
					foreach (const NamedInstance& instance, scene->instances)
					{
						// render if layer is default layer or if instance is in layer
						if (collector.isInLayer(layerIndex, instance))
						{
							RenderJobsInfo info(objectIndex, layerIndex);
							nrj += createRenderJobs(w, collector, compiler, info,
								instance, 0, NameGenerator(), options.renderBoundingBoxes);
						}
						++objectIndex;
					}
					stats.numRenderJobs = max(stats.numRenderJobs, nrj);
					
					// write back only begin iterator. the shader jobs are allocated from the end and thrown away after rendering
					w << "renderQueues.begin = jobIt;\n";
				}
				
				// render per-shader render job queues that contain non-transparent render jobs
				//w << "var resetShader = renderQueues.resetShader;\n";
				foreach (ShaderInfoPair& shaderInfoPair, collector.shaderInfoMap)
				{
					ShaderInfo& shaderInfo = shaderInfoPair.second;

					// iterate over deformer/shader combos (only one with deformer = null when deformers not in shaders)
					foreach (DeformerShaderInfoPair& p, shaderInfo.deformerShaderInfos)
					{
						DeformerShaderInfo& deformerShaderInfo = p.second;
			
						// iterate over shader types (mesh, particle)
						foreach (TargetShaderInfoPair& p, deformerShaderInfo)
						{
							// get layer if it exists
							Pointer<LayerInfo> layerInfo = getValue(p.second.layerInfoMap, layerIndex);
							
							if (layerInfo != null && layerInfo->sortMode != SORT_ALWAYS)
							{
								// set shader
								setShader(w, layerInfo);
								
								// set vertex bindings
								if (!useVertexArrayObjects)
									numVertexAttributes = enableVertexAttributes(w, numVertexAttributes, layerInfo);
								
								// render
								w << "var current = instance.shaders." << layerInfo->name << ".renderJobs;\n";
								w << "while (current)\n";
								w.beginScope();
								w << "current.render(current);\n";
								w << "current = current.next;\n";
								w.endScope();

								// reset shader
								resetShader(w, layerInfo);
							}
						}
					}
				}
				//w << "renderQueues.resetShader = resetShader;\n";

				// reset vertex bindings
				enableVertexAttributes(w, numVertexAttributes, defaultNumVertexAttributes);

				if (hasLayers)
				{
					w.endScope(); // if
				}				
			}			
			w.endScope(';'); // render()
		}
		w.writeLine();

		// init state code
		getArrays(w, "instance", "state", stateSize);
		w << initStateCode;
		w.writeLine();
				
		w << "return instance;\n";
		w.endScope(',');
	} // createInstance
	w.writeLine();

	w << "doneInstance: function(instance)\n";
	{
		w.beginScope();	
		
		// delete textures for large uniform arrays
		foreach (Pointer<LayerInfo> layerInfo, collector.layerInfos)
		{
			foreach (ShaderVariable& variable, layerInfo->materialLargeArrays)
			{
				w << "gl.deleteTexture(instance.shaders." << layerInfo->name
					<< ".uniform." << variable.name << ");\n";
			}
		}

		// delete dynamic vertex buffers for deformer output
		if (numDynamicBuffers > 0)
		{
			int index = 0;
			foreach (DynamicBufferInfo& dynamicBufferInfo, collector.dynamicBufferInfos)
			{
				w << "gl.deleteBuffer(instance.dynamicBuffer" << ident(index) << ");\n";
				++index;					
			}
			w.writeLine();
		}
				
		w.endScope(',');
	}
	w.writeLine();


	// global next render sequence number
	w << "sequence: 0,\n";
	w.writeLine();	
	
	return statePrinter;
}

} // namespace digi
