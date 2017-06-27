#include <Digi/Utility/foreach.h>
#include "VertexBuffer.h"


namespace digi {

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
		result += char(field.dataFormat.layout);
		result += char(field.dataFormat.type);
	}
	return result;	
}

void VertexBuffer::copyField(Pointer<BufferConverter> bufferConverter, Field& field,
	DataConverter::Mode fatMode, uint8_t* fatVertices, int fatVertexSize, size_t numVertices)
{
	// convert field buffer to fatVertices
	if (all(field.scale == 1.0f) && all(field.offset == 0.0f))
	{
		// no scale/offset: only convert format
		bufferConverter->convert(field.buffer->getFormat(), field.buffer->getData<void>(),
			field.dataFormat, fatMode, fatVertices + this->fatVertexOffset + field.dataOffset, fatVertexSize,
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
		global.scale = 1.0f / max(field.scale, 1e-20f);
		global.offset = -field.offset * global.scale;
		
		// convert to buffer
		bufferConverter->convert(field.buffer->getFormat(), field.buffer->getData<void>(),
			code, &global,
			field.dataFormat, fatMode, fatVertices + this->fatVertexOffset + field.dataOffset, fatVertexSize,
			numVertices);
	}
}

} // namespace digi
