#ifndef digi_Utility_MemoryPool_h
#define digi_Utility_MemoryPool_h

#include <vector>

#include "Standard.h"
#include "foreach.h"

namespace digi {

/// @addtogroup Utility
/// @{

template <typename Type, int blockSize>
class MemoryPool
{
public:
	MemoryPool()
		: block(), index(blockSize + 1)
	{
	}
	
	~MemoryPool()
	{
		foreach (Type* block, this->blocks)
		{
			delete [] block;
		}	
	}

	Type* alloc(size_t numElements)
	{
		// check if enough space in current block
		if (this->index + numElements > blockSize)
		{
			// no space in current block
			
			// check if numElements is more than half of block size
			if (numElements > blockSize / 2)
			{
				// yes: allocate an own block
				Type* p = new Type[numElements];
				this->blocks.push_back(p);
				return p;
			}
			this->block = new Type[blockSize];
			this->blocks.push_back(this->block);
			this->index = 0;			
		}
		
		Type* p = this->block + this->index;
		this->index += numElements;
		return p;
	}
	
protected:

	std::vector<Type*> blocks;
	Type* block;
	size_t index;
};

/// @}

} // namespace digi

#endif
