#ifndef digi_SceneConvert_VertexBuffer_h
#define digi_SceneConvert_VertexBuffer_h

#include <vector>

#include <Digi/ImageConvert/BufferConverter.h>
#include <Digi/Math/All.h>


namespace digi {

/// @addtogroup SceneConvert
/// @{

// vertex buffer, gets generated from vertex fields of shapes
class VertexBuffer : public Object
{
public:

	struct Field
	{
		// source data for field
		Pointer<Buffer> buffer;
		
		// format of field
		BufferFormat dataFormat;
		BufferFormat targetFormat;

		// scale of field value, used when field is stored with reduced resolution
		float4 scale;

		// offset of field value, used when field is stored with reduced resolution
		float4 offset;
			
		// offset of field (in bytes) in vertex
		int dataOffset;
		int targetOffset;
		
		
		Field()
			: scale(), offset(), dataOffset(), targetOffset() {}

		Field(Pointer<Buffer> buffer, const BufferFormat& format, float4 scale, float4 offset)
			: buffer(buffer), dataFormat(format), targetFormat(format), scale(scale), offset(offset),
			dataOffset(), targetOffset() {}
	};

	struct FieldGreater
	{
		bool operator()(const Field& a, const Field& b)
		{
			int x = a.dataFormat.layout << 8 | a.dataFormat.type;
			int y = b.dataFormat.layout << 8 | b.dataFormat.type;
			return x > y;
		}
	};

	VertexBuffer()
		: dataVertexSize(), targetVertexSize(), dataAlign(),
		fatVertexOffset(), numVertices(), bigBufferIndex(), bigBufferOffset() {}
	~VertexBuffer();

	// get an unique string for the output format (not human readable!) to combine vertex buffers of the same layout
	std::string getFormatString();
	
	// copy data of a field into fat vertex buffer
	void copyField(Pointer<BufferConverter> bufferConverter, Field& field,
		DataConverter::Mode fatMode, uint8_t* fatVertices, int fatVertexSize, size_t numVertices);

	// fields (position, normal, uv etc.)
	std::vector<Field> fields;
		
	// size of one vertex
	int dataVertexSize;
	int targetVertexSize;
	
	// alignment of buffer in data file (maximum aligment of fields)
	int dataAlign;
	
	// offset of vertex in fat vertex
	int fatVertexOffset;
	
	// number of vertices
	size_t numVertices;
	
	// the vertex data
	std::vector<uint8_t> data;
	
	// index of big buffer that contains this buffer
	int bigBufferIndex;

	// offset of this buffer in big buffer
	size_t bigBufferOffset;	
};

/// @}

} // namespace digi

#endif
