#include <digi/Utility/ArrayUtility.h>
#include <digi/Utility/IntUtility.h>
#include <digi/Utility/StringUtility.h>
#include <digi/Utility/VectorUtility.h>
#include <digi/Utility/SetUtility.h>
#include <digi/Utility/foreach.h>
#include <digi/System/Log.h>
#include <digi/CodeGenerator/CodeWriterFunctions.h>
//#include <digi/Mesh/Mesh.h> // calcACMR

#include "BufferInfo.h"


namespace digi {

/*
// VertexBuffer

VertexBuffer::~VertexBuffer()
{
}

std::string VertexBuffer::getFormatString()
{
	std::string result;
	foreach (const Field& field, this->fields)
	{
		// we can cast to char as number of layouts and formats does not exceed 255
		result += char(field.format.layout);
		result += char(field.format.type);
	}
	return result;	
}

void VertexBuffer::copyField(Pointer<BufferConverter> bufferConverter, Field& field,
	bool bigEndian, uint8_t* fatVertices, int fatVertexSize, size_t numVertices)
{
	// convert field buffer to fatVertices
	if (all(field.scale == 1.0f) && all(field.offset == 0.0f))
	{
		// no scale/offset: only convert format
		bufferConverter->convert(field.buffer->getFormat(), field.buffer->getData<void>(),
			field.format, bigEndian, fatVertices + this->vertexOffset + field.fieldOffset, fatVertexSize,
			numVertices);
	}
	else
	{
		// apply scale/offset
		const char* code =
			"struct Global\n"
			"{\n"
			"  float4 scale;\n"
			"  float4 offset;\n"
			"};\n"
			"float4 main(float4 value, Global& global)\n"
			"{\n"
			"  return value * global.scale + global.offset;\n"
			"}\n";
		
		// inverse scale/offset
		ALIGN(16) struct
		{
			float4 scale;
			float4 offset;				
		} global;
		global.scale = 1.0f / field.scale;
		global.offset = -field.offset * global.scale;
		
		// convert to buffer
		bufferConverter->convert(field.buffer->getFormat(), field.buffer->getData<void>(),
			code, &global,
			field.format, bigEndian, fatVertices + this->vertexOffset + field.fieldOffset, fatVertexSize,
			numVertices);
	}
}


// IndexBuffer

IndexBuffer::~IndexBuffer()
{
}

int IndexBuffer::Section::getAlign()
{
	int aligns[] = {1, 2, 4};
	return aligns[int(this->type)];
}
*/

// BufferInfo

BufferInfo::~BufferInfo()
{
}


// DeformerJobInfo

class FieldInfoMemberVisitor : public Type::Visitor
{
	public:
		FieldInfoMemberVisitor(std::vector<DeformerJobInfo::FieldInfo>& fieldInfos, Type::AlignMode alignMode)
			: fieldInfos(fieldInfos), alignMode(alignMode), offset(0), maxAlign(1) {}
		virtual ~FieldInfoMemberVisitor() {}
		
		// visits all leaf members with full path in structure (e.g. "foo.bar[3].x")
		virtual void visit(const std::string& member, const std::string& type)
		{
			// convert to BufferFormat
			BufferFormat format = toBufferFormat(VectorInfo(type));

			// align offset
			int componentSize = format.getComponentSize();
			if (alignMode == Type::COMPONENT_ALIGN)
			{
				this->maxAlign = max(this->maxAlign, componentSize);
				this->offset += -this->offset & (componentSize - 1);			
			}
			else if (alignMode == Type::VECTOR_ALIGN)
			{
				int align = upToPowerOfTwo(format.getMemorySize());
				this->maxAlign = max(this->maxAlign, align);
				this->offset += -this->offset & (align - 1);			
			}
			
			// add field info
			this->fieldInfos += DeformerJobInfo::FieldInfo(member, format, this->offset);
			
			// increment offset
			this->offset += format.getMemorySize();
		}
		
