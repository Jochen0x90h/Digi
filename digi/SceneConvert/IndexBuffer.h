#ifndef digi_SceneConvert_IndexBuffer_h
#define digi_SceneConvert_IndexBuffer_h

#include <vector>

#include <Digi/Base/Platform.h>
#include <Digi/Utility/Object.h>


namespace digi {

/// @addtogroup SceneConvert
/// @{


class IndexBuffer : public Object
{
public:

	virtual ~IndexBuffer();
	
	// type of index buffer
	enum Type
	{
		INDEX_8_BIT,
		INDEX_16_BIT,
		INDEX_32_BIT,
	};

	// index buffer can have multiple sections because of 16 bit limit in opengl es
	struct Section
	{
		// the base index is added to all indices in buffer
		int baseIndex;

		// number of indices (number of triangles * 3)
		int indexCount;

		Type type;

		// the index data
		std::vector<uint8_t> data;

		// index of big buffer that contains this buffer
		int bigBufferIndex;

		// offset of this buffer in big buffer
		size_t bigBufferOffset;	
			
		Section() : baseIndex(), indexCount(), type(), bigBufferIndex(), bigBufferOffset() {}
		
		int getAlign();
	};
	
	std::vector<Section> sections;
};

/// @}

} // namespace digi

#endif
