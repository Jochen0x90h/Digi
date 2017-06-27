#include "IndexBuffer.h"


namespace digi {


// IndexBuffer

IndexBuffer::~IndexBuffer()
{
}

int IndexBuffer::Section::getAlign()
{
	int aligns[] = {1, 2, 4};
	return aligns[int(this->type)];
}

} // namespace digi
