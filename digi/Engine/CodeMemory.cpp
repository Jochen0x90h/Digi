#ifdef _WIN32
	#include <Windows.h>
#else
	#include <sys/mman.h>
#endif

#include "CodeMemory.h"


namespace digi {

CodeMemory::CodeMemory(size_t numBytes)
{
	#ifdef _WIN32
		this->d = (uint8_t*)VirtualAlloc(
			NULL,
			numBytes,
			MEM_RESERVE | MEM_COMMIT,
			PAGE_EXECUTE_READWRITE);
	#else
		this->d = (uint8_t*)mmap(
			NULL,
			numBytes,
			PROT_READ | PROT_WRITE | PROT_EXEC,
			MAP_ANON | MAP_PRIVATE,
			-1,
			0);
	#endif
	this->s = numBytes;
}

CodeMemory::~CodeMemory()
{
	#ifdef _WIN32
		VirtualFree(
			this->d,
			0,
			MEM_RELEASE);
	#else
		munmap(
			this->d,
			this->s);
	#endif
}

} // namespace digi
