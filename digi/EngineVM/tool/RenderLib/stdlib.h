#ifndef stdlib_h
#define stdlib_h

#include "stddef.h"

extern "C" {
	
void* malloc(size_t);
void free(void*);
//void* realloc(void*, size_t);

}

#endif
