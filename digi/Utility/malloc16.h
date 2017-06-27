#ifndef digi_Utility_malloc16_h
#define digi_Utility_malloc16_h

#include <stdlib.h>
#include <digi/Base/Platform.h>


namespace digi {

/// @addtogroup Utility
/// @{

inline void* malloc16(size_t size)
{
	#ifdef _WIN32
		#ifdef _M_X64
			return malloc(size);
		#else
			uint8_t* data = (uint8_t*)malloc(size + 16);
			uint8_t offset = uint8_t(16 - ((size_t)data & 15));
			data += offset;
			data[-1] = offset;
			return data;		
			//return _aligned_malloc(size, 16);
		#endif
	#else
		void* data;
		posix_memalign(&data, 16, size);
		return data;
	#endif
}	

inline void free16(void* data)
{
	#ifdef _WIN32
		#ifdef _M_X64
			free(data);
		#else
			if (data != NULL)
				free((uint8_t*)data - ((uint8_t*)data)[-1]);
			//_aligned_free(data);
		#endif
	#else
		free(data);
	#endif
}

/// @}

} // namespace digi

#endif