		std::vector<DeformerJobInfo::FieldInfo>& fieldInfos;
		Type::AlignMode alignMode;
		size_t offset;
		int maxAlign;
};

void DeformerJobInfo::setOutputType(Pointer<StructType> type, Type::AlignMode alignMode)
{
	//this->outputType = type;

	// build field infos
	FieldInfoMemberVisitor visitor(this->fieldInfos, alignMode);
	type->visitMembers(visitor);

	// set aligned vertex size
	this->vertexSize = visitor.offset + (-visitor.offset & (visitor.maxAlign - 1));
}

std::string DeformerJobInfo::getFormatString()
{
	std::string result;
	foreach (const FieldInfo& fieldInfo, this->fieldInfos)
	{
		// we can cast to char as number of layouts and formats does not exceed 255
		result += char(fieldInfo.format.layout);
		result += char(fieldInfo.format.type);
	}
	return result;
}


// LayerInfo

LayerInfo::~LayerInfo()
{
}


// ShapeInfo

ShapeInfo::~ShapeInfo()
{
}


// ConstantMeshInfo

ConstantMeshInfo::~ConstantMeshInfo()
{
}

Shader::ShapeType ConstantMeshInfo::getType()
{
	return Shader::MESH;
}

StringRef ConstantMeshInfo::getName()
{
	return StringRef();
}

int ConstantMeshInfo::getNumVertices()
{
	return this->numVertices;
}

int ConstantMeshInfo::getNumVertexArrays()
{
	return int(this->indexBuffer->sections.size());
}

int ConstantMeshInfo::getBaseIndex(int vertexArrayIndex)
{
	return this->indexBuffer->sections[vertexArrayIndex].baseIndex;
}


// TextMeshInfo

TextMeshInfo::~TextMeshInfo()
{
}

Shader::ShapeType TextMeshInfo::getType()
{
	return Shader::MESH;
}

StringRef TextMeshInfo::getName()
{
	return this->name;
}

int TextMeshInfo::getNumVertices()
{
	return this->numVertices;
}

int TextMeshInfo::getNumVertexArrays()
{
	return 1;
}

int TextMeshInfo::getBaseIndex(int vertexArrayIndex)
{
	return 0;
}


// ParticleSystemInfo

ParticleSystemInfo::~ParticleSystemInfo()
{
}

Shader::ShapeType ParticleSystemInfo::getType()
{
	return Shader::SPRITES;
}

StringRef ParticleSystemInfo::getName()
{
	return this->name;
}

int ParticleSystemInfo::getNumVertices()
{
	return this->numParticles;
}

int ParticleSystemInfo::getNumVertexArrays()
{
	return 1;
}

int ParticleSystemInfo::getBaseIndex(int vertexArrayIndex)
{
	return 0;
}


// ParticleInfo

void ParticleInfo::writeBuffers(Pointer<BufferConverter> bufferConverter, ShapeInfoMap& shapeInfos,
	CodeWriter& w, DataWriter& d)
{
	DataConverter::Mode fatMode = d.isBigEndian() ? DataConverter::BE : DataConverter::LE;

	this->buffer.offset = d.align(4);
	
	foreach (ShapeInfoPair& p, shapeInfos)
	{
		if (Pointer<ParticleSystem> particleSystem = dynamicCast<ParticleSystem>(p.first))
		{
			Pointer<ParticleSystemInfo> particleSystemInfo = staticCast<ParticleSystemInfo>(p.second);
		
			// build fat particle buffer for init state
			int numFatParticles = particleSystemInfo->numInitParticles;
			if (numFatParticles > 0)
			{
				Pointer<StructType> particleType = new StructType();
				
				// step 1: calc fat particle size and build particle type
				size_t fatParticleSize = 0;
				foreach (const ParticleSystem::Field& field, particleSystem->initialState)
				{
					fatParticleSize += field.buffer->getFormat().getMemorySize();				
					particleType->addMember(field.name, toVectorInfo(field.buffer->getFormat()).toString());
				}
				
				// step 2: copy into fatParticles
				std::vector<uint8_t> fatParticles(numFatParticles * fatParticleSize);			
				size_t offset = 0;
				foreach (const ParticleSystem::Field& field, particleSystem->initialState)
				{
					BufferFormat format = field.buffer->getFormat();
					bufferConverter->convert(field.buffer->getFormat(), field.buffer->getData<void>(),
						format, fatMode, fatParticles.data() + offset, fatParticleSize,
						numFatParticles);
					offset += format.getMemorySize();
				}
				
				// step 3: write
				particleSystemInfo->offset = d.align(4) - this->buffer.offset;
				d.writeData(fatParticles.data(), fatParticles.size());
				
				// step 4: create struct
				particleType->writeType(w, "ParticleInit" + particleSystemInfo->name, Type::NO_ALIGN);
				w.writeLine();
			}
		}
	}
	
	this->buffer.size = d.align(4) - this->buffer.offset;
}

} // namespace digi
